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
* Description:                  
*
*/


#include "hsps_builds_cfg.hrh"

#include <e32base.h>
#include "hspsinstaller.h"
#include "hspsthemeserver.h"
#include "hspsinstallationhandler.h"
#include "hspsthememanagement.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CHSPSInstaller::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHSPSInstaller* CHSPSInstaller::NewL(
    ChspsThemeServer& aServer )
    {
    CHSPSInstaller* self = new ( ELeave ) CHSPSInstaller( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHSPSInstaller::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHSPSInstaller::ConstructL()
    {
    iInstallationHandler = ChspsInstallationHandler::NewL( iServer );
    }

// -----------------------------------------------------------------------------
// CHSPSInstaller::CHSPSInstaller()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CHSPSInstaller::CHSPSInstaller( 
    ChspsThemeServer& aServer ): 
    CActive(EPriorityStandard), 
    iServer( aServer ) 
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHSPSInstaller::~CHSPSInstaller()
// Destructor.
// -----------------------------------------------------------------------------
//
CHSPSInstaller::~CHSPSInstaller()
    {            
    Cancel(); // Causes call to DoCancel()    
    delete iLoop;
    delete iInstallationHandler;
    }


// -----------------------------------------------------------------------------
// CHSPSInstaller::InstallConfigurationL
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage CHSPSInstaller::InstallConfigurationL( 
    const TDesC& aFileName )
    {            	
    // Start installation by reading the manifest file
    iRet = iInstallationHandler->hspsInstallTheme( aFileName, iHeaderData );    
    if ( iRet == EhspsInstallThemeSuccess && !IsActive() )
        {                
        // Continue with remaining installation phases
        SetActive();
        iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus );        
        
        // Wait until the installation phases have been executed (async->sync) 
        iLoop = new ( ELeave )CActiveSchedulerWait();
        iLoop->Start();
        
        }
    
    return iRet;
    }

// -----------------------------------------------------------------------------
// CHSPSInstaller::RunError
// -----------------------------------------------------------------------------
//
TInt CHSPSInstaller::RunError( TInt /*aError*/ )
    {
    // Called when error occurred in asynchronous request
    iLoop->AsyncStop();    
    return KErrNone;
    }  
  
// -----------------------------------------------------------------------------
// CHSPSInstaller::RunL
// -----------------------------------------------------------------------------
//
void CHSPSInstaller::RunL()
    {
    iRet = (ThspsServiceCompletedMessage)iStatus.Int();
    switch ( iStatus.Int() )
        {                     
        case EhspsInstallPhaseSuccess:
        	{
        	// Execute next phase of the installation
            if ( !IsActive() )
                {                
                SetActive();
                iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus  );
                }
        	}
        	break;
        	
        case EhspsInstallThemeSuccess:
        case EhspsInstallThemeFailed:                        
        default:
            {
            // Allow continuation of the InstallTheme function
            iLoop->AsyncStop();            
            }
        	break;
        }
    }

// -----------------------------------------------------------------------------
// CHSPSInstaller::DoCancel()
// -----------------------------------------------------------------------------
//
void CHSPSInstaller::DoCancel()
    {
    // Cancels any outstanding operation - nothing to do  
    }
        
// End of File
