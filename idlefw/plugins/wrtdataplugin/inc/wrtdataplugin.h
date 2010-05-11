/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in main class
*
*/


#ifndef WRTDATAPLUGIN_H
#define WRTDATAPLUGIN_H

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aicontentmodel.h>

// Forward declarations
class MAiContentObserver;
class MAiContentItemIterator;
class CWrtData;
class CGulIcon;
class CLiwDefaultMap;

/**
 *  @ingroup group_wrtdataplugin
 *
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CWrtDataPlugin ) : public CHsContentPublisher
    {    
public:
    // type definitions
    
     /**
     * Plugin's network state.
     */
    enum TPluginNetworkStatus 
        {
        EUnknown, 
        EOffline,
        EOnline
        };
    
    /**
     * Plugin's state.
     */
    enum TPluginStates
        {
        ENone,
        EResume,
        ESuspend
        };
    
    /**
     * Content Items
     */
    enum TContentItem
        {
        EDefaultImage,
        EDefaultText,
        EImage1
        };  
        
public:
    // constructor and destructor

    static CWrtDataPlugin* NewL();
    
    ~CWrtDataPlugin();

private:
    // constructors
    
    /**
    * C++ constructor
    */
    CWrtDataPlugin();

    /**
    * 2nd phase constructor
    */
    void ConstructL();
    
public: 
    // from CHsContentPublisher
    
    /**
     * @see CHsContentPublisher
     */
    void Start( TStartReason aReason );

    /**
     * @see CHsContentPublisher
     */    
    void Stop( TStopReason aReason );
    
    /**
     * @see CHsContentPublisher
     */    
    void Resume( TResumeReason aReason );
    
    /**
     * @see CHsContentPublisher
     */    
    void Suspend( TSuspendReason aReason );
    
    /**
     * @see CHsContentPublisher
     */    
    void SetOnline();
    
    /**
     * @see CHsContentPublisher
     */
    void SetOffline();
    
    /**
     * @see CHsContentPublisher
     */    
    void SubscribeL( MAiContentObserver& aObserver );
    
    /**
     * @see CHsContentPublisher
     */    
    void ConfigureL( RAiSettingsItemArray& aSettings );
    
    /**
     * @see CHsContentPublisher
     */
    void SetProperty( TProperty aProperty, TAny* aAny );
    
    /**
     * @see CHsContentPublisher
     */    
    TAny* GetProperty( TProperty aProperty );
    
    /**
     * @see CHsContentPublisher
     */    
    void HandleEvent( const TDesC& aEventName, const TDesC& aParam );

public:

    /**
    * Gets the id of a content  
    *
    * @param aObjectId image or text id
    * @return id of the content
    */
    TInt GetIdL( TDesC16& aObjectId );
    
    /**
    * Gets the type of a specific content
    *
    * @param aObjectId image or text id
    * @param aType type
    * @return void
    */
    void GetTypeL( TDesC16& aObjectId, TDes16& aType );
    
    /**
    * RefereshL a specific image of text in the widget
    *
    * @param aOperation operation performed
	* @param aDataMap data map
    * @return void
    */
    void RefreshL( TDesC16& aOperation, CLiwDefaultMap* aDataMap );
    
    /**
    * Is plugin active to publish the data 
    *
    * @param void 
    * @return boolean (ETrue/EFalse)
    */
    TBool IsActive() const;
    
    /**
    * Publish a specific text of the widget  
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @param aContentValue content value
    * @return void
    */
    void PublishTextL( MAiContentObserver* aObserver, 
        TInt aContentId, const TDesC16& aContentValue );

    /**
    * Publish a specific image of the widget  
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @param aHandle image handle 
    * @param aMaskHandle handle of the mask image
    * @return void
    */
    void PublishImageL( MAiContentObserver* aObserver, 
        TContentItem aContentId, TInt aHandle, TInt aMaskHandle );

    /**
    * Publish a specific image of the widget  
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @param aPath image path / skin id pattern / mif id Pattern 
    * @return void
    */
    void PublishImageL( MAiContentObserver* aObserver,
            TContentItem aContentId, const TDesC16& aPath );
    
    /**
    * Cleans a data from the widget
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @return void
    */
    void Clean( MAiContentObserver* aObserver, 
            TInt aContentId );

   /**
    * Shows the loading icon animation 
    *
    * @param aObserver observer
    * @return void
    */
    void ShowLoadingIcon( MAiContentObserver* aObserver );

    /**
    * Hides the loading icon animation 
    *
    * @param aObserver observer
    * @return void
    */
    void HideLoadingIcon( MAiContentObserver* aObserver );    
	
    /**
     * CWrtData getter
     * @return Pointer to CWrtData
     */
    CWrtData* Data() const;

    /*
     * Plugin's network status getter
     * @return Pointer to Harvester status observer
     */
    TPluginNetworkStatus NetworkStatus() const;
    
    /**
    * Creates initial data republishing timer if needed and starts it
    */
    void StartTimer();

private:
    // new functions
    
    /**
    * Publishes widget's initial texts and images
    *
    * @param void
    * @return void
    */
    void PublishInitialDataL();
        
	/**
	* Resolves skin item id and Mif id from pattern 
	* skin( <majorId> <minorId> (<colourGroupId>) 
	* mif(<MifFileName.mif> <bitmapId> <maskId>)
	* 
	* @param aPath  skin pattern / mif pattern value
	* @param aItemId skin item id  
	* @param aMifId  mif id 
	* @param aMaskId  mask id 
	* @param aFilename mif file name
	* @return boolean (ETrue/EFalse)  
	*/
	TBool ResolveSkinIdAndMifId( const TDesC& aPath, TAknsItemID& aItemId,
	        TInt& aMifId, TInt& aMaskId, TDes& aFilename );

    /**
    * Cancels transaction in case of leave
    *
    * @param aObserver Transaction target
    */
    static void CancelTransaction( TAny* aObserver );
  
    /**
    * Cancels initial data republishing timer
    */
    void CancelTimer();

    /**
    * Stops and deletes initial data republishing timer. 
    */
    void StopTimer();

    /**
    * Timeout callback from timer. Used if publishing of initial data has failed.
    *
    * @param aPtr Contains pointer to instance of this class 
    */
    static TInt Timeout( TAny* aPtr );


private: 
    // data

    /** Iterator for plugin content, owned */
    MAiContentItemIterator* iContent;
    /** Array of content observers, owned */
    RPointerArray< MAiContentObserver > iObservers;     
    /** Number of data in the content model */
    TInt iDataCount;     
    /** Dynamic content model, owned */ 
    TAiContentItem* iContentModel;  
    /** Reference array for Published text, owned */    
    RPointerArray< HBufC > iDataArray;  
    /** Service API Data Subscriber, owned */   
    CWrtData* iData;    
    /* References array for published images, owned */     
    RArray< CGulIcon* > iIconArray;    
    /** Plugin's network status */
    TPluginNetworkStatus iNetworkStatus;    
    /** Plugin state */    
    TPluginStates iPluginState;
    /** File server session handle, owned */
    RFs iRfs;
    /** Timer for initial data republishing, owned */
    CPeriodic* iTimer;
    /** Flag to indicate if this plugin is stopped */
    TBool iStopped;
    };

#endif // WRTDATAPLUGIN_H


