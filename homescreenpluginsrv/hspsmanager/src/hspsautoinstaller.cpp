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
* Description:  Server side installation of SISX imports
*
*/


#include "hsps_builds_cfg.hrh"

#include <e32base.h>

#include "hspsthemeserver.h"
#include "hspsautoinstaller.h"
#include "hspsinstallationhandler.h"
#include "hspsmaintenancehandler.h"
#include "hspsuimanagererrorcodes.h"
#include "hspsodt.h"
#include "hspsresult.h"
#include "hspsthememanagement.h"

_LIT( KThemeDirectory,"c:\\private\\200159c0\\themes\\"); 
_LIT ( KDoubleBackSlash, "\\" );

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
ChspsAutoInstaller* ChspsAutoInstaller::NewL( ChspsThemeServer& aThemeServer )
    {
    ChspsAutoInstaller* self = NewLC( aThemeServer );
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
ChspsAutoInstaller* ChspsAutoInstaller::NewLC( ChspsThemeServer& aThemeServer )
    {
    ChspsAutoInstaller* self = new ( ELeave ) ChspsAutoInstaller( aThemeServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsAutoInstaller::ConstructL()
    {
    iResult = ChspsResult::NewL();
    iInstallationHandler = ChspsInstallationHandler::NewL( iThemeServer );
    iMaintenanceHandler = ChspsMaintenanceHandler::NewL( iThemeServer );    
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::ChspsAutoInstaller()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
ChspsAutoInstaller::ChspsAutoInstaller( ChspsThemeServer& aThemeServer )
: CActive( EPriorityStandard ), iThemeServer( aThemeServer )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::~ChspsAutoInstaller()
// Destructor.
// -----------------------------------------------------------------------------
//
ChspsAutoInstaller::~ChspsAutoInstaller()
    {
    Cancel(); // Causes call to DoCancel()
    delete iResult;
    delete iInstallationHandler;
    delete iMaintenanceHandler;
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::UnInstallTheme
// Calls the ChspsMaintenanceHandler to remove theme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsAutoInstaller::UnInstallThemeL( const TDesC& aPathFile )
    {
    // Strip app_/plugin_ prefix from the filename
    TPtrC fixedName;
    const TChar KCharUnderscore('_');    
    TInt offset = aPathFile.Locate( KCharUnderscore );
    if( offset )
        {                    
        TPtrC filePrefix( aPathFile.Left(offset+1) );                
    	fixedName.Set( aPathFile.Mid( filePrefix.Length() ) );    
        }    
    if ( !fixedName.Length() )
    	{
    	// TODO handle failure        
        iThemeServer.HandleAutoInstallerEvent( EhspsRemoveThemeFailed );
    	return;
    	}                   
    
    TParsePtrC parsePtr( fixedName );                
    TPtrC fileName( parsePtr.Name() );
                        
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr( path->Des() );    
    pathPtr.Append( KThemeDirectory );
    
    // Generate a file path from the file name by replacing underscores with backslashes and
    // by converting subdirectory names from hex format to int values 
    TLex lex( fileName );            
    lex.Mark();
    while( !lex.Eos() )
        {
        if( lex.Peek() == TChar( '_' ) )
            {            
            TPtrC token( lex.MarkedToken() );            
            TLex tmp( token );            
            
            TInt64 val( 0 );
            tmp.Val( val, EHex );            
            
            pathPtr.AppendNum( val );
            lex.Inc();             
            pathPtr.Append( KDoubleBackSlash );
            lex.Mark();
            }            
        else 
            {
            lex.Inc();      
            }       
        }

    // Last token is the theme version string            
    TPtrC token( lex.MarkedToken() );            
    pathPtr.Append( token );                           
    pathPtr.Append( KDoubleBackSlash );            
                   
    ChspsODT* odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
                              
    iThemeServer.DefinitionRepository().GetOdtHeaderL( pathPtr, ELangTest /* not used */, *odt );
    
    ThspsServiceCompletedMessage ret = iMaintenanceHandler->hspsRemoveThemeL( *odt );                                 
        
    if( ret != EhspsRemoveThemeSuccess )        
        {
        // Use brute force to remove the theme. This should not happen
        iThemeServer.DefinitionRepository().RemoveThemeL( *odt );
        }
    
    // It is success
    iThemeServer.HandleAutoInstallerEvent( EhspsRemoveThemeSuccess );
    
    CleanupStack::PopAndDestroy( 2, path ); // odt, path            
    }
    
// -----------------------------------------------------------------------------
// ChspsAutoInstaller::InstallThemeL
// Calls the hspsInstallationHandlers hspsInstallTheme and hspsInstallNextPhase methods
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsAutoInstaller::InstallThemeL( const TDesC& aManifestFileName )
    {
    ThspsServiceCompletedMessage ret = 
        iInstallationHandler->hspsInstallTheme( aManifestFileName, iHeaderData );    
        
    // without security check, EhspsInstallThemeSuccess is returned insteads of EhspsInstallPhaseSuccess
#ifndef __WINS__   
    if ( !iInstallationHandler->CheckAutoInstallationValidityL() )
        {
        iThemeServer.HandleAutoInstallerEvent( EhspsInstallThemeFailed );
        return KErrGeneral;
        }
#endif
            
    if ( ret == EhspsInstallThemeSuccess && !IsActive() )
        {
        SetActive();
        iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus  );
        }
    else
        {
        if ( iInstallationHandler->iResult->iXuikonError == KErrXmlFileNotFound
                || iInstallationHandler->iResult->iXuikonError == KErrCssFileNotFound
                || iInstallationHandler->iResult->iXuikonError == KErrDtdFileNotFound
                || iInstallationHandler->iResult->iXuikonError == KErrResourceFileNotFound )
            {
            return KErrNotFound;
            }
        iThemeServer.HandleAutoInstallerEvent( EhspsInstallThemeFailed );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::SetLogBus()
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE        
void ChspsAutoInstaller::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    iInstallationHandler->SetLogBus( aLogBus );
    }
#endif
   
// -----------------------------------------------------------------------------
// ChspsRequestClient::GethspsResultL
// Gets the installation result status
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ChspsResult& ChspsAutoInstaller::hspsResult()
    {
    return *iInstallationHandler->iResult;
    }  
                   
// -----------------------------------------------------------------------------
// ChspsAutoInstaller::RunError
// From CActive. Called when error occurred in asynchronous request
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsAutoInstaller::RunError( TInt /*aError*/ )
    {
    iThemeServer.HandleAutoInstallerEvent( EhspsServiceRequestError );
    return KErrNone;
    }  

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::RunL()
// From CActive. Called when asynchronous request is completed. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsAutoInstaller::RunL()
    {
    switch ( iStatus.Int() )
        {
        case EhspsInstallThemeSuccess:
        	{
          iThemeServer.HandleAutoInstallerEvent( EhspsInstallThemeSuccess );
        	}
            break;         
    
        case EhspsInstallPhaseSuccess:
        	{
            iThemeServer.HandleAutoInstallerEvent( EhspsInstallPhaseSuccess );
            // it cannot be active anymore in here, however..
            if ( !IsActive() )
                {
                SetActive();
                iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus  );
                }
            else
                {
                iThemeServer.HandleAutoInstallerEvent( EhspsInstallThemeFailed );
                }
        	}
            break;         
        
        default:
            {
            iThemeServer.HandleAutoInstallerEvent( (ThspsServiceCompletedMessage)iStatus.Int() );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// ChspsAutoInstaller::DoCancel()
// Cancels any outstanding operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsAutoInstaller::DoCancel()
    {
    // nothing to do  
    }
        
// End of File
