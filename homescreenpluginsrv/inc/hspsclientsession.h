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
* Description:  RhspsClientSession. RhspsClientSession is RSession for the hspsThemeServer. 
*                RhspsClientSession-implementation is a low-level connection library.
*                It is used by Xuikon Client-side API-implementations, namely
*                hspsClient and hspsRequestClient. It is not intended to be used by any 
*                Xuikon service users.
*                Some of the functions are synchronous and same asynchronous when the nature 
*                of the request requiers.
*
*
*/


#ifndef __hspsCLIENTSESSION_H__
#define __hspsCLIENTSESSION_H__

#include <e32std.h>

/**
 * Server session.
 *
 * @lib hspsclientsession.lib
 * @since S60 5.0
 * @ingroup group_hspsclients
 */
class RhspsClientSession : public RSessionBase
  {
    protected:
        /**
        * Connect.
        * 
        * @since S60 5.0
        * @return Symbian error code.
        */
        IMPORT_C TInt Connect();

        /**
        * Close.
        *   
        * @since S60 5.0
        */
        IMPORT_C void Close();
  
        /**
        * InstallTheme. 
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData        Result data.
        * @param aManifestFileName  Manifest file name.
        * @param aHeaderData        Header data.
        * @return Symbian error code.
        */
        IMPORT_C TInt InstallTheme(TDes8& aResultData, const TDesC& aManifestFileName,
                       TDes8& aHeaderData);
  
        /**
        * InstallNextPhase.
        * Asynchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aHeaderData    Header data.
        * @param aStatus        Request status. 
        */
        IMPORT_C void InstallNextPhase(TDes8& aResultData, TDes8& aHeaderData,
                         TRequestStatus& aStatus);
  
        /**
        * GetListHeaders.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData        Result data.
        * @param aSearchMaskData    Search mask data.
        * @param aHeaderData        Header data.
        * @return Symbian error code.
        */
        IMPORT_C TInt GetListHeaders(TDes8& aResultData, const TDesC8& aSearchMaskData,
                       TDes8& aHeaderData);
    
        /**
        * SetActiveTheme.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aSetMaskData   Mask data.
        * @param aHeaderData    Header data.
        * @return Symbian error code.
        */
        IMPORT_C TInt SetActiveTheme(TDes8& aResultData, const TDesC8& aSetMaskData,
                       TDes8& aHeaderData);
 
        /**
        * RemoveTheme.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aSetMaskData   Mask data.
        * @return Symbian error code.
        */
        IMPORT_C TInt RemoveTheme(TDes8& aResultData, const TDesC8& aSetMaskData);

        /**
        * RestoreDefault.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aSetMaskData   Mask data.
        * @param aHeaderData    Header data.
        * @return Symbian error code.
        */
        IMPORT_C TInt RestoreDefault(TDes8& aResultData, const TDesC8& aSetMaskData,
                       TDes8& aHeaderData);

        /**
        * GetNextHeader.
        * Asynchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aHeaderData    Header data.
        * @param aStatus        Request status.
        */
        IMPORT_C void GetNextHeader(TDes8& aResultData, TDes8& aHeaderData,
                        TRequestStatus& aStatus);
        
        /**
        * GetODT.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    ResultData.
        * @param aConfiguration Configuration.
        * @param aRequestData   Request data.
        * @param aODTPath       Odt path.
        * @return Symbian error code.
        */
        IMPORT_C TInt GetODT(TDes8& aResultData, const ThspsConfiguration& aConfiguration, 
        				const TDesC8& aRequestData, TDes& aODTPath);
                           
        /**
        * AccessResourceFile.
        * Synchronous service call.
        * 
        * @since S60 5.0 
        * @param aResultData    ResultData.
        * @param aConfiguration Configuration. 
        * @param aFileName      File name.
        * @param aFileHandle    File handle.
        * @return Symbian error code. 
        */
        IMPORT_C TInt AccessResourceFile(TDes8& aResultData, const ThspsConfiguration& aConfiguration, 
        				const TDesC& aFileName, TInt& aFileHandle);

        /**
        * GetODTUpdate.
        * Asynchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData            Result data.
        * @param aRequestNotifyData     Request notify data.
        * @param aHeaderData            Header data.
        * @param aStatus                Request status.
        */
        IMPORT_C void GetODTUpdate(TDes8& aResultData,  TDes8& aRequestNotifyData, TDes8& aHeaderData,
                            TRequestStatus& aStatus);
  

		/**
		 * Reads Odt from given path (aODTPath) and copies its resource files 
		 * to destination folder (note that given destination folder is only
		 * prefix for actual destination folder).
		 * 
		 * @since S60 5.0 
		 * @param aResultData         Result data.
		 * @param aODTPath            odt path to be read.
		 * @param aDestinationPath    target directory.
         * @return Symbian error code. 
		 **/
		IMPORT_C TInt CopyResourceFiles( TDes8& aResultData,  TDesC& aODTPath, 
										TDesC& aDestinationPath );

        /**
        * CancelRequest.
        * Synchronous service call.
        *
        * @since S60 5.0  
        * @param aCancelRequestMessage  Cancel request message.
        * @param aResultData            Result data.
        * @param aAppUid                AppUid.
        */
        IMPORT_C void CancelRequest(TInt aCancelRequestMessage, TDes8& aResultData, 
                      TInt aAppUid = 0 );  

        /**
        * GetNextHeader.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aHeaderData    Header data.
        * @return Symbian error code. 
        */
        IMPORT_C TInt GetNextHeader( TDes8& aResultData, TDes8& aHeaderData );

        /**
        * AddPlugin.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @param aNewPluginId   New plugin id.
        * @return Symbian error code. 
        */
        IMPORT_C TInt AddPlugin( TDes8& aResultData, 
        		const ThpsParamAddPlugin& aParams, 
        		TInt& aNewPluginId);
        
        /**
        * RemovePlugin.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt RemovePlugin( TDes8& aResultData, 
        		const ThpsParamRemovePlugin& aParams );

        /**
        * SetActivePlugin.
        * Set plugin configuration active.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt SetActivePlugin( TDes8& aResultData, 
                const ThpsParamSetActivePlugin& aParams );        
        
        /**
        * GetPluginOdt.
        * Synchronous service call to get plugin's odt path.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @param aOdtPath       Odt path.
        * @return Symbian error code.
        */
        IMPORT_C TInt GetPluginOdt( TDes8& aResultData, 
        		const ThspsParamGetPluginOdt& aParams, 
        		TDes& aOdtPath );
        
        /**
        * SetPluginSettings.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData        Result data.
        * @param aSearchMaskData    Search mask data.
        * @param aPluginIdData      Plugin id data.
        * @param aDomData           Dom data.
        * @return Symbian error code.
        */
        IMPORT_C TInt SetPluginSettings( TDes8& aResultData,
                         TDes8& aSearchMaskData, 
                         ThspsParamSetPluginSettings aParams,
                         TDes8& aDomData ); 
        
                
        /**
        * MovePlugins.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt MovePlugins( TDes8& aResultData, const ThpsParamMovePlugins& aParams );

        /**
        * SetConfState.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt SetConfState( 
                TDes8& aResultData, 
                const ThspsParamSetConfState& aParams );

        /**
        * ReinstallConf.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt ReinstallConf( 
                TDes8& aResultData, 
                const ThspsParamReinstallConf& aParams );

        /**
        * RestoreActiveAppConf
        * Synchronous service call.
        * @param aResultData Operation result
        * @param aParams Operation parameters
        * @since S60 5.0
        */
        IMPORT_C TInt RestoreActiveAppConf( 
                TDes8& aResultData, 
                const ThspsParamRestoreActiveAppConf& aParams );

        /**
        * ReplacePlugin.
        * Synchronous service call.
        * 
        * @since S60 5.0
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */
        IMPORT_C TInt ReplacePlugin( 
                TDes8& aResultData, 
                const ThspsParamReplacePlugin& aParams );
        
        /**        
        * Restores plugin configurations by either removing all plugins
        * from the active view or by removing all extra views.
        * In latter case, the first locked view is emptied. If there are 
        * no locked views then the first view will remain while others
        * are removed.
        * @since S60 5.2
        * @param aResultData    Result data.
        * @param aParams        Params.
        * @return Symbian error code.
        */        
        IMPORT_C TInt RestoreConfigurations(
            TDes8& aResultData, 
            const ThspsParamRestoreConfigurations& aParams );
        
    public:
    
        friend class ChspsClient;
        friend class ChspsRequestClient;      
    };

#endif //__hspsCLIENTSESSION_H__
