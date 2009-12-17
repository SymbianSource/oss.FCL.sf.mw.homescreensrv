/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Backup and restore events listener.
 *
*/


#ifndef C_CPSERVERBURLISTENER_H
#define C_CPSERVERBURLISTENER_H

#include <e32base.h>
#include <e32property.h>

/**
 *  Interface for observing Backup and Restore events.
 *
 *  @since S60 S60 v3.1
 */
class MBURListenerCallback
    {

public:
    /**
     * Enum defining the status of Backup and Restore.
     */
    enum TBURStatus
        {
        EBURStatusNone,
        EBURStatusBackup,
        EBURStatusRestore
        };

    /**
     * HandleBUREventL is called when Backup and Restore state has been changed.
     * @param aStatus Current Backup and Restore status.
     */
    virtual void HandleBUREventL( TBURStatus aStatus ) = 0;

    };

/**
 *  Central Repository observer for Backup and Restore.
 *
 *  @since S60 S60 v3.1
 */
NONSHARABLE_CLASS(CCPServerBURListener) : public CActive
    {

public:

    /**
     * Creates an instance of CCPServerBURListener implementation.
     * @param aCallBack Pointer to observer interface.
     */
    static CCPServerBURListener* NewL( MBURListenerCallback* aCallback );

    /**
     * Destructor.
     */
    virtual ~CCPServerBURListener();

    /**
     * @return EFalse if the device as a whole is not engaged in a backup or restore .
     */
    TBool CheckBUR();

private:
    /**
     * Constructor.
     * @param aCallBack Pointer to observer interface.
     */
    CCPServerBURListener( MBURListenerCallback* aCallback );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

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
     * User side interface to Publish & Subscribe.
     */
    RProperty iProperty;

    /**
     * Interface for notifying changes in Backup and Restore.
     * Not own.
     */
    MBURListenerCallback* iCallback;

    };

#endif // C_CPSERVERBURLISTENER_H
// End of File
