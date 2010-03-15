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
* Description:  Definition of a file resource
*
*/


#include "hspsresource.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsResource::ChspsResource()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsResource::ChspsResource()
    {
    iLanguage = ELangTest;
    }

// -----------------------------------------------------------------------------
// ChspsResource::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsResource::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// ChspsResource::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsResource* ChspsResource::NewL()
    {
    ChspsResource* self = new( ELeave ) ChspsResource;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
   
// -----------------------------------------------------------------------------
// ChspsResource::CloneL()
// Returns an exact copy of this ChspsResource object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsResource* ChspsResource::CloneL()
    {
    ChspsResource* clone = ChspsResource::NewL();
    
    CleanupStack::PushL( clone );
    
    clone->SetLockingPolicy( iLockingPolicy );    
    clone->SetResourceType( iResourceType );
    clone->SetLanguage( iLanguage );
    clone->SetResourceIdL( ResourceId() );    
    clone->SetFileNameL( FileName() );
    clone->SetMimeTypeL( iMimeType );
    clone->SetConfigurationUid( iConfigurationUid );
    clone->SetTagsL( Tags() );
    
    CleanupStack::Pop( clone );
    
    return clone;
    }

// Destructor
ChspsResource::~ChspsResource()
    {    
    delete iResourceID;    
    delete iFileName;
    delete iTags;
    }


// -----------------------------------------------------------------------------
// ChspsResource::ExternalizeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iLockingPolicy );    
    aStream.WriteUint32L( iResourceType );
    aStream.WriteUint32L( iLanguage );

    const TDesC& resourceId = ResourceId();
    aStream.WriteInt32L( resourceId.Length() );
    if ( resourceId.Length() > 0 )
        {
        aStream << resourceId;
        }
          
    const TDesC& fileName = FileName(); 
    aStream.WriteInt32L( fileName.Length() );
    if ( fileName.Length() > 0 )
        {
        aStream << fileName;
        }    
     
    iMimeType.ExternalizeL(aStream);    
    
    aStream.WriteUint32L( iConfigurationUid );
    
    const TDesC& tags = Tags();
    aStream.WriteInt32L( tags.Length() );
    if ( tags.Length() > 0 )
        {
        aStream << tags;
        }
    }
       
// -----------------------------------------------------------------------------
// ChspsResource::InternalizeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::InternalizeL( RReadStream& aStream )
    {
    iLockingPolicy = (ThspsLockingPolicy)aStream.ReadUint32L();    
    iResourceType = (ThspsResourceType)aStream.ReadUint32L();
    iLanguage = (TLanguage)aStream.ReadUint32L();    

    delete iResourceID;
    iResourceID = NULL;    
    TInt len = aStream.ReadInt32L();
    if( len > 0 )
        {
        iResourceID = HBufC::NewL( aStream, len );
        }
     
    delete iFileName;
    iFileName = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {
        iFileName = HBufC::NewL( aStream, len );
        }
    
    iMimeType.InternalizeL(aStream);          
    
    iConfigurationUid = (TInt)aStream.ReadUint32L();
    
    delete iTags;
    iTags = NULL;
    len = aStream.ReadInt32L();
    if( len > 0 )
        {
        iTags = HBufC::NewL( aStream, len );
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::SetLockingPolicy().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetLockingPolicy( ThspsLockingPolicy aLockingPolicy )
    {
    iLockingPolicy = aLockingPolicy;
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::LockingPolicy().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsLockingPolicy ChspsResource::LockingPolicy() const
    {
    return iLockingPolicy;        
    }

// -----------------------------------------------------------------------------
// ChspsResource::SetResourceType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetResourceType( ThspsResourceType aResourceType )
    {
    iResourceType = aResourceType;
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::ResourceType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsResourceType ChspsResource::ResourceType() const
    {
    return iResourceType;        
    }


// -----------------------------------------------------------------------------
// ChspsResource::SetResourceIdL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetResourceIdL( const TDesC& aResourceId )
    {
    if( iResourceID )
        {
        delete iResourceID;
        iResourceID = NULL;
        }
    
    if( aResourceId.Length() > 0 )
        {
        iResourceID = aResourceId.AllocL();
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::ResourceId().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& ChspsResource::ResourceId() const
    {
    if ( iResourceID )
        {
        return *iResourceID;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::SetFileNameL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetFileNameL( const TDesC& aFileName )
    {
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    
    if( aFileName.Length() > 0 )
        {
        iFileName = aFileName.AllocL();
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::FileName().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& ChspsResource::FileName() const
    {
    if ( iFileName )
        {
        return *iFileName;
        }
    else
        {
        return KNullDesC;
        }
    }    
    
// -----------------------------------------------------------------------------
// ChspsResource::SetMimeTypeL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetMimeTypeL( const TDataType aDataType )
    {
    iMimeType = aDataType;
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::MimeType().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TDataType ChspsResource::MimeType() const
    {
    return iMimeType;
    }    
    
// -----------------------------------------------------------------------------
// ChspsResource::SetLanguage().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetLanguage( const TLanguage& aLanguage )
	{
	iLanguage = aLanguage;
	}

// -----------------------------------------------------------------------------
// ChspsResource::Language().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//       
EXPORT_C const TLanguage& ChspsResource::Language() const
	{
	return iLanguage;
	}

// -----------------------------------------------------------------------------
// ChspsResource::SetConfigurationUid().
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetConfigurationUid( const TInt aConfUid )
    {
    iConfigurationUid = aConfUid;
    }

// -----------------------------------------------------------------------------
// ChspsResource::ConfigurationUid().
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsResource::ConfigurationUid() const
    {
    return iConfigurationUid;
    }

// -----------------------------------------------------------------------------
// ChspsResource::SetTagsL().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResource::SetTagsL( const TDesC& aTag )
    {
    if( iTags )
        {
        delete iTags;
        iTags = NULL;
        }
    
    if( aTag.Length() > 0 )
        {
        iTags = aTag.AllocL();
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsResource::Tags().
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& ChspsResource::Tags() const
    {
    if ( iTags )
        {
        return *iTags;
        }
    else
        {
        return KNullDesC;
        }
    }
    

//  End of File  
