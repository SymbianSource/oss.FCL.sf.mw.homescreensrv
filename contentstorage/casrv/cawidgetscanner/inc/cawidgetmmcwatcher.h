/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/

#ifndef C_CAWIDGETMMCWATCHER_H
#define C_CAWIDGETMMCWATCHER_H


#include <e32base.h>
#include <f32file.h>

/**
 *  Interface for observing MMC events.
 *
 *  @since S60 S60 v3.1
 */
class MWidgetMmcWatcherCallback
    {
    
public:        
    /**
     * MmcChangeL is called when the MMC is removed or inserted.
     */
    virtual void MmcChangeL() = 0;
    
    };

/**
 *  This active object waits for an indication that the media has changed.
 *
 *  @since S60 S60 v3.1
 */ 
class CCaWidgetMmcWatcher : public CActive
    {
    
public:

    /**
    * Two-phased constructor.
    * @param aObserver Pointer to notifier interface.
    * @param aFs file server session.
    */
    static CCaWidgetMmcWatcher* NewL( RFs& aFs,
            MWidgetMmcWatcherCallback* aObserver );

    /**
    * Two-phased constructor.
    * @param aObserver Pointer to notifier interface.
    * @param aFs file server session.
    */
    static CCaWidgetMmcWatcher* NewLC( RFs& aFs, 
            MWidgetMmcWatcherCallback* aObserver );
    
    /**
    * Destructor
    */
    ~CCaWidgetMmcWatcher();
    
private:
    
    /**
    * C++ default constructor
    * @param aObserver Pointer to notifier interface.
    * @param aFs file server session.
    */
    CCaWidgetMmcWatcher( RFs& aFs, MWidgetMmcWatcherCallback* aObserver );

    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();
    
    /**
    * Set notification request
    */
    void WaitForChangeL();        

    /**
    * From CActive.
    */
    void DoCancel();

    /**
    * From CActive.
    */
    void RunL();
    
    /**
     * From CActive.
     */
    TInt RunError( TInt aError );

private:

    /**
     * File server session. 
     * Not Own
     */
    RFs iFs;
    
    /**
     * MMC events observer
     * Not own.
     */
    MWidgetMmcWatcherCallback* iObserver;    
    };


#endif      // C_CAWIDGETMMCWATCHER_H
