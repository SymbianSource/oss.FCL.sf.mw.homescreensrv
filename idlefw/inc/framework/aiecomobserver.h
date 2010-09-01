/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CAiEcomObserver class implementation.
 *
*/


#ifndef AIECOMOBSERVER_H
#define AIECOMOBSERVER_H

// System includes
#include <e32base.h>	
#include <e32std.h>		
#include <ecom/ecom.h>

// User includes


// Class declaration
/**
 * 
 * Active Idle ECom observer
 *
 * @ingroup group_aifw
 * @lib aifw.lib 
 * @since S60 5.2 
 */
class MAiEcomObserver
    {
public:
    
    /**    
    * This is called when the IdleFw detects a change in ECom
    * plugin registrations (adding, removing or upgrading ECOM-plugins).
    * 
    * @since S60 5.2   
    */
    virtual void NotifyEcomRegistryChanged() = 0;    
    };


// Class declaration
/**
 * 
 * Active Idle ECom observer
 * 
 * @ingroup group_aifw
 * @lib aifw.lib 
 * @since S60 5.2 
 */
NONSHARABLE_CLASS( CAiEcomObserver ) : public CActive
    {
public:
    // constructors and destructor
    
    // Two-phased constructor.
    static CAiEcomObserver* NewL();

    // Two-phased constructor.
    static CAiEcomObserver* NewLC();

    // Cancel and destroy
    ~CAiEcomObserver();

public: 
    // New functions

    /**
     * Adds a new observer for plugin notifications.
     * @param aObserver Observer pointer.
     */
    void AddObserverL( MAiEcomObserver* aObserver );

private:
    // new functions

    // Start observing ecom notification
    void StartObserving();

    // Notifies observers
    void NotifyObservers();

private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CAiEcomObserver();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: 
    // from CActive

    /**
     * @see CActive
     */
    void RunL();

    /**
     * @see CActive
     */
    void DoCancel();
    
private:
    // data

    /** ECom session handle (must be closed, not deleted), Own */
    REComSession iEComSession;

    /** Own: Array of Observers - pointer not owned */
    RPointerArray< MAiEcomObserver > iObservers;
    
private:
    // friend classes
    
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiEcomObserver;
#endif
    };

#endif // AIECOMOBSERVER_H

// End of file
