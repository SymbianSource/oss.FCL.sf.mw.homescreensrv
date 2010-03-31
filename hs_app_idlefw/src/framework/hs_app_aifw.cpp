/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main AI framework class
*
*/


#include <bautils.h>
#include <coemain.h>
#include <ConeResLoader.h>
#include <e32property.h>
#include <startupdomainpskeys.h>
#include <activeidle2domainpskeys.h>
#include <activeidle2internalpskeys.h>
#include <cenrepnotifyhandler.h>
#include <aipspropertyobserver.h>
#include <aisystemuids.hrh>

#include <AknWaitDialog.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>


#include <e32cmn.h>
#include <e32def.h>

#include "hs_app_aifw.h"
#include "hs_app_aifwpanic.h"
#include "aiutility.h"
#include "hs_app_aiuicontrollermanager.h"
#include "aiuicontroller.h"
#include "aicontentmodel.h"
#include "hs_app_aicontentpluginmanager.h"
#include "hs_app_aiwspluginmanager.h"
#include "hs_app_aipluginstatemanager.h"
#include "hs_app_aiidleappregister.h"
#include "debug.h"

#include <centralrepository.h>
#include <activeidle2domaincrkeys.h>
#include "hs_app_ainetworklistener.h"


#include <data_caging_path_literals.hrh>

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiFw::NewL()
// ----------------------------------------------------------------------------
//
CAiFw::CAiFw()
    {
    }

// ----------------------------------------------------------------------------
// CAiFw::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiFw::ConstructL()
    {
#if 0
    // For AI3_test    
    RProcess proc;
    // 0x102750F0 in AI3, 0x2001CB4F in AI3_Test
    TSecureId secId( proc.SecureId() ); 
                                           
    if( secId == 0x2001CB4F )
        {
        iAIRepository = CRepository::NewL( TUid::Uid( 0x2001952B ) );
        }   
    else
        {
        iAIRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
        }
#else
    iAIRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
#endif
        
    TInt value( 0 );
    
    iAIRepository->Get( KAiMainUIController, value );
    
    if( !( value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
           value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ||
           value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
           value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ) )
        {
        // Someone wrote an invalid configuration! Reset repository.
        iAIRepository->Reset( KAiMainUIController );
        iAIRepository->Reset( KAiFirstUIController );
        iAIRepository->Delete( KAiFirstUIController + 1 );
        }
    
    iUiControllerManager = CAiUiControllerManager::NewL();
           
    iPluginManager = CAiContentPluginManager::NewL();
                                                                   
    // Hook framework as UI event observer
    iUiControllerManager->SetEventHandler( *this );    
    }

// ----------------------------------------------------------------------------
// CAiFw::NewLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CAiFw* CAiFw::NewLC()
    {
    CAiFw* self = new ( ELeave ) CAiFw;
    CleanupStack::PushL( self );
    self->ConstructL();

    __TICK( "FW: Core FW constructed" );
    __HEAP( "FW: Core FW constructed" );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiFw::~CAiFw()
// ----------------------------------------------------------------------------
//
CAiFw::~CAiFw()
    {
    if( iPluginManager )
        {
        delete iPluginManager;
        iPluginManager = NULL;
        }
        
    if( iIdleRestartObserver )
        {
        Release( iIdleRestartObserver );
        iIdleRestartObserver = NULL;
        }
    
    if( iWsPluginManager )
        {
        delete iWsPluginManager;
        iWsPluginManager = NULL;
        }
        
    if( iUiControllerManager )
        {
        delete iUiControllerManager;
        iUiControllerManager = NULL;
        }
    
    if( iNotifyHandler )
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        iNotifyHandler = NULL;
        }
    
    if( iNotifyHandlerESS )
        {
        iNotifyHandlerESS->StopListening();
        delete iNotifyHandlerESS;
        iNotifyHandlerESS = NULL;
        }
    
    if( iAIRepository )
        {
        delete iAIRepository;
        iAIRepository = NULL;
        }
    
    iLibrary1.Close();
    iLibrary2.Close();
    iLibrary3.Close();
    }

// ----------------------------------------------------------------------------
// CAiFw::RunL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CAiFw::RunL()
    {
    CAiIdleAppRegister* idleReg = CAiIdleAppRegister::NewLC();
    idleReg->RegisterL();
    CleanupStack::PopAndDestroy( idleReg );

    // Tell UI controller manager to start application framework and event loop.
    // This function returns only when the application is shut down.
    // See in CAiFw::HandleUiReadyEventL how the framework initialization continues.
    iUiControllerManager->RunApplicationL();
    }

// ----------------------------------------------------------------------------
// CAiFw::AppEnvReadyL()
// ----------------------------------------------------------------------------
//
void CAiFw::AppEnvReadyL()
    {
    // Initialize members which need to be connected to the app environment's
    // active scheduler or depend on the app environment being initialized.

    // Create state managers system state observers
    CAiPluginStateManager& stateManager( iPluginManager->StateManager() );
    
    stateManager.CreateSystemStateObserversL();

    // Connect state managers UI observer to UI controllers
    MAiUiFrameworkObserver* fwObserver( stateManager.UiFwObserver() );
    
    if ( fwObserver )
        {
        iUiControllerManager->AddObserverL( *fwObserver );
        }

    // Create WS pluign manager
    iWsPluginManager = CAiWsPluginManager::NewL
        ( iUiControllerManager->CoeEnv() );

    // CenRep notifier to listen key changes in cenrep. Application is restarted
    // if key value is changed.
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                 *iAIRepository,
                                                 CCenRepNotifyHandler::EIntKey,
                                                 KAiMainUIController );
    iNotifyHandler->StartListeningL();

    // Cenrep notifier to listen ESS changes in cenrep
    //
    iNotifyHandlerESS = CCenRepNotifyHandler::NewL( *this,
                                                    *iAIRepository,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KAIExternalStatusScreen );
    iNotifyHandlerESS->StartListeningL();

    iIdleRestartObserver = AiUtility::CreatePSPropertyObserverL(
                        TCallBack( HandleRestartEvent, this ),
                        KPSUidAiInformation,
                        KActiveIdleRestartAI2 );
    
    stateManager.ReportStateChange( ESMAISystemBoot );
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleUiReadyEventL()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleUiReadyEventL( CAiUiController& aUiController )
    {         
    if( iUiControllerManager->IsMainUiController( aUiController ) )
        {
        iUiControllerManager->LoadUIDefinition();
                
        TInt value( EIdlePhase1Ok );
        
        RProperty::Get( KPSUidStartup, 
                        KPSIdlePhase1Ok, 
                        value );
                                                      
        if( value == EIdlePhase1NOK )
            {
            RProperty::Set( KPSUidStartup, 
                            KPSIdlePhase1Ok, 
                            EIdlePhase1Ok );                                                          
            }    
        
        if( !iLibrariesLoaded )
            {
            _LIT( KAIVoiceUIDialer, "VoiceUiNameDialer.dll" );
            _LIT( KAIVoiceUIRecog, "VoiceUiRecognition.dll" );
            _LIT( KAIVCommandHandler, "vcommandhandler.dll" );
            
            iLibrary1.Load( KAIVoiceUIDialer );
            iLibrary2.Load( KAIVoiceUIRecog );
            iLibrary3.Load( KAIVCommandHandler );        
            
            iLibrariesLoaded = ETrue;
            }
        
        }
    }

// ---------------------------------------------------------------------------
// CAiFw::HandleActivateUI()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleActivateUI()
    {
    iUiControllerManager->ActivateUI();         
    }

// ---------------------------------------------------------------------------
// CAiFw::HandleUiShutdown()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleUiShutdown( CAiUiController& aUiController )
    {
    if( iUiControllerManager->IsMainUiController( aUiController ) )
        {
        if( iNotifyHandler )
            {
            iNotifyHandler->StopListening();
            delete iNotifyHandler;
            iNotifyHandler = NULL;
            }

        if( iNotifyHandlerESS )
            {
            iNotifyHandlerESS->StopListening();
            delete iNotifyHandlerESS;
            iNotifyHandlerESS = NULL;
            }
        
        iPluginManager->PluginFactory().DestroyPlugins();

        iPluginManager->StateManager().DestroySystemStateObservers();
                
        iUiControllerManager->DestroySecondaryUiControllers();

        iUiControllerManager->RemoveObserver( 
                *iPluginManager->StateManager().UiFwObserver() );
        
        if( iWsPluginManager )
            {
            delete iWsPluginManager;
            iWsPluginManager = NULL;
            }
        
        if( iIdleRestartObserver )
            {
            Release( iIdleRestartObserver );
            iIdleRestartObserver = NULL;
            } 
        }
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleLoadPluginL()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleLoadPluginL( const TAiPublisherInfo& aPublisherInfo )
    {           
    iPluginManager->PluginFactory().CreatePluginL( 
       aPublisherInfo, iUiControllerManager->UiControllers() );                                                                                                                                                 
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleDestroyPluginL()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleDestroyPluginL( const TAiPublisherInfo& aPublisherInfo )
    {    
    iPluginManager->PluginFactory().DestroyPluginL(
        aPublisherInfo, iUiControllerManager->UiControllers() );                                                                            
    }

// ----------------------------------------------------------------------------
// CAiFw::HandlePluginEvent()
// ----------------------------------------------------------------------------
//
void CAiFw::HandlePluginEvent( const TDesC& aParam )
    {
    iPluginManager->HandlePluginEvent( aParam );
    }

// ----------------------------------------------------------------------------
// CAiFw::HandlePluginEventL()
// ----------------------------------------------------------------------------
//
void CAiFw::HandlePluginEventL( const TAiPublisherInfo& aPublisherInfo, 
    const TDesC& aParam )
    {
    iPluginManager->HandlePluginEventL( aPublisherInfo, aParam );
    }

// ----------------------------------------------------------------------------
// CAiFw::HasMenuItemL()
// ----------------------------------------------------------------------------
//
TBool CAiFw::HasMenuItemL( const TAiPublisherInfo& aPublisherInfo, 
    const TDesC& aMenuItem )
    {            
    return iPluginManager->HasMenuItemL( aPublisherInfo, aMenuItem ); 
    }

// ----------------------------------------------------------------------------
// CAiFw::RefreshContent()
// ----------------------------------------------------------------------------
//
TBool CAiFw::RefreshContent( const TDesC& aContentCid )
    {
    return iPluginManager->RefreshContent( aContentCid );
    }

// ----------------------------------------------------------------------------
// CAiFw::ProcessStateChange()
// ----------------------------------------------------------------------------
//
void CAiFw::ProcessStateChange( TAifwStates aState )     
    {
    switch ( aState )
    	{
    	case EAifwOnline :
    		{
    		iPluginManager->ProcessOnlineState( ETrue );
    		}
    		break;
    	case EAifwOffline :
			{
			iPluginManager->ProcessOnlineState( EFalse );
			}
			break;
    	case EAifwPageSwitch:
    		{
    		iPluginManager->StateManager().ReportStateChange( ESMAIPageSwitch );
    		}
    		break;
    	default : 
    		break;
    	}
    
    }

// ----------------------------------------------------------------------------
// CAiFw::QueryIsMenuOpen()
// ----------------------------------------------------------------------------
//
TBool CAiFw::QueryIsMenuOpen()
    {
    return iUiControllerManager->MainUiController().IsMenuOpen();
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleNotifyInt()
// ----------------------------------------------------------------------------
//
void CAiFw::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    switch( aId )
        {
        case KAiMainUIController:
            if( aNewValue == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
                aNewValue == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ||
                aNewValue == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
                aNewValue == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE )
                {
                iUiControllerManager->ExitMainController();
                }
            else
                {
                // Someone wrote an invalid configuration! Reset repository.
                if( iAIRepository )
                    {
                    iAIRepository->Reset( KAiMainUIController );
                    iAIRepository->Reset( KAiFirstUIController );
                    iAIRepository->Delete( KAiFirstUIController + 1 );
                    }
                }
            break;
        case KAIExternalStatusScreen:
            if( ( aNewValue & 0x7FFFFFFF ) != 0 )
                {
                TRAP_IGNORE( SwapUiControllerL( EFalse ) );
                }
            else
                {
                TRAP_IGNORE( SwapUiControllerL( ETrue ) );
                }
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CAiFw::SwapUiControllerL()
// ----------------------------------------------------------------------------
//
void CAiFw::SwapUiControllerL( TBool aToExtHS )
    {
    TUid uid = { KCRUidActiveIdleLV };
    CRepository* cenRep = CRepository::NewL( uid );

    if( !aToExtHS ) // Switch to XML UI
        {
        cenRep->Create( KAiFirstUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        
        cenRep->Set( KAiFirstUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        cenRep->Delete( KAiFirstUIController + 1 );
        
        cenRep->Set( KAiMainUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML );
        }
    else // Switch to ExtHS
        {
        cenRep->Delete( KAiFirstUIController );
        cenRep->Delete( KAiFirstUIController + 1 );
        cenRep->Set( KAiMainUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        }
    
    delete cenRep;

    // Restart
    iUiControllerManager->ExitMainController();
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleRestartEvent()
// ----------------------------------------------------------------------------
//
TInt CAiFw::HandleRestartEvent( TAny* aSelf )
    {
    CAiFw* self = static_cast<CAiFw*>( aSelf );
    
    TInt value( 0 );
    
    if( self->iIdleRestartObserver )
        {
        TInt err( self->iIdleRestartObserver->Get( value ) );

        // Check the PS keys value and call manager with approriate parameter.
        // Report either "idle foreground" or "idle background"
        if( value == KActiveIdleRestartCode )
            {
            self->iUiControllerManager->ExitMainController();
            }
        }
    
    return KErrNone;
    }

// End of file
