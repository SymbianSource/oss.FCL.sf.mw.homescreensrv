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
* Description:  SAPI data plug-in main class
*
*/


#ifndef SAPIDATAPLUGIN_H
#define SAPIDATAPLUGIN_H

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aicontentmodel.h>

// Forward declarations
class MAiContentObserver;
class MAiContentItemIterator;
class CSapiData;
class CGulIcon;
class CLiwDefaultMap;

/**
 *  @ingroup group_sapidataplugin
 *
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSapiDataPlugin ) : public CHsContentPublisher                    
    {   
public:
    // data types
    
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
	    EStopped,
	    EStarted,
		EResume,
		ESuspend			
		};
	
public:
    // constructor and destructor
    
    /**
    * Part of the two phased constuction
    *
    * @param none
    * @return none
    */
    static CSapiDataPlugin* NewL();
    
    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CSapiDataPlugin();

private:
    // constructors
    
    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CSapiDataPlugin();
	
    /**
    * Part of the two phased construction
    *
    * @param void
    * @return void
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

    /**
     * @see CHsContentPublisher
     */
    TBool HasMenuItem( const TDesC16& aMenuItem );
     
private:
    // new functions
    
    void StartL( TStartReason aReason );
    void StopL( TStopReason aReason );
    
public: 
    // new functions
	
    /**
    * Publishes widget's texts and images
    *
    * @param void
    * @return void
    */
    void PublishL();	
	
	/**
	* Gets the id of a content  
	*
	* @param aObjectId image or text id
	* @return id of the content
	*/
	TInt GetIdL(TDesC& aObjectId);
	
    /**
    * Gets the type of a specific content
    *
    * @param aObjectId image or text id
    * @return void
    */
	const TDesC& GetTypeL( TDesC& aObjectId );
	
    /**
    * RefereshL a specific image of text in the widget
    *
    * @param aContentType   content type
    * @param aOperation     operation performed
    * @param aDataMap       data map if available. Can be NULL.
    *                       Ownership NOT transferred.
    * @return void
    */
    void RefreshL( TDesC& aContentType,
                   TDesC& aOperation,
                   CLiwDefaultMap* aDataMap );
    
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
	void PublishTextL(MAiContentObserver* aObserver, 
			TInt& aContentId, TDesC& aContentValue);

	/**
    * Publish a specific image of the widget  
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @param aHandle image handle 
    * @param aMaskHandle handle of the mask image
    * @return void
    */
    void PublishImageL(MAiContentObserver* aObserver, 
            TInt& aContentId, TInt aHandle, TInt aMaskHandle);

    /**
    * Publish a specific image of the widget  
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @param aPath image path / skin id pattern / mif id Pattern 
    * @return void
    */
    void PublishImageL(MAiContentObserver* aObserver,
            TInt aContentId, TDesC& aPath );
    
    /**
    * Cleans a data from the widget
    *
    * @param aObserver observer
    * @param aContentId content model id
    * @return void
    */
    void Clean(MAiContentObserver* aObserver, 
            TInt& aContentId );
    
    /**
     * CSapiData getter
     * @return Pointer to CSapiData
     */
    CSapiData* Data() const;

	/*
	 * Plugin's network status getter
	 */
	TPluginNetworkStatus NetworkStatus() const;
	
private: 
    // data
    /** Iterator for plugin content, owned */    
    MAiContentItemIterator* iContent;
    /** Array of content observers, owned */
    RPointerArray<MAiContentObserver> iObservers;       
	/** Number of data in the content model */
	TInt iDataCount;	
	/** Dynamic content model, owned */	
	TAiContentItem* iContentModel;	
	/** Reference array for Published text, owned */
	RPointerArray< HBufC > iDataArray;	
	/** Service API Data Subscriber, owned */	
	CSapiData* iData;    
    /** References array for published images, owned */     
    RArray< CGulIcon* > iIconArray;    
    /** Handle to file server session, owned */    
    RFs iRfs;    
    /** Plugin's network status */
    TPluginNetworkStatus iNetworkStatus;       
	/** Plugin state */    
    TPluginStates iPluginState;
    /** Flag to indicate whether observer is registered */
    TBool iPubObsRegistered;
    };

#endif // SAPIDATAPLUGIN_H

// End of file

