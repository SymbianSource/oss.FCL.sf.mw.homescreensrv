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
* Description:  Class ChspsResult is a Xuikon utility class that defines Additional 
*                Return Code for Xuikon Application Theme Management Services on service
*                request return.
*                It is quaranteed that ChspsResult-object is always accessible after
*                client request whether the result was successful or not.
*                ChspsResult-class has attributes that informs the result as follows:
*                - iSystemError - Symbian OS returned error code
*                - iXuikonError - Xuikon defined error code in Xuikon error space
*                - iIntValue1   - additional information relevant in the result. 
*                - iIntValue2   - additional information relevant in the result.   
*                
*
*/


#include "hspsresult.h"

#include <s32mem.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsResult::ChspsResult()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsResult::ChspsResult()
    {
    }

// -----------------------------------------------------------------------------
// ChspsResult::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsResult::ConstructL()
    {
    ResetData();
    }

// -----------------------------------------------------------------------------
// ChspsResult::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsResult* ChspsResult::NewL()
    {
    ChspsResult* self = new( ELeave ) ChspsResult;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
ChspsResult::~ChspsResult()
    {
    }

// -----------------------------------------------------------------------------
// ChspsResult::ExternalizeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResult::ExternalizeL( RDesWriteStream& aStream ) const
    {
    aStream.WriteUint32L( (TUint)iSystemError );
    aStream.WriteUint32L( (TUint)iXuikonError );
    aStream.WriteUint32L( (TUint)iIntValue1 );
    aStream.WriteUint32L( (TUint)iIntValue2 );
    }
       
// -----------------------------------------------------------------------------
// ChspsResult::InternalizeL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResult::InternalizeL( RDesReadStream& aStream )
    {
    iSystemError = (TInt)aStream.ReadUint32L();
    iXuikonError = (TInt)aStream.ReadUint32L();
    iIntValue1 = (TInt)aStream.ReadUint32L();
    iIntValue2 = (TInt)aStream.ReadUint32L();
    }
    
// -----------------------------------------------------------------------------
// ChspsResult::GetDataLength()
// Returns data length of ChspsResult-object for streamin functions.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsResult::GetDataLength() const
    {
    TInt len = sizeof( iSystemError );
    len += sizeof(iXuikonError);
    len += sizeof(iIntValue1);
    len += sizeof(iIntValue2);
    return len;
    }
  
// -----------------------------------------------------------------------------
// ChspsResult::ResetData()
// Resets ChspsResult's data members.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsResult::ResetData()
    {
    iSystemError = 0;
    iXuikonError = 0;
    iIntValue1 = 0;
    iIntValue2 = 0;
    }
    

//  End of File  
