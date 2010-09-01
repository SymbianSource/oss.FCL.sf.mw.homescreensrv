/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TESTINGTOOLS_H
#define TESTINGTOOLS_H

#include <btsapinternalpskeys.h>
//#include <telephonyinternalpskeys.h>
#include <activeidle2domainpskeys.h>
#include <e32property.h>
#include "stubdataholder.h"

LOCAL_C void ChangeBTSAPValueL( TInt aValue )
    {
    RProperty::Define( KPSUidBluetoothSapConnectionState,
                                KBTSapConnectionState,
                                RProperty::EInt );
                                
    User::LeaveIfError( RProperty::Set( KPSUidBluetoothSapConnectionState,
                    KBTSapConnectionState,
                    aValue ) );
    
    }
    
LOCAL_C void DeleteBTSAPKey()
    {
    RProperty::Delete( KPSUidBluetoothSapConnectionState,
                                KBTSapConnectionState );
    }
    
    
LOCAL_C void EmulateSimRegFailEvent()
    {
    
        // Read capability: ReadUserData.
    _LIT_SECURITY_POLICY_C1( KReadUserPolicy, ECapabilityReadUserData ); 
    // Write capability: WriteDeviceData.
    _LIT_SECURITY_POLICY_C1( KWriteDevicePolicy, ECapabilityWriteDeviceData );

    RProperty::Define( 
         	KPSUidAiInformation,
         	KActiveIdleState,
         	RProperty::EInt,
            KReadUserPolicy,
            KWriteDevicePolicy );

    
    User::LeaveIfError( RProperty::Set( KPSUidAiInformation,
                    KActiveIdleState,
                    EPSAiForeground ) );    
    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iRegistrationStatus = ENWRegistrationDenied;

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkConnectionFailure );
    }


LOCAL_C void EmulateHCZEvent( const TDesC& aZoneName )
    {
    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iViagIndicatorType = ENWViagIndicatorTypeHomeZone;
    nwInfo->iViagTextTag.Copy( aZoneName );

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageCurrentHomeZoneMessage );
    }

LOCAL_C void EmulateMCNEvent( const TDesC& aName )
    {
    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iMCNIndicatorType = ENWMCNIndicatorTypeActive;
    nwInfo->iMCNName.Copy( aName );

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageCurrentCellInfoMessage );
    }

LOCAL_C void EmulateCUGEvent( TInt aIndex )
    {
    CStubDataHolder::Instance()->RemoveSSSetting( ESSSettingsCug );    
    CStubDataHolder::Instance()->SetSSSettingsValueL( ESSSettingsCug, aIndex );        
    RArray<MSSSettingsObserver*> observers = CStubDataHolder::Instance()->SSSettingsObservers();
    
    for( TInt i( 0 ); i < observers.Count(); i++ )
        {
        observers[i]->PhoneSettingChanged( ESSSettingsCug, aIndex );        
        }    
    }


LOCAL_C void EncodePLMNField( TDes& aBuf, TBool aShowSPN, TInt aMCC, TInt aMNC )
    {
    TPtr8 ptr( (TUint8*)aBuf.Ptr(), aBuf.MaxSize() );
    ptr.Append( TChar( aShowSPN ) );    
    
    //one PLMN pair
    ptr.Append( TChar( 1 ) );    
    
    TBuf8<3> mcc;
    TBuf8<3> mnc;
    
//    mcc.Format( _L8("%d"), aMCC );
  //  mnc.Format( _L8("%d"), aMNC );
    mcc.AppendNumFixedWidthUC( aMCC, EDecimal, 3 );//"241"
    mnc.AppendNumFixedWidthUC( aMNC, EDecimal, 3 );//"091"
    
    ptr.Append( TChar( ( mcc[0] - 48 ) | ( ( mcc[1] - 48 ) << 4 ) ) );    
    ptr.Append( TChar( ( mcc[2] - 48 ) | ( ( mnc[2] - 48 ) << 4 ) ) );    
    ptr.Append( TChar( ( mnc[0] - 48 ) | ( ( mnc[1] - 48 ) << 4 ) ) );    
    
    aBuf.SetLength( 3 );
        
    }


#endif      //  TESTINGTOOLS_H

