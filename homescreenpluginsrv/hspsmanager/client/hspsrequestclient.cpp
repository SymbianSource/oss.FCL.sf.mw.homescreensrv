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
* Description:  Client-side API implementation of Client Request Services of
*                hsps Application Management Service APIs. 
*                For details, see hspsThemeManagement.h.
*
*
*/


#include <e32svr.h>
#include <s32strm.h>
#include <s32mem.h>
#include <e32cmn.h> 
#include <f32file.h>
#include <s32file.h>

#include "hspsrequestclient.h"
#include "hspsodt.h"
#include "hspsresource.h"
#include "hspsresult.h"
#include "hspsreqnotifparam.h"
#include "hspsdomdocument.h"
#include "hspsdomnode.h"
#include "hspsdomdepthiterator.h"
#include "hsps_builds_cfg.hrh"
#ifdef _hsps_PERFORMANCE_TEST_
#include "hspstimemon.h"
#endif

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif

_LIT( KPrivateFolderPrefix, "c:\\private\\" ); 
_LIT( KDoubleBackSlash, "\\" );

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// ChspsRequestClient::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C ChspsRequestClient* ChspsRequestClient::NewL( MhspsClientRequestServiceObserver& aObserver )
    {
    ChspsRequestClient* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C ChspsRequestClient* ChspsRequestClient::NewLC( MhspsClientRequestServiceObserver& aObserver )
    {
#ifdef _hsps_PERFORMANCE_TEST_
    ChspsTimeMon::PrintUserMem( _L("ChspsRequestClient::NewLC(): - initialising..") );
#endif//_hsps_PERFORMANCE_TEST_
    ChspsRequestClient* self = new ( ELeave ) ChspsRequestClient( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
#ifdef _hsps_PERFORMANCE_TEST_
    ChspsTimeMon::PrintUserMem( _L("CLIENT: - ChspsRequestClient::NewLC(): - done.") );
#endif//_hsps_PERFORMANCE_TEST_    
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::ConstructL()
    {
#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsRequestClient::ConstructL(): - connecting to hspsThemeServer .." ) );
        }
#endif
    
    User::LeaveIfError( iSession.Connect() );

#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsRequestClient::ConstructL(): - connected." ) );
        }
#endif
    
    iODTValid = EFalse;
    iResult = ChspsResult::NewL();
    iReqNotifParams = ChspsRequestNotificationParams::NewL();
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::ChspsRequestClient()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
ChspsRequestClient::ChspsRequestClient( MhspsClientRequestServiceObserver& aObserver)
    : CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::~ChspsRequestClient()
// Destructor.
// -----------------------------------------------------------------------------
//
ChspsRequestClient::~ChspsRequestClient()
    {
    Cancel(); // Causes call to DoCancel()
    iSession.Close();
    delete iElement;
    delete iResult;
    delete iReqNotifParams;
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::hspsGetODT()
// Sends a request to the server for getting ODT for application.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsRequestClient::hspsGetODT(TInt aAppUid, ChspsODT& aODT)
    {
    ThspsServiceCompletedMessage ret = EhspsGetODTFailed;
    
    iODTValid = EFalse;    
    
    // Cancel any possibly outstanding requests
   // hspsCancelGetODTUpdate();

   
    // Input validation
    if ( aAppUid > 0 )
    	{
        	        
	    // Trying to find a theme configuration with the specified application uid,
	    // active theme is not known yet (fetched from cenrep at server side)
	    ThspsConfiguration configuration;
	    configuration.rootUid = aAppUid;
	    configuration.themeUid = 0;
	    configuration.type = EhspsAppConfiguration;
	    
	    // Retrieve the application configuration	    
	    TRAPD( err, CallGetOdtInFileL( configuration, aODT ) );
	    if ( !err )
	        {
	        ret = EhspsGetODTSuccess;    
	        }
    	}
           
    return ret;
    }
    
// -----------------------------------------------------------------------------
// ChspsRequestClient::hspsGetODTUpdate()
// Sends a request to the server
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsRequestClient::hspsGetODTUpdate()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError;
    
    iResult->ResetData();
    iReqNotifParams->ResetData();
    
    if ( !IsActive() && iODTValid )
        {
        iStatus = KRequestPending;
        SetActive();          
                
        
        iSession.GetODTUpdate( iResultData, iReqNotifData, iHeaderData,  iStatus );
        
#ifdef _hsps_PERFORMANCE_TEST_
    ChspsTimeMon::PrintUserMem( _L("ChspsRequestClient::hspsGetODTUpdate(): - theme changes subscribed.") );
#endif//_hsps_PERFORMANCE_TEST_
        ret = EhspsServiceRequestSheduled; 
        }
    else
    	{
#ifdef HSPS_LOG_ACTIVE
    	if( iLogBus )
    	    {
    	    if ( iODTValid )
    	        {
                iLogBus->LogText( _L( "ChspsRequestClient::hspsGetODTUpdate(): - error, theme is not valid." ) );
                }
    	    else
    	        {
    	        iLogBus->LogText( _L( "ChspsRequestClient::hspsGetODTUpdate(): - error, allready subscribed." ) );
    	        }
    	    }
#endif
		ret = EhspsServiceRequestError; 
    	}

    return ret;
    } 
    
    
// -----------------------------------------------------------------------------
// ChspsRequestClient::hspsCancelGetODTUpdate()
// Sends a cancel request to the server ...
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

EXPORT_C ThspsServiceCompletedMessage ChspsRequestClient::hspsCancelGetODTUpdate()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestCanceled; 
    iResult->ResetData();
    iReqNotifParams->ResetData();
    if ( IsActive() )
        {
        Cancel();
        
#ifdef HSPS_LOG_ACTIVE
	    if( iLogBus )
	        {
	        iLogBus->LogText( _L( "ChspsRequestClient::hspsCancelGetODTUpdate(): - subscription cancelled." ) );
	        }
#endif
        
        ret = EhspsServiceRequestCanceled;
        }
    return ret; 
    }   


// -----------------------------------------------------------------------------
// ChspsRequestClient::hspsAccessResourceFile()
// Sends a request to the server to get access to a resource file
// Access a private file in the hspsThemeServer private area
// This function cooperates with the server to get a file handle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsRequestClient::hspsAccessResourceFile( 
		const TDesC& aResourceFileName, 
		const ThspsConfiguration& aConfiguration, 
		RFile& aFile )
    {                    
#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( 
            _L( "ChspsRequestClient::hspsAccessResourceFile(): - %S." ),
            &aResourceFileName 
            );
        }
#endif   
    
    // Get the file handle and fileserver handle for the file we want from the server
    TInt fileSubSessionHandle;
    TInt fileServerHandle = iSession.AccessResourceFile( 
    		iResultData, 
    		aConfiguration, 
    		aResourceFileName, 
    		fileSubSessionHandle);
                            
    // If we got valid handles
    TInt errorCode = KErrNotFound;
    if ( fileServerHandle > 0 && fileSubSessionHandle > 0 )
        {
        // Adopt the file
        errorCode = aFile.AdoptFromServer( fileServerHandle, fileSubSessionHandle );
        }            
    
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError;
    if ( !errorCode )
        {
        // Success
        UpdatehspsResult();
        ret = EhspsAccessResourceFileSuccess;
        }
    else
        {         
        // Failure        
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode;
        }
    
    return ret;
    } 
     
// -----------------------------------------------------------------------------
// ChspsRequestClient::GethspsResultL
// Fetch latest result data on service request return.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsRequestClient::GethspsResult(ChspsResult& aResult)
    {
    aResult.iSystemError = iResult->iSystemError; 
    aResult.iXuikonError = iResult->iXuikonError; 
    aResult.iIntValue1 = iResult->iIntValue1;
    aResult.iIntValue2 = iResult->iIntValue2;  
    }  
             
// -----------------------------------------------------------------------------
// ChspsRequestClient::SetLogBus
// Set log bus.
// -----------------------------------------------------------------------------
#ifdef HSPS_LOG_ACTIVE
EXPORT_C void ChspsRequestClient::SetLogBus( void* aLogBus )
    {
    iLogBus = (ChspsLogBus*)aLogBus;
    }
#else
EXPORT_C void ChspsRequestClient::SetLogBus( void* /*aLogBus*/ )
    {
    }
#endif

// -----------------------------------------------------------------------------
// ChspsRequestClient::UpdatehspsResultL
// Constructs hspsResult-object from return data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::UpdatehspsResult()
    {
    TInt errorCode = KErrNone;
    if ( iResultData.Length() )
    	{
	    RDesReadStream readBuf( iResultData );
	    TRAP( errorCode, iResult->InternalizeL( readBuf ) );
	    readBuf.Close();
	    if ( errorCode )
	    	{
	    	iResult->ResetData();
	    	}
    	}
    else
    	{
    	iResult->ResetData();
    	}
    }  

// -----------------------------------------------------------------------------
// ChspsRequestClient::UpdatehspsResultL
// Constructs hspsResult-object from return data.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::UpdatehspsReqNotifParams()
    {
    TInt errorCode = KErrNone;
    if ( iReqNotifData.Length() )
    	{
	    RDesReadStream readBuf(iReqNotifData);
	    TRAP (errorCode, iReqNotifParams->InternalizeL(readBuf));
	    readBuf.Close();
	    if ( errorCode )
	    	{
	    	iReqNotifParams->ResetData();
	    	}
    	}
    else
    	{
    	iReqNotifParams->ResetData();
    	}
    }  

// -----------------------------------------------------------------------------
// ChspsRequestClient::CallGetOdtInFileL()
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::CallGetOdtInFileL( 		
		const ThspsConfiguration& aConfiguration, 
		ChspsODT& aODT )
    {
    iODT = &aODT;         
    HBufC8* requestData = aODT.MarshalHeaderL();
    CleanupStack::PushL(requestData);
        
    TFileName odtPath;
    ThspsServiceCompletedMessage ret = (ThspsServiceCompletedMessage) iSession.GetODT( 
    			iResultData, 
    			aConfiguration, 
    			requestData->Des(), 
    			odtPath );    
    
    CleanupStack::Pop(requestData); 
    delete requestData;
    
    if ( ret != EhspsGetODTSuccess )
        {
        User::Leave( EhspsGetODTFailed );
        }
    
    // Constructs or resets iResult -object from iResultData data
    UpdatehspsResult();
    
    iODTValid = ETrue;
    
    // Get a handle to the file object
    RFile odtfile;
    ret = hspsAccessResourceFile( odtPath, aConfiguration, odtfile );
    if ( ret != EhspsAccessResourceFileSuccess )
        {
        User::Leave( EhspsAccessResourceFileFailed );
        }        
    CleanupClosePushL( odtfile );
    
    // set stream on file
    CFileStore* store = CDirectFileStore::FromLC( odtfile );
    RStoreReadStream instream;
    CleanupClosePushL( instream );

    // stream in the ODT
	instream.OpenLC( *store, store->Root() );	
	instream >> aODT;
    
    CleanupStack::PopAndDestroy( 2, &instream ); // stream object and instream
	CleanupStack::PopAndDestroy( 2, &odtfile ); // store, odtfile     
      
    // copy resources if found
	if( aODT.ResourceCount() )
		{        		
		/* c:\\private\\"application uid"\\resources\\ */
		TPath destinationPath( KPrivateFolderPrefix );
		destinationPath.AppendNum( aODT.RootUid(), EHex ); 	// application uid
		destinationPath.Append( KDoubleBackSlash );

		// Copy resource files        		 
		ret = (ThspsServiceCompletedMessage)iSession.CopyResourceFiles( 
		             iResultData, 
		             odtPath, 
		             destinationPath );
		if( ret != EhspsResourceCopySuccess )
			{
			User::Leave( EhspsResourceCopyFailed );
			}
		}        
    }
 
// -----------------------------------------------------------------------------
// ChspsRequestClient::RunError
// From CActive. Called when error occurred in asynchronous request
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsRequestClient::RunError( TInt aError )
    {
    iResult->iSystemError = aError;
    iResult->iXuikonError = aError;
    TRAPD( err, iObserver.HandlehspsRequestClientMessageL( EhspsServiceRequestError, *iReqNotifParams) ) ;
    return err;
    }

// -----------------------------------------------------------------------------
// ChspsRequestClient::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::RunL()
    {
    switch ( iStatus.Int() )
        {
        case EhspsGetODTUpdateSuccess:
        	{
            // server informs that the update process has completed successfully
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateSuccess, *iReqNotifParams);
        	}
            break;   
            
        case EhspsGetODTUpdateFailed:
        	{
            // server informs that the update process has failed
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateFailed, *iReqNotifParams);
        	}
            break;        
             
        case EhspsGetODTUpdateStatus:
        	{
            // server ask to reload the theme, possible activation lost or other change in status
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsRequestClient::RunL(): - EhspsGetODTUpdateStatus received." ) );
                }
#endif
            
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateStatus, *iReqNotifParams);
        	}
            break;   
            
        case EhspsGetODTUpdateHot:
        case EhspsSetActivePluginSuccess:
        	{
            // hot update, new theme available on server, reload
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsRequestClient::RunL(): - EhspsGetODTUpdateHot received." ) );
                }
#endif
            
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateHot, *iReqNotifParams);
        	}
            break;   

        case EhspsGetODTUpdateEmpty:
        	{
        	UpdatehspsResult();
        	UpdatehspsReqNotifParams();
        	// no update available
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateEmpty, *iReqNotifParams);
        	}
            break ;
            
        case EhspsServiceRequestCanceled:
            // the request was canceled
            // flow through
        case EhspsServiceNotSupported:
        	{
            // the request was not supported
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            iObserver.HandlehspsRequestClientMessageL(EhspsServiceRequestCanceled, *iReqNotifParams);
        	}
            break;        
        case EhspsSettingsUpdated:
        	{
            // server informs that the update process has completed successfully
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsSettingsUpdated, *iReqNotifParams);
        	}
            break;
        case EhspsSetPluginSettingsSuccess:    
        case EhspsAddPluginSuccess:
        case EhspsReplacePluginSuccess:
        case EhspsRemovePluginSuccess:
            {
            // server informs that the update process has completed successfully
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            // inform observer
            iObserver.HandlehspsRequestClientMessageL(EhspsGetODTUpdateHot, *iReqNotifParams);
            }
            break;
        default:
        	{
            UpdatehspsResult();
            UpdatehspsReqNotifParams();
            iObserver.HandlehspsRequestClientMessageL(EhspsServiceRequestError, *iReqNotifParams);
        	}
        }
    }


// -----------------------------------------------------------------------------
// ChspsRequestClient::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
//
void ChspsRequestClient::DoCancel()
    {
    // rootUid (application or interface uid) is not used at the server side, thus passed as zero
    const TInt rootUid = 0;
    iSession.CancelRequest( (TInt) EhspsCancelGetODTUpdate, iResultData, rootUid );
    iResult->ResetData();
    iReqNotifParams->ResetData();
    }
    
// End of File
