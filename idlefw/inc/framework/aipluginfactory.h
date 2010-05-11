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
* Description:  Plugin factory class for Active idle framework.
*
*/



#ifndef _AIPLUGINCFACTORY_H
#define _AIPLUGINCFACTORY_H

// System includes
#include <e32base.h>
#include <ecom/implementationinformation.h>

// User includes

// Forward declarations
class CAiUiControllerManager;
class CAiStateManager;
class CAiCpsCommandBuffer;
class TAiFwPublisherInfo;
class CHsContentPublisher;
class THsPublisherInfo;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 *  Plugin factory class for Active idle framework.
 *
 *  @lib aifw
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CAiPluginFactory ) : public CTimer
	{
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */	
    static CAiPluginFactory* NewL( CAiUiControllerManager& aManager );

    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Destructor
     */    
    ~CAiPluginFactory();

public:
    // new functions

    /**
     * Lists KInterfaceUidHsContentPlugin ECom implementations 
     * 
     * @since S60 5.2
     */
    void ListImplementationsL();
    
    /**
     * Schedules plugin loading
     *
     * @since S60 5.2
     * @param aInfo Plugin to load           
     */    
    void LoadPlugin( const TAiFwPublisherInfo& aInfo );

    /**
     * Schedules plugin destroyal
     *
     * @since S60 5.2
     * @param aInfo Plugin to destroy
     */        
    void DestroyPlugin( const TAiFwPublisherInfo& aInfo );
    
    /**
     * Destroy plugin
     *
     * @since S60 5.2
     * @param aUid Implementation UID of a plugin to destroy.
     */
    void DestroyPlugin( 
        const TUid& aUid );      
    
    /**
     * Destroys all plugins during system shutdown
     * 
     * @since S60 5.2
     */
    void DestroyAllPlugins();
    
    /**
     * Flushes cps command buffer
     * 
     * @since S60 5.2
     */    
    void FlushCommandBuffer();
    
    /**
     * Finds plugin by publisher info.
     *
     * @since S60 5.2
     * @param aInfo publisher info.
     * @return Pointer to plugin, NULL if not found. Factory keeps plugin's ownership.
     */        
    CHsContentPublisher* PluginByInfo( 
        const THsPublisherInfo& aPublisherInfo ) const;

    /**
     * Finds plugin by uid.
     *
     * @since S60 5.2
     * @param aInfo publisher uid.
     * @return Pointer to plugin, NULL if not found. Factory keeps plugin's ownership.
     */            
    CHsContentPublisher* PluginByUid( const TUid& aUid ) const;
    
    /**
     * Finds plugin by name.
     *
     * @since S60 5.2
     * @param aInfo publisher info.
     * @return Pointer to plugin, NULL if not found. Factory keeps plugin's ownership.
     */                
    CHsContentPublisher* PluginByName( const TDesC& aName ) const;
      
    /**
     * Sets state manager
     * 
     * @since S60 5.2
     * @param aStateManager State Manager
     */
    void SetStateManager( CAiStateManager* aStateManager );
    
    /**
     * Gets all plugins from factory
     * 
     * @since S60 5.2
     * @return Array of plugins
     */
    RPointerArray< CHsContentPublisher >& Publishers() const;
        
private:
    // from CTimer
    
    /**
     * @see CTimer
     */
    void RunL();
    
    /**
     * @see CTimer
     */
    void DoCancel();
    
private:	
    // private constructors
    
    /**
     * C++ default constructor
     */	
    CAiPluginFactory( CAiUiControllerManager& aManager );
		
private:    								
    // new functions
	      
    TInt DoCreatePlugin( 
        const TAiFwPublisherInfo& aPublisherInfo );                     
                                                
    void DoDestroyPlugin( 
        const TAiFwPublisherInfo& aPublisherInfo );         
    
    void DoCreatePluginL( 
        const TAiFwPublisherInfo& aPublisherInfo );
                 
    void SubscribePluginL( 
        CHsContentPublisher& aContentPublisher,		    
        const THsPublisherInfo& aPublisherInfo );
                                         
    void ConfigurePluginL(         
        CHsContentPublisher& aContentPublisher,
        const THsPublisherInfo& aPublisherInfo );		                           
           
    void HandleQueueChanged();
        
    static TInt ProcessQueue( TAny* aAny );
    
private:     
    // data	

    /** UI Controller Manager, Not owned */
    CAiUiControllerManager& iUiControllerManager;
    /** State Manager, Not owned */
    CAiStateManager* iStateManager;
    /** Cps command buffer, Owned */
    CAiCpsCommandBuffer* iCommandBuffer;
    /** Queue starter, Owned */
    CPeriodic* iStarter;
    /** Array of loaded data plugins, Owned */
    mutable RPointerArray< CHsContentPublisher > iPublishers;
    /** Ecom implementation info, Owned */
    RImplInfoPtrArray iEComPlugins;		          
    /** Load queue, Owned */
    RArray< TAiFwPublisherInfo > iLoadQueue;
    /** Destroy queue, Owned */
    RArray< TAiFwPublisherInfo > iDestroyQueue;
    /** Flag to determine wheter flush is allowed */
    TBool iAllowFlush;
    
private: 
    // friend classes
    
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiPluginFactory;
#endif
    };

#endif // _AIPLUGINCFACTORY_H

// End of File.
