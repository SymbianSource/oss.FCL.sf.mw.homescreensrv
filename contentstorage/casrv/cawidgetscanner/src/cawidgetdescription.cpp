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
    iEntryId(KNoId), iMmcId()
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
    iDescription.CreateL( aEntry->GetDescription() );
    //package uid
    TBuf<KMaxUidName> uidDesc;
    aEntry->FindAttribute( KCaPackageUid, uidDesc );
    if ( uidDesc.Length() )
        {
        TLex uidLex(uidDesc);
        User::LeaveIfError( uidLex.Val( iPackageUid, EHex ));
        }
    iTitle.CreateL( aEntry->GetText() );
    iIconUri.CreateL( aEntry->Icon()->FileName() );
    //library
    iLibrary.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetLibrary, iLibrary );
    //path
    iPath.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetPath, iPath );
    //uri
    iUri.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetUri, iUri );
    //mmc id
    iMmcId.CreateL(KMassStorageIdLength);
    aEntry->FindAttribute( KCaAttrMmcId, iMmcId );
    //service xml
    iServiceXml.CreateL( KCaMaxAttrValueLen );
    aEntry->FindAttribute( KAttrWidgetServiceXml,iServiceXml );
    
    iFlags = aEntry->GetFlags();
    iModificationTime.CreateL(KCaMaxAttrValueLen);
    aEntry->FindAttribute( KCaAttrInstallationTime, iModificationTime );
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
    iPath.Close();
    iModificationTime.Close();
    iMmcId.Close();
    iServiceXml.Close();
    iManifestFilePathName.Close();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCaWidgetDescription::Compare(
        const CCaWidgetDescription& aFirst,const CCaWidgetDescription& aSecond)
    {
    if( aFirst.GetUri() == aSecond.GetUri() )
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
            aToCompare.GetLibrary() != KNoLibrary &&
            aToCompare.GetModificationTime() == GetModificationTime() &&
            aToCompare.GetServiceXml() == GetServiceXml()
            )
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
void CCaWidgetDescription::SetMmcIdL( const TDesC& aMmcId )
    {
    iMmcId.Close();
    iMmcId.CreateL( aMmcId );
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
void CCaWidgetDescription::SetUsed( TBool aUsed )
    {
    SetFlag( EUsed, aUsed );
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
void CCaWidgetDescription::SetModificationTimeL( const TDesC& aModificationTime )
    {
    iModificationTime.Close();
    iModificationTime.CreateL( aModificationTime );
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
TPtrC CCaWidgetDescription::GetMmcId( ) const
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
TPtrC CCaWidgetDescription::GetPath( ) const
    {
    return iPath;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetPathL( const TDesC& aPath )
    {
	iPath.Close();
	iPath.CreateL(aPath);
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
TPtrC CCaWidgetDescription::GetModificationTime( ) const
    {
    return iModificationTime;
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
TBool CCaWidgetDescription::IsUsed( ) const
    {
    return iFlags & EUsed;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CCaWidgetDescription::IsVisible( ) const
    {
    return iFlags & EVisible;
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
    entry->SetFlags( iFlags );
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
    if ( iMmcId != KNullDesC )
        {
        entry->AddAttributeL( KCaAttrMmcId, iMmcId );
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
    if ( iPath != KNullDesC )
        {
        entry->AddAttributeL(KAttrWidgetPath, iPath);
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
        // aSkinId and AppId not used for widgets - KNullDesC
        entry->SetIconDataL(iIconUri, KNullDesC, KNullDesC);
        }

    if( iModificationTime != KNullDesC )
        {
        entry->AddAttributeL( KCaAttrInstallationTime, iModificationTime );
        }
    if ( iServiceXml != KNullDesC )
        {
        entry->AddAttributeL( KAttrWidgetServiceXml, iServiceXml);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::RemoveMmcId( )
    {
    iMmcId.Close();
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetDescription::SetServiceXmlL(const TDesC& aServiceXml)
    {
    iServiceXml.Close();
    iServiceXml.CreateL( aServiceXml );
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TPtrC CCaWidgetDescription::GetServiceXml() const
    {
    return iServiceXml;
    }

/*
 * Set manifest file path name
 * @param aManifestFilePt
 */
void CCaWidgetDescription::SetManifestFilePathNameL( 
    const TDesC& aManifestFilePathName )
    {
    iManifestFilePathName.Close();
    iManifestFilePathName.CreateL( aManifestFilePathName );
    }

/*
 * Get service fw xml filename
 * @return Service xml filename
 */
TPtrC CCaWidgetDescription::GetManifestFilePathName() const
    {
    return iManifestFilePathName;
    }
//  End of File
