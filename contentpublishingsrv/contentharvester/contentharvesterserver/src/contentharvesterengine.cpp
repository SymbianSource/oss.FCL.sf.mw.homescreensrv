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
* Description:  Content Harvester Engine
 *
*/


// for CleanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <contentharvesterplugin.h> // harvesting plugin
#include <LiwServiceHandler.h>
#include <e32property.h>

#include "charvesterpluginvalidator.h"
#include "contentharvesterpluginuids.hrh"
#include "cpglobals.h"
#include "cpdebug.h"
#include "contentharvesterengine.h"

// LOCAL CONSTANTS
const TInt KADatFactorySettingsServerPluginUid( 0x102830EF);



// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CContentHarvesterEngine::CContentHarvesterEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContentHarvesterEngine::CContentHarvesterEngine()
    {
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContentHarvesterEngine::ConstructL()
    {
    CP_DEBUG(_L8("Create Engine" ));

    CP_DEBUG(_L8("Start delayed update" ));
    // start updating process

    iServiceHandler = CLiwServiceHandler::NewL( );
    // for convinience keep pointers to Service Handler param lists
    CLiwGenericParamList* inParamList = &iServiceHandler->InParamListL( );
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL( );

    RCriteriaArray criteriaArray;
    CleanupResetAndDestroyPushL( criteriaArray );

    // create Liw criteria
    CLiwCriteriaItem* criterion = CLiwCriteriaItem::NewLC( KLiwCmdAsStr,
        KCPInterface, KCPService );
    criterion->SetServiceClass( TUid::Uid( KLiwClassBase ) );

    criteriaArray.AppendL( criterion );
    CleanupStack::Pop( criterion );

    // attach Liw criteria
    iServiceHandler->AttachL( criteriaArray );
    // get BS Service interface
    iServiceHandler->ExecuteServiceCmdL( *criterion, *inParamList,
        *outParamList );

    CleanupStack::PopAndDestroy( &criteriaArray );

    TInt pos( 0);
    outParamList->FindFirst( pos, KCPInterface );
    if ( pos != KErrNotFound )
        {
        iCPSInterface = (*outParamList)[pos].Value().AsInterface( );
        }
    
    iPluginManager = CHarvesterPluginValidator::NewL( 
                                    TUid::Uid( KContentHarvesterPluginUid ),
                                    iCPSInterface );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEngine::~CContentHarvesterEngine
// Destructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterEngine::~CContentHarvesterEngine()
    {
    if ( iCPSInterface )
        {
        iCPSInterface->Close( );
        }
    delete iServiceHandler;
    delete iPluginManager;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterEngine* CContentHarvesterEngine::NewL()
    {
    CContentHarvesterEngine* self = CContentHarvesterEngine::NewLC( );
    CleanupStack::Pop( self ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterEngine* CContentHarvesterEngine::NewLC()
    {
    CContentHarvesterEngine* self = new ( ELeave ) CContentHarvesterEngine();
    CleanupStack::PushL( self );
    self->ConstructL( ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEngine::Update
// Pass request for update to all installed plugins
// -----------------------------------------------------------------------------
//
void CContentHarvesterEngine::Update()
    {
    CP_DEBUG(_L8("CContentHarvesterEngine::Update" ));
    CContentHarvesterPlugin*    plugin( NULL );
    CContentHarvesterPlugin*    fsplugin = 
                                static_cast<CContentHarvesterPlugin*>
                                ( iPluginManager->GetImplementation( 
                                TUid::Uid( KADatFactorySettingsServerPluginUid) ) );
								
	if (fsplugin)
		{							
		TRAP_IGNORE( fsplugin->UpdateL() );
		}
	
	for ( TInt i = 0; i < iPluginManager->GetCount(); i++ )
		{
		plugin = static_cast<CContentHarvesterPlugin*>
                               ( iPluginManager->GetImplementation( i ) );
		if( plugin!= fsplugin )
			{
			TRAP_IGNORE( plugin->UpdateL() );    
			}
		}	
    }

// End of File


