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
* Description:  Implementaion of ChspsClientSession class.
*                For details, see hspsClientSession.h.
*
*
*/


#include <e32svr.h>
#include "hsps_builds_cfg.hrh"
#include "hspsthememanagement.h"
#include "hspsthemeserver.h"
#include "hspsclientsession.h"

const TInt KRetryMax = 256;
const TInt KRetryInterval = 100000; // 100ms

// Standard server startup code
static TInt StartServer()
    {
    const TUidType serverUid(KNullUid,KNullUid,KhspsThemeServerUid3);
    // ------------------------------------------------------------------------ 
    // EPOC and EKA2 just create a new server process. Simultaneous
    // launching of two such processes should be detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    // ------------------------------------------------------------------------
    RProcess server;
    TInt r=server.Create(KhspsThemeServerName,KNullDesC);
    if (r!=KErrNone)
        {
#ifdef _hsps_DEBUG_          
        RDebug::Print(_L("hspsClientSession: server start failed %d"),r);
#endif        
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat!=KRequestPending)
        {
        server.Kill(0);   // abort startup
        }
    else
        {
        server.Resume();  // logon OK - start the server
        }

    User::WaitForRequest(stat);   // wait for start or death
    // ------------------------------------------------------------------------ 
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    ///
    r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }


// -----------------------------------------------------------------------------
// RhspsClientSession::Connect
// This is the standard retry pattern for server connection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::Connect()
    {     
    TVersion ver = TVersion( KhspsThemeServerMajorVersionNumber,
                             KhspsThemeServerMinorVersionNumber,
                             KhspsThemeServerBuildVersionNumber);
    
    TInt retry = KRetryMax;
    
    for(;;)
        {
        TInt r = CreateSession( KhspsThemeServerName, ver, KDefaultMessageSlots );
  
        if( r != KErrNotFound && r != KErrServerTerminated )
            {
            return r;
            }        

        retry--;
        if( retry == 0 )
            {
            return r;
            }        
        
        r = StartServer();
        if( r != KErrNone && r != KErrAlreadyExists )
            {
            return r;
            }        
        
        if( r == KErrAlreadyExists )
            {
            User::After( KRetryInterval );
            }
        }
    }
  
// -----------------------------------------------------------------------------
// RhspsClientSession::Close()
// Closes the session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RhspsClientSession::Close()
    {
    RSessionBase::Close();
    }


// ----------------------------------------------------------------------------------------
// Client Server functions follow
// ----------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// RhspsClientSession::InstallTheme
// Installation service
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::InstallTheme(TDes8& aResultData, const TDesC& aManifestFileName,
                       TDes8& aHeaderData)
    {
    aHeaderData.Zero();
    aResultData.Zero();
    return SendReceive(EhspsInstallTheme,TIpcArgs(&aResultData, &aManifestFileName, &aHeaderData)); 
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::InstallNextPhase
// Calls for the next phase of the installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RhspsClientSession::InstallNextPhase(TDes8& aResultData, TDes8& aHeaderData,
                         TRequestStatus& aStatus)
    {
    aHeaderData.Zero();
    aResultData.Zero();
    TIpcArgs args;
    args.Set(0, &aResultData );
    args.Set(1, &KNullDesC8);
    args.Set(2, &aHeaderData);
    SendReceive(EhspsInstallNextPhase, args, aStatus);  
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::GetListHeaders
// Maintenance service, header listing
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::GetListHeaders(TDes8& aResultData, const TDesC8& aSearchMaskData,
                         const TBool aCopyLogos, TDes8& aHeaderData)
    {
    aHeaderData.Zero();
    aResultData.Zero();
    TPckg<TInt> intPkg( aCopyLogos );
    return SendReceive( EhspsGetListHeaders, 
            TIpcArgs(&aResultData, &aSearchMaskData, &aHeaderData, &intPkg) );  
    } 
  
// -----------------------------------------------------------------------------
// RhspsClientSession::GetNextHeader
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RhspsClientSession::GetNextHeader(TDes8& aResultData, TDes8& aHeaderData, 
                        TRequestStatus& aStatus)
    {
    aHeaderData.Zero();
    aResultData.Zero();
    TIpcArgs args;
    args.Set(0, &aResultData );
    args.Set(1, &KNullDesC8);
    args.Set(2, &aHeaderData);
    SendReceive(EhspsGetNextHeader, args, aStatus); 
    }
  
// -----------------------------------------------------------------------------
// RhspsClientSession::SetActiveTheme
// Theme activation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::SetActiveTheme(TDes8& aResultData, const TDesC8& aSetMaskData,
                         TDes8& aHeaderData)
    {
    aHeaderData.Zero();
    aResultData.Zero(); 
    return SendReceive(EhspsSetActiveTheme, TIpcArgs(&aResultData, &aSetMaskData, &aHeaderData)); 
    }
  
// -----------------------------------------------------------------------------
// RhspsClientSession::RestoreDefault
// Restore defaults
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::RestoreDefault(TDes8& aResultData, const TDesC8& aSetMaskData,
                         TDes8& aHeaderData)
    {
    aHeaderData.Zero(); 
    aResultData.Zero();
    return SendReceive( EhspsRestoreDefault, TIpcArgs(&aResultData, &aSetMaskData, &aHeaderData) ); 
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::RemoveTheme
// Theme removal
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::RemoveTheme(TDes8& aResultData, const TDesC8& aSetMaskData)
    {
    aResultData.Zero();
    return SendReceive(EhspsRemoveTheme, TIpcArgs(&aResultData, &aSetMaskData));  
    }

/* Client Request service */
// -----------------------------------------------------------------------------
// RhspsClientSession::GetODT
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::GetODT(
		TDes8& aResultData, 
		const ThspsConfiguration& aConfiguration,
		const TDesC8& aRequestData, 
		TDes& aODTPath)
    {
    // Setup packaged input for the service               
    TPckgC<ThspsConfiguration> packagedStruct(aConfiguration);
        
    aResultData.Zero();
    aODTPath.Zero();
    return SendReceive(EhspsGetODT,TIpcArgs(&aResultData, &packagedStruct, &aRequestData, &aODTPath )); 
    }
  
// -----------------------------------------------------------------------------
// RhspsClientSession::GetODTUpdate
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RhspsClientSession::GetODTUpdate(TDes8& aResultData,
                                               TDes8& aRequestNotifyData,
                                               TDes8& aHeaderData,   
                                               TRequestStatus& aStatus)
    {
    aHeaderData.Zero();
    aResultData.Zero();
    aRequestNotifyData.Zero();
    TIpcArgs args;
    args.Set(0, &aResultData );
    args.Set(1, &aRequestNotifyData);
    args.Set(2, &aHeaderData);
  
    SendReceive(EhspsGetODTUpdate, args, aStatus);  
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::AccessResourceFile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::AccessResourceFile(TDes8& aResultData,
		const ThspsConfiguration& aConfiguration, const TDesC& aFileName, TInt& aFileHandle)
    {
    // Setup packaged input for the service               
    TPckgC<ThspsConfiguration> packagedStruct(aConfiguration);
           
    // Setup packaged output from the service
    TPckgBuf<TInt> packagedFileHandle(0);

    aResultData.Zero();
    
    //IPC slots and contents:
    //#0 (unused?)
    //#1 uids in a struct
    //#2 the file name to open
    //#3 the handle to the RFile that will be filled in by the server
            
    //The return value contains the handle to the RFs that can be used to access the file    
    TInt fileServerHandle = SendReceive(
    		EhspsAccessResourceFile,
    		TIpcArgs(&aResultData, &packagedStruct, &aFileName, &packagedFileHandle)
    		);        
    
    // Set received file handle
    aFileHandle=packagedFileHandle();
    
    return fileServerHandle;
    }
// -----------------------------------------------------------------------------
// RhspsClientSession::CopyResourceFiles
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::CopyResourceFiles( TDes8& aResultData, TDesC& aODTPath, 
													TDesC& aDestinationPath )
	{
	aResultData.Zero();
	return SendReceive( EhspsCopyResources, TIpcArgs( &aResultData, &aODTPath, &aDestinationPath ));
	}

/* Generic functions */
// -----------------------------------------------------------------------------
// RhspsClientSession::CancelRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RhspsClientSession::CancelRequest(TInt aCancelRequestMessage, 
                                                          TDes8& aResultData, TInt aAppUid )
    {
    aResultData.Zero();
    SendReceive(aCancelRequestMessage, TIpcArgs(&aResultData, aAppUid ));
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::GetNextHeader
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::GetNextHeader( TDes8& aResultData, TDes8& aHeaderData )
    {
    aHeaderData.Zero();
    aResultData.Zero();
    TIpcArgs args;
    args.Set( 0, &aResultData );
    args.Set( 1, &KNullDesC8 );
    args.Set( 2, &aHeaderData );
    return SendReceive( EhspsGetNextHeader, args ); 
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::AddPlugin
// Adds a plugin configuration into the defined application configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::AddPlugin( TDes8& aResultData, const ThpsParamAddPlugin& aParams, TInt& aNewPluginId )
    {            
    aResultData.Zero();                          
    TPckgC<ThpsParamAddPlugin> packagedStruct(aParams);    
    TPckg<TInt> idDes( aNewPluginId );    
    return SendReceive(EhspsAddPlugin, TIpcArgs(&aResultData, &packagedStruct, &idDes));             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::RemovePlugin
// Removes a plugin configuration from the defined application configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::RemovePlugin( TDes8& aResultData, 
		const ThpsParamRemovePlugin& aParams )
    {            
    aResultData.Zero();                          
    TPckgC<ThpsParamRemovePlugin> packagedStruct(aParams);            
    return SendReceive(EhspsRemovePlugin, TIpcArgs(&aResultData, &packagedStruct));             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::SetActivePlugin
// Set plugin configuration active.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::SetActivePlugin( TDes8& aResultData, 
        const ThpsParamSetActivePlugin& aParams )
    {            
    
    aResultData.Zero();                          
    TPckgC<ThpsParamSetActivePlugin> packagedStruct( aParams );            
    return SendReceive( EhspsSetActivePlugin, TIpcArgs( &aResultData, &packagedStruct ) );
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::ReplacePlugin
// Replaces a plugin configuration in the active application configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::ReplacePlugin( 
        TDes8& aResultData, 
        const ThspsParamReplacePlugin& aParams )
    {            
    aResultData.Zero();                          
    TPckgC<ThspsParamReplacePlugin> packagedStruct(aParams);            
    return SendReceive(EhspsReplacePlugin, TIpcArgs(&aResultData, &packagedStruct));             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::GetPluginOdt
// Returns odt path accoring given uid
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::GetPluginOdt( TDes8& aResultData, 
		const ThspsParamGetPluginOdt& aParams, 
		TDes& aOdtPath )
    {
    aResultData.Zero();     
    TPckgC<ThspsParamGetPluginOdt> packagedStruct(aParams);    
    return SendReceive( EhspsGetPluginOdt, TIpcArgs(&aResultData, &packagedStruct, &aOdtPath) );             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::SetPluginSettings
// Personalizes settings in a plugin configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::SetPluginSettings( TDes8& aResultData,
        TDes8& aSearchMaskData,
        ThspsParamSetPluginSettings aParams,
        TDes8& aDomData )
    {
    aResultData.Zero();                          
    TPckgC<ThspsParamSetPluginSettings> packagedStruct(aParams);  
    return SendReceive(EhspsSetPluginSettings, TIpcArgs( &aResultData, &aSearchMaskData, &packagedStruct, &aDomData ));  
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::MovePlugins
// Updates plugin positions within a configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::MovePlugins( TDes8& aResultData, const ThpsParamMovePlugins& aParams )
    {
    aResultData.Zero();                          
    TPckgC<ThpsParamMovePlugins> packagedStruct(aParams);            
    return SendReceive(EhspsMovePlugins, TIpcArgs(&aResultData, &packagedStruct));             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::SetConfState
// Updates configuration state
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::SetConfState( 
    TDes8& aResultData, 
    const ThspsParamSetConfState& aParams )
    {
    aResultData.Zero();                          
    TPckgC<ThspsParamSetConfState> packagedStruct( aParams );            
    return SendReceive( EhspsSetConfState, TIpcArgs( &aResultData, &packagedStruct ) );             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::ReinstallConf
// Updates configuration state
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::ReinstallConf( 
    TDes8& aResultData, 
    const ThspsParamReinstallConf& aParams )
    {
    aResultData.Zero();                          
    TPckgC<ThspsParamReinstallConf> packagedStruct( aParams );            
    return SendReceive( EhspsReinstallConf, TIpcArgs( &aResultData, &packagedStruct ) );             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::RestoreActiveAppConf
// Restores active application configuration
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::RestoreActiveAppConf( 
    TDes8& aResultData, 
    const ThspsParamRestoreActiveAppConf& aParams )
    {
    aResultData.Zero();                          
    TPckgC<ThspsParamRestoreActiveAppConf> packagedStruct( aParams );            
    return SendReceive( EhspsRestoreActiveAppConf, TIpcArgs( &aResultData, &packagedStruct ) );             
    }

// -----------------------------------------------------------------------------
// RhspsClientSession::RestoreConfigurations
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RhspsClientSession::RestoreConfigurations(
    TDes8& aResultData, 
    const ThspsParamRestoreConfigurations& aParams )
    {
    aResultData.Zero();                                         
    TPckgC<ThspsParamRestoreConfigurations> packagedStruct( aParams );
    return SendReceive( EhspsRestoreConfigurations, TIpcArgs( &aResultData, &packagedStruct ) );
    }

// end of file

