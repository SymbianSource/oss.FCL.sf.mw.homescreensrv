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

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <ConeResLoader.h>
#include <MSSSettingsObserver.h>
#include <MProfileChangeObserver.h>
#include "hs_app_maiprofilepluginnotifier.h"

class MProfileEngine;
class CProfileChangeNotifyHandler;
class MProfilesNamesArray;
class RSSSettings;

/**
 *  @ingroup group_profileplugin
 *
 *  CAiProfileEngine
 *
 *  @since S60 v3.2
 */
class CAiProfileEngine : public CBase, 
						 public MSSSettingsObserver,
						 public MProfileChangeObserver					 
{

public:  // Constructors and destructor

    /**
    * Constructor to use in the object creation. Initializes the necessary data.
    *
    * @param MAiProfilePluginNotifier* aNotifier a pointer to a object implementing MAiProfilePluginNotifier
            (the object ordering a notification of the content status change)
    * @return none
    */
    CAiProfileEngine( MAiProfilePluginNotifier* aProfilePluginNotifier );

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
    
    /**
    * Resumes the engine
    *
    * @param void
    * @return void
    */
    void ResumeL();

    /**
    * Suspends the engine
    *
    * @param void
    * @return void
    */
    void Suspend();

public:

	const TDesC& ActiveProfileName() const;
	
	const TDesC& SwapProfileName() const;
		
	TInt NumberOfProfiles();
	
	const TDesC& ProfileNameByIndex( TInt aIndex ) const;
	
	void HandleAiEventL( TInt aEvent, const TDesC& aParam );
	
	void SetActiveProfileL( const TInt aProfileId );
	
	TBool IsActiveProfileSilentL();
	
	TBool IsActiveProfileTimedL();
	
	void UpdateProfileNamesL();
	
	TBool IsOffline();
   
private:

    void ConstructL();
    
    void SetActiveProfileNameL( const TDesC& aName );
    
    void SetSwapProfileNameL( const TDesC& aName );
    
    void SetProfileNameListL( const MProfilesNamesArray& aArray );
    
    void HandleSwitchByNameL( const TDesC& aParam );
    
    void HandleSwitchByIndexL( const TDesC& aParam );
    
    void HandleSwapL( const TDesC& aParam );
    
    void HandleEditActiveProfileL();
    
    TBool ShowOfflineMessageL();
           
  
protected:

// from base class MSSSettingsObserver

    /**
     * Called when SS Settings changes.
     */
    void PhoneSettingChanged( TSSSettingsSetting aSetting, TInt aNewValue );


// from base class MProfileChangeObserver

    /**
     * Called when active profile changes.
     */
    void HandleActiveProfileEventL( TProfileEvent aProfileEvent, TInt aProfileId );
    
private:
    /**
     * Pointer to the class implementing the MAiProfilePluginNotifier interface
     */
    MAiProfilePluginNotifier* iProfilePluginNotifier;
    
    /**
     * Client to listen SS setting changes.
     */
    RSSSettings iSSSettings;

    /**
     * Profile engine.
     * Own.
     */
    MProfileEngine* iProfileEngine;

    /**
     * Profile change notifier.
     * Own
     */
    CProfileChangeNotifyHandler* iProfileNotifier;
    
    /*
     * Active profile name
     * Own
     */    
    HBufC* iActiveProfileName;
    
    /*
     * Swap profile name
     * Own
     */    
    HBufC* iSwapProfileName;
    
    /**
     * Array of Profile Name Pointer.
     * Own.
     */
    RPointerArray<HBufC> iProfileNamePointerArray;
    
    /**
     * Resource loader for own resources.
     */
    RConeResourceLoader iResourceLoader;

};

#endif // CAIPROFILEENGINE_H


