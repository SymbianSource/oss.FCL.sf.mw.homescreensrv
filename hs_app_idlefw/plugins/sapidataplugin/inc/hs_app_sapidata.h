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


#ifndef SAPIDATA_H
#define SAPIDATA_H

// INCLUDE FILES
#include <liwcommon.h> 
#include <AknsItemID.h>
#include <aieventhandlerextension.h>
#include "hs_app_sapidatapluginconst.h"
#include "aicontentpublisher.h"

class MLiwInterface;
class CLiwServiceHandler;
class CSapiDataObserver;
class CSapiDataPlugin;


class CContentItem :  public CBase
	{
	public :

	/**
	* Part of the two phased constuction
	*
	* @param none
	* @return none
	*/
	static CContentItem* NewL();

	
	/*
	* Destructor  
	*/
	~CContentItem();

	private :
		
	/* 
	* Constructor  
	*/
		CContentItem();   
	
	/**
	* Part of the two phased construction
	*
	* @param none
	* @return void
	*/
	void ConstructL();

	public :

    TInt iId;
	/* item id */
	HBufC* iName;
	/* type (image/text) */
    HBufC* iType;
    
	HBufC* iContentType;
	};

/**
 *  @ingroup group_sapidataplugin
 *
 *  Sapi data
 *
 *  @since S60 v3.2
 */
class CSapiData : public CBase
   {
   public:
 
    /**
    * Part of the two phased construction
    *
    * @param aPlugin refrence of the plugin
    * @return none
    */
    static CSapiData* NewL(CSapiDataPlugin* aPlugin);
    
    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CSapiData();
    
   private :


    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CSapiData();
   
    
    /**
    * Part of the two phased construction
    *
    * @param aPlugin reference of the plugin
    * @return void
    */
    void ConstructL(CSapiDataPlugin* aPlugin);
   
    /**
    * Change the publisher status
    *
    * @param aStatus new status of the publisher
    * @return void
    */
    void ChangePublisherStatusL(const TDesC& aStatus);
    
    /**
    * Gets the menu item from the publisher
    *
    * @param none
    * @return void
    */
    void GetMenuItemsL();
    
   public :
    
    /**
    * Configures the subscriber and data to subscribe.
    *  
    * @param aConfigurations  Information about the subscriber 
    *  and the data to subscribe.
    * @return void
    */
    void ConfigureL(RAiSettingsItemArray& aConfigurations);
    
    void SetContentIdL(const TDesC8& aId);
    
    /**
	* Execute the command to get the data from CPS
	*
	* @param aRegistry type of registry (publisher/cp_data) 
	* @param aInFilter input filter for the command
	* @param outParamList output data map 
	* @return void 
	*/
    void ExecuteCommandL(const TDesC& aRegistry ,
    		CLiwDefaultMap* aInFilter, CLiwGenericParamList* outParamList  );
   
    /**
	* Checks Can Update for this publisher 
	* 
	* @param aPublisher  publisher.
    * @param aContentType  content type.
    * @param aContentId content Id.
    * @return bool
	*/
    TBool CanUpdate( TDesC& aPublisher, TDesC& aContentType, TDesC& aContentId );
    
    /**
   	* Removes all the data from the widget which matches to 
   	* this publisher, contentype, contentid values
   	*
    * @param aObserver to publish data
    * @param aContentType content type 
    * @return void
   	*/
    void RemoveL( MAiContentObserver* aObserver,  TDesC& aContentType );
   
    /**
	* Checks is this menu item is supported by the publisher
	* 
	* @param aMenuItem  menu item name.
	* @return  boolean (ETrue/EFalse) 
	*/
    TBool HasMenuItem(const TDesC& aMenuItem );
    
    /**
    * Publish the data to widget.
    *   
    * @param aObserver to publish data
    * @param aContentType content type
    * @return void
    */
    void PublishL( MAiContentObserver* aObserver, const TDesC& aContentType );
    
    /**
    * Publish the updated data to widget.
    *   
    * @param aObserver to publish data
    * @param aDataMap data map with updated data
    * @return void
    */
    void PublishDataL(MAiContentObserver* aObserver, CLiwDefaultMap* aDataMap );
    
    /**
    * Tigger for execution of a action for a specific content id.
    * 
    * @param aObjectId object Id.
    * @param aTrigger name of the trigger.
    * @return void
    */
    void ExecuteActionL(const TDesC& aObjectId, const TDesC& aTrigger);
     
    /**
    * Register to publisher registry for all (update) action
    * 
    * @param none
    * @return void
    */
    void RegisterPublisherObserverL();
    
    /**
    * Register to content registry for all (add/delete/update) action
    * 
    * @param none
    * @return void
    */
    void RegisterContentObserverL();
     
    /**
    * Called by the observer to refresh the changed content
    *   
    * @param aPublisher  publisher.
    * @param aContentType  content type.
    * @param aContentId content Id.
    * @param aOperation operation (add/delete/update/execute).
    * @return void
    */
    void RefreshL( TDesC& aPublisher, TDesC& aContentType, 
            TDesC& aContentId, TDesC& aOperation );
     
    /**
    * Createts the filter map and push it in the stack
    * 
    * @return filter map
    */
    CLiwDefaultMap* CreateFilterLC();
    
    /**
    * Createts the filter map and push it in the stack
    * 
    * @param aConType content type
    * @return filter map
    */
    CLiwDefaultMap* CreateFilterLC(const TDesC& aConType);
    
    /**
    * Createts the filter map and push it in the stack
    * 
    * @param aConType content type
    * @param aContentId cotent Id
    * @return filter map
    */
    CLiwDefaultMap* CreateFilterLC(const TDesC& aContentType,
    		const TDesC& aContentId);
    		
    /**
    * Is the pugin is active to publish the data.
    * 
    * @param None
    * @return boolean (ETrue/EFalse).
    */
    TBool IsPluginActive();
     
    /**
    * Resume the publisher
    * 
    * @param None
    * @return void
    */
    void ResumeL();
    
    /**
	* Suspend the publisher
	* 
	* @param None
	* @return void
	*/
    void SuspendL();
    
    /**
	* Activate the publisher
	* 
	* @param None
	* @return void
	*/
    void ActivateL();
    
    /**
	* Deactivate the publisher
	* 
	* @param None
	* @return void
	*/
    void DeActivateL();
    
    /**
	* OnLineL 
	* 
	* @param None
	* @return void
	*/
    void OnLineL();
    
    /**
	* OffLineL 
	* 
	* @param None
	* @return void
	*/
    void OffLineL();
    
    /**
	* InActiveL 
	* 
	* @param None
	* @return void
	*/
	void InActiveL();
	
    /**
	* Update the publisher status 
	* 
	* @param aPublisher publisher name
	* @return void
	*/
    void UpdatePublisherStatusL(TDesC& aPublisher);
    
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
	* Sets the on resume update needed status  
	* 
	* @param aStatus update needed status (ETrue/EFalse)
	* @return void  
	*/
    void SetUpdateNeeded(TBool aStatus);
    
    private :   
    
    // Subscriber interface
    // own
    MLiwInterface* iInterface;
      
    // Data Observer to CPS content registry
    // Own // iConObserver;
    CSapiDataObserver* iContentObserver;
    
    // Data Observer to CPS publisher registry
    // Own // iConObserver;
    CSapiDataObserver* iPubObserver;
   
    // Service handler 
    // Own
    CLiwServiceHandler* iServiceHandler;
    
    // Array of configurations
    // Own
    RPointerArray<CContentItem> iItemList;

    // Number of configurations 
    TInt iItemCount;
    
    // Command name in configuration Array
    HBufC8* iCommandName;
	/* publisher id */
	HBufC* iPublisher;
	/* content type */
	HBufC* iContentType;
	/* content id */
	HBufC* iContentId;

    // Reference of the sapi data plugin
    // Not owned
    CSapiDataPlugin* iPlugin;
    
    // Menu item names
    // Own
    RPointerArray<HBufC> iMenuItems; 
    
    // Trigger names for the menu items
    // Own
    RPointerArray<HBufC8> iMenuTriggers;
    
    // Store the status of update needed on resume
    TBool iUpdateNeeded;
    };

#endif /*SAPIDATA_H*/