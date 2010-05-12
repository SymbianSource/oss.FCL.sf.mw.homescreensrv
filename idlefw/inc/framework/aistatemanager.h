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
* Description:  State Manager
*
*/


#ifndef _AISTATEMANAGER_H
#define _AISTATEMANAGER_H

// System includes
#include <e32base.h>
#include <babitflags.h>

// User includes
#include <aifwdefs.h>
#include "aistateobserver.h"

// Forward declarations
class CAiPluginFactory;
class CAiCpsCommandBuffer;
class CHsContentPublisher;
class THsPublisherInfo;

/**
 * State Manager
 * 
 * @ingroup group_aifw
 * @lib aifw.lib
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CAiStateManager ) : public CBase,
    public MAiStateObserver
    {
private:
    // Data types
    enum TState
        {
        ESuspended = 0,        
        EAlive,                
        };
    
    enum TFlags
        {
        EIsForeground = 0,
        EIsLightsOn,        
        EIsOnline,
        EShutdown
        };
    
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CAiStateManager* NewL( CAiPluginFactory& aFactory );

    /**
     * Two-phased constructor. Leaving on stack
     */
    static CAiStateManager* NewLC( CAiPluginFactory& aFactory );

    /**
     * Destructor
     */
    ~CAiStateManager();

private:
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CAiStateManager( CAiPluginFactory& aFactory );

private:
    // from MAiStateObserver
    
    /**
     * @see MAiStateObserver
     */
    void NotifyStateChange( 
        TAiFwState aReason );

    /**
     * @see MAiStateObserver
     */    
    TInt NotifyLoadPlugin( const THsPublisherInfo& aInfo, 
        TAiFwLoadReason aReason );
    
    /**
     * @see MAiStateObserver
     */    
    void NotifyDestroyPlugin( const THsPublisherInfo& aInfo,
        TAiFwDestroyReason aReason );

    /**
     * @see MAiStateObserver
     */        
    void NotifyReloadPlugins();
    

    /**
     * @see MAiStateObserver
     */            
    void NotifyReleasePlugins( const RArray<TUid>& aUidList );

private:
    // new functions
        
    /**
     * Evaluates next state 
     * 
     * @since S60 5.2
     * @return Next state
     */
    TState EvaluateNextState() const;
    
    /**
     * Process state change for all plugins
     * 
     * @since S60 5.2
     * @param aNextState Next state where plugins are driven
     */
    void ProcessStateChange( TState aNextState );
    
    /**
     * Process general theme state change for all plugins
     * 
     * @since S60 5.2     
     */    
    void ProcessGeneralThemeChange();

    /**
     * Process backup/restore state change for all plugins
     * 
     * @since S60 5.2     
     * @param aStarted ETrue when backup started
     */        
    void ProcessBackupRestore( TBool aStarted );

    /**
     * Process online / offline state change for all plugins
     * 
     * @since S60 5.2
     */
    void ProcessOnlineStateChange();
    
    /** 
     * Runs plugin startup sequence
     * 
     * @since S60 5.2     
     * @param aPlugin Plugin to start
     * @param aReason Start reason
     */
    void StartPlugin( CHsContentPublisher& aPlugin, 
        CHsContentPublisher::TStartReason aReason );

    /** 
     * Runs plugin shutdown sequence
     * 
     * @since S60 5.2     
     * @param aPlugin Plugin to stop
     * @param aReason Stop reason
     */    
    void StopPlugin( CHsContentPublisher& aPlugin,
        CHsContentPublisher::TStopReason aReason );
        
    /**
     * Destroys all plugins from plugin factory
     * 
     * @since S60 5.2
     */
    void DestroyPlugins();
                         
    /**
     * Flushes cps command buffer
     * 
     * @since S60 5.2
     */
    void FlushCommandBuffer();
    
private:
    // data
    
    /** Plugin Factory, Not owned */
    CAiPluginFactory& iFactory;
    /** CPS Command buffer, Owned */
    CAiCpsCommandBuffer* iCommandBuffer;
    /** Current state */
    TState iCurrentState;    
    /** Flags */
    TBitFlags32 iFlags;
    /** Halted flag */
    TBool iHalt;   
    /** List of plugins which should be reloaded */
    RArray<THsPublisherInfo> iReloadPlugins;

private:
    // friend classes
    
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiStateManager;
#endif        
    };

#endif // _AISTATEMANAGER_H

// End of file

