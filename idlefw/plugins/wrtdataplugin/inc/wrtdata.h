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

// System includes
#include <liwcommon.h> 
#include <AknsItemID.h>

// User includes
#include <hscontentpublisher.h>
#include "wrtdatapluginconst.h"

// Forward declarations
class MLiwInterface;
class CLiwServiceHandler;
class CWrtDataObserver;
class CWrtDataPlugin;
class MAiCpsCommandBuffer;

/**
 *  @ingroup group_wrtdataplugin
 *
 *  Wrt data
 *
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CWrtData ) : public CBase
    {    
public:
    
    /**
    * Part of the two phased constuction
    *
    * @param none
    * @return none
    */
    static CWrtData* NewL( CWrtDataPlugin* aPlugin );
    
    /*
    * Destructor  
    */
    ~CWrtData();

private :

    /**
    * Default Constructor
    *
    */
    CWrtData();
    
    /**
    * Part of the two phased construction
    *
    * @param aPlugin reference of the plugin
    * @return void
    */
    void ConstructL(CWrtDataPlugin* aPlugin);
    
public : 
    // new functions 
    
    /**
    * Configures the subscriber and data to subscribe.
    *  
    * @param aConfigurations  Information about the subscriber 
    *  and the data to subscribe.
    * @return void
    */
    void ConfigureL(RAiSettingsItemArray& aConfigurations);
 
    /**
    * Checks is this menu item is supported by the publisher
    * 
    * @param aMenuItem  menu item name.
    * @return  boolean (ETrue/EFalse) 
    */
    TBool HasMenuItem(const TDesC16& aMenuItem );

    /**
    * Register to CPS for all (add/delete/update/execute) action
    * 
    * @param none
    * @return void
    */
    void RegisterL();
    
   /**
    * Update the publisher status 
    * 
    * @param None
    * @return void
    */
    void UpdatePublisherStatusL();     
          
    /**
    * Publish updated data for all the items in the widget
    *     
    * @param aObserver to publish data
    * @return void
    */
    void PublishDefaultImageL( MAiContentObserver* aObserver );
            
    /**
    * Publish the updated data
    *   
    * @param aObserver to publish data
    * @param aDataMap data map
    * @return void
    */
    void PublishL( MAiContentObserver* aObserver, CLiwDefaultMap* aDataMap );
    
    /**
    * Called by the observer to refresh the changed content
    *   
    * @param aContentId content Id.
    * @param aOperation operation (add/delete/update/execute).
    * @param aDataMap data map.
    * @return void
    */
    void RefreshL( TDesC& aContentId, TDesC& aOperation, CLiwDefaultMap* aDataMap );
            
    /**
    * Tigger for execution of a action for a specific content id.
    * 
    * @param aObjectId object Id.
    * @param aTrigger name of the trigger.
    * @return void
    */
    void ExecuteActionL(const TDesC& aObjectId, const TDesC& aTrigger);

    /**
    * Is the pugin is active to publish the data.
    * 
    * @param None
    * @return boolean (ETrue/EFalse).
    */
    TBool IsPluginActive();

    /**
    * Notify the status to the publisher
    *
    * @param aStatus new status of the publisher
    * @return void
    */
    void NotifyPublisherL(const TDesC8& aStatus);
    
    /**
    * Sets property value.
    *
    * @since S60 5.2
    * @param aAny - contains pointer to command buffer.
    * @param aNameSpace - plugin name space id
    */
    void SetCommandBuffer(TAny* aAny, const TDesC8& aNameSpace);
    
private: 
    // new functions
     
    /**
    * Createts the filter map
    * 
    * @return filter map
    */
    CLiwDefaultMap* CreateFilterLC( );
      
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
    * Gets the menu item from the publisher
    *
    * @param none
    * @return void
    */
    void GetMenuItemsL();
    
    /**
    * Gets the widgent name and uid 
    *
    * @param aName  - widget name
    * @param aAppUID - widget uid
    * @return void
    */
    void GetWidgetNameAndUidL(TDes& aName, TDes& aAppUID );
    
    /**
    * Resolves the Uid from the string  
    *
    * @param aUidDes  - uid in string
    * @param aUid - uid
    * @return ETure/EFalse
    */ 
    TBool ResolveUid(const TDesC& aUidDes, TUid& aUid );
    
    /**
    * Creates icon from the uid
    *
    * @param aHandle  - icon handle
    * @param aMaskHandle - mask handle
    * @param aAppUid - application uid
    * @return ETure/EFalse
    */   
    void CreateIconFromUidL(TInt& aHandle, TInt& aMaskHandle, const TUid& aAppUid );

    /**
    * Resend the the current plugin status to publisher 
    *
    * @param aActionsList new list of status for the publisher
    * @return void
    */
    void ReSendNotificationL(CLiwDefaultList* aActionsList);
            
private :   
    // data
    TBuf<KHsPublisherNamespaceMaxLength> iPluginId;
    /** CPS Command Buffer Interface, Not Owned */
    MAiCpsCommandBuffer* iCpsExecute;
    /** Subscriber interface, owned */    
    MLiwInterface* iInterface;     
    /** Data Observer to CPS, owned */
    CWrtDataObserver* iObserver;   
    /** Service handler, owned */    
    CLiwServiceHandler* iServiceHandler;
    /** Command name in configuration Array, owned */
    HBufC8* iCommandName;      
    /** Reference of the wrt data plugin, not owned */    
    CWrtDataPlugin* iPlugin;    
    /** Menu item names, owned */    
    RPointerArray<HBufC16> iMenuItems;     
    /** Trigger names for the menu items, owned */    
    RPointerArray<HBufC8> iMenuTriggers;    
    /** Content id, owned */
    HBufC* iContentId;
    };

#endif /*WRTDATA_H*/
