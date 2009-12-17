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
* Description:  UI controller manager class
*
*/


#include "aiuicontrollermanager.h"
#include "aiuicontroller.h"
#include "aicontentpublisher.h"
#include "aicontentobserver.h"
#include "activeidle2domaincrkeys.h"
#include "aifwpanic.h"
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <centralrepository.h>

#include <aisystemuids.hrh>

#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiUiControllerManager::CAiUiControllerManager()
// ----------------------------------------------------------------------------
//
CAiUiControllerManager::CAiUiControllerManager()
    {
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::LoadMainControllerL()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::LoadMainControllerL(
    CRepository& aCenRepConfig )
    {
    TInt value( 0 );
    
    // Main UI Controller must be configured correctly
    User::LeaveIfError( aCenRepConfig.Get( KAiMainUIController, value ) );
    
    CAiUiController* controller = CAiUiController::NewL( TUid::Uid( value ) );
    
    iCreatedUICList.Append( value );
    CleanupStack::PushL( controller );
    
    iMainUiController = controller->MainInterface();
    
    // Main UI controller must be configured correctly
    if( !iMainUiController )
        {
        __PRINT( __DBG_FORMAT("Main UI controller interface not found from controller 0x%x"), value );
        User::Leave( KErrNotFound );
        }
    
    iUiControllerArray.AppendL( controller );
    CleanupStack::Pop( controller );
    
    // Register this as a UI framework observer of the main UI controller
    iMainUiController->SetUiFrameworkObserver( *this );
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::LoadSecondaryControllersL()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::LoadSecondaryControllersL(
    CRepository& aCenRepConfig )
    {
    TInt value( 0 );
    
    // Instantiate rest of the UI controllers.
    for( TInt key = KAiFirstUIController; 
         key <= KAiLastUIController && aCenRepConfig.Get( key, value ) == KErrNone;
        ++key )
        {
        // skip empty entries
        if( value == 0 )
            {
            continue;
            }
        
        if( iCreatedUICList.Find( value ) != KErrNotFound )
            {
            continue;
            }
        
        iCreatedUICList.Append( value );
        
        CAiUiController* controller = 
            CAiUiController::NewL( TUid::Uid( value ) );
        
        CleanupStack::PushL( controller );
        
        iUiControllerArray.AppendL( controller );
        CleanupStack::Pop( controller );
        
        // Get the secondary interface
        MAiSecondaryUiController* secController( 
                    controller->SecondaryInterface() ); 
                        
        if( secController )
            {
            MAiUiFrameworkObserver* uiFwObserver(
                    secController->UiFrameworkObserver() );
        
            if( uiFwObserver )
                {
                // Add secondary controller as UI framework event observer. 
                User::LeaveIfError( 
                        iUiFrameworkObservers.InsertInAddressOrder( uiFwObserver ) );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::ConstructL()
    {
    __HEAP("FW: Init - Create UI Ctrls");
    __TIME_MARK(t);

    CRepository* cenRep( NULL );

#if 0
    // For AI3_test    
    RProcess proc;
    // 0x102750F0 in AI3, 0x2001CB4F in AI3_Test
    TSecureId secId( proc.SecureId() ); 
                                           
    if( secId == 0x2001CB4F )
        {
        cenRep = CRepository::NewL( TUid::Uid( 0x2001952B ) );
        }   
    else
        {
        cenRep = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
        }
#else
    cenRep = CRepository::NewLC( TUid::Uid( KCRUidActiveIdleLV ) );
#endif

    LoadMainControllerL( *cenRep );
    
    // Failing on secondary is not fatal. Ignore leaves.
    TRAP_IGNORE( LoadSecondaryControllersL( *cenRep ) );
                       
    CleanupStack::PopAndDestroy( cenRep );
    
    __TIME_ENDMARK("FW: Create UI Ctrls", t);
    __HEAP("FW: Done - Create UI Ctrls");
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::NewL()
// ----------------------------------------------------------------------------
//
CAiUiControllerManager* CAiUiControllerManager::NewL()
    {
    CAiUiControllerManager* self = new (ELeave) CAiUiControllerManager;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::~CAiUiControllerManager()
// ----------------------------------------------------------------------------
//
CAiUiControllerManager::~CAiUiControllerManager()
    {
    iUiControllerArray.ResetAndDestroy();
    
    iUiFrameworkObservers.Reset();
    
    iCreatedUICList.Reset();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::UiControllers() 
// ----------------------------------------------------------------------------
//
RPointerArray< CAiUiController >& CAiUiControllerManager::UiControllers() const
    {
    return iUiControllerArray;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::IsMainUiController()
// ----------------------------------------------------------------------------
//
TBool CAiUiControllerManager::IsMainUiController(
    CAiUiController& aUiController ) const
    {
    return ( aUiController.MainInterface() == iMainUiController );
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::SetEventHandler()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::SetEventHandler(
    MAiFwEventHandler& aEventHandler )
    {
    for ( TInt i = 0; i < iUiControllerArray.Count(); i++ )
        {
        iUiControllerArray[i]->SetEventHandler( aEventHandler );
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::RunApplicationL()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::RunApplicationL()
    {
    iMainUiController->RunApplicationL();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::ActivateUI()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::ActivateUI()
    {
    for( TInt i = 0; i < iUiControllerArray.Count(); i++ )
        {
        iUiControllerArray[i]->ActivateUI();
        }
    }
    
// ----------------------------------------------------------------------------
// CAiUiControllerManager::LoadUIDefinition()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::LoadUIDefinition()
    {       
    for( TInt i = 0; i < iUiControllerArray.Count(); i++ )
        {
        CAiUiController* uiCtl( iUiControllerArray[i] );
        
        MAiSecondaryUiController* secIntr( uiCtl->SecondaryInterface() );
        
        if( secIntr )
            {
            secIntr->SetCoeEnv( CoeEnv() );
            }
        
        TRAPD( err, uiCtl->LoadUIDefinitionL() );
        
        if( err != KErrNone )            
            {
            if( IsMainUiController( *uiCtl ) )
                {
                // Main ui controller failing is fatal
                _LIT(KAIFWStartupFailed, "FW startup failed.");
                
                User::Panic( KAIFWStartupFailed, 0 );                
                }
            else
                {
                // Secondary UI controller failed, delete it
                delete uiCtl;
                uiCtl = NULL;
                
                iUiControllerArray.Remove( i );               
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::CoeEnv()
// ----------------------------------------------------------------------------
//
CCoeEnv& CAiUiControllerManager::CoeEnv() const
    {
    return iMainUiController->CoeEnv();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::MainUiController()
// ----------------------------------------------------------------------------
//
MAiMainUiController& CAiUiControllerManager::MainUiController() const
    {
    return *iMainUiController;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::DestroySecondaryUiControllers()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::DestroySecondaryUiControllers()
    {
    for( TInt i = iUiControllerArray.Count() - 1; i >= 0; --i )
        {
        CAiUiController* uiController = iUiControllerArray[i];
        
        if( !IsMainUiController( *uiController ) )
            {
            delete uiController;
            uiController = NULL;
            
            iUiControllerArray.Remove(i);
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::ExitMainController()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::ExitMainController()
    {
    iMainUiController->Exit();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::AddObserverL()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::AddObserverL( 
    MAiUiFrameworkObserver& aUiFwObserver )
    {
    User::LeaveIfError( 
            iUiFrameworkObservers.InsertInAddressOrder( &aUiFwObserver ) );
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::RemoveObserver()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::RemoveObserver( 
    MAiUiFrameworkObserver& aUiFwObserver )
    {
    TInt index( iUiFrameworkObservers.FindInAddressOrder( &aUiFwObserver ) );
    
    if( index != KErrNotFound )
        {
        iUiFrameworkObservers.Remove( index );
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::RemovePluginFromUI()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::RemovePluginFromUI( CAiContentPublisher& aPlugin )
    {
    // Get MAiPropertyExtension from plugin
    MAiPropertyExtension* plugin =
        static_cast< MAiPropertyExtension* >( 
                aPlugin.Extension( KExtensionUidProperty ) );
    
    // Inform all UI controller that this plugin need to be removed from UI.
    if( plugin )
        {
        for ( TInt i = 0; i < iUiControllerArray.Count(); i++ )
            {
            iUiControllerArray[i]->RemovePluginFromUI( *plugin );
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::HandleResourceChange()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::HandleResourceChange( TInt aType )
    {
    for( TInt i = 0; i < iUiFrameworkObservers.Count(); i++ )
        {
        iUiFrameworkObservers[i]->HandleResourceChange( aType );
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::HandleForegroundEvent()
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::HandleForegroundEvent( TBool aForeground )
    {
    for( TInt i = 0; i < iUiFrameworkObservers.Count(); i++ )
        {
        iUiFrameworkObservers[i]->HandleForegroundEvent( aForeground );
        }
    }
    
// End of file
