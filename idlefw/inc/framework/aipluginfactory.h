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
NONSHARABLE_CLASS( CAiPluginFactory ) : public CBase
	{
public:
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */	
    static CAiPluginFactory* NewL( CAiUiControllerManager& aManager );
	
    /**
     * Destructor
     */    
    ~CAiPluginFactory();

public:
    // new functions
		
    /**
     * Create plugin
     *
     * @since S60 5.2
     * @param aPublisherInfo plugin to create. Factory keeps plugin's ownership. 
     * @return KErrNone if plugin is created succesfully, otherwise system wide error code.      
     */
    TInt CreatePlugin( 
        const THsPublisherInfo& aPublisherInfo );						
                         						
    /**
     * Destroy plugin
     *
     * @since S60 5.2
     * @param aPublisherInfo plugin to destroy.      
     */
    void DestroyPlugin( 
        const THsPublisherInfo& aPublisherInfo );		    
                         
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
            
private:	
    // private constructors

    /**
     * Leaving constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */	
    CAiPluginFactory( CAiUiControllerManager& aManager );
		
private:    								
    // new functions
	       
    void CreatePluginL( 
        const THsPublisherInfo& aPublisherInfo );
                            
    void SubscribeContentObserversL( 
        CHsContentPublisher& aContentPublisher,		    
        const THsPublisherInfo& aPublisherInfo );
                                         
    void ConfigurePluginL(         
        CHsContentPublisher& aContentPublisher,
        const THsPublisherInfo& aPublisherInfo );		                           
    
    RPointerArray< CHsContentPublisher >& Publishers() const;
    
    void ResolvePluginsToUpgradeL( 
        RArray< THsPublisherInfo >& aArray );
    
private:     
    // data	

    /** UI Controller Manager, Not owned */
    CAiUiControllerManager& iUiControllerManager;
    /** Array of loaded data plugins, Owned */
    mutable RPointerArray< CHsContentPublisher > iPublishers;
    /** Ecom implementation info, Owned */
    RImplInfoPtrArray iEComPlugins;		          
    
private: 
    // friend classes
    friend class CAiStateManager;
    
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiPluginFactory;
#endif
    };

#endif // _AIPLUGINCFACTORY_H

// End of File.
