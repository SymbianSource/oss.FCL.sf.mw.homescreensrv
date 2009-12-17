/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle <-> Phone app synchronizer implementation.
*
*/


#include "aiidleappregisterimpl.h"
#include <e32property.h>
#include <aisystemuids.hrh>
#include <activeidle2domainpskeys.h>

namespace 
	{
    _LIT_SECURITY_POLICY_C1( KTelephonyInformationReadPolicy, ECapabilityReadDeviceData );
    _LIT_SECURITY_POLICY_C1( KTelephonyInformationWritePolicy, ECapabilityWriteDeviceData );
    _LIT_SECURITY_POLICY_PASS( KTelephonyInformationPolicyAlwaysPass );

    const TInt KActOnSendKey = 1;
    
     TInt SetIdleAppPS()
        {
        return RProperty::Set( 
        	KPSUidAiInformation, 
            KActiveIdleUid, 
            AI_UID3_AIFW_EXE );
        }
        
     TInt DefineIdleAppPS()
        {
        return RProperty::Define( 
        	KPSUidAiInformation,
            KActiveIdleUid,
          	RProperty::EInt,
            KTelephonyInformationReadPolicy,
            KTelephonyInformationWritePolicy );
        }
     
     TInt DefineIdleStatePS()
     	{
        RProperty::Define( 
         	KPSUidAiInformation,
         	KActiveIdlePopupState,
         	RProperty::EInt,
         	KTelephonyInformationPolicyAlwaysPass,
         	KTelephonyInformationWritePolicy );
        return RProperty::Define( 
         	KPSUidAiInformation,
         	KActiveIdleState,
         	RProperty::EInt,
         	KTelephonyInformationPolicyAlwaysPass,
         	KTelephonyInformationWritePolicy );
     	}
     	
      TInt DefineIdleSendKeyPS()
        {
        return RProperty::Define( 
        	KPSUidAiInformation,
            KActiveIdleActOnSendKey,
          	RProperty::EInt,
            KTelephonyInformationReadPolicy,
            KTelephonyInformationWritePolicy );
        }
    TInt SetIdleSendKeyPS()
        {
        return RProperty::Set( 
        	KPSUidAiInformation, 
            KActiveIdleActOnSendKey, 
            KActOnSendKey );
        }    
        
      TInt DefineIdleSimRegFailedReceivedPS()
        {
        return RProperty::Define( 
        	KPSUidAiInformation,
            KActiveIdleSimRegFailedReceived,
          	RProperty::EInt,
            KTelephonyInformationReadPolicy,
            KTelephonyInformationWritePolicy );
        }

    TInt DefineIdleAI2RestartPS()
        {
        return RProperty::Define( 
        	KPSUidAiInformation,
            KActiveIdleRestartAI2,
          	RProperty::EInt,
            KTelephonyInformationReadPolicy,
            KTelephonyInformationWritePolicy );
        }
	TInt SetIdleAI2RestartPS()
        {
        return RProperty::Set( 
        	KPSUidAiInformation, 
            KActiveIdleRestartAI2, 
            0 );
        }
        
      TInt DefineIdleLaunchPS()
      	{
      	return RProperty::Define( 
        	KPSUidAiInformation,
        	KActiveIdleLaunch,
        	RProperty::EInt,
        	ECapabilityReadDeviceData,
        	ECapabilityWriteDeviceData );
      	}
      
      TInt DefineIdleSendNumKeysToPhonePS()
          {
          return RProperty::Define( 
              KPSUidAiInformation,
              KActiveIdleForwardNumericKeysToPhone,
              RProperty::EInt,
              ECapabilityReadDeviceData,
              ECapabilityWriteDeviceData );
          
          }
      
      TInt SetIdleSendNumKeysToPhonePS()
          {
          return RProperty::Set(KPSUidAiInformation,
                  KActiveIdleForwardNumericKeysToPhone,
                  EPSAiForwardNumericKeysToPhone);
          }
	}

CAiIdleAppRegisterImpl* CAiIdleAppRegisterImpl::NewLC()
    {
    CAiIdleAppRegisterImpl* self = new(ELeave) CAiIdleAppRegisterImpl;
    CleanupStack::PushL( self );
    return self;
    }

CAiIdleAppRegisterImpl::~CAiIdleAppRegisterImpl()
    {
    }

void CAiIdleAppRegisterImpl::RegisterL()
    {
    // Give own uid for phone.
    TInt setPSResult = SetIdleAppPS();
    if ( setPSResult == KErrNotFound )
        {
        // Key not defined yet -> try to define
        const TInt err = DefineIdleAppPS();
        if( err == KErrNone || err == KErrAlreadyExists )
            {
            // Try setting again
            setPSResult = SetIdleAppPS();
            }
        }
    
    DefineIdleSendKeyPS();
    
    DefineIdleSimRegFailedReceivedPS();
    
    DefineIdleLaunchPS();
    
    // Set the default value to 1 so the send key press is reacted
    SetIdleSendKeyPS();
    
    // Failure to set the telephony P&S key is fatal
    User::LeaveIfError( setPSResult );
    
    // Define idle state key
    DefineIdleStatePS();
    
    DefineIdleAI2RestartPS();
    
    SetIdleAI2RestartPS();
    
    DefineIdleSendNumKeysToPhonePS();
    
    SetIdleSendNumKeysToPhonePS();
    }

CAiIdleAppRegisterImpl::CAiIdleAppRegisterImpl()
    {
    }

EXPORT_C CAiIdleAppRegister* CAiIdleAppRegister::NewLC()
    {
    return CAiIdleAppRegisterImpl::NewLC();
    }
    
