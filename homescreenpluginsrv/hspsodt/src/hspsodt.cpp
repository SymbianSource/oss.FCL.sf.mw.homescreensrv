/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class represents an Object Description Tree of Xuikon.
*
*/


#include "hspsodt.h"
#include <s32strm.h>
#include <s32mem.h>
#include "hspsthememanagement.h"
#include "hspsdomdocument.h"
#include "hspsresource.h"

/* Literal delim is used in separation of theme header and and other data in ODT-streaming. */
_LIT(KDelim, "#");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsODT::ChspsODT
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsODT::ChspsODT()
    {
    }

// -----------------------------------------------------------------------------
// ChspsODT::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsODT::ConstructL()
    {
    iDomDocument = ChspsDomDocument::NewL();
    iResourceList = new( ELeave ) CArrayPtrSeg<ChspsResource>( KPathListGranularity ); 
    }

// -----------------------------------------------------------------------------
// ChspsODT::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsODT* ChspsODT::NewL()
    {
    ChspsODT* self = new( ELeave ) ChspsODT;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsODT::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsODT* ChspsODT::NewLC( const TDesC8& aStreamData )
    {
    ChspsODT* data = ChspsODT::NewL();
    CleanupStack::PushL( data );
    RDesReadStream stream( aStreamData );
    CleanupClosePushL( stream );
    data->InternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    return data;
    }
    
// Destructor
ChspsODT::~ChspsODT()
    {
    delete iDescription;
    delete iLogoFile;
    delete iPreviewFile;
    delete iProviderName;
    delete iThemeFullName;
    delete iThemeShortName;
    delete iThemeVersion;
    delete iPackageVersion;
    // clean up the array
    if( iResourceList )
        {
        iResourceList->ResetAndDestroy();
        delete iResourceList;
        }
    delete iDomDocument;
    }


// -----------------------------------------------------------------------------
// ChspsODT::MarshalHeaderL
// Marshals the ODT header into descriptor
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C HBufC8* ChspsODT::MarshalHeaderL() const
    {
    CBufFlat* buf = CBufFlat::NewL( KMaxHeaderDataLength8 );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );     //stream over the buffer
    CleanupClosePushL( stream );
    ExternalizeHeaderL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    //Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL( buf->Size() );
    TPtr8 ptr( des->Des() );
    buf->Read( 0, ptr, buf->Size() );
    CleanupStack::PopAndDestroy( buf );
    
    return des;
    }

// -----------------------------------------------------------------------------
// ChspsODT::UnMarshalHeaderLC
// Unmarshals the ODT header from descriptor stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C ChspsODT* ChspsODT::UnMarshalHeaderLC( const TDesC8& aStreamData )
    {
    ChspsODT* data = ChspsODT::NewL();
    CleanupStack::PushL( data );
    RDesReadStream stream( aStreamData );
    CleanupClosePushL( stream );
    data->InternalizeHeaderL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    return data;
    }

// -----------------------------------------------------------------------------
// ChspsODT::UnMarshalHeaderL
// Unmarshals the ODT header from descriptor stream
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::UnMarshalHeaderL( const TDesC8& aStreamData )
    {

    RDesReadStream stream( aStreamData );
    CleanupClosePushL( stream );
    InternalizeHeaderL( stream );
    CleanupStack::PopAndDestroy( &stream );
    }

// -----------------------------------------------------------------------------
// ChspsODT::ExternalizeL
// Externalizes the ODT
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::ExternalizeL( RWriteStream& aStream ) const
    {
    ExternalizeHeaderL( aStream );
    ExternalizeResourceListL( aStream );   
    aStream << *iDomDocument;    
    }

// -----------------------------------------------------------------------------
// ChspsODT::InternalizeL
// Internalizes the ODT
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
EXPORT_C void ChspsODT::InternalizeL( RReadStream& aStream )
    {
    InternalizeHeaderL( aStream );
    // consumes header delimiter
    aStream.ReadInt16L();
    InternalizeResourceListL( aStream );    
    delete iDomDocument;
    iDomDocument = NULL;
    iDomDocument = ChspsDomDocument::NewL( aStream );
    }

// -----------------------------------------------------------------------------
// ChspsODT::ExternalizeHeaderL
// Externalizes the ODT header
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
void ChspsODT::ExternalizeHeaderL( RWriteStream& aStream ) const
    {
    if ( iPackageVersion )
        {
        aStream << *iPackageVersion;
        }
    else
        {
        aStream << KNullDesC;
        }
    
    aStream.WriteUint32L( iFamilyMask );
    aStream.WriteUint32L( iConfigurationType );
    aStream.WriteUint32L( iRootUid );
    aStream.WriteUint32L( iProviderUid );
    aStream.WriteUint32L( iThemeUid );
    aStream.WriteInt32L( iMultiInstance );
    if ( iDescription )
        {
        aStream << *iDescription;
        }    
    else
        {
        aStream << KNullDesC;
        }
    if ( iLogoFile )
        {
        aStream << *iLogoFile;
        }    
    else
        {
        aStream << KNullDesC;
        }
    if ( iPreviewFile )
        {
        aStream << *iPreviewFile;
        }    
    else
        {
        aStream << KNullDesC;
        }
    if ( iProviderName )
        {
        aStream << *iProviderName;
        }
    else
        {
        aStream << KNullDesC;
        }
    
    if ( iThemeFullName )
        {
        aStream << *iThemeFullName;
        }
    else 
        {
        aStream << KNullDesC;
        }
        
    if ( iThemeShortName )
        {
        aStream << *iThemeShortName;
        }
    else 
        {
        aStream << KNullDesC;
        }
        
    if ( iThemeVersion )
        {
        aStream << *iThemeVersion;
        }
    else
        {
        aStream << KNullDesC;
        }
    aStream.WriteInt32L( iLanguage );
    aStream.WriteUint32L( iFlags );
    // end of the header delimiter
    aStream.WriteL( KDelim );
    }

// -----------------------------------------------------------------------------
// ChspsODT::InternalizeHeaderL
// Internalizes the ODT header
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
EXPORT_C void ChspsODT::InternalizeHeaderL( RReadStream& aStream )
    {
    HBufC* version = HBufC::NewL( aStream, KMaxFileName );
    CleanupStack::PushL( version );
    if ( iPackageVersion && version->Des().Compare( iPackageVersion->Des() ) != 0 )
        {
        // Package version check requested (iPackageVersion defined) 
        // and package version not supported
        User::Leave( KErrNotSupported );
        }
    if ( !iPackageVersion && version->Length() )
        {
        // Package version check not requested
        iPackageVersion = version->AllocL();
        }
    CleanupStack::PopAndDestroy( version );
    
    iFamilyMask = aStream.ReadUint32L();    
    iConfigurationType = aStream.ReadUint32L();
    iRootUid = aStream.ReadUint32L();
    iProviderUid = aStream.ReadUint32L();
    iThemeUid = aStream.ReadUint32L();
    iMultiInstance = aStream.ReadInt32L();
    
    delete iDescription;
    iDescription = NULL;
    iDescription = HBufC::NewL(aStream, KMaxDescLength );
    
    delete iLogoFile;
    iLogoFile = NULL;
    iLogoFile = HBufC::NewL(aStream, KMaxFileName );
    
    delete iPreviewFile;
    iPreviewFile = NULL;
    iPreviewFile = HBufC::NewL(aStream, KMaxFileName );
        
    delete iProviderName;
    iProviderName = NULL;
    iProviderName = HBufC::NewL(aStream, KMaxFileName );
    
    delete iThemeFullName;
    iThemeFullName = NULL;
    iThemeFullName = HBufC::NewL(aStream, KMaxFileName );
    
    delete iThemeShortName;
    iThemeShortName = NULL;
    iThemeShortName = HBufC::NewL(aStream, KMaxFileName );
    
    delete iThemeVersion;
    iThemeVersion = NULL;
    iThemeVersion = HBufC::NewL(aStream, KMaxFileName );
        
    iLanguage = aStream.ReadInt32L();
    iFlags = aStream.ReadUint32L();
    }

// -----------------------------------------------------------------------------
// ChspsODT::AddResourceL
// Adds a resource into Resource List. Takes ownership.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::AddResourceL( ChspsResource* aResource )
    {
    const TInt resourceIndex = CheckResourceL( aResource );   
    if ( resourceIndex >= 0 ) 
    	{
        ChspsResource* resource = iResourceList->At( resourceIndex );
        delete resource;
        resource = NULL;
        iResourceList->Delete( resourceIndex );
    	}
    iResourceList->AppendL( aResource );
    }

// -----------------------------------------------------------------------------
// ChspsODT::CheckResourceL
// Check for duplicates
// -----------------------------------------------------------------------------
TInt ChspsODT::CheckResourceL( ChspsResource* aResource )
	{
	if ( !aResource )
		{
		User::Leave( KErrArgument );
		}
	
	for ( TInt index=0; index < ResourceCount(); index++ )
		{
		ChspsResource& r = ResourceL(index);
		if ( r.ResourceId() == aResource->ResourceId() 
				&& r.FileName() == aResource->FileName() )
			{
			return index;
			}
		}
	
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// ChspsODT::DeleteResourceL
// Deletes resource from the element array
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::DeleteResourceL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= ResourceCount() )
        {
        User::Leave( KErrArgument );
        }
        
    ChspsResource* resource = iResourceList->At( aIndex );    
    if ( resource )
        {
        delete resource;
        resource = NULL;
        iResourceList->Delete( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::ResourceL
// Get the resource by the index
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C ChspsResource& ChspsODT::ResourceL( TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= ResourceCount() )
        {
        User::Leave( KErrArgument );
        }
        
    return ( *iResourceList->At(aIndex));
    }

// -----------------------------------------------------------------------------
// ChspsODT::ElementCount
// Returns the amount of elements
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsODT::ResourceCount() const
    {
    return iResourceList->Count();
    }

// -----------------------------------------------------------------------------
// ChspsODT::ExternalizeResourceListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::ExternalizeResourceListL( RWriteStream& aStream ) const
    {
    // Stream out the resource list
	TInt count = iResourceList->Count();
	aStream.WriteInt32L(count);
	ChspsResource* resource;
	for (TInt i=0;i<count;i++)
	    {
	    resource = iResourceList->At(i);
	    resource->ExternalizeL(aStream);
	    }
    }

// -----------------------------------------------------------------------------
// ChspsODT::InternalizeResourceListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::InternalizeResourceListL( RReadStream& aStream )
    {
    // clean up the array
    if( iResourceList )
        {
        iResourceList->ResetAndDestroy();
        }
	// stream in the resource list
	TInt count = aStream.ReadInt32L();
	for (TInt i=0;i<count;i++)
		{
        ChspsResource* resource = ChspsResource::NewL();
        CleanupStack::PushL(resource);
        resource->InternalizeL(aStream);
       	if ( iResourceList )
       	    {
       	    iResourceList->AppendL( resource );
       	    }
        CleanupStack::Pop( resource ); // now owned by array
        resource = NULL;
		}
    }


// -----------------------------------------------------------------------------
// ChspsODT::SetRootUid
// Set RootUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetRootUid( TInt aUid )
    {
    iRootUid = aUid;
    }

// -----------------------------------------------------------------------------
// ChspsODT::RootUid
// Get RootUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsODT::RootUid() const
    {
    return iRootUid;        
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetProviderUid
// Set ProviderUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetProviderUid( TInt aUid )
    {
    iProviderUid = aUid;
    }

// -----------------------------------------------------------------------------
// ChspsODT::ProviderUid
// Get ProviderUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsODT::ProviderUid() const
    {
    return iProviderUid;    
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetThemeUid
// Set ThemeUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetThemeUid( TInt aUid )
    {
    iThemeUid = aUid;
    }

// -----------------------------------------------------------------------------
// ChspsODT::ThemeUid
// Get ThemeUid
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C  TInt ChspsODT::ThemeUid() const
    {
    return iThemeUid; 
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetProviderNameL
// Set ProviderNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetProviderNameL( const TDesC& aName )
    {
    delete iProviderName;
    iProviderName = NULL;
    iProviderName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::ProviderName
// Get ProviderName
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::ProviderName() const
    {
    if ( iProviderName )
        {
        return *iProviderName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetThemeFullNameL
// Set ThemeFullNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetThemeFullNameL( const TDesC& aName )
    {
    delete iThemeFullName;
    iThemeFullName = NULL;
    iThemeFullName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::ThemeFullName
// Get ThemeFullName
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::ThemeFullName() const
    {
    if ( iThemeFullName )
        {
        return *iThemeFullName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetThemeShortNameL
// Set ThemeShortNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::SetThemeShortNameL( const TDesC& aName )
    {
    delete iThemeShortName;
    iThemeShortName = NULL;
    iThemeShortName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::ThemeShortName
// Get ThemeShortName
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::ThemeShortName() const
    {
    if ( iThemeShortName )
        {
        return *iThemeShortName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetThemeVersionL
// Set ThemeVersionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::SetThemeVersionL( const TDesC& aVersion )
    {
    delete iThemeVersion;
    iThemeVersion = NULL;
    iThemeVersion = aVersion.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::ThemeVersion
// Get ThemeVersion
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& ChspsODT::ThemeVersion() const
    {
    if ( iThemeVersion )
        {
        return *iThemeVersion;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetOdtLanguage
// Set OdtLanguage
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::SetOdtLanguage( TInt aLanguage )
    {
    iLanguage = aLanguage;
    }

// -----------------------------------------------------------------------------
// ChspsODT::OdtLanguage
// Get OdtLanguage
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsODT::OdtLanguage() const
    {
    return iLanguage;        
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetFlags
// Set Flags
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetFlags( TUint aFlags )
    {
    iFlags = aFlags;
    }

// -----------------------------------------------------------------------------
// ChspsODT::Flags
// Get Flags
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TUint ChspsODT::Flags() const
    {
  return iFlags;
    }

// -----------------------------------------------------------------------------
// ChspsODT::DomDocument
// Get DomDocument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDocument& ChspsODT::DomDocument() const
    {
    return *iDomDocument;
    }
  
// -----------------------------------------------------------------------------
// ChspsODT::CloneL()
// Makes a clone of this ODT and returns pointer to it
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsODT* ChspsODT::CloneL()
    {
    ChspsODT* clone = new (ELeave) ChspsODT;
    CleanupStack::PushL( clone );
    clone->ConstructL();
    clone->SetConfigurationType( iConfigurationType );
    clone->SetRootUid( iRootUid );
    clone->SetProviderUid( iProviderUid );
    clone->SetThemeUid( iThemeUid );
    if( iProviderName )
        {
        clone->SetProviderNameL( *iProviderName );
        }
    if( iThemeFullName )
        {
        clone->SetThemeFullNameL( *iThemeFullName );
        }
    if( iThemeShortName )
        {
        clone->SetThemeShortNameL( *iThemeShortName );
        }
    if( iThemeVersion )
        {
        clone->SetThemeVersionL( *iThemeVersion );
        }
    if( iPackageVersion )
        {
        clone->SetPackageVersionL( *iPackageVersion );
        }
    if( iDescription )
        {
        clone->SetDescriptionL( *iDescription );
        }
    clone->SetOdtLanguage( iLanguage );
    clone->SetFlags( iFlags );
          
    TInt resourceCount = iResourceList->Count();

    for ( TInt index = 0; index < resourceCount ; index++ )
        {
        ChspsResource& resource = ResourceL( index );
        clone->AddResourceL( resource.CloneL() );
        }
       
    CleanupStack::Pop( clone );
    return clone;
    }
    
// -----------------------------------------------------------------------------
// ChspsODT::CopyDomDocumentL()
// Clones the aDom and sets it as this ChspsODT's DomDocument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsODT::CopyDomDocumentL( ChspsDomDocument& aDom )
    {
    delete iDomDocument;
    iDomDocument = NULL;
    iDomDocument = aDom.CloneL();
    }    

// -----------------------------------------------------------------------------
// ChspsODT::SetConfigurationType()
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsODT::SetConfigurationType( TUint aType )
	{
	iConfigurationType = aType;
	}
  
// -----------------------------------------------------------------------------
// ChspsODT::ConfigurationType()
// -----------------------------------------------------------------------------
//
EXPORT_C TUint ChspsODT::ConfigurationType() const
	{
	return iConfigurationType;
	}

// -----------------------------------------------------------------------------
// ChspsODT::SetPackageVersionL
// Set package version
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::SetPackageVersionL( const TDesC& aVersion )
    {
    delete iPackageVersion;
    iPackageVersion = NULL;
    iPackageVersion = aVersion.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::PackageVersion
// Get package version
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C const TDesC& ChspsODT::PackageVersion() const
    {
    if ( iPackageVersion )
        {
        return *iPackageVersion;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetFamily
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetFamily( const TUint32 aFamilyMask )
    {    
    iFamilyMask = aFamilyMask;
    }
                 
// -----------------------------------------------------------------------------
// ChspsODT::Family
// -----------------------------------------------------------------------------
EXPORT_C TUint32 ChspsODT::Family() const
    {    
    return iFamilyMask;    
    }   

// -----------------------------------------------------------------------------
// ChspsODT::SetMultiInstance
// Set MultiInstance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetMultiInstance( TInt aMultiInstance )
    {
    iMultiInstance = aMultiInstance;
    }

// -----------------------------------------------------------------------------
// ChspsODT::MultiInstance
// Get MultiInstance
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C  TInt ChspsODT::MultiInstance() const
    {
    return iMultiInstance; 
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetDescriptionL
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetDescriptionL( const TDesC& aDesc )
    {    
    delete iDescription;
    iDescription = NULL;
    iDescription = aDesc.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::Description
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::Description() const
    {
    if ( iDescription )
        {
        return *iDescription;
        }
    else
        {
        return KNullDesC;
        } 
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetLogoFileL
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetLogoFileL( const TDesC& aPath )
    {    
    delete iLogoFile;
    iLogoFile = NULL;
    iLogoFile = aPath.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::LogoFile
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::LogoFile() const
    {
    if ( iLogoFile )
        {
        return *iLogoFile;
        }
    else
        {
        return KNullDesC;
        } 
    }

// -----------------------------------------------------------------------------
// ChspsODT::SetPreviewFileL
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::SetPreviewFileL( const TDesC& aPath )
    {    
    delete iPreviewFile;
    iPreviewFile = NULL;
    iPreviewFile = aPath.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsODT::PreviewFile
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsODT::PreviewFile() const
    {
    if ( iPreviewFile )
        {
        return *iPreviewFile;
        }
    else
        {
        return KNullDesC;
        } 
    }

//  End of File  
