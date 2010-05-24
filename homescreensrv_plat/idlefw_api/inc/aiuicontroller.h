/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UI controller API
*
*/


#ifndef C_AIUICONTROLLER_H
#define C_AIUICONTROLLER_H

#include <ecom/ecom.h>
#include "aicontentmodel.h"
#include "aifwdefs.h"
#include "aiuicontrolleruid.hrh"
#include "aipluginsettings.h"

class MAiContentObserver;
class MAiFwEventHandler;
class MAiUiFrameworkObserver;
class CCoeEnv;
class CAiContentPublisher;
class MAiMainUiController;
class MAiSecondaryUiController;

/**
 * ECom interface UID for CAiUiContoller
 */
const TUid KInterfaceUidUiController = { AI_UID_ECOM_INTERFACE_UICONTROLLER };

/**
 * UI Controller ECom interface for Active Idle.
 *
 * @since Series 60 3.2
 */
class CAiUiController : public CBase
    {
public:

// Constructors and destructor

    /**
     * Static factory method.
     *
     * @since Series 60 3.2
     * @param ECom implementatoin uid
     * @return class instance
     */
    inline static CAiUiController* NewL(TUid aImpUid);

    /**
     * Static factory method.
     *
     * @since Series 60 3.2
     * @param mimetype
     * @return class instance
     */
    inline static CAiUiController* NewL(const TDesC8& aMime);

    /**
     * Destructor.
     */
    inline virtual ~CAiUiController();

// New functions

    /**
     * Instructs this UI controller to load its UI definition.
     *
     * @since Series 60 3.2
     */
    virtual void LoadUIDefinitionL() = 0;

    /**
     * Retrieves the Content Publisher Plug-in list defined in this UI 
     * controller's UI model.
     *
     * @since Series 60 3.2
     */
    virtual void GetPluginsL(RAiPublisherInfoArray& aPlugins) = 0;

    /**
     * Retrieves plug-in settings specified in this UI controller's UI definition.
     *
     * @param aPubInfo      Publisher info of the plug-in for which to 
     *                      retrieve settings for.
     * @param aSettings array to get settings items to.
     */
    virtual void GetSettingsL(const TAiPublisherInfo& aPubInfo, 
    							RAiSettingsItemArray& aSettings) = 0;

    /**
     * Activate UI managed by this UI controller.
     *
     * @since Series 60 3.2
     */
    virtual void ActivateUI() = 0;

    /**
     * Returns content observer interface implemented by this UI controller.
     *
     * @since Series 60 3.2
     * @return reference to content observer implementation.
     */
    virtual MAiContentObserver& GetContentObserver() = 0;

    /**
     * Sets the Active Idle Framework event handler for this UI Controller.
     *
     * @since Series 60 3.2
     * @param aEventHandler Framework event handler for this UI controller. 
     *                      Set to NULL to disable event callbacks from this
     *                      UI Controller.
     */
    virtual void SetEventHandler(MAiFwEventHandler& aEventHandler) = 0;
    
    /**
     * Remove specified plugin from the UI.
     *
     * @param aPlugin plugin that is removed.
     */     
    virtual void RemovePluginFromUI( MAiPropertyExtension& aPlugin ) = 0;

    /**
     * Returns the main UI Controller interface, or NULL if this is not the 
     * main UI controller.
     */
    virtual MAiMainUiController* MainInterface() = 0;

    /**
     * Returns the secondary UI Controller interface, or NULL if this is not
     * a secondary UI controller.
     */
    virtual MAiSecondaryUiController* SecondaryInterface() = 0;   
    
private:     // Data

    TUid iDestructKey; // An identifier used during destruction

    };

inline CAiUiController* CAiUiController::NewL(TUid aImplUid)
    {
    TAny* ptr( NULL );
    TRAPD( err, ptr = REComSession::CreateImplementationL(aImplUid,
        _FOFF(CAiUiController, iDestructKey)));

    return reinterpret_cast<CAiUiController*> (ptr);
    }

inline CAiUiController* CAiUiController::NewL(const TDesC8& aMime)
    {
    TEComResolverParams params;
    params.SetDataType(aMime);
    TAny* ptr = REComSession::CreateImplementationL(KInterfaceUidUiController,
        _FOFF(CAiUiController, iDestructKey), params);
    return reinterpret_cast<CAiUiController*> (ptr);
    }

inline CAiUiController::~CAiUiController()
    {
    REComSession::DestroyedImplementation(iDestructKey);
    }


/**
 * Main UI Controller interface for Active Idle.
 *
 * @see CAiUiController
 * @since S60 3.2
 */
class MAiMainUiController
    {
public:
    /**
     * Starts application framework and application event loop.
     * This function returns only when the application is shut down.
     *
     * @exception Any of the system-wide error codes in case application
     *            creation or execution fails with an error.
     */
    virtual void RunApplicationL() = 0;
    
    /**
     * Returns the CONE environment object this main UI controller uses.
     */
    virtual CCoeEnv& CoeEnv() = 0;
    
    /**
     * Sets UI framework observer for this main UI controller.
     */
    virtual void SetUiFrameworkObserver( MAiUiFrameworkObserver& aObserver ) = 0; 
    
    /**
    * Exits the main ui controller
    **/
    virtual void Exit() = 0;
    
    /**
     * Only main ui controller might know if menu is open.
     */
    virtual TBool IsMenuOpen() = 0;
    };

/**
 * Secondary UI Controller interface for Active Idle.
 *
 * @see CAiUiController
 * @since S60 3.2
 */
class MAiSecondaryUiController
    {
public:
    /**
     * Sets the CONE environment object for this secondary UI controller to use.
     *
     * @param aCoeEnv the CONE environment object to use.
     */
    virtual void SetCoeEnv( CCoeEnv& aCoeEnv ) = 0;
    
    /**
     * Returns the UI framework observer of this secondary UI controller.
     *
     * @return The UI framework observer, or NULL if observer is not supported.
     */
    virtual MAiUiFrameworkObserver* UiFrameworkObserver() = 0; 
    };

#endif // C_AIUICONTROLLER_H

// End of File.
