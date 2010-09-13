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
#ifndef TSORIENTATIONMONITOR_H
#define TSORIENTATIONMONITOR_H
#include <e32base.h>
#include <sensrvdatalistener.h>
#include "tsorientationobserver.h"

class CRepository;

class CTsOrientationMonitor: public CActive,
                             public MSensrvDataListener
    {
public:
    static CTsOrientationMonitor* NewL( MTsOrientationObserver& aObserver );
    ~CTsOrientationMonitor();

private:
    CTsOrientationMonitor( MTsOrientationObserver& aObserver );
    void ConstructL();
    void SubscribeL();
    void StartSensorMonitoringL();
    void StopSensorMonitoring();

private://from CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);
    
public:
    void DataReceived( CSensrvChannel& aChannel, 
                       TInt aCount, 
                       TInt aDataLost );
    
    void DataError( CSensrvChannel& aChannel, 
                    TSensrvErrorSeverity aError );
    
    void GetDataListenerInterfaceL( TUid aInterfaceUid, TAny*& aInterface );

private:
    MTsOrientationObserver& iObserver;
    CRepository* iRepository;
    CSensrvChannel* iChannel;

    };
#endif //TSORIENTATIONMONITOR_H
