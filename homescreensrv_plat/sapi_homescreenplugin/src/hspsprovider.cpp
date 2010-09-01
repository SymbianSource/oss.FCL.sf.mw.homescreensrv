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
* Description:  Implementation of CHSPSProvider class
*
*/


#include <ecom/implementationproxy.h>

#include "hspsconfigurationif.h"
#include "hspsprovider.h"
#include "hspsliwvocabulary.hrh"

using namespace LIW;

/* HSPS service provider name */
_LIT8( KHSPSService,                  "Service.HSPS" );

/* HSPS service interface names */
_LIT8( KHSPSConfigurationInterface,   "IConfiguration" );

/* Generic Liw command name */
_LIT8( KCommand,                      "cmd" );

// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================


// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHSPSProvider* CHSPSProvider::NewL()
    {
    return new( ELeave ) CHSPSProvider();
    }

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
//
CHSPSProvider::CHSPSProvider()
    {
    }

// -----------------------------------------------------------------------------
// Called by the LIW framework to initialise necessary information 
// from the Service Handler. This method is called when the consumer makes 
// the attach operation.
// -----------------------------------------------------------------------------
//
void CHSPSProvider::InitialiseL( 
    MLiwNotifyCallback& /*aFrameworkCallback*/, 
    const RCriteriaArray& aInterest )
    {
    TInt count = aInterest.Count();
    for ( TInt index = 0; index < count ; index++ )
        {
        if( aInterest[ index ]->ContentType() == KHSPSService )
            {
            return;
            }
        }

    User::Leave( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// Called by the LIW framework to instantiate the requested service interface
// -----------------------------------------------------------------------------
//
void CHSPSProvider::HandleServiceCmdL( 
    const TInt& aCmdId, 
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList,
    TUint /*aCmdOptions*/,
    const MLiwNotifyCallback* /*aCallback*/ )
    {
    TPtrC8 cmdName;

    if ( aCmdId == KLiwCmdAsStr )
        {
        TInt pos = 0;	
        const TLiwGenericParam* cmd = aInParamList.FindFirst( pos, KCommand );
        if ( cmd )
            {
            cmdName.Set( cmd->Value().AsData() );
            }
        }
    if ( cmdName == KHSPSConfigurationInterface )
        {
        //find appUid
        TInt pos = 0;
        TPtrC8 uid;
        const TLiwGenericParam* appUid = aInParamList.FindFirst( pos, 
        													   KHspsLiwAppUid );
        if( appUid )
        	{
        	uid.Set( appUid->Value().AsData() );
        	//Create interface.
    	    CHSPSConfigurationIf* interface = 
	            CHSPSConfigurationIf::NewL( uid );
	        CleanupStack::PushL( interface );
	        aOutParamList.AppendL( TLiwGenericParam( 
	            KHSPSConfigurationInterface, TLiwVariant( interface ) ) );
	        CleanupStack::Pop( interface );
        	}
        else
        	{
        	aOutParamList.Reset();
    	    aOutParamList.AppendL( TLiwGenericParam( EGenericParamError, 
    	    					   TLiwVariant( KErrArgument ) ) );
        	}
        }
    else
        {
        aOutParamList.AppendL( TLiwGenericParam( 
            EGenericParamError, 
            TLiwVariant( ( TInt32 )KErrArgument ) ) );
        }
    }

// ======== GLOBAL FUNCTIONS ===================================================

// -----------------------------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x200159B8, CHSPSProvider::NewL)
    };

// -----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of file
