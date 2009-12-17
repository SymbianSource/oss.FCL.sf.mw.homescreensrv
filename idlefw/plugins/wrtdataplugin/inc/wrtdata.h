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


#ifndef WRTDATA_H
#define WRTDATA_H

// INCLUDE FILES
#include <LiwCommon.h> 
#include <AknsItemID.h>
#include "wrtdatapluginconst.h"
#include "aicontentpublisher.h"

class MLiwInterface;
class CLiwServiceHandler;
class CWrtDataObserver;
class CWrtDataPlugin;

/**
 *  @ingroup group_wrtdataplugin
 *
 *  Wrt data
 *
 *  @since S60 v3.2
 */
class CWrtData : public CBase
   {
   public:

    /**
    * Part of the two phased construction
    *
    * @param aPlugin refrence of the plugin
    * @return none
    */
    static CWrtData* NewL(CWrtDataPlugin* aPlugin);
    
    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CWrtData();
    
   private :
    
    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CWrtData();
   
    
    /**
    * Part of the two phased construction
    *
    * @param aPlugin reference of the plugin
    * @return void
    */
    void ConstructL(CWrtDataPlugin* aPlugin);
   
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
    
    /**
    * Publish updated data for all the items in the widget
    *     
    * @param aObserver to publish data
    * @return void
    */
    void PublishAllL( MAiContentObserver* aObserver );
    
    /**
	* Execute the command to get the data from CPS
	* 
	* @param aInFilter input filter for the command
	* @param aOutDataMap output data map 
	* @param aRegistry type of registry (publisher/cp_data)
	* @return void 
	*/
    void ExecuteCommandL(CLiwDefaultMap* aInFilter, 
    		CLiwDefaultMap* aOutDataMap, const TDesC16& aRegistry );
   
    /**
	* Checks is this menu item is supported by the publisher
	* 
	* @param aMenuItem  menu item name.
	* @return  boolean (ETrue/EFalse) 
	*/
    TBool HasMenuItem(const TDesC16& aMenuItem );
    
    /**
    * Publish the updated data
    *   
    * @param aObserver to publish data
    * @return void
    */
    void PublishL( MAiContentObserver* aObserver );
    
    /**
    * Tigger for execution of a action for a specific content id.
    * 
    * @param aObjectId object Id.
    * @param aTrigger name of the trigger.
    * @return void
    */
    void ExecuteActionL(const TDesC& aObjectId, const TDesC& aTrigger);
     
    /**
    * Register to CPS for all (add/delete/update/execute) action
    * 
    * @param none
    * @return void
    */
    void RegisterL();
     
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
    * Createts the filter map
    * 
    * @return filter map
    */
    CLiwDefaultMap* CreateFilterLC( );
      
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
	* @param None
	* @return void
	*/
    void UpdatePublisherStatusL();
    
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

    private :   
    
    // Subscriber interface
    // own
    MLiwInterface* iInterface;
      
    // Data Observer to CPS
    // Own
    CWrtDataObserver* iObserver;
   
    // Service handler 
    // Own
    CLiwServiceHandler* iServiceHandler;

    // Command name in configuration Array
    HBufC8* iCommandName;
      
    // Reference of the wrt data plugin
    // Not owned
    CWrtDataPlugin* iPlugin;
    
    // Menu item names
    // Own
    RPointerArray<HBufC16> iMenuItems; 
    
    // Trigger names for the menu items
    // Own
    RPointerArray<HBufC8> iMenuTriggers;
    
    HBufC* iContentId;
    };

#endif /*WRTDATA_H*/
