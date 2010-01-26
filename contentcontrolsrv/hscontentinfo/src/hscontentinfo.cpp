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

// System include files
#include <utf.h>

// User include files
#include "hscontentinfo.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------
// CHsContentInfo::NewL()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentInfo* CHsContentInfo::NewL()
    {
    CHsContentInfo* self = CHsContentInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::NewL()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentInfo* CHsContentInfo::NewL( RReadStream& aStream )
    {
    CHsContentInfo* self = new ( ELeave ) CHsContentInfo();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }   
    
// -----------------------------------------------------------------------
// CHsContentInfo::NewLC()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentInfo* CHsContentInfo::NewLC()
    {
    CHsContentInfo* self = new ( ELeave ) CHsContentInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::CloneL()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentInfo* CHsContentInfo::CloneL()
    {
    CHsContentInfo* clone = new ( ELeave ) CHsContentInfo();
    CleanupStack::PushL( clone );
    
    clone->iName = Name().AllocL();
    clone->iUid = Uid().AllocL();
    clone->iPluginId = PluginId().AllocL();
    clone->iType = Type().AllocL();
    clone->iPublisherId = PublisherId().AllocL();
    clone->iDescription = Description().AllocL();
    clone->iIconPath = IconPath().AllocL();
    clone->iMaxInstance = iMaxInstance;
    clone->iCanBeAdded = iCanBeAdded;
    clone->iCanBeRemoved = iCanBeRemoved;
    clone->iInstallationTime = iInstallationTime;
    clone->iPublisherUid = PublisherUid().AllocL();
    clone->iIsWrt = iIsWrt;
    
    CleanupStack::Pop();
    return clone;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::ConstructL()
// -----------------------------------------------------------------------
//
void CHsContentInfo::ConstructL()
    {
    }

// -----------------------------------------------------------------------
// CHsContentInfo::CHsContentInfo()
// -----------------------------------------------------------------------
//
CHsContentInfo::CHsContentInfo()
    {
    }

// -----------------------------------------------------------------------
// CHsContentInfo::~CHsContentInfo()
// -----------------------------------------------------------------------
//
CHsContentInfo::~CHsContentInfo()
    {
    delete iName;
    delete iUid;
    delete iPluginId;
    delete iType;
    delete iPublisherId;
    delete iDescription;
    delete iIconPath;
    delete iPublisherUid;
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = 0;
    iName = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aName );
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = 0;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetPublisherIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetPublisherIdL(
    const TDesC& aPublisherId )
    {
    delete iPublisherId;
    iPublisherId = 0;
    iPublisherId = aPublisherId.AllocL();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetUidL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetUidL( const TDesC8& aUid )
    {
    delete iUid;
    iUid = 0;
    iUid = aUid.AllocL();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetPluginIdL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetPluginIdL( const TDesC8& aId )
    {
    delete iPluginId;
    iPluginId = 0;
    iPluginId = aId.AllocL();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetTypeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetTypeL( const TDesC8& aType )
    {
    delete iType;
    iType = 0;
    iType = aType.AllocL();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::Name
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHsContentInfo::Name()const
    {
    return ( iName ) ? *iName : KNullDesC();  
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::PublisherId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CHsContentInfo::PublisherId()const
    {
    return ( iPublisherId ) ? *iPublisherId : KNullDesC(); 
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHsContentInfo::Uid()const
    {
    return ( iUid ) ? *iUid : KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::NameAs8BitLC
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHsContentInfo::NameAs8BitLC()const
    {    
    HBufC8* ret( CnvUtfConverter::ConvertFromUnicodeToUtf8L( Name() ) );    
    CleanupStack::PushL( ret );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::PluginId
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHsContentInfo::PluginId()const
    {
    return ( iPluginId ) ? *iPluginId : KNullDesC8();
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::Type
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHsContentInfo::Type()const
    {
    return ( iType ) ? *iType : KNullDesC8();
    }

// -----------------------------------------------------------------------
// CPublisherInfo::SetMaxWidgets()
// -----------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetMaxWidgets( TInt aMaxWidget )
   {
   iMaxInstance = aMaxWidget;
   }

// -----------------------------------------------------------------------
// CPublisherInfo::MaxWidgets()
// -----------------------------------------------------------------------
//
EXPORT_C TInt CHsContentInfo::MaxWidgets() const
   {
   return iMaxInstance;
   }
   
// -----------------------------------------------------------------------
// CHsContentInfo::ExternalizeL()
// -----------------------------------------------------------------------
//     
EXPORT_C void CHsContentInfo::ExternalizeL( RWriteStream& aStream )
	{
	// Keep this externalize routine in-sync with header file's members
	
    // externalize iName	
    aStream.WriteInt16L( Name().Size() );
    aStream << Name();
	
    // externalize iUid
    aStream.WriteInt16L( Uid().Size() );
    aStream << Uid();
        
    // externalize iPluginId;
    aStream.WriteInt16L( PluginId().Size() );
    aStream << PluginId();
    
    // externalize iType;       
    aStream.WriteInt16L( Type().Size() );
    aStream << Type();
   
    // externalize iPublisherId;		
    aStream.WriteInt16L( PublisherId().Size() );
    aStream << PublisherId();

    // externalize iDescription;        
    aStream.WriteInt16L( Description().Size() );
    aStream << Description();
       
    // externalize iIconPath;        
    aStream.WriteInt16L( IconPath().Size() );
    aStream << IconPath();

    // externalize iMaxInstance
    aStream.WriteInt16L( iMaxInstance );
    // externalize iCanBeAdded
    aStream.WriteInt16L( iCanBeAdded );
    // externalize iCanBeRemoved
    aStream.WriteInt16L( iCanBeRemoved );    
    
    TInt64 time( iInstallationTime.Int64() );
    
    TUint32 low( I64LOW( time ) );
    TUint32 high( I64HIGH( time ) );

    // externalize iInstallationTime
    aStream.WriteUint32L( low );
    aStream.WriteUint32L( high );
    
    // externalize iPublisherUid
    aStream.WriteInt16L( PublisherUid().Size() );
    aStream << PublisherUid();
    
    // externalise iIsWrt
    aStream.WriteInt16L( iIsWrt );
	}

// -----------------------------------------------------------------------
// CHsContentInfo::SetCanBeAdded()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetCanBeAdded( TBool aValue )
    {
    iCanBeAdded = aValue;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::CanBeAdded()
// -----------------------------------------------------------------------
//   
EXPORT_C TBool CHsContentInfo::CanBeAdded() const
    {
    return iCanBeAdded;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::SetCanBeRemoved()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetCanBeRemoved( TBool aValue )
    {
    iCanBeRemoved = aValue;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::CanBeRemoved()
// -----------------------------------------------------------------------
//   
EXPORT_C TBool CHsContentInfo::CanBeRemoved() const
    {
    return iCanBeRemoved;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::SetInstallationTime()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetInstallationTime( TTime& aTime )
    {
    iInstallationTime = aTime;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::InstallationTime()
// -----------------------------------------------------------------------
//   
EXPORT_C TTime CHsContentInfo::InstallationTime() const
    {
    return iInstallationTime;
    }

// ---------------------------------------------------------------------------
// CHsContentInfo::SetDescriptionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CHsContentInfo::SetDescriptionL( const TDesC8& aDescription )
    {
    delete iDescription;
    iDescription = NULL;
    iDescription = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aDescription );
    }

// -----------------------------------------------------------------------
// CHsContentInfo::SetDescriptionL()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetDescriptionL( const TDesC& aDescription )
    {
    delete iDescription;
    iDescription = NULL;
    iDescription = aDescription.AllocL();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::Description()
// -----------------------------------------------------------------------
//   
EXPORT_C const TDesC& CHsContentInfo::Description() const
    {
    return ( iDescription ) ? *iDescription : KNullDesC();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::SetIconPathL()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetIconPathL( const TDesC& aIconPath )
    {
    delete iIconPath;
    iIconPath = NULL;
    
    iIconPath = aIconPath.AllocL();    
    }

// -----------------------------------------------------------------------
// CHsContentInfo::IconPath()
// -----------------------------------------------------------------------
//   
EXPORT_C const TDesC& CHsContentInfo::IconPath() const
    {
    return ( iIconPath ) ? *iIconPath : KNullDesC();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::InternalizeL()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::InternalizeL( RReadStream& aStream )
    {
    TInt length( 0 );	

    // internalize iName
    length = aStream.ReadInt16L();
    delete iName;
    iName = NULL;
    iName = HBufC::NewL( aStream, length );
    
    // internalize iUid    
    length = aStream.ReadInt16L();
    delete iUid;
    iUid = NULL;
    iUid = HBufC8::NewL( aStream, length );

    // internalize iPluginId
    length = aStream.ReadInt16L();
    delete iPluginId;
    iPluginId = NULL;
    iPluginId = HBufC8::NewL( aStream, length );
    
    // internalize iType
    length = aStream.ReadInt16L();
    delete iType;
    iType = NULL;
    iType = HBufC8::NewL( aStream, length );
    
    // internalize iPlublisherId;
    length = aStream.ReadInt16L();
    delete iPublisherId;
    iPublisherId = NULL;
    iPublisherId = HBufC16::NewL( aStream, length );

    // internalize iDescription;
    length = aStream.ReadInt16L();
    delete iDescription;
    iDescription = NULL;
    iDescription = HBufC16::NewL( aStream, length );

    // internalize iIconPath;
    length = aStream.ReadInt16L();
    delete iIconPath;
    iIconPath = NULL;
    iIconPath = HBufC16::NewL( aStream, length );
        
    // internalize iMaxInstance
    iMaxInstance = aStream.ReadInt16L();
    // internalize iCanBeAdded
    iCanBeAdded = aStream.ReadInt16L();
    // internalize iCanBeRemoved
    iCanBeRemoved = aStream.ReadInt16L();
    
    TUint32 low( aStream.ReadUint32L() );
    TUint32 high( aStream.ReadUint32L() );
    
    iInstallationTime = TTime( MAKE_TINT64( high, low ) );
    
    // internalize iPublisherUid    
    length = aStream.ReadInt16L();
    delete iPublisherUid;
    iPublisherUid = NULL;
    iPublisherUid = HBufC8::NewL( aStream, length );
    
    // internalize iIsWrt
    iIsWrt = aStream.ReadInt16L();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::Size()
// -----------------------------------------------------------------------
//   
EXPORT_C TInt CHsContentInfo::Size( )
    {
    TInt size( 0 );   
    size = size + Name().Size();        // name
    size = size + Uid().Size();         // uid
    size = size + PluginId().Size();    // plugin id
    size = size + Type().Size();        // type
    size = size + PublisherId().Size(); // publisher id
    size = size + Description().Size(); // description
    size = size + IconPath().Size();    // icon path
    size = size + sizeof( TInt16 );     // max instance
    size = size + sizeof( TInt16 );     // can be added
    size = size + sizeof( TInt16 );     // can be removed
    size = size + sizeof( TUint32 );    // installation time low
    size = size + sizeof( TUint32 );    // installation time high
    size = size + PublisherUid().Size();// publisher uid
    size = size + sizeof( TInt16 );     // is wrt
    
    return size;
    }   

// -----------------------------------------------------------------------
// CHsContentInfo::SetPublisherUidL()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetPublisherUidL(
    const TDesC8& aPublisherUid )
    {
    delete iPublisherUid;
    iPublisherUid = 0;
    iPublisherUid = aPublisherUid.AllocL();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::PublisherUid()
// -----------------------------------------------------------------------
// 
EXPORT_C const TDesC8& CHsContentInfo::PublisherUid() const
    {
    return ( iPublisherUid ) ? *iPublisherUid : KNullDesC8();
    }

// -----------------------------------------------------------------------
// CHsContentInfo::SetIsWrt()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfo::SetIsWrt( TBool aIsWrt )    
    {
    iIsWrt = aIsWrt;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::IsWrt()
// -----------------------------------------------------------------------
// 
EXPORT_C TBool CHsContentInfo::IsWrt() const
    {
    return iIsWrt;
    }

// -----------------------------------------------------------------------
// CHsContentInfo::MarshalL()
// -----------------------------------------------------------------------
//   
EXPORT_C HBufC8* CHsContentInfo::MarshalL( )
    {

    // Externalize message
    CBufFlat* reqBuf = CBufFlat::NewL( Size() );
    CleanupStack::PushL( reqBuf );
    RBufWriteStream reqStream( *reqBuf );
    CleanupClosePushL( reqStream );
    ExternalizeL( reqStream );
    CleanupStack::PopAndDestroy( &reqStream );
    
    // Append externalized messgae to a descriptor
    HBufC8* msgDesc = HBufC8::NewL( reqBuf->Size() );
    TPtr8 msgPtr( NULL, 0 ); 
    msgPtr.Set( msgDesc->Des() );
    reqBuf->Read( 0, msgPtr, reqBuf->Size() );
    CleanupStack::PopAndDestroy( reqBuf );
    
    return msgDesc;
    
    }   

// End of file
