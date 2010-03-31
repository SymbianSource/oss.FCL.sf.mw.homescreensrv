/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AI2 main class
*
*/


#ifndef C_AIFW_H
#define C_AIFW_H

#include <e32base.h>
#include <aicontentpublisher.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>

#include "aifwdefs.h"
#include "aicontentmodel.h"
#include "aifweventhandler.h"

class CAiUiControllerManager;
class CAiContentPluginManager;
class CAiWsPluginManager;
class CAiPluginStateManager;
class RConeResourceLoader;
class CAiContentPublisher;
class MAiPSPropertyObserver;
class CAiNetworkListener;

/**
 * @ingroup group_aifw
 * 
 * Active Idle Framework main class.
 */
NONSHARABLE_CLASS( CAiFw ) : 
        public CBase,
        public MAiFwEventHandler,
        public MCenRepNotifyHandlerCallback        
    {
public: // Constructors and destructors    

    /**
     * Creates and returns a new Active Idle Framework object.
     *
     * @return  new Active Idle Framework object. The object is also left
     *          on the cleanup stack.
     * @exception Any of the system-wide error codes if framework creation
     *            fails due to an unrecoverable error.
     */
    IMPORT_C static CAiFw* NewLC();
  
    ~CAiFw();
  
public: // New functions
      
    /**
     * Runs the Active Idle Framework. This function returns when the
     * framework is shut down.
     *
     * @exception Any of the system-wide error codes if the framework 
     *            encounters a fatal initialization or run-time error.
     */
    IMPORT_C void RunL();
        
private: // Constructors      
    CAiFw();
    void ConstructL();
  
private: // From MAiFwEventHandler    
        
    /**
     * @see MAiFwEventHandler
     */
    void AppEnvReadyL();

    /**
     * @see MAiFwEventHandler
     */    
    void HandleUiReadyEventL( CAiUiController& aUiController );

    /**
     * @see MAiFwEventHandler
     */    
    void HandleActivateUI();
    
    /**
     * @see MAiFwEventHandler
     */    
    void HandleUiShutdown( CAiUiController& aUiController );
    
    /**
     * @see MAiFwEventHandler
     */        
    void HandleLoadPluginL( const TAiPublisherInfo& aPublisherInfo );
    
    /**
     * @see MAiFwEventHandler
     */        
    void HandleDestroyPluginL( const TAiPublisherInfo& aPublisherInfo );            
    
    /**
     * @see MAiFwEventHandler
     */        
    void HandlePluginEvent( const TDesC& aParam );
    
    /**
     * @see MAiFwEventHandler
     */        
    void HandlePluginEventL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aParam );

    /**
     * @see MAiFwEventHandler
     */    
    TBool HasMenuItemL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aMenuItem );        

    /**
     * @see MAiFwEventHandler
     */        
    TBool RefreshContent( const TDesC& aContentCid );
    
    /**
     * @see MAiFwEventHandler
     */        
    TBool QueryIsMenuOpen();

    /**
     * @see MAiFwEventHandler
     */            
    void ProcessStateChange( TAifwStates aState );    
        
private: // From MCenRepNotifyHandlerCallback

    /**
     * @see MCenRepNotifyHandlerCallback
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    
private: // New functions        
                 
    static TInt HandleFocusChangeEvent( TAny* aSelf );
    static TInt HandleRestartEvent( TAny* aSelf );
    void SwapUiControllerL( TBool aToExtHS );

private: // Data     
    
    /**
     * UI Controller manager, Owned.     
     */
    CAiUiControllerManager* iUiControllerManager;

    /**
     * Content plugin manager, Owned.     
     */
    CAiContentPluginManager* iPluginManager;

    /**
     * Window server plug-in manager, Owned.
     */
    CAiWsPluginManager* iWsPluginManager;

    /**
     * Notify handler for cenrep, Owned.     
     */
    CCenRepNotifyHandler* iNotifyHandler;
    
    /**
     * Notify handler for cenrep, Owned.     
     */    
    CCenRepNotifyHandler* iNotifyHandlerESS;

    /**
     * Idle repository, Owned.
     */
    CRepository* iAIRepository;
    
    /**
     * Idle restart PS observer, Owned.
     */
    MAiPSPropertyObserver* iIdleRestartObserver;
    
    TBool iLibrariesLoaded;
    
    RLibrary iLibrary1;
    RLibrary iLibrary2;
    RLibrary iLibrary3;       
    };

#endif // C_AIFW_H

