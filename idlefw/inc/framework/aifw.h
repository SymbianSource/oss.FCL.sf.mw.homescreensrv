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


#ifndef _AIFW_H
#define _AIFW_H

// System includes
#include <e32base.h>
#include <cenrepnotifyhandler.h>

// User includes
#include <aifweventhandler.h>

// Forward declarations
class CAiUiControllerManager;
class CAiStateManager;
class CAiStateProvider;
class CAiPluginFactory;
class CAiEventHandler;
class CAiWsPluginManager;
class THsPublisherInfo;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 * Active Idle Framework main class.
 */
NONSHARABLE_CLASS( CAiFw ) : public CBase, public MAiFwEventHandler,                
    public MCenRepNotifyHandlerCallback        
    {
public: 
    // constructors and destructor    

    /**
     * Creates and returns a new Active Idle Framework object.
     *
     * @return  new Active Idle Framework object. The object is also left
     *          on the cleanup stack.
     * @exception Any of the system-wide error codes if framework creation
     *            fails due to an unrecoverable error.
     */
    IMPORT_C static CAiFw* NewLC();
  
    /**
     * Destructor
     */
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
        
private: 
    // constructors
    
    /**
     * Default C++ constructor
     */
    CAiFw();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
  
private: 
    // from MAiFwEventHandler    
        
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
    void HandlePluginEvent( const TDesC& aParam );
    
    /**
     * @see MAiFwEventHandler
     */        
    void HandlePluginEventL( 
        const THsPublisherInfo& aPublisherInfo, 
        const TDesC& aParam );

    /**
     * @see MAiFwEventHandler
     */    
    TBool HasMenuItemL( 
        const THsPublisherInfo& aPublisherInfo, 
        const TDesC& aMenuItem );        

    /**
     * @see MAiFwEventHandler
     */        
    TBool RefreshContent( const TDesC& aContentCid );

    /**
     * @see MAiFwEventHandler
     */            
    TBool RefreshContent( 
        const THsPublisherInfo& aPublisherInfo,
        const TDesC& aContentCid );

    /**
     * @see MAiFwEventHandler
     */            
    TBool SuspendContent( 
        const THsPublisherInfo& aPublisherInfo,
        const TDesC& aContentCid );
    
    /**
     * @see MAiFwEventHandler
     */        
    TBool QueryIsMenuOpen();
        
private: 
    // from MCenRepNotifyHandlerCallback

    /**
     * @see MCenRepNotifyHandlerCallback
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );
    
public:
    // new functions
    
    /**
     * Get repository
     * 
     * @since S60 5.2
     * @return Repositury
     */
    CRepository& Repository() const;
    
private: 
    // new functions        

    void SwapUiControllerL( TBool aToExtHS );

private: 
    // data     
    
    /** UI Controller manager, Owned. */
    CAiUiControllerManager* iUiControllerManager;
    /** Plugin factory, Owned */
    CAiPluginFactory* iFactory;
    /** State manager, Owned */
    CAiStateManager* iStateManager;
    /** State provider, Owned */
    CAiStateProvider* iStateProvider;
    /** Plugin event handler, Owned. */
    CAiEventHandler* iEventHandler;
    /** Window server plug-in manager, Owned. */
    CAiWsPluginManager* iWsPluginManager;
    /** Notify handler for cenrep, Owned. */
    CCenRepNotifyHandler* iNotifyHandler;    
    /** Notify handler for cenrep, Owned. */
    CCenRepNotifyHandler* iNotifyHandlerESS;
    /** Idle repository, Owned. */
    CRepository* iRepository;    
    
    TBool iLibrariesLoaded;
    
    RLibrary iLibrary1;
    RLibrary iLibrary2;
    RLibrary iLibrary3;       
    };

#endif // _AIFW_H

