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
* Description: castoragetask.h
*
*/

#ifndef C_CASTORAGETASK_H
#define C_CASTORAGETASK_H

//includes
#include <e32base.h>

class CCaStorageProxy;
class CCaInnerEntry;
class MCaRunningTaskHandler;

/**
 * Class update entry with window group id of running process
 */
NONSHARABLE_CLASS( CCaStorageTask): public CActive
    {
public:
    /**
     * Function execute task. Ownership is transfered to task handler.
     * Task take ownership over window group name.
     * @param aTaskHandler - running tasks storage
     * @param aStorage - applications storage
     * @param aUid - application UID
     * @param aWgId - application window group id
     */
    static void ExecuteL( MCaRunningTaskHandler& aTaskHandler,
                          CCaStorageProxy& aStorage,
                          TUid aUid, 
                          TInt aWgId );
    
    /**
     * Destructor
     */
    ~CCaStorageTask();
private:
    /**
     * First step constructor
     * @param aTaskHandler - running tasks storage
     * @param aStorage - applications storage
     * @param aUid - application UID
     * @param aWgId - application window group id
     */
    CCaStorageTask( MCaRunningTaskHandler& aTaskHandler, 
                    CCaStorageProxy& aStorage, 
                    TUid aUid, 
                    TInt aWgId );
    
    /**
     * Second step constructor
     */
    void ConstructL();
    
    /**
     * Function unregister task form task handler and release resources allocated for task
     */
    void UnregisterD();
    
    /**
     * Function retrive all entry for application.
     * @param aArray - destination set of application entry
     */
    void GetEntriesL( RPointerArray<CCaInnerEntry>& aArray );
    
    /**
     * Function find and update application entry with window group id. 
     */
    TInt UpdateEntryL();
    
    /**
     * Functioin update application entry with window group id.
     * @param aEntry - application entry
     */
    TInt UpdateEntryL( CCaInnerEntry& aEntry );

private: //from CActive
    /**
     * @see CActive
     */
    void RunL();
    
    
    /**
     * @see CActive
     */
    TInt RunError(TInt aError);
    
    /**
     * @see CActive
     */
    void DoCancel();

private:
    //members
    
    /**
     * Reference to task handler instance 
     */
    MCaRunningTaskHandler& iTaskHandler;
    
    /**
     * Reference to content storage proxy. 
     */
    CCaStorageProxy& iStorage;
    
    /**
     * Application UID
     */
    const TUid iUid;
    
    /**
     * Applicaion window group id
     */
    const TInt iWgId;
    
    /**
     * Timer to re-activate task if try fail
     */
    RTimer iTimer;
    
    /**
     * Task execution counter 
     */
    TInt iCount;
    };

#endif //C_CASTORAGETASK_H

