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
* Description:  HSPS Theme Server Session. For more information, see the header.
*
*/


// INCLUDE FILES
#include <f32file.h>

#include "hsps_builds_cfg.hrh"
#ifdef _hsps_PERFORMANCE_TEST_
#include "hspstimemon.h"
#endif //_hsps_PERFORMANCE_TEST_
#include "hspsthemeserversession.h"
#include "hspsthemeserver.h"
#include "hspsinstallationhandler.h"
#include "hspsmaintenancehandler.h"
#include "hspsclientrequesthandler.h"
#include "hspsserverutil.h"
#include "hspsrominstaller.h"
#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbusfile.h>
#endif

// CONSTANTS

//
//
// Class ChspsThemeServerSession
//
//

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ChspsThemeServerSession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ChspsThemeServerSession::ChspsThemeServerSession(
    const TInt aAppUid ) : 
    iAppUid( aAppUid ),
    iIconFileCopyRequired( ETrue ),
    iResourceFileCopyRequired( ETrue )
    {
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ChspsThemeServerSession
// 2nd phase construct for sessions - called by the CServer framework
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CreateL()
    {
#ifdef HSPS_LOG_ACTIVE    
    iLogBus = ChspsLogBusFile::NewL( ChspsLogBusFile::CreateLogFilename( _L("themeserver_session") ) );
    iLogBus->LogText( _L( "ChspsThemeServerSession::CreateL" ) );
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
#endif
    
    Server().AddSession( this );
    iHoldingResources = EFalse;
    User::LeaveIfError( iFs.Connect() );
    Server().CheckConfigurationL( iAppUid );
    }

// Destructor
ChspsThemeServerSession::~ChspsThemeServerSession()
    {
    iFs.Close();
    
    if ( iInstallationHandler )
        {
        delete iInstallationHandler;
        Server().DefinitionRepository().Unlock();
        }
  
    delete iMaintenanceHandler;
    // decrease number of resource holders if this sessions did hold one
    if ( iHoldingResources )
        {
        Server().DecreaseRequestClientCount();      
        }
    delete iClientRequestHandler;
   
    Server().DropSession( this );

#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsThemeServerSession::~ChspsThemeServerSession" ) );
        iLogBus->LogText( _L( "--------------------------------------------------------" ) );
        }
    // Delete as last if another components use this log bus instance.
    delete iLogBus;
    iLogBus = NULL;
#endif    
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::Server
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline ChspsThemeServer& ChspsThemeServerSession::Server()
    {
    return *static_cast<ChspsThemeServer*>(const_cast<CServer2*>(CSession2::Server()));
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ServiceL
// Handle a client request.
// Leaving is handled by ChspsThemeServerSession::ServiceError() which reports
// the error code to the client
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::ServiceL( const RMessage2& aMessage )
    {
    TRAPD( errorCode, DoServiceL( aMessage ) );
    if ( errorCode && !aMessage.IsNull() )        
        {
        aMessage.Complete( EhspsServiceRequestError );    
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ServiceL: - error in calling function %d." ), aMessage.Function() );
#endif
        }

#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ServiceL completed with code: %d" ), errorCode );
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
#endif        
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::DoServiceL
// -----------------------------------------------------------------------------
void ChspsThemeServerSession::DoServiceL( const RMessage2& aMessage )    
    {    
    switch ( aMessage.Function() )
        {
        // installation:
        case EhspsInstallTheme:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsInstallTheme" ) );
#endif            
            InstallThemeL( aMessage );
            break;
            } 
          
        case EhspsInstallNextPhase:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsInstallNextPhase" ) );
#endif                   
            InstallNextPhaseL( aMessage );
            break;
            }   
          
        case EhspsCancelInstallTheme:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsCancelInstallTheme" ) );
#endif                    
            CancelInstallThemeL( aMessage );
            break;
            }
    
        case EhspsReinstallConf:
            {
#ifdef HSPS_LOG_ACTIVE
            iLogBus->LogText( _L( "DoServiceL: EhspsReinstallConf" ) );
#endif            
            ReinstallConfL( aMessage );
            break;
            }
    
            // maintenance
        case EhspsGetListHeaders:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsGetListHeaders" ) );
#endif            
            GetListHeadersL( aMessage );
            break;
            }
          
        case EhspsGetNextHeader:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsGetNextHeader" ) );
#endif                    
            GetNextHeaderL( aMessage );
            break;
            }
          
        case EhspsCancelGetListHeaders:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsCancelGetListHeaders" ) );
#endif                    
            CancelGetListHeadersL( aMessage );
            break;
            }
        
        case EhspsSetActiveTheme:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsSetActiveTheme" ) );
#endif            
            SetActiveThemeL( aMessage );
            break;
            }
    
        case EhspsRestoreDefault:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsRestoreDefault" ) );
#endif                    
            RestoreDefaultL( aMessage );
            break;
            }
          
        case EhspsRemoveTheme:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsRemoveTheme" ) );
#endif                    
            RemoveThemeL( aMessage );
            break;
            }
      
        // client request
        case EhspsGetODT:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsGetODT" ) );
#endif            
            GetOdtL( aMessage );
            break;
            }
        	
        case EhspsGetODTUpdate:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsGetODTUpdate" ) );
#endif            
            GetOdtUpdateL( aMessage );
            break;
            }
          
        case EhspsCancelGetODTUpdate:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsCancelGetODTUpdate" ) );
#endif                    
            CancelGetOdtUpdateL( aMessage );
            break;
            }
                   
        case EhspsAccessResourceFile:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsAccessResourceFile" ) );
#endif                    
            AccessResourceFileL( aMessage );
            break;
            } 
            
        case EhspsCopyResources:
        	{
#ifdef HSPS_LOG_ACTIVE    	
        	iLogBus->LogText( _L( "DoServiceL: EhspsCopyResources" ) );
#endif    	        	
        	if( iResourceFileCopyRequired )
        	    {
                CopyResourceFilesL( aMessage );
                iResourceFileCopyRequired = EFalse;
        	    }
        	else
        	    {
                aMessage.Complete( EhspsResourceCopySuccess );        	
        	    }
        	break;
        	}
        case EhspsAddPlugin:
        	{
#ifdef HSPS_LOG_ACTIVE    	
        	iLogBus->LogText( _L( "DoServiceL: EhspsAddPlugin" ) );
#endif    	        	
        	AddPluginL( aMessage );
        	break;
        	}
        case EhspsRemovePlugin:
        	{
#ifdef HSPS_LOG_ACTIVE    	
        	iLogBus->LogText( _L( "DoServiceL: EhspsRemovePlugin" ) );
#endif    	        	
        	RemovePluginL( aMessage );
        	break;
        	}
        case EhspsSetActivePlugin:
            {
#ifdef HSPS_LOG_ACTIVE      
            iLogBus->LogText( _L( "DoServiceL: EhspsSetActivePlugin" ) );
#endif                  
            SetActivePluginL( aMessage );
            break;
            }        	
        case EhspsReplacePlugin:
            {
#ifdef HSPS_LOG_ACTIVE      
            iLogBus->LogText( _L( "DoServiceL: EhspsReplacePlugin" ) );
#endif                  
            ReplacePluginL( aMessage );
            break;
            }        	
        case EhspsSetPluginSettings:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: EhspsSetPluginSettings" ) );
#endif                    
            SetPluginSettingsL( aMessage );
            break;
            } 	
        case EhspsGetPluginOdt:
        	{
#ifdef HSPS_LOG_ACTIVE    	
            iLogBus->LogText( _L( "DoServiceL: EhspsGetPluginOdt" ) );
#endif            
         	GetPluginOdtL( aMessage );
        	break;
        	}
        case EhspsMovePlugins:
        	{
#ifdef HSPS_LOG_ACTIVE    	
        	iLogBus->LogText( _L( "DoServiceL: EhspsMovePlugins" ) );
#endif    	        	
        	MovePluginsL( aMessage );
        	break;
        	}
        case EhspsSetConfState:
            {
#ifdef HSPS_LOG_ACTIVE    	        
            iLogBus->LogText( _L( "DoServiceL: EhspsSetConfState" ) );
#endif    
            SetConfStateL( aMessage );
            break;
            }
	    case EhspsRestoreActiveAppConf:
	        {
#ifdef HSPS_LOG_ACTIVE              
	        iLogBus->LogText( _L( "DoServiceL: EhspsRestoreActiveAppConf" ) );
#endif
	        RestoreActiveAppConfL( aMessage );
	        break;
	        }
	    case EhspsRestoreConfigurations:
	        {
#ifdef HSPS_LOG_ACTIVE              
            iLogBus->LogText( _L( "DoServiceL: EhspsRestoreConfigurations" ) );
#endif
            RestoreConfigurationsL( aMessage );
            break;	        
	        }
        default:
            {
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "DoServiceL: - unrecognized function %d called." ), aMessage.Function() );
#endif                                    
            User::Leave( KErrNotSupported ); 
            break;
            }
            
        } // switch   
    }
  

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ServiceError
// Handle an error from ChspsThemeServerSession::ServiceL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
    {
#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "hspsThemeServer: ChspsThemeServerSession::ServiceError(): %d" ), aError ); 
#endif
            
    CSession2::ServiceError(aMessage,aError);
    }

// Installation 
// ************
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::InstallThemeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::InstallThemeL(const RMessage2& aMessage)
    {
    // delete previous handler and create new one for this installation
    if ( iInstallationHandler )
        {
        delete iInstallationHandler;
        iInstallationHandler = NULL;
        }
    iInstallationHandler = ChspsInstallationHandler::NewL( Server() );
    
#ifdef HSPS_LOG_ACTIVE    
    // set logging bus.
    iInstallationHandler->SetLogBus( iLogBus );
#endif    
    
    // call the service
    iInstallationHandler->ServiceInstallThemeL(aMessage);
    } 

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::InstallNextPhaseL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::InstallNextPhaseL(const RMessage2& aMessage)
    {
    // make an asynchronous request
    if (!iInstallationHandler)
        {
        User::Leave( KErrNotReady );
        }
    else
        {
        // call the service
        iInstallationHandler->ServiceInstallNextPhaseL( aMessage );
        }
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CancelInstallTheme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CancelInstallThemeL(const RMessage2& aMessage)
    {
    if ( iInstallationHandler )
        {
        // wait until service has finalised itself
        ThspsServiceCompletedMessage ret = iInstallationHandler->hspsCancelInstallTheme();
        // clean up the service
        delete iInstallationHandler;
        iInstallationHandler = NULL;
        if ( !aMessage.IsNull() )
    	    {
       	    aMessage.Complete( ret );	
    	    }
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    } 

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ReinstallConf
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::ReinstallConfL( const RMessage2& aMessage )
    {

    ThspsParamReinstallConf params;        
    TPckg<ThspsParamReinstallConf> packagedStruct( params );    
    aMessage.ReadL( 1, packagedStruct );                      

    Server().ReinstallConfL( params.appUid, params.confUId );
        
    aMessage.Complete( EhspsReinstallConfSuccess );
    
    } 


// Maintenance 
// ***********
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::GetListHeadersL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::GetListHeadersL(const RMessage2& aMessage)
    {
    // Exception: should be reset since previous instance might be pending
    if ( iMaintenanceHandler )
        {
        delete iMaintenanceHandler;
        iMaintenanceHandler = NULL;
        }
    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceGetListHeadersL( aMessage );
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::GetNextHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::GetNextHeaderL(const RMessage2& aMessage)
    {
    if ( !iMaintenanceHandler )
        {
        User::Leave( KErrNotReady );
        }
    else
        {
        iMaintenanceHandler->ServiceGetNextHeaderL(aMessage); 
        }
    } 
  
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CancelGetListHeaders
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CancelGetListHeadersL(const RMessage2& aMessage)
    {
    if ( iMaintenanceHandler )
        {
        ThspsServiceCompletedMessage ret = iMaintenanceHandler->hspsCancelGetListHeaders();
        delete iMaintenanceHandler;
        iMaintenanceHandler = NULL;  
        if ( !aMessage.IsNull() )
    	    {
       	    aMessage.Complete( EhspsServiceRequestCanceled );	
    	    }	       
        }
    else
        {
        User::Leave( KErrNotReady );  
        }
    } 
  

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetActiveThemeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetActiveThemeL(const RMessage2& aMessage)
    {
    // make a synchronous request    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceSetActiveThemeL(aMessage);
    }
  

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::RestoreDefaultL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::RestoreDefaultL(const RMessage2& aMessage)
    {
    // make a synchronous request    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );    
    iMaintenanceHandler->ServiceRestoreDefaultL(aMessage);
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::RemoveThemeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::RemoveThemeL(const RMessage2& aMessage)
    {
    // make a synchronous request    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );    
    iMaintenanceHandler->ServiceRemoveThemeL(aMessage);
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::AddPluginL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::AddPluginL(const RMessage2& aMessage)
    {
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceAddPluginL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::RemovePluginL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::RemovePluginL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceRemovePluginL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetActivePluginL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetActivePluginL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceSetActivePluginL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ReplacePluginL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::ReplacePluginL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceReplacePluginL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetPluginSettingsL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetPluginSettingsL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceSetPluginSettingsL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::GetPluginOdtL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::GetPluginOdtL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceGetPluginOdtL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::MovePluginsL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::MovePluginsL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceMovePluginsL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetConfStateL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetConfStateL(const RMessage2& aMessage)
    {    
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceSetConfStateL( aMessage );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::RestoreActiveAppConfL
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::RestoreActiveAppConfL(const RMessage2& aMessage)
    {
	CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceRestoreActiveAppConfL( aMessage );
    }

// Client Request 
// **************
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::GetOdtL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::GetOdtL(const RMessage2& aMessage)
    {
    //Make an asynchronous request    
    CreateClientRequestHandlerL();    
    iClientRequestHandler->ServiceGetOdtL( aMessage );
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::GetOdtUpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::GetOdtUpdateL(const RMessage2& aMessage)
    {
    // make an asynchronous request 
    if (!iClientRequestHandler)
        {
        User::Leave( KErrNotReady );
        }
    else
        {
        iClientRequestHandler->ServiceGetOdtUpdateL(aMessage);  
        }
    } 

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::RestoreConfigurationsL
// -----------------------------------------------------------------------------
void ChspsThemeServerSession::RestoreConfigurationsL( const RMessage2& aMessage )
    {
    CreateMaintenanceHandlerL( aMessage.SecureId().iId );
    iMaintenanceHandler->ServiceRestoreConfigurationsL( aMessage );        
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CancelGetOdtUpdate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CancelGetOdtUpdateL(const RMessage2& aMessage)
    {
    if ( iClientRequestHandler )
        {
        ThspsServiceCompletedMessage ret = iClientRequestHandler->hspsCancelGetODTUpdate();
        if ( !aMessage.IsNull() )
    	    {
       	    aMessage.Complete( EhspsServiceRequestCanceled );	
    	    }
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    } 

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::AccessResourceFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::AccessResourceFileL(const RMessage2& aMessage)
    {
    //Make a synchronous request    
    CreateClientRequestHandlerL();        
        
#ifdef _hsps_PERFORMANCE_TEST_
        ChspsTimeMon::PrintUserMem( _L("hspsThemeServer::AccessResourceFileL(): - accessing..") );
#endif //_hsps_PERFORMANCE_TEST_        

    // call the service with TRAP
    iClientRequestHandler->ServiceAccessResourceFileL( aMessage );
    if ( !iHoldingResources )
        {
        iHoldingResources = ETrue;  
        // add number of resource holders
        Server().IncreaseRequestClientCount();
        }

#ifdef _hsps_PERFORMANCE_TEST_
        ChspsTimeMon::PrintUserMem( _L("hspsThemeServer::AccessResourceFileL(): - file accessed.") );
#endif //_hsps_PERFORMANCE_TEST_    

    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::FindWidgetUids
// -----------------------------------------------------------------------------
void ChspsThemeServerSession::FindWidgetUidsL(
        ChspsODT& aAppODT,
        RArray<TInt>& aUidArray ) 
    {    
    aUidArray.Reset();
    const TInt resourceCount = aAppODT.ResourceCount();
    for( TInt resourceIndex = 0; resourceIndex < resourceCount; resourceIndex++ )
        {      
        ChspsResource& r = aAppODT.ResourceL( resourceIndex );
        const TInt uid = r.ConfigurationUid();        
        if ( uid > 0 &&
             aUidArray.Find( uid ) == KErrNotFound )
            {
            aUidArray.AppendL( uid );
            }
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CopyResourceFilesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CopyResourceFilesL(const RMessage2& aMessage)
	{	 
	// read paths from the buffer
	TFileName odtPath;
    TPath destinationPath;  // Private root folder of the client application
	aMessage.ReadL( 1, odtPath, 0 );
	aMessage.ReadL( 2, destinationPath, 0 );
	    	
	ChspsODT* odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
	
    // phase counter for cleaning and debug prints
    TInt phaseCounter = 1;
    
	RFile odtfile;
	CleanupClosePushL( odtfile );	    
    TInt error = odtfile.Open( iFs, odtPath, EFileRead );            	
    if( error == KErrNone )
    	{
    	phaseCounter++;
    	
	    // set stream on file
    	CFileStore* store = CDirectFileStore::FromLC( odtfile );
	    RStoreReadStream instream;
	    CleanupClosePushL( instream );
		instream.OpenLC( *store, store->Root() );
		
	    // stream in the ODT
		instream >> *odt;
		
		// the file is no longer needed
		odtfile.Close();
		
		// destroy the stream object and close the instream
	    CleanupStack::PopAndDestroy(3); // instream,store 
    	}
    
    CleanupStack::PopAndDestroy(1, &odtfile); // odtfile
    
    if( error == KErrNone )
 		{
 		phaseCounter++;
 		 		
        RArray<TInt> uidArray;
        CleanupClosePushL( uidArray );
        
        // Find unique configuration UIDs from the resource array
        FindWidgetUidsL( *odt, uidArray );
 		        
        CFileMan* fileManager = NULL;
 		fileManager = CFileMan::NewL( iFs );
 		CleanupStack::PushL( fileManager );
 		                        
        // Loop widgets belonging to the root configuration
        const TInt uidCount = uidArray.Count();        
	    for( TInt i = 0; i < uidCount && !error; i++ )
	    	{	    		    	
	    	// Copy widget's resources to client's private folder	    	
	    	error = hspsServerUtil::CopyResourceFilesL(
                    *odt,
                    iFs,
                    *fileManager,
                    Server().DeviceLanguage(),                     
                    uidArray[i],
                    destinationPath );	    
    		}
	    
	    uidArray.Reset();
	    
	    CleanupStack::PopAndDestroy( 2, &uidArray ); // fileManager, uidArray    	           
 		} // KErrNone
 	
 	//Tidy-up
    if ( odt )
        {
        CleanupStack::PopAndDestroy( odt );
        }    
 	 	 	       
    if( error )
    	{
#ifdef HSPS_LOG_ACTIVE    	
        iLogBus->LogText( _L( "ChspsThemeServerSession::CopyResourceFilesL(): - creating target directory failed! ( error : %d in phase: %d)." ),
                error,
                phaseCounter );
#endif    	                
    	                
    	aMessage.Complete( EhspsResourceCopyFailed );
    	}
    else
    	{
    	aMessage.Complete( EhspsResourceCopySuccess );
    	}
	}
// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CreateMaintenanceHandlerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CreateMaintenanceHandlerL(
        const TUint aSecureId )
    {
    if( !iMaintenanceHandler )
        {        
        iMaintenanceHandler = ChspsMaintenanceHandler::NewL( Server(), aSecureId );
        iMaintenanceHandler->SetServerSession( this );
#ifdef HSPS_LOG_ACTIVE    
        iMaintenanceHandler->SetLogBus( iLogBus );
#endif
        }

    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::CreateClientRequestHandlerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::CreateClientRequestHandlerL()
    {
    if( !iClientRequestHandler )
        {        
        iClientRequestHandler = ChspsClientRequestHandler::NewL( Server() );

#ifdef HSPS_LOG_ACTIVE  
        iClientRequestHandler->SetLogBus( iLogBus );        
#endif  
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::FileSystem
// -----------------------------------------------------------------------------
//
RFs& ChspsThemeServerSession::FileSystem()
    {
    return iFs;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::IconFileCopyRequired
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServerSession::IconFileCopyRequired() const
    {
    return iIconFileCopyRequired;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetIconFileCopyRequired
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetIconFileCopyRequired( const TBool aCopyRequired )
    {
    iIconFileCopyRequired = aCopyRequired;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::ResourceFileCopyRequired
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServerSession::ResourceFileCopyRequired() const
    {
    return iResourceFileCopyRequired;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::SetResourceFileCopyRequired
// -----------------------------------------------------------------------------
//
void ChspsThemeServerSession::SetResourceFileCopyRequired( const TBool aCopyRequired )
    {
    iResourceFileCopyRequired = aCopyRequired;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::AppUid
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServerSession::AppUid() const
    {
    return iAppUid;
    }

// end of file

