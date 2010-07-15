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
* Description:  UI controller stub
*
*/


#ifndef _AIUICONTROLLER_STUB_H
#define _AIUICONTROLLER_STUB_H

// System includes
#include <e32base.h>

// User includes
#include <aiuicontroller.h>
#include <aicontentobserver.h>

#include <hscontentpublisher.h>

// Forward declarations
class MAiContentObserver;
class MAiFwEventHandler;
class MAiFwStateHandler;
class MAiMainUiController;
class MAiSecondaryUiController;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 *  Active Idle UI Controller stub.
 *
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CAiUiControllerStub ) : public CAiUiController,
    public MAiMainUiController,
    public MAiSecondaryUiController,
    public MAiContentObserver
	{
public: 		
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */ 
    static CAiUiControllerStub* NewL();

    /**
     * Two-phased constructor.
     */     
    static CAiUiControllerStub* NewL( TUid aImpUid );
    
    /**
     * Destructor
     */        
    ~CAiUiControllerStub();
    
public: 
    // from CAiUiController
    
    /**
     * Instructs this UI controller to load its UI definition.
     *
     * @since Series 60 3.2
     */
    void LoadUIDefinitionL();

    /**
     * Retrieves plug-in settings specified in this UI controller's UI definition.
     *
     * @param aPublisherInfo Publisher info of the plug-in for which to 
     *                       retrieve settings for.
     * @param aSettings array to get settings items to.
     */
    void GetSettingsL( 
        const THsPublisherInfo& aPublisherInfo, 
        RAiSettingsItemArray& aSettings );

    /**
     * Activate UI managed by this UI controller.
     *
     * @since Series 60 3.2
     */
    void ActivateUI();

    /**
     * Returns content observer interface implemented by this UI controller.
     *
     * @since Series 60 3.2
     * @return reference to content observer implementation.
     */
    MAiContentObserver& GetContentObserver();

    /**
     * Sets the Active Idle Framework event handler for this UI Controller.
     *
     * @since Series 60 3.2
     * @param aEventHandler Framework event handler for this UI controller. 
     *                      Set to NULL to disable event callbacks from this
     *                      UI Controller.
     */
    void SetEventHandler( MAiFwEventHandler& aEventHandler );
    
    /**
     * Sets plugin state handler
     * 
     * @since S60 5.2
     * @param aHandler Plugin State Handler     
     */
    void SetStateHandler( MAiFwStateHandler& aStateHandler ); 
    
    /**
     * Returns the main UI Controller interface, or NULL if this is not the 
     * main UI controller.
     */
    MAiMainUiController* MainInterface();

    /**
     * Returns the secondary UI Controller interface, or NULL if this is not
     * a secondary UI controller.
     */
    MAiSecondaryUiController* SecondaryInterface();   

public:
    // from MAiMainUiController
    
    /**
     * @see MAiMainUiController
     */
    void RunApplicationL();

    /**
     * @see MAiMainUiController
     */
    CCoeEnv& CoeEnv();
    
    /**
     * @see MAiMainUiController
     */    
    void Exit();

    /**
     * @see MAiMainUiController
     */        
    TBool IsMenuOpen();
       
public:
    // from MAiSecondaryUiController

    /**
     * @see MAiSecondaryUiController
     */        
    void SetCoeEnv( CCoeEnv& aCoeEnv );
    
public:
    // from MAiContentObserver
    
    /**
     * @see MAiContentObserver
     */
    TInt StartTransaction( TInt aTxId );

    TInt Commit( TInt aTxId );

    TInt CancelTransaction( TInt aTxId );

    TBool CanPublish( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );

    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex );

    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex );

    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );
    
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );

    TInt Clean( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );

    TAny* Extension( TUid aUid );

    TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const;     
    
    TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue );
    
    TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue,  
            MAiContentObserver::TValueType aValueType);    
private: 
    // private constructors

    /**
     * Leaving constructor
     */    
    void ConstructL();
    
    /**
     * C++ default constructor
     */     
    CAiUiControllerStub();
        
public: 
    // data
    TBool iMain;
    TBool iRunning;
    TBool iExit;
    TBool iActive; 
    TBool iSettings;
    TBool iUiDefinition;
    TBool iEventHandler;
    TBool iStateHandler;    
	};

#endif // _AIUICONTROLLER_STUB_H

