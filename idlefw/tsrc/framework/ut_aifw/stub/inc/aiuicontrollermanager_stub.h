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
* Description:  UI controller manager stub
*
*/


#ifndef _AIUICONTROLLERMANAGER_STUB_H
#define _AIUICONTROLLERMANAGER_STUB_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CAiUiController;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 *  Active Idle UI Controller manager.
 *
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CAiUiControllerManager ) : public CBase                             
	{
public: 		
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */ 
    static CAiUiControllerManager* NewL();
    
    /**
     * Destructor
     */        
    ~CAiUiControllerManager();
    
public: 
    // new functions
    
    /**
     * Gets UI controllers
     * 
     * @since S60 5.2
     * @return Array of UI controllers
     */
    RPointerArray< CAiUiController >& UiControllers() const;
    
    /**
     * Actives UI by calling ActivateUI() for each UI controller 
     * 
     * @since S60 5.2
     */
    //void ActivateUI();
    
    /**
     * Gets the main UI controller
     * 
     * @since S60 5.2
     * @return Main UI controller
     */
    //MAiMainUiController& MainUiController() const;
    
    /**
     * Queries whether aUiController is the main UI controller
     * 
     * @since S60 5.2
     * @return ETrue if main UI controller, EFalse otherwise
     */
    //TBool IsMainUiController( CAiUiController& aUiController ) const;
            
    /**
     * Calls RunApplicationL for the main UI controller.
     * 
     * @since S60 5.2
     */
    //void RunApplicationL();
    
    /**
     * Calls LoadUIDefinitionL for each UI controller.
     * 
     * @since S60 5.2
     */
    //void LoadUIDefinition();
    
    /**
     * Returns the main UI Controller's CONE environment object.
     * 
     * @since S60 5.2
     * @return Control Environment
     */
    //CCoeEnv& CoeEnv() const;
    
    /**
     * Destroys all UI controllers except the main controller.
     * 
     * @since S60 5.2
     */
    //void DestroySecondaryUiControllers();
        
    /**
     * Exits main UI controller
     * 
     * @since S60 5.2
     */		
    //void ExitMainController();
    
    /** 
     * Sets Fw state handler for each UI conttroller
     * 
     * @since S60 5.2
     * @param aHandler State handler to set
     */
    //void SetStateHandler( MAiFwStateHandler& aHandler );    
                    
private: 
    // private constructors

    /**
     * Leaving constructor
     */    
    void ConstructL();
    
    /**
     * C++ default constructor
     */     
    CAiUiControllerManager();
        
private: 
    // data
    
    /** UI controllers, Owned */
    mutable RPointerArray< CAiUiController > iUiControllerArray;    
	};

#endif // _AIUICONTROLLERMANAGER_STUB_H

