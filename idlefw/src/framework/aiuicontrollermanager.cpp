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

// System includes
#include <centralrepository.h>

// User includes
#include <aisystemuids.hrh>
#include "aiuicontroller.h"
#include "aicontentobserver.h"
#include "activeidle2domaincrkeys.h"
#include "aifw.h"

#include "aiuicontrollermanager.h"

#include "aifwpanic.h"

#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiUiControllerManager::CAiUiControllerManager()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerManager::CAiUiControllerManager()
    {
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::LoadMainControllerL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::LoadMainControllerL( CRepository& aRepository )    
    {
    TInt value( 0 );
    
    // Main UI Controller must be configured correctly
    User::LeaveIfError( aRepository.Get( KAiMainUIController, value ) );
    
    CAiUiController* controller = CAiUiController::NewL( TUid::Uid( value ) );
    CleanupStack::PushL( controller );

    iCreatedUICList.AppendL( value );   
    
    iMainUiController = controller->MainInterface();
    
    // Main UI controller must be configured correctly
    if( !iMainUiController )
        {
        __PRINT( __DBG_FORMAT("Main UI controller interface not found from controller 0x%x"), value );
        User::Leave( KErrNotFound );
        }
    
    iUiControllerArray.AppendL( controller );
    CleanupStack::Pop( controller );    
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::LoadSecondaryControllersL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::LoadSecondaryControllersL(
    CRepository& aRepository )
    {
    TInt value( 0 );
    
    // Instantiate rest of the UI controllers.
    for( TInt key = KAiFirstUIController; 
         key <= KAiLastUIController && aRepository.Get( key, value ) == KErrNone;
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
        
        iCreatedUICList.AppendL( value );
        
        CAiUiController* controller = 
            CAiUiController::NewL( TUid::Uid( value ) );
        
        CleanupStack::PushL( controller );
        
        iUiControllerArray.AppendL( controller );
        CleanupStack::Pop( controller );        
        }
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::ConstructL( CAiFw* aAiFw )
    {
    __HEAP("FW: Init - Create UI Ctrls");
    __TIME_MARK(t);
   
    CRepository& repository( aAiFw->Repository() );
    
    LoadMainControllerL( repository );
    
    // Failing on secondary is not fatal. Ignore leaves.
    TRAP_IGNORE( LoadSecondaryControllersL( repository ) );
                           
    for ( TInt i = 0; i < iUiControllerArray.Count(); i++ )
        {
        iUiControllerArray[i]->SetEventHandler( *aAiFw );
        }
        
    __TIME_ENDMARK("FW: Create UI Ctrls", t);
    __HEAP("FW: Done - Create UI Ctrls");
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::NewL()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerManager* CAiUiControllerManager::NewL( CAiFw* aAiFw )
    {
    CAiUiControllerManager* self = 
        new ( ELeave ) CAiUiControllerManager;
    
    CleanupStack::PushL( self );
    self->ConstructL( aAiFw );
    CleanupStack::Pop( self ); // self
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::~CAiUiControllerManager()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerManager::~CAiUiControllerManager()
    {
    iUiControllerArray.ResetAndDestroy();
           
    iCreatedUICList.Reset();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::UiControllers()
//
// ----------------------------------------------------------------------------
//
RPointerArray< CAiUiController >& CAiUiControllerManager::UiControllers() const
    {
    return iUiControllerArray;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::IsMainUiController()
//
// ----------------------------------------------------------------------------
//
TBool CAiUiControllerManager::IsMainUiController(
    CAiUiController& aUiController ) const
    {
    return ( aUiController.MainInterface() == iMainUiController );
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::RunApplicationL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::RunApplicationL()
    {
    iMainUiController->RunApplicationL();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::ActivateUI()
//
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
//
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
//
// ----------------------------------------------------------------------------
//
CCoeEnv& CAiUiControllerManager::CoeEnv() const
    {
    return iMainUiController->CoeEnv();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::MainUiController()
//
// ----------------------------------------------------------------------------
//
MAiMainUiController& CAiUiControllerManager::MainUiController() const
    {
    return *iMainUiController;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerManager::DestroySecondaryUiControllers()
//
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
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::ExitMainController()
    {
    iMainUiController->Exit();
    }
    
// ----------------------------------------------------------------------------
// CAiUiControllerManager::SetStateHandler()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerManager::SetStateHandler( MAiFwStateHandler& aHandler )
    {
    for ( TInt i = 0; i < iUiControllerArray.Count(); i++ )
        {
        iUiControllerArray[i]->SetStateHandler( aHandler );
        }    
    }

// End of file
