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
* Description:  HSPS server session
*
*
*/


#ifndef __hspsTHEMESERVERSESSION_H__
#define __hspsTHEMESERVERSESSION_H__

#include <e32base.h>
#include <f32file.h>

#include "hsps_builds_cfg.hrh"

// Forward declarations
class ChspsODT;
class ChspsInstallationHandler;
class ChspsMaintenanceHandler;
class ChspsClientRequestHandler;
class ChspsThemeServer;

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

/**
 * @ingroup group_hspsserversession
 * This class provides HSPS Server Session
 */
class ChspsThemeServerSession : public CSession2
    {
public:
    
	/**
    * ChspsThemeServerSession
    * @param aAppUid Application UID using this session
    * @since S60 3.1
    */
    ChspsThemeServerSession(
        const TInt aAppUid );
    
    /**
    * CreateL
    * @since S60 3.1
    */
    void CreateL();
  
    /**
     * Shares RFs instance from session to handlers it owns.
     * @return Reference to RFs instance
     */
    RFs& FileSystem();

    /**
     * Return whether icon file copy required.
     * 
     * @return TBool    ETrue if icon file copy is required. Otherwise false.
     */
    TBool IconFileCopyRequired() const;

    /**
     * Set icon file copy requirement flag.
     * 
     * @param aCopyRequired     Value to be set.
     */
    void SetIconFileCopyRequired( const TBool aCopyRequired );    

    /**
     * Return whether resource file copy required.
     * 
     * @return TBool    ETrue if icon file copy is required. Otherwise false.
     */
    TBool ResourceFileCopyRequired() const;

    /**
     * Set resource file copy requirement flag.
     * 
     * @param aCopyRequired     Value to be set.
     */
    void SetResourceFileCopyRequired( const TBool aCopyRequired );    

    /**
     * Get app uid of session
     * 
     * @return App uid. 
     */
    TInt AppUid() const;     
    
private:
    
	/**
    * ~ChspsThemeServerSession
    * @since S60 3.1
    */
    ~ChspsThemeServerSession();
    
    /**
    * Server
    * @since S60 3.1
    */
    inline ChspsThemeServer& Server();
    
    /**
    * ServiceL, traps DoServiceL call and completes service 
    * requests when an error occures.    
    * @since S60 5.0
    */
    void ServiceL(const RMessage2& aMessage);
    
    /**
     * Executes requested service.
     * @since S60 5.0
     */
    void DoServiceL(const RMessage2& aMessage);
   
    /**
    * ServiceError
    * @since S60 3.1
    */
    void ServiceError(const RMessage2& aMessage,TInt aError);

    /* hspsInstallationService requests */
    /**
    * InstallThemeL
    * @since S60 3.1
    */
    void InstallThemeL(const RMessage2& aMessage); 
    
    /**
    * InstallNextPhaseL
    * @since S60 3.1
    */
    void InstallNextPhaseL(const RMessage2& aMessage);
    
    /**
    * CancelInstallThemeL
    * @since S60 3.1
    */
    void CancelInstallThemeL(const RMessage2& aMessage);

    /**
    * ReinstallConfL
    * @since S60 5.0
    */
    void ReinstallConfL(const RMessage2& aMessage);

    /* hspsMaintenanceService requests */    
    /**
    * GetListHeadersL
    * @since S60 3.1
    */
    void GetListHeadersL(const RMessage2& aMessage);
    
    /**
    * GetNextHeaderL
    * @since S60 3.1
    */
    void GetNextHeaderL(const RMessage2& aMessage);      
   
    /**
    * CancelGetListHeadersL
    * @since S60 3.1
    */
    void CancelGetListHeadersL(const RMessage2& aMessage);
    
    /**
    * SetActiveThemeL
    * @since S60 3.1
    */
    void SetActiveThemeL(const RMessage2& aMessage);
    
    /**
    * RestoreDefaultL
    * @since S60 3.1
    */
    void RestoreDefaultL(const RMessage2& aMessage);
    
    /**
    * RemoveThemeL
    * @since S60 3.1
    */
    void RemoveThemeL(const RMessage2& aMessage);    
  
    /**
    * AddPluginL
    * @since S60 5.0
    */	
    void AddPluginL(const RMessage2& aMessage);
    
    /**
    * RemovePluginL
    * @since S60 5.0
    */	
    void RemovePluginL(const RMessage2& aMessage);

    /**
    * SetActivePluginL
    * 
    * @since S60 5.0
    * @param aMessage IPC message data.
    */  
    void SetActivePluginL( const RMessage2& aMessage );    

    /**
     * ReplacePluginL
     * @since S60 5.0
     */
    void ReplacePluginL(const RMessage2& aMessage);
    
    /**
     * SetPluginSettingsL
     * @since S60 5.0
     */  
    void SetPluginSettingsL(const RMessage2& aMessage);
    
    /**
     * GetPluginOdtL
     * @since S60 5.0
     */  
    void GetPluginOdtL(const RMessage2& aMessage);    
    
    /**
    * MovePluginsL
    * @since S60 5.0
    */
    void MovePluginsL(const RMessage2& aMessage);

    /**
    * SetConfStateL
    * @since S60 5.0
    */
    void SetConfStateL(const RMessage2& aMessage);

    /**
    * RestoreActiveAppConfL
    * @since S60 5.0
    */
    void RestoreActiveAppConfL(const RMessage2& aMessage);
    
    /**
    * RestoreConfigurationsL
    * @since S60 5.2
    */
    void RestoreConfigurationsL( const RMessage2& aMessage );

    /****************************************
     * hspsClientRequestService requests
     ****************************************/
    
    /**
    * GetOdtL
    * @since S60 3.1
    */
    void GetOdtL(const RMessage2& aMessage);
         
    /**
    * GetOdtUpdateL
    * @since S60 3.1
    */
    void GetOdtUpdateL(const RMessage2& aMessage);
    
    /**
    * CancelGetOdtUpdateL
    * @since S60 3.1
    */
    void CancelGetOdtUpdateL(const RMessage2& aMessage);

    /**
    * AccessResourceFileL
    * @since S60 3.1
    */
    void AccessResourceFileL(const RMessage2& aMessage);

    /**
     * Copies (overwrites) odt's resource files under private folder.
     * Creates folder for each plug-in if needed.
     * @since S60 5.0.1 
     */
    void CopyResourceFilesL(const RMessage2& aMessage);
        
    
    /**
    * CreateMaintenanceHandlerL
    * 
    * Creates a new maintenance handler if one does not already exist.    
    */
    void CreateMaintenanceHandlerL( const TUint aSecureId );

    /**
    * CreateClientRequestHandlerL
    * 
    * Creates a new client request handler if one does not already exist.
    */
    void CreateClientRequestHandlerL();
    
    /**
     * Finds unique widgets, configuration UIDs, from resource an array.
     * @since S60 5.0.1 
     * @param aAppODT is an instance of an application configuration ODT
     * @param aUidArray holds found UIDs in an array          
     */
    void FindWidgetUidsL(
        ChspsODT& aAppODT,
        RArray<TInt>& aUidArray );
            
private:
    RFs iFs;
    ChspsInstallationHandler* iInstallationHandler;    
    ChspsMaintenanceHandler* iMaintenanceHandler;    
    ChspsClientRequestHandler* iClientRequestHandler;    
    RMessagePtr2 iMessagePtr;   
    TBool iHoldingResources;
    TInt iAppUid;
    
    
    /**
     * Boolean to indicate that icon files need to be copied.
     */
    TBool iIconFileCopyRequired;

    /**
     * Boolean to indicate that resource files need to be copied.
     */
    TBool iResourceFileCopyRequired;
    
#ifdef HSPS_LOG_ACTIVE
    /**
     * Log bus.
     */        
    ChspsLogBus* iLogBus;
#endif    
  };


#endif //__hspsTHEMESERVERSESSION_H__
