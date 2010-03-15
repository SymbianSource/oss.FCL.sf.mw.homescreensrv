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
// ChspsODT::CopyODTDataL()
// Helper to ODT cloning. Prevents duplicate code in two clone methods.
// -----------------------------------------------------------------------------
//
void ChspsODT::CopyODTDataL( const ChspsODT& aSource, ChspsODT& aTarget )
    {
    // Properties.
    aTarget.SetFamily(              aSource.Family() );
    aTarget.SetConfigurationType(   aSource.ConfigurationType() );
    aTarget.SetRootUid(             aSource.RootUid() );
    aTarget.SetProviderUid(         aSource.ProviderUid() );
    aTarget.SetThemeUid(            aSource.ThemeUid() );
    aTarget.SetProviderNameL(       aSource.ProviderName() );
    aTarget.SetThemeFullNameL(      aSource.ThemeFullName() );
    aTarget.SetThemeShortNameL(     aSource.ThemeShortName() );
    aTarget.SetThemeVersionL(       aSource.ThemeVersion() );
    aTarget.SetPackageVersionL(     aSource.PackageVersion() );
    aTarget.SetDescriptionL(        aSource.Description() );
    aTarget.SetLogoFileL(           aSource.LogoFile() );
    aTarget.SetPreviewFileL(        aSource.PreviewFile() );
    aTarget.SetMultiInstance(       aSource.MultiInstance() );
    aTarget.SetOdtLanguage(         aSource.OdtLanguage() );
    aTarget.SetFlags(               aSource.Flags() );
    
    // Resources.
    aTarget.DeleteAllResources();
    TInt resourceCount = aSource.ResourceCount();    
    for ( TInt index = 0; index < resourceCount ; index++ )
        {
        ChspsResource* resource = ( aSource.ResourceL( index ) ).CloneL();
        CleanupStack::PushL( resource );
        aTarget.AddResourceL( resource );
        CleanupStack::Pop( resource );
        resource = NULL;    
        }

    // DOM tree.
    aTarget.CopyDomDocumentL( aSource.DomDocument() );    
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
    HBufC8* buf = HBufC8::NewLC( HeaderSize() );
    TPtr8 ptr = buf->Des();
    RDesWriteStream stream( ptr );
    stream.PushL();
    ExternalizeHeaderL( stream );
    stream.Pop();
    stream.Close();
    CleanupStack::Pop( buf );
    
    return buf;
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
    const TDesC& packageVersion = PackageVersion();
    aStream.WriteInt32L( packageVersion.Length() );
    if( packageVersion.Length() > 0 )
        {
        aStream << packageVersion;
        }
        
    aStream.WriteUint32L( iThemeUid );
    
    const TDesC& providerName = ProviderName();
    aStream.WriteInt32L( providerName.Length() );
    if( providerName.Length() > 0 )
        {
        aStream << providerName;
        }         

    const TDesC& themeFullName = ThemeFullName();
    aStream.WriteInt32L( themeFullName.Length() );
    if( themeFullName.Length() > 0 )
        {
        aStream << themeFullName;
        }      
    
    const TDesC& themeShortName = ThemeShortName();
    aStream.WriteInt32L( themeShortName.Length() );
    if( themeShortName.Length() > 0 )
        {
        aStream << themeShortName;
        }    

    const TDesC& themeVersion = ThemeVersion();
    aStream.WriteInt32L( themeVersion.Length() );
    if( themeVersion.Length() > 0 )
        {
        aStream << themeVersion;
        }              
        
    const TDesC& description = Description();
    aStream.WriteInt32L( description.Length() );
    if( description.Length() > 0 )
        {
        aStream << description;
        }            

    const TDesC& logoFile = LogoFile();
    aStream.WriteInt32L( logoFile.Length() );
    if( logoFile.Length() > 0 )
        {
        aStream << logoFile;
        }        

    const TDesC& previewFile = PreviewFile();
    aStream.WriteInt32L( previewFile.Length() );
    if( previewFile.Length() > 0 )
        {
        aStream << previewFile;
        }            

    aStream.WriteUint32L( iFamilyMask );    
    aStream.WriteUint32L( iConfigurationType );    
    aStream.WriteUint32L( iRootUid );    
    aStream.WriteUint32L( iProviderUid );  
    aStream.WriteUint32L( iFlags );
    
    aStream.WriteInt32L( iLanguage );    
    aStream.WriteInt32L( iMultiInstance );    
    }


// -----------------------------------------------------------------------------
// ChspsODT::HeaderSize
// Calculate header size in bytes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
TInt ChspsODT::HeaderSize() const
    {
    TInt size = sizeof( TInt32 );
    if( PackageVersion().Length() > 0 )
        {         
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += PackageVersion().Size();
        }
    
    size += sizeof( TUint32 ); // iThemeUid
    
    size += sizeof( TInt32 );
    if( ProviderName().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += ProviderName().Size();
        }    
    
    size += sizeof( TInt32 );
    if( ThemeFullName().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += ThemeFullName().Size();
        }    
    
    size += sizeof( TInt32 );
    if( ThemeShortName().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += ThemeShortName().Size();
        }    
    
    size += sizeof( TInt32 );
    if( ThemeVersion().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += ThemeVersion().Size();
        }    
    
    size += sizeof( TInt32 );
    if( Description().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += Description().Size();
        }    
    
    size += sizeof( TInt32 );
    if( LogoFile().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += LogoFile().Size();
        }    
    
    size += sizeof( TInt32 );
    if( PreviewFile().Length() > 0 )
        {
        size += sizeof( TUint32 ); // String streaming insert also max. 32bit member of TCardinality.
        size += PreviewFile().Size();
        }    
    
    size += sizeof( TUint32 ) * 5; // iFamilyMask, iConfigurationType,
                                   // iRootUid, iProviderUid, iFlags

    size += sizeof( TInt32 ) * 2; // iLanguage, iMultiInstance                
    
    return size;
    }

// -----------------------------------------------------------------------------
// ChspsODT::InternalizeHeaderL
// Internalizes the ODT header
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
EXPORT_C void ChspsODT::InternalizeHeaderL( RReadStream& aStream )
    {    
    TInt len = aStream.ReadInt32L();
    HBufC* version = NULL;
    if( len > 0 )
        {
        version = HBufC::NewL( aStream, len );
        }
    CleanupStack::PushL( version );    
    
    // Check version request.
    if( iPackageVersion )
        {
        TBool supported = ETrue;
        if( version == NULL )
            {
            supported = EFalse;
            }
        else if( version &&
                 version->Compare( *iPackageVersion ) != 0 )
            {
            supported = EFalse;
            }    
    
        if( !supported )
            {
            // Package version check requested (iPackageVersion defined) 
            // and package version not supported
            User::Leave( KErrNotSupported );
            }
        }
    
    if( version )
        {
        delete iPackageVersion;
        iPackageVersion = NULL;    
        iPackageVersion = version->AllocL();
        }
    CleanupStack::PopAndDestroy( version );

    iThemeUid = aStream.ReadUint32L();
          
    delete iProviderName;
    iProviderName = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {
        iProviderName = HBufC::NewL( aStream, len );
        }
    
    delete iThemeFullName;
    iThemeFullName = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {
        iThemeFullName = HBufC::NewL( aStream, len );
        }
    
    delete iThemeShortName;
    iThemeShortName = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {    
        iThemeShortName = HBufC::NewL( aStream, len );
        }

    delete iThemeVersion;
    iThemeVersion = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {    
        iThemeVersion = HBufC::NewL( aStream, len );
        }

    delete iDescription;
    iDescription = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {    
        iDescription = HBufC::NewL( aStream, len );
        }

    delete iLogoFile;
    iLogoFile = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {    
        iLogoFile = HBufC::NewL( aStream, len );
        }

    delete iPreviewFile;
    iPreviewFile = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {    
        iPreviewFile = HBufC::NewL( aStream, len );
        }
    
    iFamilyMask = aStream.ReadUint32L();     
    iConfigurationType = aStream.ReadUint32L();    
    iRootUid = aStream.ReadUint32L();    
    iProviderUid = aStream.ReadUint32L();    
    iFlags = aStream.ReadUint32L();
    
    iLanguage = aStream.ReadInt32L();
    iMultiInstance = aStream.ReadInt32L();        
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
// ChspsODT::DeleteResourceListL
// Deletes all resources from the ODT.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsODT::DeleteAllResources()
    {
    iResourceList->ResetAndDestroy();
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
    if( iProviderName )
        {
        delete iProviderName;
        iProviderName = NULL;
        }
    
    if( aName.Length() != 0 )
        {        
        iProviderName = aName.AllocL();
        }
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
    if( iThemeFullName )
        {
        delete iThemeFullName;
        iThemeFullName = NULL;
        }
    
    if( aName.Length() != 0 )
        {        
        iThemeFullName = aName.AllocL();
        }
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
    if( iThemeShortName )
        {
        delete iThemeShortName;
        iThemeShortName = NULL;
        }
    
    if( aName.Length() != 0 )
        {        
        iThemeShortName = aName.AllocL();
        }    
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
    if( iThemeVersion )
        {
        delete iThemeVersion;
        iThemeVersion = NULL;
        }
    
    if( aVersion.Length() != 0 )
        {        
        iThemeVersion = aVersion.AllocL();
        }    
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
EXPORT_C ChspsODT* ChspsODT::CloneL() const
    {
    ChspsODT* clone = ChspsODT::NewL();
    CleanupStack::PushL( clone );    
    ChspsODT::CopyODTDataL( *this, *clone );
    CleanupStack::Pop( clone );            
    return clone;
    }
 
// -----------------------------------------------------------------------------
// Copies data from an exisiting ODT
// -----------------------------------------------------------------------------
EXPORT_C void ChspsODT::CloneL( ChspsODT& aODT )
    {
    ChspsODT::CopyODTDataL( aODT, *this );    
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
    if( iPackageVersion )
        {
        delete iPackageVersion;
        iPackageVersion = NULL;
        }
    
    if( aVersion.Length() != 0 )
        {        
        iPackageVersion = aVersion.AllocL();
        }
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
    if( iDescription )
        {
        delete iDescription;
        iDescription = NULL;
        }
    
    if( aDesc.Length() != 0 )
        {        
        iDescription = aDesc.AllocL();
        }
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
    if( iLogoFile )
        {
        delete iLogoFile;
        iLogoFile = NULL;
        }
    
    if( aPath.Length() != 0 )
        {        
        iLogoFile = aPath.AllocL();
        }
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
    if( iPreviewFile )
        {
        delete iPreviewFile;
        iPreviewFile = NULL;
        }
    
    if( aPath.Length() != 0 )
        {        
        iPreviewFile = aPath.AllocL();
        }
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
