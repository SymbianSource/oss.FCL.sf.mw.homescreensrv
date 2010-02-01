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
* Description:  Client-side API implementation of Installation and Maintenence
*                Services of Application Management Service APIs. 
*                See hspsThemeManagement.h.
*
*
*/


#include <e32svr.h>

#include "hsps_builds_cfg.hrh"

#include "hspsclient.h"
#include "hspsodt.h"
#include "hspsresult.h"
#include "hspspluginidlist.h"
#include "hspsdomdocument.h"

#include <s32mem.h>
#include <s32file.h>

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// ChspsClient::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C ChspsClient* ChspsClient::NewL( MhspsThemeManagementServiceObserver& aObserver )
    {
    ChspsClient* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// ChspsClient::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C ChspsClient* ChspsClient::NewLC( MhspsThemeManagementServiceObserver& aObserver )
    {
    ChspsClient* self = new ( ELeave ) ChspsClient( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsClient::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsClient::ConstructL()
    {
    User::LeaveIfError( iSession.Connect() );
    iOdt = ChspsODT::NewL();
    iResult = ChspsResult::NewL();
    }

// -----------------------------------------------------------------------------
// ChspsClient::ChspsClient()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
ChspsClient::ChspsClient( MhspsThemeManagementServiceObserver& aObserver)
: CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    iCancelRequestMessage = (ThspsServiceRequestMessage)KErrNone;
    iODTMode = EFalse;
    iSubscription = EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsClient::~ChspsClient()
// Destructor.
// -----------------------------------------------------------------------------
//
ChspsClient::~ChspsClient()
    {
    Cancel(); // Causes call to DoCancel()
    iSession.Close();
    delete iOdt;
    delete iSearchMaskData;
    delete iSetMaskData;
    delete iResult;
    }


// -----------------------------------------------------------------------------
// ChspsClient::hspsInstallTheme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsInstallTheme(const TDesC& aManifestFileName,
                                ChspsODT& aHeader)
    {
    ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
    TBuf8<KMaxHeaderDataLength8> headerData;
    TInt errorCode = KErrNone;
    iODTMode = ETrue;
    ret = (ThspsServiceCompletedMessage) iSession.InstallTheme( iResultData, aManifestFileName,
                                  headerData );
    UpdatehspsResult( iResultData );
    if ( ret == EhspsInstallThemeSuccess || ret == EhspsInstallPhaseSuccess )
    {

    TRAP( errorCode, aHeader.UnMarshalHeaderL(headerData) );
    if ( errorCode )
        {
        ret = EhspsInstallThemeFailed;
        
#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsClient::hspsInstallTheme(): - failed, bad headerdata returned, system error %d." ),
                errorCode );
        }
#endif
    
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode; 
        }
      }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsInstallTheme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsInstallTheme(const TDesC& /*aManifestFileName*/
                                                                          , TDes8& /*aHeaderData*/)
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsClient::hspsInstallNextPhaseL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsInstallNextPhaseL( ChspsODT& aHeader )
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError; 
    iResult->ResetData();
    if ( !IsActive() )
        {
        iODTMode = ETrue;
        iCancelRequestMessage = EhspsCancelInstallTheme;
        iHeader = &aHeader;
        iStatus = KRequestPending;
        SetActive();
        iSession.InstallNextPhase( iResultData, iHeaderData, iStatus );
        ret = EhspsServiceRequestSheduled;
        }
    else
        {
        ret = EhspsServiceRequestError; 
        }
    return ret;
    }    
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsInstallNextPhaseL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsInstallNextPhaseL( TDes8& /*aHeaderData*/ )
    {
    return EhspsServiceNotSupported;
    }   
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsCancelInstallTheme()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsCancelInstallTheme()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError; 
    iResult->ResetData();
    if ( IsActive() )
        {
        Cancel();
        ret = EhspsServiceRequestCanceled;
        }
    else 
        {
        ret = EhspsServiceRequestError;
        }
    return ret;
    }    
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsReinstallConf
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsReinstallConf( 
    TInt aAppUid,
    TInt aConfUid )
    {
    ThspsServiceCompletedMessage ret = EhspsReinstallConfFailed;
    
    ThspsParamReinstallConf params;
    params.appUid = aAppUid;
    params.confUId = aConfUid;
    
    ret = ( ThspsServiceCompletedMessage )iSession.ReinstallConf( iResultData, params );
    
    UpdatehspsResult( iResultData );
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsGetListHeaders
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsGetListHeaders( const ChspsODT& aSearchMask, 
                                CArrayPtrFlat<ChspsODT>& aHeaderList )
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError; 
    TBuf8<KMaxHeaderDataLength8> headerData;
    TInt errorCode = 0;
    iHeaderList = &aHeaderList;
    if (iSearchMaskData)
        {
        delete iSearchMaskData;
        iSearchMaskData = NULL;
        }
    TRAP(errorCode, iSearchMaskData = aSearchMask.MarshalHeaderL());
    if (errorCode)
        {
        ret = EhspsServiceRequestError; 
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode;
        }
    else
        {
        iODTMode = ETrue; 
        if ( IsActive() ) 
            {
            // cancel previous subscription first
            hspsCancelGetListHeaders();
            }        
        ret = (ThspsServiceCompletedMessage)iSession.GetListHeaders(iResultData, 
                                                             iSearchMaskData->Des(),headerData );

#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClient::hspsGetListHeaders(): - theme query sent for RootUid %d." ),
                    aSearchMask.RootUid() );
            }
#endif
        
        UpdatehspsResult( iResultData );
        }
    return ret;
    } 


// -----------------------------------------------------------------------------
// ChspsClient::hspsGetListHeaders
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsGetListHeaders(const TDesC8&  /*aSearchMaskData*/,
                            CArrayPtrSeg<HBufC8>& /*aHeaderDataList*/)
    {
    return EhspsServiceNotSupported;
    }
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsGetNextHeader()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsGetNextHeader()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError; 
    iResult->ResetData();
    if ( !IsActive() )
        {
        iCancelRequestMessage = EhspsCancelGetListHeaders;
        iStatus = KRequestPending;
        SetActive(); 
        iSession.GetNextHeader( iResultData, iHeaderData, iStatus );
        iSubscription = ETrue;

#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClient::hspsGetNextHeader(): - themes subscribed." ) );
            }
#endif
    
        ret = EhspsServiceRequestSheduled;
        }
    else
        {
        ret = EhspsServiceRequestError; 
        }
    return ret;
    }    
    
// -----------------------------------------------------------------------------
// ChspsClient::hspsCancelGetListHeaders()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsCancelGetListHeaders()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestError; 
    iResult->ResetData();
    if ( IsActive() )
        {
        iSubscription = EFalse;
        Cancel();
        ret = EhspsServiceRequestCanceled;
        }
    else 
        {
        ret = EhspsServiceRequestError;
        }
    return ret;
    }          
    

// -----------------------------------------------------------------------------
// ChspsClient::hspsSetActiveTheme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsSetActiveTheme(const ChspsODT& aSetMask,
                                  ChspsODT& aHeader)
    {
    TInt errorCode = 0;
    ThspsServiceCompletedMessage ret = EhspsSetActiveThemeFailed; 
    TBuf8<KMaxHeaderDataLength8> headerData;
    if (iSetMaskData)
        {
        delete iSetMaskData;
        iSetMaskData = NULL;
        }
    TRAP( errorCode, iSetMaskData = aSetMask.MarshalHeaderL() );
    if ( errorCode )
        {
        ret = EhspsServiceRequestError; 
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode;       
        }
    else
        {
#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClient::hspsSetActiveTheme(): - requested for: AppUid = %d, ThemeUid = %d." ),
                    aSetMask.RootUid(),
                    aSetMask.ThemeUid() );
            }
#endif
        
        ret = (ThspsServiceCompletedMessage)iSession.SetActiveTheme( iResultData, 
                                       iSetMaskData->Des(), headerData );
        if (ret == EhspsSetActiveThemeSuccess)
            {
            TRAP(errorCode, aHeader.UnMarshalHeaderL(headerData));
            if ( errorCode )
                {
                ret = EhspsServiceRequestError;
                iResult->iSystemError = errorCode;
                iResult->iXuikonError = errorCode; 

#ifdef HSPS_LOG_ACTIVE
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClient::hspsSetActiveTheme(): - failed, bad ODT returned, system error %d." ),
                            errorCode );
                    }
#endif
                }
            else
                {
#ifdef HSPS_LOG_ACTIVE
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClient::hspsSetActiveTheme(): - activated for AppUid = %d, ThemeUid = %d." ),
                            aHeader.RootUid(),
                            aHeader.ThemeUid() );
                    }
#endif
    
                UpdatehspsResult( iResultData ); 
                }
            }
        else
            {
            UpdatehspsResult( iResultData ); 
            } 
        }
    return ret;
    } 
    

// -----------------------------------------------------------------------------
// ChspsClient::hspsRestoreDefault
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsRestoreDefault(const ChspsODT& aSetMask,
                                  ChspsODT& aHeader)
    {
    TInt errorCode = 0;
    ThspsServiceCompletedMessage ret = EhspsRestoreDefaultFailed; 
    TBuf8<KMaxHeaderDataLength8> headerData;
    if ( iSetMaskData )
        {
        delete iSetMaskData;
        iSetMaskData = NULL;
        } 
        
    TRAP( errorCode, iSetMaskData = aSetMask.MarshalHeaderL() );
    if ( errorCode )
        {
        ret = EhspsServiceRequestError; 
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode; 
        }
    else
        {
        ret = ( ThspsServiceCompletedMessage )iSession.RestoreDefault( iResultData,
                                iSetMaskData->Des(), headerData );
        if ( ret == EhspsRestoreDefaultSuccess )
            {
            TRAP( errorCode, aHeader.UnMarshalHeaderL( headerData ) );
            if ( errorCode )
                {
                ret = EhspsServiceRequestError;
                iResult->iSystemError = errorCode;
                iResult->iXuikonError = errorCode;           
                }
            else
                {
                UpdatehspsResult( iResultData );
                }
            }
        else
            {
            UpdatehspsResult( iResultData );
            }
        }
    return ret;
    } 

// -----------------------------------------------------------------------------
// ChspsClient::hspsSetRemoveThemeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsRemoveThemeL(const ChspsODT& aSetMask)
    {
    TInt errorCode = 0;
    ThspsServiceCompletedMessage ret = EhspsRemoveThemeFailed; 
    if (iSetMaskData)
        {
        delete iSetMaskData;
        iSetMaskData = NULL;
        }      
    TRAP(errorCode, iSetMaskData = aSetMask.MarshalHeaderL());
    if (errorCode)
        {
        ret = EhspsServiceRequestError; 
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode;                  
        }
    else
      {
      ret = (ThspsServiceCompletedMessage)iSession.RemoveTheme(iResultData, iSetMaskData->Des());
      UpdatehspsResult( iResultData );
      }
    return ret;
    } 


// -----------------------------------------------------------------------------
// ChspsClient::hspsSetActiveTheme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsSetActiveTheme(const TDesC8& /*aSetMaskData*/
                                                                      , TDes8& /*aHeaderData*/)
    {
    return EhspsServiceNotSupported;
    }
  

// -----------------------------------------------------------------------------
// ChspsRequestClient::GethspsResultL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsClient::GethspsResult(ChspsResult& aResult)
    {
    aResult.iSystemError = iResult->iSystemError; 
    aResult.iXuikonError = iResult->iXuikonError; 
    aResult.iIntValue1 = iResult->iIntValue1;
    aResult.iIntValue2 = iResult->iIntValue2;  
    }  
             
// -----------------------------------------------------------------------------
// ChspsClient::hspsGetHeaders
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsClient::hspsGetHeaders( const ChspsODT& aSearchMask, 
                                CArrayPtrFlat<ChspsODT>& aHeaderList )
    {
    iHeaderList = &aHeaderList;
    // Convert search mask ODT to binary stream
    if (iSearchMaskData)
        {
        delete iSearchMaskData;
        iSearchMaskData = NULL;
        }
    TRAPD( err, iSearchMaskData = aSearchMask.MarshalHeaderL() );

    if ( err == KErrNone )
        {
        // Get the header count
        ThspsServiceCompletedMessage ret = 
            ( ThspsServiceCompletedMessage )iSession.GetListHeaders(
                iResultData, 
                iSearchMaskData->Des(),
                iHeaderData );
                
        if ( ret == EhspsGetListHeadersSuccess )
            {
            // Header count > 0
            UpdatehspsResult( iResultData );
            TInt headerCount = iResult->iIntValue1;
            // Get headers
            for ( TInt i = 0; 
                  i < headerCount && err == KErrNone; 
                  i++ )
                {
                ret = ( ThspsServiceCompletedMessage )iSession.GetNextHeader( 
                    iResultData,
                    iHeaderData );
                if ( ret == EhspsGetListHeadersUpdate )
                    {
                    TRAP( err, AppendHeaderListL( iHeaderData) );
                    }
                else
                    {
                    // Request failed
                    err = KErrGeneral;
                    }
                }
            }
        else if ( ret == EhspsGetListHeadersEmpty )
            {
            // Empty header list
            err = KErrNotFound;
            }
        else
            {
            // Header count request failed
            err = KErrGeneral;
            }
        }

    return err;
    } 

// -----------------------------------------------------------------------------
// ChspsClient::hspsAddPlugin
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsAddPlugin(
		const TInt aAppUid,
		const TInt aConfId,
		const TInt aPluginUid,
		const TInt aPosition,
		TInt& aAddedPluginId )
    {
    ThspsServiceCompletedMessage ret = EhspsAddPluginFailed;
    
    if ( aAppUid > 0 && aPluginUid > 0 )    
        {        
        ThpsParamAddPlugin params;
        params.appUid = aAppUid;        
        params.configurationId = aConfId; 
        params.pluginUid = aPluginUid;
        params.positionIndex = aPosition;
        
        // Try adding the plugin configuration
        ret = ( ThspsServiceCompletedMessage )iSession.AddPlugin( iResultData, params, aAddedPluginId );
        
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );                            
        }

    return ret;
    } 

// -----------------------------------------------------------------------------
// ChspsClient::hspsRemovePlugin
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsRemovePlugin(
		const TInt aAppUid,        		
		const TInt aPluginId )
	{
	ThspsServiceCompletedMessage ret = EhspsRemovePluginFailed;
	    
    if ( aAppUid > 0 && aPluginId > 0 )    
        {        
        ThpsParamRemovePlugin params;
        params.appUid = aAppUid;                 
        params.pluginId = aPluginId;
                
        // Try removing the plugin configuration instance
        ret = ( ThspsServiceCompletedMessage )iSession.RemovePlugin( iResultData, params );
        
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );                            
        }

    return ret;
	}

// -----------------------------------------------------------------------------
// ChspsClient::hspsSetActivePlugin
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsSetActivePlugin( const TInt aAppUid,
                                                                        const TInt aPluginId )
    {    
    ThspsServiceCompletedMessage ret = EhspsSetActivePluginFailed;
        
    if( aPluginId > 0 )    
        {        
        ThpsParamSetActivePlugin params;
        params.appUid = aAppUid;
        params.pluginId = aPluginId;
        
        // Try activation.
        ret = ( ThspsServiceCompletedMessage )iSession.SetActivePlugin( iResultData, params );
        
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );                            
        }

    return ret;  
    }

// -----------------------------------------------------------------------------
// ChspsClient::hspsReplacePlugin
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsReplacePlugin(
        const TInt aAppUid,
        const TInt aPluginId,               
        const TInt aConfUid )
    {
    ThspsServiceCompletedMessage ret = EhspsReplacePluginFailed;
    
    if ( aPluginId > 0 && aConfUid > 0 )    
        {                        
        ThspsParamReplacePlugin params;
        params.appUid = aAppUid;
        params.pluginId = aPluginId;
        params.confUid = aConfUid;
        
        // Try adding the plugin configuration
        ret = ( ThspsServiceCompletedMessage )iSession.ReplacePlugin( iResultData, params );
        
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );                            
        }

    return ret;
    } 

// -----------------------------------------------------------------------------
// ChspsClient::hspsSetPluginSettings
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsSetPluginSettings(
        const ChspsODT& aHeader, 
        const TInt aPluginId, 
        ChspsDomDocument& aDom,
        const TBool aPluginStoringStatus )
    {
    ThspsServiceCompletedMessage ret = EhspsSetPluginSettingsFailed;
    
    TInt errorCode = 0;    
    TBuf8<KMaxHeaderDataLength8> searchMaskData;
    HBufC8* temp = NULL;
    HBufC8* tempDom = NULL;
   
    TRAP( errorCode, temp = aHeader.MarshalHeaderL() );
    TRAP( errorCode, tempDom = aDom.MarshallL() );
    
    if( errorCode )
        {
        ret = EhspsServiceRequestError;
        iResult->iSystemError = errorCode;
        iResult->iXuikonError = errorCode;
        }
    else
        {
        searchMaskData = temp->Des();
        TPtr8 tempDomPtr = tempDom->Des();
        
        ThspsParamSetPluginSettings params;
        params.pluginId = aPluginId;
        params.storingStatus = aPluginStoringStatus;
                
        
        // Try set settings of the plugin Id given in params
        ret = ( ThspsServiceCompletedMessage )iSession.SetPluginSettings( iResultData, searchMaskData, params, tempDomPtr );
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );
        }

    delete tempDom;
    delete temp;
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsClient::hspsGetPluginDomL
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsGetPluginOdtL(
		const TInt aAppUid,        		
		const TInt aPluginUid,
		ChspsODT* aPluginOdt )
	{
	ThspsServiceCompletedMessage ret = EhspsGetPluginOdtFailed;
    
    if ( aAppUid > 0 && aPluginUid > 0 )    
        {        
    	ThspsParamGetPluginOdt params;
    	params.appUid = aAppUid;
    	params.pluginUid = aPluginUid;
    	TPath odtPath;
    	
        ret = ( ThspsServiceCompletedMessage )iSession.GetPluginOdt( iResultData, params, odtPath );
        
        // Get error codes from the server op.
        UpdatehspsResult( iResultData );
        
        if( ret == EhspsGetPluginOdtSuccess )
             {
             // Streaming odt from file
             TInt success = StreamOdtL( odtPath, aAppUid, aPluginOdt );

             if( success != KErrNone )
            	 {
            	 ret = EhspsGetPluginOdtFailed;
            	 }
             }
        }	
	return ret;
	}

// -----------------------------------------------------------------------------
// ChspsClient::hspsMovePluginsL
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsMovePluginsL(
        		const TInt aAppUid,        
        		const TInt aConfId,
        		const CArrayFixFlat<TInt>& aPluginIdList )
	{
	ThspsServiceCompletedMessage ret = EhspsMovePluginsFailed;

    if ( aAppUid > 0 && aConfId > 0 && aPluginIdList.Count() )    
        {        
        ThpsParamMovePlugins params;
        params.appUid = aAppUid;           
        params.configurationId = aConfId;
        
        // Setup an array that can be externalized 
        const TInt KGranularity = 6;
        ChspsPluginIdList* idArray = new ( ELeave )ChspsPluginIdList( KGranularity );
        CleanupStack::PushL( idArray );                
        for( TInt idIndex=0; idIndex < aPluginIdList.Count(); idIndex++ )        	
        	{        	
        	idArray->AppendL( aPluginIdList.At(idIndex) );
        	}        
        
        // Externalize array into the params.pluginIdBuf descriptor                        
        RDesWriteStream writeBuf( params.pluginIdsBuf );
        CleanupClosePushL( writeBuf );
        TRAPD( errorCode, idArray->ExternalizeL( writeBuf ) );
        CleanupStack::PopAndDestroy( &writeBuf );
        
        if ( !errorCode )        	
        	{        	                                
	        // Try updating of the plugins list
	        ret = ( ThspsServiceCompletedMessage )iSession.MovePlugins( iResultData, params );
	        
	        // Get error codes from the server op.
            UpdatehspsResult( iResultData );
        	}
        
        CleanupStack::PopAndDestroy( idArray );                                           
        }

    return ret;
	}

// -----------------------------------------------------------------------------
// ChspsClient::hspsSetConfState
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsSetConfState(
                const TInt aAppUid,        
                const TInt aConfId,
                const ThspsConfigurationState aState,
                const ThspsConfStateChangeFilter aFilter )
    {
    ThspsServiceCompletedMessage ret = EhspsSetConfStateFailed;
    
    ThspsParamSetConfState params;
    params.appUid = aAppUid;
    params.confId = aConfId;
    params.state = aState;
    params.filter = aFilter;

    ret = ( ThspsServiceCompletedMessage )iSession.SetConfState( iResultData, params );
    
    UpdatehspsResult( iResultData );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsClient::hspsRestoreActiveAppConf
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsRestoreActiveAppConf(
    const TInt aAppUid,
    const TInt aConfUid )
    {
    ThspsServiceCompletedMessage ret = EhspsRestoreActiveAppConfFailed;
    
    ThspsParamRestoreActiveAppConf params;
    params.appUid = aAppUid;
    params.confUid = aConfUid;

    ret = ( ThspsServiceCompletedMessage )iSession.RestoreActiveAppConf( iResultData, params );
    
    UpdatehspsResult( iResultData );
    
    return ret;
    }


// -----------------------------------------------------------------------------
// ChspsClient::hspsRestoreConfigurations
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsRestoreConfigurations(
    const TInt aAppUid,
    const TBool aReinstall )
    {
    ThspsServiceCompletedMessage ret = EhspsRestoreConfigurationsFailed;
    
    ThspsParamRestoreConfigurations params;
    params.appUid = aAppUid;
    params.restoreAll = aReinstall;

    ret = ( ThspsServiceCompletedMessage )iSession.RestoreConfigurations( iResultData, params );
    
    UpdatehspsResult( iResultData );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsClient::hspsPluginUpdateL
// -----------------------------------------------------------------------------
//
EXPORT_C ThspsServiceCompletedMessage ChspsClient::hspsPluginUpdateL(
                const ChspsODT& /*aPluginUid*/  )
    {
    return EhspsUpdatePluginFailed;
    }

// -----------------------------------------------------------------------------
// ChspsClient::SetLogBus
// Set log bus.
// -----------------------------------------------------------------------------
#ifdef HSPS_LOG_ACTIVE
EXPORT_C void ChspsClient::SetLogBus( void* aLogBus )
    {
    iLogBus = (ChspsLogBus*)aLogBus;
    }
#else
EXPORT_C void ChspsClient::SetLogBus( void* /*aLogBus*/ )
    {
    }
#endif

// -----------------------------------------------------------------------------
// ChspsClient::UpdatehspsResult
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClient::UpdatehspsResult( TDesC8& aResultData )
    {
    TInt errorCode = KErrNone;
    if( aResultData.Length() )
        {
        RDesReadStream readBuf(iResultData);
        TRAP( errorCode, iResult->InternalizeL( readBuf ) );
        readBuf.Close();
        if( errorCode )
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
// ChspsClient::AppendHeaderListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClient::AppendHeaderListL( TDesC8& aHeaderData )
    {
    ChspsODT* header = ChspsODT::NewL();
    CleanupStack::PushL( header );
    header->UnMarshalHeaderL( aHeaderData );
    iHeaderList->AppendL( header );
    CleanupStack::Pop( header );
    }

// -----------------------------------------------------------------------------
// ChspsClient::RunError
// From CActive. Called when error occurred in asynchronous request
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsClient::RunError( TInt aError )
    {
    iResult->iSystemError = aError;
    iResult->iXuikonError = aError;
    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
    return KErrNone;
    }  
     
     
// -----------------------------------------------------------------------------
// ChspsClient::RunL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClient::RunL()
    {
    TInt errorCode = KErrNone;
    switch ( iStatus.Int() )
        {
        // the server has completed the request, signalled the client
        // thread and the clients active scheduler runs the active object.
        // Now do something with it      
        case EhspsInstallThemeSuccess:
            // the request was successful and installation is completed
            if ( iODTMode )
                {
                TRAP( errorCode, iHeader->UnMarshalHeaderL(iHeaderData) );
                if ( errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode; 
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );
                    iObserver.HandlehspsClientMessage(EhspsInstallThemeSuccess);
                    }
                }
            else
              {
              UpdatehspsResult( iResultData );
              iObserver.HandlehspsClientMessage(EhspsInstallThemeSuccess);
              }
                
            break;         
    
        case EhspsInstallPhaseSuccess:
            // the request was successful
            if ( iODTMode )
                {
                TRAP( errorCode, iHeader->UnMarshalHeaderL(iHeaderData));
                if ( errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode;
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );
                    iObserver.HandlehspsClientMessage(EhspsInstallPhaseSuccess); 
                    // send request to install next phase
                    hspsInstallNextPhaseL( *iHeader );
                    } 
                }
            else
                {
                UpdatehspsResult( iResultData );
                // send request to install next phase
                iObserver.HandlehspsClientMessage(EhspsInstallPhaseSuccess);
                hspsInstallNextPhaseL(iHeaderData);
                }
            break;         
    
        case EhspsInstallThemeFailed:
            // the request was unsuccessful
            UpdatehspsResult( iResultData );
            iObserver.HandlehspsClientMessage( EhspsInstallThemeFailed );
            break ;
        
        case EhspsGetListHeadersUpdate:
            if (iODTMode)
                {
                ChspsODT* odt = ChspsODT::NewL();
                CleanupStack::PushL( odt );
                odt->UnMarshalHeaderL(iHeaderData);
                iHeaderList->AppendL(odt);
                CleanupStack::Pop( odt );
                // inform observer  
                if (errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode;
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );

#ifdef HSPS_LOG_ACTIVE
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsClient::RunL(): - a theme header received. AppUid = %d, ThemeUid = %d." ),
                                odt->RootUid(),
                                odt->ThemeUid() );
                        }
#endif
                    
                    iObserver.HandlehspsClientMessage( EhspsGetListHeadersUpdate );
                    
                    // request next
                    hspsGetNextHeader();
                    }
                }
            else
                {
                HBufC8* hd = iHeaderData.AllocL();
                CleanupStack::PushL( hd );
                iHeaderDataList->AppendL(hd);
                CleanupStack::Pop( hd );
                
                // inform observer  
                if (errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode;
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );
                    iObserver.HandlehspsClientMessage( EhspsGetListHeadersUpdateData );
                    // request next
                    hspsGetNextHeader();
                    } 
                }
            break;     
            
        case EhspsGetListHeadersRestart:
            // header list has changed radically
            // resetting the list and start filling it again
            if (iODTMode)
                {
                // resetting and start filling the header list again
                iHeaderList->ResetAndDestroy();
                ChspsODT* odt = ChspsODT::NewL();
                CleanupStack::PushL( odt );
                odt->UnMarshalHeaderL(iHeaderData);
                iHeaderList->AppendL(odt);
                CleanupStack::Pop( odt );
                
                // inform observer  
                if (errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode;
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );

#ifdef HSPS_LOG_ACTIVE
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsClient::RunL(): - list restarted." ) );
                        }
#endif
                    
                    iObserver.HandlehspsClientMessage( EhspsGetListHeadersRestart );
                    // request next
                    hspsGetNextHeader();      
                    }
                }             
            else
                {
                // resetting and starting filling the header data list again
                iHeaderDataList->Reset(); 
                HBufC8* hd = iHeaderData.AllocL();
                CleanupStack::PushL( hd );
                iHeaderDataList->AppendL(hd);
                CleanupStack::Pop( hd );

                // inform observer  
                if ( errorCode )
                    {
                    iResult->iSystemError = errorCode;
                    iResult->iXuikonError = errorCode;
                    iObserver.HandlehspsClientMessage( EhspsServiceRequestError );
                    }
                else
                    {
                    UpdatehspsResult( iResultData );
                    iObserver.HandlehspsClientMessage( EhspsGetListHeadersRestartData );   
                    // request next
                    hspsGetNextHeader();    
                    } 
                }
            break;     
            
        case EhspsGetListHeadersEmpty:
            // header list is empty
            UpdatehspsResult( iResultData );
            if (iODTMode)
                {
                // resetting and start filling the header list again
                iHeaderList->ResetAndDestroy();
                
#ifdef HSPS_LOG_ACTIVE
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClient::RunL(): - list empty." ) );
                    }
#endif
    
                // inform observer 
                iObserver.HandlehspsClientMessage(EhspsGetListHeadersEmpty);
                }
            else
                {
                iHeaderDataList->Reset(); 
                // inform observer
                iObserver.HandlehspsClientMessage(EhspsGetListHeadersEmpty);    
                }
            // requesting possible update
            hspsGetNextHeader();   
            break;
       
        case EhspsGetListHeadersFailed:
            // service failure occurred
            UpdatehspsResult( iResultData );
            iObserver.HandlehspsClientMessage(EhspsGetListHeadersFailed);
            break;
               
        case EhspsServiceRequestCanceled:
            // the request was canceled
            UpdatehspsResult( iResultData );
            iObserver.HandlehspsClientMessage( EhspsServiceRequestCanceled );
            break;
        
        case EhspsServiceNotSupported:
            // the request was canceled
            UpdatehspsResult( iResultData );
            iObserver.HandlehspsClientMessage( EhspsServiceNotSupported );
            break;          

        default:
            UpdatehspsResult( iResultData );
            iObserver.HandlehspsClientMessage( EhspsServiceRequestError ); 
        }
    }

// -----------------------------------------------------------------------------
// ChspsClient::DoCancel()
// Cancels any outstanding operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClient::DoCancel()
    {
    iSession.CancelRequest( (TInt) iCancelRequestMessage, iResultData );   
    }

// -----------------------------------------------------------------------------
// ChspsClient::StreamOdtL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsClient::StreamOdtL( TDes& aPath, const TInt aAppUid, ChspsODT* aOdt )
	{
	TInt errorCode = KErrNotFound;
	RFile odtfile;
	ThspsConfiguration conf;
	conf.rootUid = aAppUid;
	// conf.themeUid is not needed in AccessResourceFile()
	TInt fileSubSessionHandle;
	
	// get fs handle
	TInt fileServerHandle = iSession.AccessResourceFile( 
			iResultData, 
			conf, 
			aPath, 
			fileSubSessionHandle );

	// If we got valid handles
	if ( fileServerHandle > 0 && fileSubSessionHandle > 0 )
	    {
	    // Adopt the file
	    errorCode = odtfile.AdoptFromServer( fileServerHandle, fileSubSessionHandle );
	    }

	if ( errorCode != KErrNone )
	    {
	    // Failure	    
	    iResult->iSystemError = errorCode;
	    iResult->iXuikonError = errorCode;
	    }
	else
	    {
	    // Success
	    UpdatehspsResult( iResultData );

		CleanupClosePushL( odtfile );
		
		// set stream on file
		CFileStore* store = CDirectFileStore::FromLC( odtfile );
		RStoreReadStream instream;
		CleanupClosePushL( instream );
		
		instream.OpenLC( *store, store->Root() );
		
		// stream in the ODT
		instream >> *aOdt;
		
		TInt count = aOdt->DomDocument().DomNodeCount();
		
		// Destroy the stream object and close the instream
		#ifdef _DEBUG			
		CleanupStack::PopAndDestroy( 2, &instream );
		
		// Destroy the direct file store object (closes the file) 
		CleanupStack::PopAndDestroy( store );
		CleanupStack::PopAndDestroy();  // odtfile
		#else	   
		CleanupStack::PopAndDestroy(4);
		#endif
		
		errorCode = KErrNone;
	    }


	return errorCode;
	}

// End of File
