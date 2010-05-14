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
* Description:  Task monitor engine
 *
*/


#ifndef TSFSWENGINE_H
#define TSFSWENGINE_H

#define __E32SVR_H__

#include <e32event.h>

#include <e32base.h>
#include <e32cmn.h>
#include <s32strm.h>
#include <fbs.h>
#include <eikenv.h>
#include <w32std.h>
#include <e32hashtab.h>
#include <apgcli.h>
#include <e32property.h>
#include "tsfswentry.h"
#include "tsfswobservers.h"
#include "hsdataprovider.h"
#include "hsdataobserver.h"

class MTsFswEngineObserver;
class CTsFastSwapPreviewProvider;
class CApaWindowGroupName;
class CBitmapRotator;
class CTsRotationTask;
class CTSCpsNotifier;

class CTsFswDataList;

// descriptor big enough to store hex repr of 32-bit integer plus 0x prefix
typedef TBuf<10> TAppUidHexString;

/**
 * Engine for fast swap server.
 * Performs task monitoring, notifications about wg changes are
 * received from the appui.
 */
NONSHARABLE_CLASS( CTsFswEngine ) : public CBase,
        public MTsFswTaskListObserver

    {
    enum TTsFswFgAppType
        {
        /**
         * Means that even when the foreground app has another app embedded
         * into it the uid of the container application will be returned.
         */
        EUseStandaloneUid,
        /**
         * Means that if the foreground app has another app embedded
         * into it then the returned uid will be the uid of the embedded
         * app.
         */
        EUseEmbeddedUid
        };
    
public:
    /**
     * Creates a new instance.
     * @param   aObserver   ref to observer
     */
    static CTsFswEngine* NewL( MHsDataObserver& aObserver );

    /**
     * @copydoc NewL
     */
    static CTsFswEngine* NewLC( MHsDataObserver& aObserver );

    /**
     * Destructor.
     */
    ~CTsFswEngine();

    /**
     * Returns a reference to the current content.
     * Also performs sanity checks, e.g. associates application icons
     * when no screenshot has been received.
     * @return  ref to content array
     */
    const RTsFswArray& FswDataL();




private:

    // from MTsFswTaskListObserver
    void UpdateTaskList();



private:
    /**
     * Constructor.
     */
    CTsFswEngine( MHsDataObserver& aObserver );

    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();

    /**
     * Gets the window group list and reconstructs the fsw content.
     * @return   TBool   ETrue if the list has been modified
     */
    TBool CollectTasksL();


    /**
     * Callback for the iUpdateStarter timer.
     * Calls CollectTasksL and notifies the observer if the task list
     * has really been modified.
     */
    static TInt UpdateStarterCallback( TAny* aParam );

    /**
     * Returns the uid of the foreground app or KNullUid.
     * Will never return hidden apps, only those which can
     * also be seen in the array returned by FswDataL.
     *
     * In case of embedded apps the behaviour is controlled
     * by aType: it will return either the uid of the embedded
     * application or the container app.
     *
     * @param   aType   @see CTsFswClient::TTsFswFgAppType
     */
    TUid ForegroundAppUidL( TInt aType );
    
    /**
     * Gets and publishes the foreground app uid to CFW.
     */
    void PublishFgAppUidL();

private:
    // data    
    MHsDataObserver& iObserver;

    // window server session
    RWsSession iWsSession;

    // apparc session
    RApaLsSession iAppArcSession;

    // timer to defer content refresh
    CPeriodic* iUpdateStarter;


    // window group ids returned by last WindowGroupList call
    RArray<TInt> iWgIds;
  
    TUid iFgAppUid;


    CTsFswDataList* iDataList;//own

    };

#endif
