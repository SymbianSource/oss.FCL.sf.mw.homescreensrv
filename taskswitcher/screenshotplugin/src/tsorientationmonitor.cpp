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
#include <centralrepository.h>
#include <sensrvchannelfinder.h>
#include <sensrvchannel.h>
#include <sensrvorientationsensor.h>

#include "tsorientationmonitor.h"
const TUid KSensorCenrepUid  = {0x2002C384};
const TUint32 KSensorCenrepKey  = 0x1;
//------------------------------------------------------------------------------
CTsOrientationMonitor* CTsOrientationMonitor::NewL( MTsOrientationObserver& aObserver )
    {
    CTsOrientationMonitor* self = new (ELeave) CTsOrientationMonitor(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------------
CTsOrientationMonitor::CTsOrientationMonitor( MTsOrientationObserver& aObserver )
:
    CActive(EPriorityStandard),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::ConstructL()
    {
    iRepository = CRepository::NewL( KSensorCenrepUid );
    SetActive();
    TRequestStatus* status( &iStatus );
    User::RequestComplete( status, KErrNone );
    }

//------------------------------------------------------------------------------
CTsOrientationMonitor::~CTsOrientationMonitor()
    {
    Cancel();
    delete iChannel;
    delete iRepository;
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::SubscribeL()
    {
    User::LeaveIfError( iRepository->NotifyRequest(KSensorCenrepKey, iStatus) );
    SetActive();
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::StartSensorMonitoringL()
    {
    CSensrvChannelFinder *sensrvChannelFinder = CSensrvChannelFinder::NewLC();

    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL(channelInfoList);

    TSensrvChannelInfo mySearchConditions;

    //Search only Orientation events.
    mySearchConditions.iChannelType = KSensrvChannelTypeIdOrientationData;

    sensrvChannelFinder->FindChannelsL(channelInfoList, mySearchConditions);
    
    if( channelInfoList.Count() )
        {
        iChannel = CSensrvChannel::NewL(channelInfoList[0]);
        iChannel->OpenChannelL();
        iChannel->StartDataListeningL(this, 1, 1, 0);
        }

    CleanupStack::Pop(&channelInfoList);
    CleanupStack::PopAndDestroy(sensrvChannelFinder);
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::StopSensorMonitoring()
    {
    if(0 != iChannel)
        {
        iChannel->StopDataListening();
        }
    delete iChannel;
    iChannel = 0;
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::DoCancel()
    {
    iRepository->NotifyCancel(KSensorCenrepKey);
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    TInt sensorStatus;
    User::LeaveIfError(iRepository->Get(KSensorCenrepKey, sensorStatus));
    (0 != sensorStatus ) ? StartSensorMonitoringL() : StopSensorMonitoring();
    SubscribeL();
    }

//------------------------------------------------------------------------------
TInt CTsOrientationMonitor::RunError(TInt aError)
    {
    if( KErrCancel != aError )
        {
        TRAP_IGNORE(SubscribeL());
        }
    return KErrNone;
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::DataReceived( CSensrvChannel& aChannel, 
                                          TInt aCount, 
                                          TInt /*aDataLost*/ )
    {
    if (aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdOrientationData)
        {
        TPckgBuf<TSensrvOrientationData> dataBuf;
        for(TInt i = 0; i < aCount; ++i) 
            {
            aChannel.GetData(dataBuf);
            iObserver.OrientationChanged( TSensrvOrientationData::EOrientationDisplayRightUp == dataBuf().iDeviceOrientation ? 270 : 0);
            }
        }
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::DataError( CSensrvChannel& /*aChannel*/, 
                                       TSensrvErrorSeverity /*aError*/ )
    {
    //No implementation required
    }

//------------------------------------------------------------------------------
void CTsOrientationMonitor::GetDataListenerInterfaceL( TUid /*aInterfaceUid*/, 
                                                       TAny*& /*aInterface*/ )
    {
    //No implementation required
    }
