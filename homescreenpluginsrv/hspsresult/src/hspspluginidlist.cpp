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
* Leo Zheng - GCC-E compilation error fix (2098)
*
* Description:  Class ChspsPluginIdList is a HSPS utility class for passing
* 				 plugin ids from a client process to the server process.
*                
*
*/


#include "hspspluginidlist.h"

#include <s32mem.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsPluginIdList::ChspsPluginIdList()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsPluginIdList::ChspsPluginIdList( TInt aGranularity ) : CArrayFixFlat<TInt>( aGranularity )
    {
    }

// Destructor
EXPORT_C ChspsPluginIdList::~ChspsPluginIdList()
    {
    }

// -----------------------------------------------------------------------------
// ChspsPluginIdList::ExternalizeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsPluginIdList::ExternalizeL( RDesWriteStream& aStream ) const
    {    
    const TInt count = Count();
    aStream.WriteUint32L( (TUint)count );    
    for( TInt i=0; i< count; i++ )
    	{    	
    	aStream.WriteUint32L( (TUint)At(i) );
    	}
    }
       
// -----------------------------------------------------------------------------
// ChspsPluginIdList::InternalizeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsPluginIdList::InternalizeL( RDesReadStream& aStream )
    {
    TInt count = aStream.ReadUint32L();
    for( TInt i=0; i< count; i++ )
    	{    	
    	AppendL( aStream.ReadUint32L() );
    	}
    }    

// -----------------------------------------------------------------------------
// ChspsPluginIdList::GetDataLength()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsPluginIdList::GetDataLength()
    {    
    TInt len = sizeof( TUint ) + Count() * sizeof( TUint ); 
    return len;
    }

//  End of File  
