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

#include "cadef.h"
#include "cawidgetscannerparser.h"
#include "widgetscannerutils.h"

// CONSTANTS

const TInt KChunkSize = 8192;
const TInt KDriveLetterLength = 2;
const TInt KModificationTimeLength = 17;

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
_LIT8(KWidgetProvider, "widgetprovider");

_LIT8(KWidgetManifest, "hswidgetmanifest");
_LIT8(KIcon, "icon");
_LIT8(KServiceXml, "servicexml");

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
    delete iWidgetDescriptor;
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
    TXmlEngElement docElement = doc.DocumentElement();

    if (docElement.Name() == KWidgetProvider) {
        TXmlEngElement element;
        RXmlEngNodeList<TXmlEngElement> elementList;
        CleanupClosePushL(elementList);
        docElement.GetElementsByTagNameL(elementList, KWidget);

        while (elementList.HasNext())
            {
            element = elementList.Next();
            if ( element.HasAttributes() )
                {
                ParseWidgetL( aFilePath, element, aPackageUid );
                }
            }
        CleanupStack::PopAndDestroy(&elementList);
    } else if (docElement.Name() == KWidgetManifest) {

        delete iWidgetDescriptor;
        iWidgetDescriptor = NULL;
        iWidgetDescriptor = CCaWidgetDescription::NewL();
        iWidgetDescriptor->SetPackageUidL(aPackageUid);
        iWidgetDescriptor->SetVisible(ETrue);


        SetMmcIdL( iWidgetDescriptor );

        RXmlEngNodeList<TXmlEngElement> childElementList;
        CleanupClosePushL(childElementList);
        docElement.GetChildElements(childElementList);
        TXmlEngElement element;
        while (childElementList.HasNext())
          {
          element = childElementList.Next();
          if (element.Name() == KUri)
              {
              ParseUriL(element);
              }
          else if (element.Name() == KTitle)
              {
              ParseTitleL(element);
              }
          else if (element.Name() == KIcon)
              {
              ParseIconL(element,aPackageUid);
              }
          else if (element.Name() == KDescription)
              {
              ParseDescriptionL(element);
              }
          else if (element.Name() == KHidden)
              {
              ParseHiddenL(element);
              }
          else if (element.Name() == KServiceXml)
              {
              ParseServiceXmlL(element);
              }
           }
        CleanupStack::PopAndDestroy(&childElementList);

        //set path for hs to use, trim last 2 chars (doubleslash)
        HBufC *libraryPath = GetManifestDirectoryPathLC( aPackageUid );
        iWidgetDescriptor->SetPathL(libraryPath->Mid(0,libraryPath->Length()-1));
        CleanupStack::PopAndDestroy(libraryPath);
        
        HBufC *libraryPath2 = GetManifestDirectoryPathLC( aPackageUid );
        libraryPath2 = libraryPath2->ReAllocL(libraryPath2->Length() + iWidgetDescriptor->GetUri().Length());
        CleanupStack::Pop(1);
        CleanupStack::PushL(libraryPath2);
        libraryPath2->Des().Append(iWidgetDescriptor->GetUri());
        iWidgetDescriptor->SetLibraryL(*libraryPath2);
        CleanupStack::PopAndDestroy(libraryPath2);
        
                
        TTime modificationTime;
        iFs.Modified( aFilePath, modificationTime);
        TInt64 modificationIntTime = modificationTime.Int64();
        RBuf16 rBuf;
        rBuf.CleanupClosePushL();
        rBuf.CreateL( KModificationTimeLength );
        rBuf.AppendNum( modificationIntTime );
        iWidgetDescriptor->SetModificationTimeL( rBuf );
        CleanupStack::PopAndDestroy( &rBuf );

        TInt index = iWidgets.Find( iWidgetDescriptor, CCaWidgetDescription::Compare );
        if ( index != KErrNotFound )
            {
            delete iWidgets[index];
            iWidgets.Remove( index );
            }
        iWidgets.AppendL( iWidgetDescriptor );//ownership transfer
        iWidgetDescriptor = NULL;
    }

    CleanupStack::PopAndDestroy(&doc);
    CleanupStack::PopAndDestroy(&parser);
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::ParseWidgetL(
        const TDesC& aFilePath, TXmlEngElement aElement,
        const TDesC& aPackageUid )
    {
    CCaWidgetDescription* widget = CCaWidgetDescription::NewLC();

    SetUriL( aElement, widget );
    SetLibraryL( aElement, aPackageUid, widget);
    SetTitleL( aElement, widget );
    SetDescriptionL( aElement, widget );
    SetVisibilityL( aElement, widget );
    SetIconUriL( aElement, aPackageUid, widget);
    widget->SetPackageUidL( aPackageUid );
    SetMmcIdL( widget );

    TTime modificationTime;
    iFs.Modified( aFilePath, modificationTime);
    TInt64 modificationIntTime = modificationTime.Int64();
    RBuf16 rBuf;
    rBuf.CleanupClosePushL();
    rBuf.CreateL( KModificationTimeLength );
    rBuf.AppendNum( modificationIntTime );
    widget->SetModificationTimeL( rBuf );
    CleanupStack::PopAndDestroy( &rBuf );

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
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
        aElement.AttributeValueL( KUri ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        aWidget->SetUriL( *attributeValue );
        }
    CleanupStack::PopAndDestroy( attributeValue );
}

// ----------------------------------------------------------------------------
//    keep in mind that setLibrary also setsPath
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerParser::SetLibraryL( TXmlEngElement & aElement,
    const TDesC & aPackageUid, CCaWidgetDescription * aWidget )
{
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
        aElement.AttributeValueL( KLibrary ) );
    CleanupStack::PushL( attributeValue );

    if( attributeValue->Compare( KNullDesC ) != 0 )
        {
        HBufC *libraryPath = GetManifestDirectoryPathLC( aPackageUid );

        //set path for hs to use, trim last 2 chars (doubleslash)
        aWidget->SetPathL(libraryPath->Mid(0,libraryPath->Length()-1));

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
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
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
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
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
    HBufC *hidden = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                aElement.AttributeValueL( KHidden ) );
    CleanupStack::PushL( hidden );

    if( hidden->Compare( KTrue ) == 0 )
        {
        aWidget->SetVisible( EFalse );
        }
    else
        {
        aWidget->SetVisible( ETrue );
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
    HBufC *attributeValue = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
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

    TChar massStorageDrive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
                DriveInfo::EDefaultMassStorage, massStorageDrive ) );
    if( iCurrentDriveLetter == removableDrive )
        {
        RBuf mmcId;
        mmcId.CreateL(KMassStorageIdLength);
        mmcId.CleanupClosePushL();
        WidgetScannerUtils::CurrentMmcId( iFs, mmcId );
        widget->SetMmcIdL( mmcId );
        CleanupStack::PopAndDestroy( &mmcId );
        }
    else if( iCurrentDriveLetter == massStorageDrive )
        {
        widget->SetMmcIdL( KCaMassStorage );
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
    HBufC* result =
        HBufC16::NewLC( iImportPath.Length() + KDriveLetterLength );
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

void CCaWidgetScannerParser::ParseUriL(TXmlEngElement & aElement)
    {
    if (aElement.Text().Length())
        {
        HBufC *utf16 =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L( aElement.Text() );
        CleanupStack::PushL( utf16 );
        iWidgetDescriptor->SetUriL( *utf16 );
        CleanupStack::PopAndDestroy( utf16 );
        }

    }
void CCaWidgetScannerParser::ParseTitleL(TXmlEngElement & aElement)
    {
    if (aElement.Text().Length())
        {
        HBufC *utf16 =
            CnvUtfConverter::ConvertToUnicodeFromUtf8L( aElement.Text() );
        CleanupStack::PushL( utf16 );
        iWidgetDescriptor->SetTitleL( *utf16 );
        CleanupStack::PopAndDestroy( utf16 );
        }
    }
void CCaWidgetScannerParser::ParseIconL(TXmlEngElement & aElement,
    const TDesC & aPackageUid )
    {
    HBufC *utf16 =
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( aElement.Text() );
    CleanupStack::PushL( utf16 );
    if ( utf16->Compare( KNullDesC ) )
        {
        HBufC* iconUriPath = GetManifestDirectoryPathLC( aPackageUid );
        iconUriPath = iconUriPath->ReAllocL( iconUriPath->Length() + utf16->Length() );
        CleanupStack::Pop(1);
        CleanupStack::PushL(iconUriPath);
        TPtr iconUriPathModifier( iconUriPath->Des() );
        iconUriPathModifier.Append( *utf16 );
        iWidgetDescriptor->SetIconUriL( *iconUriPath );
        CleanupStack::PopAndDestroy( iconUriPath );
        }

    CleanupStack::PopAndDestroy( utf16 );
    }

void CCaWidgetScannerParser::ParseDescriptionL(TXmlEngElement & aElement)
    {
    HBufC *desc =
        CnvUtfConverter::ConvertToUnicodeFromUtf8L( aElement.Text() );
    CleanupStack::PushL( desc );

    if( desc->Compare( KNullDesC ) != 0 )
       {
       iWidgetDescriptor->SetDescriptionL( *desc );
       }
    CleanupStack::PopAndDestroy( desc );
    }
void CCaWidgetScannerParser::ParseHiddenL( TXmlEngElement& aElement )
    {
    if( aElement.Text().Compare( _L8("true") ) == 0 )
        {
        iWidgetDescriptor->SetVisible( EFalse );
        }

    }
void CCaWidgetScannerParser::ParseServiceXmlL( TXmlEngElement& aElement )
    {
    HBufC *serviceXml = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aElement.Text() );
    CleanupStack::PushL( serviceXml );
       
    if (serviceXml->Compare(KNullDesC) != 0)
        {
        iWidgetDescriptor->SetServiceXmlL( *serviceXml );
        }
    CleanupStack::PopAndDestroy( serviceXml );
    }
//  End of File
