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

// System includes
#include <startupdomainpskeys.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>

// User includes
#include "aiuicontrollermanager.h"
#include "aiuicontroller.h"
#include "aieventhandler.h"
#include "aistatemanager.h"
#include "aistateprovider.h"
#include "aipluginfactory.h"
#include "aiwspluginmanager.h"
#include "aiidleappregister.h"

#include <activeidle2domaincrkeys.h>
#include <activeidle2domainpskeys.h>
#include <aipspropertyobserver.h>
#include <aisystemuids.hrh>

#include "aiutility.h"
#include "aifwpanic.h"

#include "aifw.h"

#include "debug.h"

// Constants


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiFw::CAiFw()
//
// ----------------------------------------------------------------------------
//
CAiFw::CAiFw()
    {
    }

// ----------------------------------------------------------------------------
// CAiFw::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiFw::ConstructL()
    {
    __PRINTS( "*** CAiFw::ConstructL" );
    __TIME_MARK( time );
#if 0
    // For AI3_test    
    RProcess proc;
    // 0x102750F0 in AI3, 0x2001CB4F in AI3_Test
    TSecureId secId( proc.SecureId() ); 
                                           
    if( secId == 0x2001CB4F )
        {
        iRepository = CRepository::NewL( TUid::Uid( 0x2001952B ) );
        }   
    else
        {
        iRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
        }
#else
    iRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
#endif
        
    TInt value( 0 );
    
    iRepository->Get( KAiMainUIController, value );
    
    if ( !( value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
           value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ||
           value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
           value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ) )
        {
        // Someone wrote an invalid configuration! Reset repository.
        iRepository->Reset( KAiMainUIController );
        iRepository->Reset( KAiFirstUIController );
        iRepository->Delete( KAiFirstUIController + 1 );
        }
    
    iUiControllerManager = CAiUiControllerManager::NewL( this );
    
    iFactory = CAiPluginFactory::NewL( *iUiControllerManager );
    
    iStateManager = CAiStateManager::NewL( *iFactory );
               
    iStateProvider = CAiStateProvider::NewL( *iStateManager );
    
    iEventHandler = CAiEventHandler::NewL( *iFactory );
    
    iUiControllerManager->SetStateHandler( *iStateProvider );
    
    __TIME_ENDMARK( "CAiFw::ConstructL, done", time );
    }

// ----------------------------------------------------------------------------
// CAiFw::NewLC()
//
// ----------------------------------------------------------------------------
//
EXPORT_C CAiFw* CAiFw::NewLC()
    {
    __TICK( "CAiFw::NewLC" );
    
    CAiFw* self = new ( ELeave ) CAiFw;
    CleanupStack::PushL( self );
    
    self->ConstructL();

    __PRINTS( "*** CAiFw::NewLC - done" );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiFw::~CAiFw()
//
// ----------------------------------------------------------------------------
//
CAiFw::~CAiFw()
    {        
    if ( iNotifyHandler )
        {
        iNotifyHandler->StopListening();
        }

    delete iNotifyHandler;
    iNotifyHandler = NULL;
    
    if ( iNotifyHandlerESS )
        {
        iNotifyHandlerESS->StopListening();
        }

    delete iNotifyHandlerESS;
    iNotifyHandlerESS = NULL;
        
    delete iWsPluginManager;
    iWsPluginManager = NULL;
    
    delete iEventHandler;
    iEventHandler = NULL;
    
    delete iStateProvider;
    iStateProvider = NULL;
    
    delete iStateManager;
    iStateManager = NULL;
    
    delete iFactory;
    iFactory = NULL;
    
    delete iUiControllerManager;
    iUiControllerManager = NULL;
                                
    delete iRepository;
    iRepository = NULL;
           
    iLibrary1.Close();
    iLibrary2.Close();
    iLibrary3.Close();
    }

// ----------------------------------------------------------------------------
// CAiFw::RunL()
//
// ----------------------------------------------------------------------------
//
EXPORT_C void CAiFw::RunL()
    {
    __TICK( "CAiFw::RunL" );
        
    __PRINTS( "*** CAiFw::RunL - CAiIdleAppRegister::NewLC" );
    __TIME_MARK( time );
            
    CAiIdleAppRegister* registry = CAiIdleAppRegister::NewLC();
    registry->RegisterL();
    CleanupStack::PopAndDestroy( registry );

    __TIME_ENDMARK( "CAiFw::RunL - CAiIdleAppRegister::NewLC, done", time );
        
    // Tell UI controller manager to start application framework and event loop.
    // This function returns only when the application is shut down.    
    iUiControllerManager->RunApplicationL();
    
    __PRINTS( "*** CAiFw::RunL - done" );
    }

// ----------------------------------------------------------------------------
// CAiFw::AppEnvReadyL()
//
// ----------------------------------------------------------------------------
//
void CAiFw::AppEnvReadyL()
    {
    __TICK( "CAiFw::AppEnvReadyL" );
    __TIME_MARK( time );
    
    // Initialize members which need to be connected to the app environment's
    // active scheduler or depend on the app environment being initialized.

    CCoeEnv& env( iUiControllerManager->CoeEnv() );

    // Create WS pluign manager
    iWsPluginManager = CAiWsPluginManager::NewL( env );
    
    // Finalise factory construction
    iFactory->ConstructL();
    
    // Start state provider
    iStateProvider->StartL( env );
               
    // CenRep notifier to listen key changes in cenrep. 
    // Application is restarted if key value is changed.
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iRepository,  
        CCenRepNotifyHandler::EIntKey, KAiMainUIController );
                                                                                                                                                  
    iNotifyHandler->StartListeningL();

    // Cenrep notifier to listen ESS changes in cenrep    
    iNotifyHandlerESS = CCenRepNotifyHandler::NewL( *this, *iRepository,                                                    
         CCenRepNotifyHandler::EIntKey, KAIExternalStatusScreen );
                                                     
    iNotifyHandlerESS->StartListeningL();
    
    __PRINTS( "*** CAiFw::AppEnvReadyL - done" );
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleUiReadyEventL()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandleUiReadyEventL( CAiUiController& aUiController )
    {         
    __TICK( "CAiFw::HandleUiReadyEventL" );
    
    if ( iUiControllerManager->IsMainUiController( aUiController ) )
        {
        iUiControllerManager->LoadUIDefinition();

        TInt value( EIdlePhase1Ok );
        
        RProperty::Get( KPSUidStartup, KPSIdlePhase1Ok, value ); 
                                                                                              
        if ( value == EIdlePhase1NOK )
            {
            __TICK( "CAiFw::HandleUiReadyEventL - Setting EIdlePhase1Ok" );
            
            RProperty::Set( KPSUidStartup, KPSIdlePhase1Ok, EIdlePhase1Ok );                                                                                                          
            }                      
    
        if ( !iLibrariesLoaded )
            {
            __PRINTS( "*** CAiFw::HandleUiReadyEventL - load libraries" );
            __TIME_MARK( time );
            
            _LIT( KAIVoiceUIDialer, "VoiceUiNameDialer.dll" );
            _LIT( KAIVoiceUIRecog, "VoiceUiRecognition.dll" );
            _LIT( KAIVCommandHandler, "vcommandhandler.dll" );
            
            iLibrary1.Load( KAIVoiceUIDialer );
            iLibrary2.Load( KAIVoiceUIRecog );
            iLibrary3.Load( KAIVCommandHandler );        
            
            iLibrariesLoaded = ETrue;
            
            __TIME_ENDMARK( "CAiFw::HandleUiReadyEventL - load libraries, done", time );            
            }             
        }
    
    __PRINTS( "*** CAiFw::HandleUiReadyEventL - done" );
    }

// ---------------------------------------------------------------------------
// CAiFw::HandleActivateUI()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandleActivateUI()
    {
    __PRINTS( "*** CAiFw::HandleActivateUI" );
    __TIME_MARK( time );
    
    iUiControllerManager->ActivateUI();     
    
    __TIME_ENDMARK( "CAiFw::HandleActivateUI, done", time );
    }

// ---------------------------------------------------------------------------
// CAiFw::HandleUiShutdown()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandleUiShutdown( CAiUiController& aUiController )
    {
    if ( iUiControllerManager->IsMainUiController( aUiController ) )
        {
        if ( iNotifyHandler )
            {
            iNotifyHandler->StopListening();
            }

        delete iNotifyHandler;
        iNotifyHandler = NULL;

        if ( iNotifyHandlerESS )
            {
            iNotifyHandlerESS->StopListening();
            }

        delete iNotifyHandlerESS;
        iNotifyHandlerESS = NULL;
        
        iUiControllerManager->DestroySecondaryUiControllers();

        delete iWsPluginManager;
        iWsPluginManager = NULL;
        
        iStateProvider->Stop();
        
        delete iFactory;
        iFactory = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAiFw::HandlePluginEvent()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandlePluginEvent( const TDesC& aParam )
    {
    iEventHandler->HandlePluginEvent( aParam );
    }

// ----------------------------------------------------------------------------
// CAiFw::HandlePluginEventL()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandlePluginEventL( const THsPublisherInfo& aPublisherInfo, 
    const TDesC& aParam )
    {
    iEventHandler->HandlePluginEventL( aPublisherInfo, aParam );
    }

// ----------------------------------------------------------------------------
// CAiFw::HasMenuItemL()
//
// ----------------------------------------------------------------------------
//
TBool CAiFw::HasMenuItemL( const THsPublisherInfo& aPublisherInfo, 
    const TDesC& aMenuItem )
    {            
    return iEventHandler->HasMenuItemL( aPublisherInfo, aMenuItem ); 
    }

// ----------------------------------------------------------------------------
// CAiFw::RefreshContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiFw::RefreshContent( const TDesC& aContentCid )
    {
    return iEventHandler->RefreshContent( aContentCid );
    }

// ----------------------------------------------------------------------------
// CAiFw::RefreshContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiFw::RefreshContent( const THsPublisherInfo& aPublisherInfo, 
    const TDesC& aContentCid ) 
    {
    return iEventHandler->RefreshContent( aPublisherInfo, aContentCid );
    }

// ----------------------------------------------------------------------------
// CAiFw::SuspendContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiFw::SuspendContent( const THsPublisherInfo& aPublisherInfo,     
    const TDesC& aContentCid )
    {
    return iEventHandler->SuspendContent( aPublisherInfo, aContentCid );
    }

// ----------------------------------------------------------------------------
// CAiFw::QueryIsMenuOpen()
//
// ----------------------------------------------------------------------------
//
TBool CAiFw::QueryIsMenuOpen()
    {
    return iUiControllerManager->MainUiController().IsMenuOpen();
    }

// ----------------------------------------------------------------------------
// CAiFw::HandleNotifyInt()
//
// ----------------------------------------------------------------------------
//
void CAiFw::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    switch ( aId )
        {
        case KAiMainUIController:
            if ( aNewValue == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
                aNewValue == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE ||
                aNewValue == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML ||
                aNewValue == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE )
                {
                iUiControllerManager->ExitMainController();
                }
            else
                {
                // Someone wrote an invalid configuration! Reset repository.               
                iRepository->Reset( KAiMainUIController );
                iRepository->Reset( KAiFirstUIController );
                iRepository->Delete( KAiFirstUIController + 1 );
                }
            break;
        case KAIExternalStatusScreen:
            if ( ( aNewValue & 0x7FFFFFFF ) != 0 )
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
//
// ----------------------------------------------------------------------------
//
void CAiFw::SwapUiControllerL( TBool aToExtHS )
    {      
    if( !aToExtHS ) // Switch to XML UI
        {
        iRepository->Create( KAiFirstUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        
        iRepository->Set( KAiFirstUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        
        iRepository->Delete( KAiFirstUIController + 1 );
        
        iRepository->Set( KAiMainUIController, 
                AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML );
        }
    else // Switch to ExtHS
        {
        iRepository->Delete( KAiFirstUIController );
        
        iRepository->Delete( KAiFirstUIController + 1 );
        
        iRepository->Set( KAiMainUIController, 
            AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE );
        }
       
    // Restart
    iUiControllerManager->ExitMainController();
    }

// ----------------------------------------------------------------------------
// CAiFw::Repository()
//
// ----------------------------------------------------------------------------
//
CRepository& CAiFw::Repository() const
    {
    return *iRepository;
    }

// End of file
