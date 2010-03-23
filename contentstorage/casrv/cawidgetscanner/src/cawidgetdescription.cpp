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

#include "cadef.h"
#include "cawidgetdescription.h"
#include "cainnerentry.h"
#include "widgetscannerutils.h"

// CONST
const TInt KNoId = -1;
_LIT( KDoubleSlash, "\\" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCaWidgetDescription::CCaWidgetDescription
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCaWidgetDescription::CCaWidgetDescription() :
    iEntryId(KNoId), iFlags(EVisible)
    {
    }

// -----------------------------------------------------------------------------
// CCaWidgetDescription::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::ConstructL( )
    {
    }

// -----------------------------------------------------------------------------
// CCaWidgetDescription::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::ConstructL( CCaInnerEntry* aEntry )
    {
    iEntryId = aEntry->GetId();
    iDescription.CreateL(aEntry->GetDescription().Length());
    iDescription = aEntry->GetDescription();
    //package uid
    TBuf<KMaxUidName> uidDesc;
    aEntry->FindAttribute( KCaPackageUid, uidDesc );
    if ( uidDesc.Length() )
    	{
    	TLex uidLex(uidDesc);
    	User::LeaveIfError( uidLex.Val( iPackageUid, EHex ));
    	}
    iTitle.CreateL(aEntry->GetText().Length());
    iTitle = aEntry->GetText();
    iIconUri.CreateL(aEntry->GetIcon().iFileName.Length());
    iIconUri = aEntry->GetIcon().iFileName;
    //library
    iLibrary.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetLibrary, iLibrary );
    //uri
    iUri.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetUri, iUri );
    //mmc id
    TBuf<KUidChars> mmcId;
    if( aEntry->FindAttribute( KCaAttrMmcId, mmcId ) )
    	{
        TLex mmcLex( mmcId );
        User::LeaveIfError( mmcLex.Val( iMmcId, EHex ));
    	}
    if( aEntry->GetFlags() &  EMissing )
        {
        iFlags = iFlags | EMissing;
        }
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetDescription* CCaWidgetDescription::NewL( )
    {
    CCaWidgetDescription* self = NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaWidgetDescription::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetDescription* CCaWidgetDescription::NewLC( )
    {
    CCaWidgetDescription* self = new( ELeave ) CCaWidgetDescription();
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaWidgetDescription::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetDescription* CCaWidgetDescription::NewLC( CCaInnerEntry* aEntry )
    {
    CCaWidgetDescription* self = new( ELeave ) CCaWidgetDescription();
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCaWidgetDescription::~CCaWidgetDescription()
    {
    iTitle.Close();
    iDescription.Close();
    iUri.Close();
    iIconUri.Close();
    iLibrary.Close();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCaWidgetDescription::Compare(
        const CCaWidgetDescription& aFirst,const CCaWidgetDescription& aSecond)
    {
    if( aFirst.GetLibraryName() == aSecond.GetLibraryName() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCaWidgetDescription::Compare( const CCaWidgetDescription& aToCompare )
    {
    if( aToCompare.GetLibrary() == GetLibrary() &&
            aToCompare.GetDescription() == GetDescription() &&
            aToCompare.GetUri() == GetUri() &&
            aToCompare.GetIconUri() == GetIconUri() &&
            aToCompare.GetTitle() == GetTitle() &&
            aToCompare.GetLibrary() != KNoLibrary )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetMmcId( TUint aMmcId )
    {
    iMmcId = aMmcId;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetPackageUidL( const TDesC& aPackageUid )
    {
    TLex lexer( aPackageUid );
    User::LeaveIfError( lexer.Val( iPackageUid,EHex));
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetTitleL( const TDesC& aTitle )
    {
    iTitle.Close();
    iTitle.CreateL(aTitle);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetDescriptionL( const TDesC& aDescription )
    {
    iDescription.Close();
    iDescription.CreateL(aDescription);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetUriL( const TDesC& aUri )
    {
    iUri.Close();
    iUri.CreateL(aUri);
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetIconUriL( const TDesC& aIconUri )
    {
    iIconUri.Close();
    iIconUri.CreateL(aIconUri);
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetLibraryL( const TDesC& aLibrary )
    {
    iLibrary.Close();
    iLibrary.CreateL(aLibrary);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetMissing( TBool aMissing )
    {
    SetFlag( EMissing, aMissing );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetVisible( TBool aVisible )
    {
    SetFlag( EVisible, aVisible );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetFlag( TInt aFlag, TBool aValue )
    {
    if( aValue )
        {
        iFlags = iFlags | aFlag;
        }
    else
        {
        iFlags = iFlags & ~aFlag;
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CCaWidgetDescription::GetEntryId( ) const
    {
    return iEntryId;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint CCaWidgetDescription::GetMmcId( ) const
    {
    return iMmcId;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetLibrary( ) const
    {
    return iLibrary;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetDescription( ) const
    {
    return iDescription;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetUri( ) const
    {
    return iUri;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetIconUri( ) const
    {
    return iIconUri;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetTitle( ) const
    {
    if ( iTitle == KNullDesC )
    	{
    	return GetLibraryName();
        }
    else
    	{
    	return iTitle;
    	}
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CCaWidgetDescription::IsMissing( ) const
    {
    return iFlags & EMissing;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CCaInnerEntry* CCaWidgetDescription::GetEntryLC( ) const
    {
    CCaInnerEntry* entry = CCaInnerEntry::NewLC();
    entry->SetEntryTypeNameL( KCaTypeWidget );
    entry->SetRole( EItemEntryRole );
    if ( iFlags & EVisible )
        {
        entry->SetFlags( EVisible );
        }

    if ( iPackageUid )
    	{
    	TBuf<KMaxUidName> uidDesc;
    	uidDesc.AppendNum( iPackageUid,EHex );
    	entry->AddAttributeL( KCaPackageUid, uidDesc );
    	}

    if ( iEntryId != KNoId )
        {
        entry->SetId( iEntryId );
        }
    if ( iMmcId )
        {
        TBuf<KMaxUidName> mmcId;
        mmcId.AppendNum( iMmcId, EHex);
        entry->AddAttributeL( KCaAttrMmcId, mmcId );
        }
    if ( iLibrary != KNullDesC )
        {
        entry->AddAttributeL(KAttrWidgetLibrary, iLibrary);
        entry->SetFlags(entry->GetFlags() | ERemovable);
        if (WidgetScannerUtils::IsFileOnDrive( iLibrary,
                DriveInfo::EDefaultRom ) )
            {
            entry->SetFlags( entry->GetFlags() & ~ERemovable );
            }
        }

    if ( iTitle != KNullDesC )
        {
        entry->SetTextL(iTitle);
        entry->AddAttributeL(KCaAttrLongName, iTitle);
        }
    else
        {
        TPtrC libraryName( GetLibraryName() );
        entry->SetTextL( libraryName );
        entry->AddAttributeL( KCaAttrLongName, libraryName );
        }

    if ( iDescription != KNullDesC )
        {
        entry->SetDescriptionL( iDescription );
        }
    if ( iUri != KNullDesC )
        {
        entry->AddAttributeL(KAttrWidgetUri, iUri);
        }
    if ( iIconUri != KNullDesC)
        {
        entry->SetIconDataL(0, 0, 0, 0, iIconUri);
        }
    return entry;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetLibraryName( ) const
    {
    TPtrC libraryName( iLibrary );
    TInt pos( 0 );
    while ( ( pos = libraryName.Find( KDoubleSlash ) ) != KErrNotFound )
        {
        libraryName.Set( libraryName.Mid( pos + 1 ) );
        }
    pos = libraryName.Find( KDllExt );
    if (pos == (libraryName.Length() - KDllExt().Length()))
        {
        libraryName.Set( libraryName.Mid( 0, pos ));
        }
    return libraryName;
    }
//  End of File
