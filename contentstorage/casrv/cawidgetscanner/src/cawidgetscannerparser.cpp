/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
 *
*/


// INCLUDE FILES

#include <driveinfo.h>
#include <xmlengnodelist.h>
#include <xmlengdomparser.h>
#include <xmlengdocument.h>
#include <utf.h>

#include "cawidgetscannerparser.h"
#include "widgetscannerutils.h"

// CONSTANTS

const TInt KChunkSize = 8192;
const TInt KDriveLetterLength = 2;

_LIT( KManifest, ".manifest");
_LIT( KColen, ":" );
_LIT( KImportDir, "\\private\\20022F35\\import\\widgetregistry\\" );
//_LIT( KImportDir, "import\\widgetregistry\\" );
_LIT( KDoubleSlash, "\\" );
_LIT( KTrue, "true" );
_LIT8(KWidget, "widget" );
_LIT8(KUri, "uri" );
_LIT8(KLibrary, "library" );
_LIT8(KTitle, "title" );
_LIT8(KDescription, "description" );
_LIT8(KHidden, "hidden" );
_LIT8(KIconUri, "iconuri" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCaWidgetScannerParser::CCaWidgetScannerParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerParser::CCaWidgetScannerParser( RFs& aFs ):
    iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ConstructL( )
    {
    iImportPath.CreateL( KMaxPath );
    //iFs.PrivatePath( iImportPath );
    iImportPath.Append( KImportDir );
    iDomImpl.OpenL();
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerParser* CCaWidgetScannerParser::NewL( RFs& aFs )
    {
    CCaWidgetScannerParser* self = NewLC( aFs );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerParser::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerParser* CCaWidgetScannerParser::NewLC( RFs& aFs )
    {
    CCaWidgetScannerParser* self = new( ELeave ) CCaWidgetScannerParser( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCaWidgetScannerParser::~CCaWidgetScannerParser()
    {
    iImportPath.Close();
    iWidgets.ResetAndDestroy();
    iDomImpl.Close();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
RWidgetArray& CCaWidgetScannerParser::WidgetsScanL( )
    {
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    iWidgets.ResetAndDestroy();

    for ( TInt driveNumber=EDriveZ; driveNumber >= EDriveA; driveNumber-- )
        {
        if ( driveList[driveNumber] )
            {
            User::LeaveIfError( iFs.DriveToChar( driveNumber,
                    iCurrentDriveLetter ) );
            ScanOnDriveL( );
            }
        }
    return iWidgets;
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ScanOnDriveL( )
    {
    CDir* directories = GetDirectoriesLC( );
    if ( directories )
        {
        for ( TInt i(0); i<directories->Count( ); i++ )
            {
            if((*directories)[i].IsDir())
                {
                ParseDirectoryL((*directories)[i].iName);
                }
            }
        }
    CleanupStack::PopAndDestroy( directories );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ParseDirectoryL( const TDesC& aDirectoryName )
    {
    HBufC* manifestDirectoryPath =
        GetManifestDirectoryPathLC( aDirectoryName );

    CDir* fileList = NULL;

    User::LeaveIfError( iFs.GetDir( *manifestDirectoryPath,
                         KEntryAttMatchExclude|KEntryAttDir,
                         ESortByDate, fileList ) );
    CleanupStack::PushL( fileList );

    for ( TInt i = 0; i<fileList->Count( ); i++ )
        {
        if( (*fileList)[i].iName.Find( KManifest ) != KErrNotFound )
            {
            RBuf fullFilePath;
            CleanupClosePushL( fullFilePath );
            fullFilePath.CreateL( manifestDirectoryPath->Length() +
                    (*fileList)[i].iName.Length());
            fullFilePath.Append( *manifestDirectoryPath );
            fullFilePath.Append( (*fileList)[i].iName );
            //if file is corrupted we go to the next one
            TRAP_IGNORE(ParseManifestFileL( fullFilePath, aDirectoryName ));
            CleanupStack::PopAndDestroy( &fullFilePath );
            }
        }
    CleanupStack::PopAndDestroy( fileList );
    CleanupStack::PopAndDestroy( manifestDirectoryPath );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ParseManifestFileL(
        const TDesC& aFilePath, const TDesC& aPackageUid )
    {
    RXmlEngDOMParser parser;
    CleanupClosePushL(parser);
    User::LeaveIfError(parser.Open(iDomImpl));
    RXmlEngDocument doc = parser.ParseFileL(aFilePath, KChunkSize);
    CleanupClosePushL(doc);
    TXmlEngElement docElement = doc.AsElement();
    TXmlEngElement element;
    RXmlEngNodeList<TXmlEngElement> elementList;
    CleanupClosePushL(elementList);

    element = docElement.FirstChild().AsElement();
    element.GetElementsByTagNameL(elementList, KWidget);

    while (elementList.HasNext())
        {
        element = elementList.Next();
        if ( element.HasAttributes() )
            {
            ParseWidgetL( element, aPackageUid );
            }
        }
    CleanupStack::PopAndDestroy(&elementList);
    CleanupStack::PopAndDestroy(&doc);
    CleanupStack::PopAndDestroy(&parser);
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ParseWidgetL(
        TXmlEngElement aElement, const TDesC& aPackageUid )
    {
    CCaWidgetDescription* widget = CCaWidgetDescription::NewLC();

    SetUriL( aElement, widget );
    SetLibraryL( aElement, aPackageUid, widget);
    SetTitleL( aElement, widget );
    SetDescriptionL( aElement, widget );
    SetVisibilityL( aElement, widget );
    SetIconUriL( aElement, aPackageUid, widget);
    widget->SetPackageUidL( aPackageUid );
    SetMmcIdL( widget);

    TInt index = iWidgets.Find( widget, CCaWidgetDescription::Compare );
    if ( index != KErrNotFound )
        {
        delete iWidgets[index];
        iWidgets.Remove( index );
        }
    iWidgets.AppendL( widget );//ownership transfer
    CleanupStack::Pop( widget );
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetUriL(
    TXmlEngElement & aElement, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
        aElement.AttributeValueL( KUri ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        aWidget->SetUriL( *attributeValue );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetLibraryL( TXmlEngElement & aElement,
    const TDesC & aPackageUid, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
        aElement.AttributeValueL( KLibrary ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        HBufC *libraryPath = GetManifestDirectoryPathLC( aPackageUid );
        libraryPath->ReAllocL(
            libraryPath->Length() + attributeValue->Length());

        TPtr libraryPathModifier( libraryPath->Des() );
        libraryPathModifier.Append( *attributeValue );
        aWidget->SetLibraryL( *libraryPath );

        CleanupStack::PopAndDestroy( libraryPath );
        }
    else
        {
        aWidget->SetLibraryL( KNoLibrary );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetTitleL(
    TXmlEngElement & aElement, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
                aElement.AttributeValueL( KTitle ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        aWidget->SetTitleL( *attributeValue );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetDescriptionL(
    TXmlEngElement & aElement, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
                aElement.AttributeValueL( KDescription ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        aWidget->SetDescriptionL( *attributeValue );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetVisibilityL(
    TXmlEngElement & aElement, CCaWidgetDescription * aWidget )
{
    HBufC *hidden = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
                aElement.AttributeValueL( KHidden ) );
    CleanupStack::PushL( hidden );

    if( hidden->Compare( KTrue ) == 0 )
        {
        aWidget->SetVisible( EFalse );
        }
    CleanupStack::PopAndDestroy( hidden );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetIconUriL( TXmlEngElement & aElement,
    const TDesC & aPackageUid, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf7L(
                aElement.AttributeValueL( KIconUri ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        HBufC* iconUriPath = GetManifestDirectoryPathLC( aPackageUid );
        iconUriPath->ReAllocL(
            iconUriPath->Length() + attributeValue->Length() );

        TPtr iconUriPathModifier( iconUriPath->Des() );
        iconUriPathModifier.Append( *attributeValue );
        aWidget->SetIconUriL( *iconUriPath );

        CleanupStack::PopAndDestroy( iconUriPath );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetMmcIdL( CCaWidgetDescription * widget )
{
    TChar removableDrive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
                DriveInfo::EDefaultRemovableMassStorage, removableDrive ) );

    if( iCurrentDriveLetter == removableDrive )
        {
        widget->SetMmcId( WidgetScannerUtils::CurrentMmcId( iFs ) );
        }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CDir* CCaWidgetScannerParser::GetDirectoriesLC( )
    {
    CDir* result = NULL;
    HBufC* path = FullPathLC( );
    iFs.GetDir( *path, KEntryAttDir, ESortByName, result );
    CleanupStack::PopAndDestroy( path );
    CleanupStack::PushL( result );
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
HBufC* CCaWidgetScannerParser::FullPathLC( )
    {
    HBufC* result = HBufC16::NewLC( iImportPath.Length() + KDriveLetterLength );
    TPtr modifier( result->Des() );
    modifier.Append( iCurrentDriveLetter );
    modifier.Append( KColen );
    modifier.Append( iImportPath );
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
HBufC* CCaWidgetScannerParser::GetManifestDirectoryPathLC(
        const TDesC& aDirectoryName )
    {
    HBufC* result = HBufC16::NewLC( KMaxPath );
    TPtr modifier( result->Des() );
    HBufC* path = FullPathLC( );
    modifier.Append( *path );
    CleanupStack::PopAndDestroy( path );
    modifier.Append( aDirectoryName );
    modifier.Append( KDoubleSlash );
    return result;
    }

//  End of File
