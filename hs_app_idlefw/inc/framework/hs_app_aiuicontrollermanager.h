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
* Description:  UI controller manager
*
*/


#ifndef C_AIUICONTROLLERMANAGER_H
#define C_AIUICONTROLLERMANAGER_H


#include <e32base.h>
#include "aiuiframeworkobserver.h"
#include "aicontentmodel.h"
#include "aifwdefs.h"

class CAiUiController;
class CAiContentPublisher;
class MAiMainUiController;
class MAiFwEventHandler;
class CRepository;
class CCoeEnv;

/**
 * @ingroup group_aifw
 * 
 *  Active Idle UI Controller manager.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiUiControllerManager ) : public CBase,
                                              public MAiUiFrameworkObserver
	{
	public: // Constructors and destructor		

		static CAiUiControllerManager* NewL();
		~CAiUiControllerManager();
		
	public: // New functions
		
        /**
         * Returns all UI controllers in an array.
         */
		RPointerArray< CAiUiController >& UiControllers() const;
		
		/**
		 * Calls ActivateUI() for all UI controllers.
		 */
		void ActivateUI();
		
        /**
         * Returns the main ui controller object.
         */
		MAiMainUiController& MainUiController() const;
		
        /**
         * Returns true if aUiController is the main UI controller.
         */
		TBool IsMainUiController(CAiUiController& aUiController) const;
		    
        /**
         * Sets framework event handler for all UI controllers.
         */
		void SetEventHandler(MAiFwEventHandler& aEventHandler);
		
        /**
         * Calls RunApplicationL for the main UI controller.
         */
		void RunApplicationL();
		
        /**
         * Calls LoadUIDefinitionL for all UI controllers.
         */
		void LoadUIDefinition();
		
		/**
		 * Returns the main UI Controller's CONE environment object.
		 */
		CCoeEnv& CoeEnv() const;
		
        /**
         * Destroys all UI controllers except the main controller.
         */
		void DestroySecondaryUiControllers();
		
		/**
		 * Adds an UI Framework observer. No duplicates are allowed.
		 */
		void AddObserverL( MAiUiFrameworkObserver& aUiFwObserver );

		/**
		 * Removes an UI Framework observer.
		 */
		void RemoveObserver( MAiUiFrameworkObserver& aUiFwObserver );
		
		/**
		 * Removes plugin from UI.
		 */
		void RemovePluginFromUI( CAiContentPublisher& aPlugin );

        /**
         * Exits main UI controller
         */		
        void ExitMainController();
        
    private:  // From MAiUiFrameworkObserver
    
        void HandleResourceChange( TInt aType );
        void HandleForegroundEvent( TBool aForeground );
                 	
    private: // Constructors

		CAiUiControllerManager();
		void ConstructL();
        
    private: // new functions
        
		void LoadMainControllerL(CRepository& aCenRepConfig);
        void LoadSecondaryControllersL(CRepository& aCenRepConfig);
		
	private: // data
    	/**
		 * UI controller array.
		 * Own.
		 */
		mutable RPointerArray<CAiUiController> iUiControllerArray;
		
    	/**
		 * Main UI controller for app session. Owned in above array.
		 */
		MAiMainUiController*  iMainUiController;
		
		/**
		 * List of UI framework observers to delegate events
		 */
		RPointerArray<MAiUiFrameworkObserver> iUiFrameworkObservers;
		
		/**
		 * List to check for duplicated creations.
		 */
		RArray<TInt> iCreatedUICList;
	};

#endif // C_AIUICONTROLLERMANAGER_H

