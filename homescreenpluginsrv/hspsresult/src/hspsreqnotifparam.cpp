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
* Description:  Request Notification parameters  
*                
*
*/

#include "hspsdefinitionrepository.h"
#include "hspsreqnotifparam.h"

#include <s32mem.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::ChspsRequestNotificationParams()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsRequestNotificationParams::ChspsRequestNotificationParams()
    {
    }

// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsRequestNotificationParams::ConstructL()
    {
    ResetData();
    }

// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsRequestNotificationParams* ChspsRequestNotificationParams::NewL()
    {
    ChspsRequestNotificationParams* self = new( ELeave ) ChspsRequestNotificationParams;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
ChspsRequestNotificationParams::~ChspsRequestNotificationParams()
    {
    iPluginIds.Close();
    delete iName;
    }

// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::ExternalizeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsRequestNotificationParams::ExternalizeL( RDesWriteStream& aStream ) const
    {
    aStream.WriteUint32L( (TUint)iEvent );
    aStream.WriteUint32L( (TUint)iAppUid );
    aStream.WriteUint32L( (TUint)iAppConfUid );
    aStream.WriteUint32L( (TUint)iOrigUid );
    aStream.WriteUint32L( (TUint)iPluginUid );
    aStream.WriteUint32L( (TUint)iCount );
    
    for(TInt i =0; i < iCount; i++)
        {
        aStream.WriteUint32L( (TUint)iPluginIds[i] );
        }
   
    if ( iName )
        {
        aStream << *iName;
        }
    else
        {
        aStream << KNullDesC;
        }
    }
       
// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::InternalizeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsRequestNotificationParams::InternalizeL( RDesReadStream& aStream )
    {
    iEvent = (TInt)aStream.ReadUint32L();
    iAppUid = (TInt)aStream.ReadUint32L();
    iAppConfUid = (TInt)aStream.ReadUint32L();
    iOrigUid = (TInt)aStream.ReadUint32L();
    iPluginUid = (TInt)aStream.ReadUint32L();
    iCount = (TInt)aStream.ReadUint32L();
    iPluginIds.Reset();
    for(TInt i=0; i < iCount; i++ )
        {
        iPluginIds.AppendL((TInt)aStream.ReadUint32L());
        }
    delete iName;
    iName = NULL;
    iName = HBufC::NewL(aStream, KMaxFileName );
    }
    
// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::GetDataLength()
// Returns data length of ChspsRequestNotificationParams-object for streamin functions.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsRequestNotificationParams::GetDataLength() const
    {
    TInt len = sizeof( iEvent );
    len += sizeof(iAppUid);
    len += sizeof(iAppConfUid);
    len += sizeof(iPluginUid);
    len += sizeof(iCount);
    for(TInt i = 0; i < 0; i++)
        {
        len += sizeof(iPluginIds[i]);
        }
    
    
    if(iName)
        {
        len += KMaxFileName;
        }    
    return len;
    }
  
// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::ResetData()
// Resets ChspsRequestNotificationParams's data members.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsRequestNotificationParams::ResetData()
    {
    iEvent = EhspsNoEvent;
    iAppUid = 0;
    iAppConfUid = 0;
    iOrigUid = 0;
    iPluginUid = 0;
    iCount = 0;
    iPluginIds.Reset();
    delete iName;
    iName = NULL;
    }
// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::SetNameL
// Set Name
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsRequestNotificationParams::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsRequestNotificationParams::Name()
// Get Name
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C const TDesC& ChspsRequestNotificationParams::Name() const
    {
    if ( iName )
        {
        return *iName;
        }
    else
        {
        return KNullDesC;
        }
    }    

//  End of File  
