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
* Description:  Header file for the CAiProfileEngine class
*
*/


#ifndef CAIPROFILEENGINE_H
#define CAIPROFILEENGINE_H

// System includes
#include <e32base.h>
#include <badesca.h>
#include <ConeResLoader.h>
#include <MSSSettingsObserver.h>
#include <MProEngActiveProfileObserver.h>
#include <MProEngProfileNameArrayObserver.h>
#include <MProEngProfileActivationObserver.h>

// User includes
#include "maiprofilepluginnotifier.h"

// Forward declarations
class MProEngEngine;
class MProEngNotifyHandler;
class MProEngProfileNameArray;

class RSSSettings;
class CAknQueryDialog;

/**
 *  @ingroup group_profileplugin
 *
 *  CAiProfileEngine
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAiProfileEngine ) : public CBase, 
    public MSSSettingsObserver,
    public MProEngActiveProfileObserver,
    public MProEngProfileNameArrayObserver,
    public MProEngProfileActivationObserver     						
    {
public:  
    // constructors and destructor

    /**
    * Part of the two phased constuction
    *
    * @param MAiProfilePluginNotifier* aNotifier a pointer to a object implementing MAiProfilePluginNotifier
            (the object ordering a notification of the content status change)
    * @return Pointer to the created CAiProfileEngine object
    */
    static CAiProfileEngine* NewL( MAiProfilePluginNotifier* aProfilePluginNotifier );

    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CAiProfileEngine();

private:
    // constructors
    
    /**
    * Constructor to use in the object creation. Initializes the necessary data.
    *
    * @param MAiProfilePluginNotifier* aNotifier a pointer to a object implementing MAiProfilePluginNotifier
            (the object ordering a notification of the content status change)
    * @return none
    */
    CAiProfileEngine( MAiProfilePluginNotifier* aProfilePluginNotifier );
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
public:
    // New functions
    
	const TDesC& ActiveProfileName() const;
	
	const TDesC& SwapProfileName() const;
		
	TInt NumberOfProfiles() const;
	
	const TDesC& ProfileNameByIndex( TInt aIndex ) const;
	
	void HandleAiEventL( TInt aEvent, const TDesC& aParam );
	
	void SetActiveProfileL( const TInt aProfileId );
	
	TBool IsActiveProfileSilentL() const;
	
	TBool IsActiveProfileTimedL() const;
	
	void UpdateProfileNamesL();
	
	TBool IsOffline() const;
   
private:   
    // new functions
    
    void SetActiveProfileNameL( const TDesC& aName );
    
    void SetSwapProfileNameL( const TDesC& aName );
    
    void SetProfileNameListL( const MProEngProfileNameArray& aArray );
    
    void HandleSwitchByNameL( const TDesC& aParam );
    
    void HandleSwitchByIndexL( const TDesC& aParam );
    
    void HandleSwapL( const TDesC& aParam );
    
    void HandleEditActiveProfileL();
    
    TBool ShowOfflineMessageL();
    
    void DetermineTimedAndSilentStatesL();
    
    void NotifyContentUpdate();
  
private:
    // from MSSSettingsObserver

    /**
     * @see MSSSettingsObserver
     */
    void PhoneSettingChanged( 
        TSSSettingsSetting aSetting, TInt aNewValue );
        
private:
    // from MProEngActiveProfileObserver

    /**
     * @see MProEngActiveProfileObserver    
     */        
    void HandleActiveProfileModifiedL();
    
private:
    // from MProEngProfileNameArrayObserver
    
    /**
     * @see MProEngProfileNameArrayObserver    
     */        
    void HandleProfileNameArrayModificationL();
    
private:
    // from MProEngProfileActivationObserver
    
    /**
     * @see MProEngProfileActivationObserver    
     */    
    void HandleProfileActivatedL( TInt aProfileId ); 
    
private:
    // data
    
    /** MAiProfilePluginNotifier interface, not owned */ 
    MAiProfilePluginNotifier* iProfilePluginNotifier;    
    /** Client to listen SS setting changes, owned */
    RSSSettings iSSSettings;
    /** Profile engine, owned */
    MProEngEngine* iProfileEngine;
    /** Profile change notifier, owned */
    MProEngNotifyHandler* iProfileNotifier;    
    /** Active profile name, owned */
    HBufC* iActiveProfileName;    
    /** Swap profile name, owned */
    HBufC* iSwapProfileName;    
    /** Array of Profile Names, owned */
    RPointerArray< HBufC > iProfileNamePointerArray;    
    /** Resource loader, owned */
    RConeResourceLoader iResourceLoader;
    /** Flag to indicate whether active profile is timed */
    TBool iTimed;
    /** Flag to indicate whether active profile is silent */
    TBool iSilent;
    /** Offline query dialog */
    CAknQueryDialog* iOfflineQueryDialog;
    };

#endif // CAIPROFILEENGINE_H

// End of file
