/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "ccontentpublishinginterface.h"
#include "cpdebug.h"
#include "cpglobals.h"
#include "cpclient.h"
#include "cpclientiterable.h"

using namespace LIW;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CContentPublishingInterface* CContentPublishingInterface::NewL()
    {
    CContentPublishingInterface* self = CContentPublishingInterface::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CContentPublishingInterface* CContentPublishingInterface::NewLC()
    {
    CContentPublishingInterface* self = 
        new( ELeave ) CContentPublishingInterface;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CContentPublishingInterface::~CContentPublishingInterface()
    {
    CP_DEBUG( _L8("CContentPublishingInterface::~CContentPublishingInterface") );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CContentPublishingInterface::CContentPublishingInterface()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CContentPublishingInterface::ConstructL()
    {
    CP_DEBUG( _L8("CContentPublishingInterface::ConstructL") );
    CDataSourceInterface::ConstructL();
    }


// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CContentPublishingInterface::ProcessCommandL( const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    CP_DEBUG( _L8("CContentPublishingInterface::ProcessCommandL") );
    
    if ( aCmdName.CompareF( KExecuteAction ) == 0 )
        {
        iCPClient->ExecuteActionL( aInParamList, aCmdOptions );
        }
    else if ( aCmdName.CompareF( KExecuteMultipleActions ) == 0 )
        {
        iCPClient->ExecuteMultipleActionsL( aInParamList, aCmdOptions );
        }
    else
        {
        CDataSourceInterface::ProcessCommandL(aCmdName,
            aInParamList, aOutParamList, aCmdOptions, aCallback);
        }
    }
