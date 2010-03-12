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
#include <hscontentinfo.h>

// User include files
#include "hscontentinfoarray.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------
// CHsContentInfoArray::NewL()
// -----------------------------------------------------------------------
//
EXPORT_C CHsContentInfoArray* CHsContentInfoArray::NewL()
    {
    CHsContentInfoArray* self = new ( ELeave ) CHsContentInfoArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------
// CHsContentInfoArray::NewL()
// -----------------------------------------------------------------------
//    
EXPORT_C CHsContentInfoArray* CHsContentInfoArray::NewL( RReadStream& aStream )
    {
    CHsContentInfoArray* self = new ( ELeave ) CHsContentInfoArray();
    CleanupStack::PushL( self );
    self->InternalizeL( aStream );
    CleanupStack::Pop( self );
    return self;
    }   

// -----------------------------------------------------------------------
// CHsContentInfoArray::ConstructL()
// -----------------------------------------------------------------------
//
void CHsContentInfoArray::ConstructL()
    {
    }

// -----------------------------------------------------------------------
// CHsContentInfoArray::CHsContentInfoArray()
// -----------------------------------------------------------------------
//
CHsContentInfoArray::CHsContentInfoArray()
    {
    }

// -----------------------------------------------------------------------
// CHsContentInfoArray::~CHsContentInfoArray()
// -----------------------------------------------------------------------
//
CHsContentInfoArray::~CHsContentInfoArray()
    {
    iArray.ResetAndDestroy();        
    }

// -----------------------------------------------------------------------
// CHsContentInfoArray::Array()
// -----------------------------------------------------------------------
//
EXPORT_C RPointerArray< CHsContentInfo >& CHsContentInfoArray::Array()
    {
    return iArray;
    }

// -----------------------------------------------------------------------
// CHsContentInfoArray::ExternalizeL()
// -----------------------------------------------------------------------
//     
EXPORT_C void CHsContentInfoArray::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt16L( iArray.Count() );
        
    for( int i = 0; i < iArray.Count(); i++ )
        {
        CHsContentInfo* info = iArray[i];
        info->ExternalizeL( aStream );
        }
    }

   
// -----------------------------------------------------------------------
// CHsContentInfoArray::InternalizeL()
// -----------------------------------------------------------------------
//   
EXPORT_C void CHsContentInfoArray::InternalizeL( RReadStream& aStream )
    {
    TInt count = aStream.ReadInt16L();
        
    for( int i = 0; i < count; i++ )
        {
        CHsContentInfo* info = CHsContentInfo::NewL( aStream );
        iArray.AppendL( info );
        }                
    }

// -----------------------------------------------------------------------
// CHsContentInfoArray::Size()
// -----------------------------------------------------------------------
//   
EXPORT_C TInt CHsContentInfoArray::Size( )
    {
    TInt size( sizeof( TInt16 ) );   
    for ( TInt i = 0; i < iArray.Count(); i++ )
        {
        size = size + iArray[ i ]->Size();
        }
    return size;
    }   

// -----------------------------------------------------------------------
// CHsContentInfoArray::MarshalL()
// -----------------------------------------------------------------------
//   
EXPORT_C HBufC8* CHsContentInfoArray::MarshalL( )
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
