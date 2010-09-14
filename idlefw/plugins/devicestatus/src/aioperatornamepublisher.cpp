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
* Description:  Operator/Service provider name publisher.
*
*/


#include <StringLoader.h>
#include <centralrepository.h>
#include <avkondomainpskeys.h>
#include <e32property.h>
#include <aidevicestatuscontentmodel.h>
#include <ProEngFactory.h>
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileName.h>
#include <MProEngNotifyHandler.h>

#include <aidevstaplgres.rsg>
#include "aioperatornamepublisher.h"
#include "ainetworkinfolistener.h"
#include "aiprioritizer.h"
#include "ainwidpriorities.h"
#include "activeidle2domaincrkeys.h"
#include "debug.h"

//Delay used to animate
const TInt KCleanOperationDelay = 2 * 1000000;


const TInt KBitShiftByFour = 4;
const TInt KIsDigitLowLimit = 0;
const TInt KIsDigitHighLimit = 10;
const TInt KOfflineProfileId =  5;

LOCAL_C void AppendDigit( TDes& aCode, TInt aValue )
    {
    // add a digit if valid value.
    if ( aValue >= KIsDigitLowLimit && aValue < KIsDigitHighLimit )
        {
        aCode.AppendNumUC( static_cast<TUint8>( aValue ) );
        }
    }


//Convert string to integer.
LOCAL_C TInt StrToInt( const TDesC& aDesc )
    {
    TLex lex( aDesc );

    TInt ret;
    TInt err = lex.Val( ret );

    if( err != KErrNone )
        {
        ret = KErrNotFound;
        }
    return ret;
    }


// ======== MEMBER FUNCTIONS ========

CAiOperatorNamePublisher::CAiOperatorNamePublisher()
: iPriority( EAiInvalidPriority )
    {
    }


void CAiOperatorNamePublisher::ConstructL()
    {
    iListener = CAiNetworkInfoListener::InstanceL();
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    iProfileEngine = CreateProfileEngineL();
    iProfileNotifier = ProEngFactory::NewNotifyHandlerL();
    iProfileNotifier->RequestProfileActivationNotificationsL( *this );
    }


CAiOperatorNamePublisher* CAiOperatorNamePublisher::NewL()
    {
    CAiOperatorNamePublisher* self = new( ELeave ) CAiOperatorNamePublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiOperatorNamePublisher::~CAiOperatorNamePublisher()
    {
    if( iListener )
        {
	    iListener->RemoveObserver( *this );
        iListener->Release();
        }
    if( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    if ( iProfileNotifier )
        {
        iProfileNotifier->CancelAll();
        delete iProfileNotifier;
        }
    if( iProfileEngine )
        {
        iProfileEngine->Release();
        }
    }


void CAiOperatorNamePublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    }


void CAiOperatorNamePublisher::HandleNetworkInfoChange(
                const MNWMessageObserver::TNWMessages& /*aMessage*/,
                const TNWInfo& /*aInfo*/,
                const TBool aShowOpInd )
    {
    __PRINTS("XAI: CAiOperatorNamePublisher::HandleNetworkInfoChange");    
    if ( iSuspended )
        {
        __PRINTS("XAI: suspended");
        return;
        }
    
    if( aShowOpInd )
        {
        TRAP_IGNORE  ( RefreshL( ETrue ));
        }
    else
        {
        if ( iProfileEngine->ActiveProfileId() != KOfflineProfileId )
            {
            TRAP_IGNORE (
                iPrioritizer->TryToCleanL( *iBroadcaster,
                                        EAiDeviceStatusContentNetworkIdentity,
                                        iPriority ));
            }
        }    
    }


void CAiOperatorNamePublisher::Subscribe( MAiContentObserver& /*aObserver*/,
									                CHsContentPublisher& aExtension,
                                                    MAiPublishPrioritizer& aPrioritizer,
                                                    MAiPublisherBroadcaster& aBroadcaster )
    {
    iExtension = &aExtension;
    iPrioritizer = &aPrioritizer;
    iBroadcaster = &aBroadcaster;
    }

void CAiOperatorNamePublisher::RefreshIfActiveL( TBool aClean )
    {
    if ( !iSuspended )
        {
        RefreshL( aClean );
        }
    }

void CAiOperatorNamePublisher::RefreshL( TBool aClean )
    {
    __PRINTS("XAI: NamePublisher > RefreshL");
    iSuccess = EFalse;

    if ( iSuspended )
        {
        __PRINTS("XAI: NamePublisher > RefreshL - iSuspended");
        return;
        }
    
    if ( iProfileEngine->ActiveProfileId() == KOfflineProfileId )
        {
        MProfile* profile = iProfileEngine->ActiveProfileLC();
        const MProfileName& name = profile->ProfileName();
        iPriority = EAiOfflineProfile;
        iPrioritizer->TryToPublishL( *iBroadcaster,
                                      EAiDeviceStatusContentNetworkIdentity,
                                      name.Name(),
                                      iPriority );
        iSuccess = ETrue;
        CleanupStack::PopAndDestroy();//profile
        return;
        }
    else if ( iPriority == EAiOfflineProfile )
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
                                    EAiDeviceStatusContentNetworkIdentity,
                                    iPriority );
        iPriority = EAiInvalidPriority;
        }
    else if( aClean )
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
                                    EAiDeviceStatusContentNetworkIdentity,
                                    iPriority );
        }
    
    __PRINT(__DBG_FORMAT("XAI: NamePublisher > RefreshL - iPriority = %d"), iPriority );
    
    const TNWInfo& nwInfo = iListener->NetworkInfo();

#if defined(WINSCW) || defined(__WINS__)
    // Show operator name in emulator
#else
    if ( nwInfo.iRegistrationStatus == ENWNotRegisteredNoService ||
    	 	nwInfo.iRegistrationStatus == ENWRegistrationUnknown ||
    	 	!iListener->IsOperatorIndicatorAllowed() )
    	{
    	return;
    	}
#endif

    TBool showSimultaneously = EFalse;

    TUid uid = { KCRUidActiveIdleLV };
    CRepository* cenRep = CRepository::NewLC( uid );
    TInt err = cenRep->Get( KAISPNAndEONS, showSimultaneously );
    if( err )
    	{
    	showSimultaneously = EFalse;
    	}
    CleanupStack::PopAndDestroy( cenRep );
    
    const TBool isKeyLockEnabled = IsKeyLockEnabled();

    //Check if PLMN  must be shown.

    const TBool showPLMN = nwInfo.iServiceProviderNameDisplayReq & ENWDisplayPLMNRequired;
    __PRINT(__DBG_FORMAT("XAI: NamePublisher > RefreshL - showPLMN = %d"), showPLMN );
    
    TBool isInSPDI = EFalse;
    TBool showSPN = EFalse;

    //Check if we are in SPDI and SPN (Service Provider Name) must be shown.
    CheckServiceProviderDisplayListStatus(  StrToInt( nwInfo.iCountryCode ),
                                            StrToInt( nwInfo.iNetworkId ),
                                            showSPN,
                                            isInSPDI );

    //SPN must be shown if it is defined in PLMNField or if it is set in service provider name
    //display requirements.
    showSPN =
        showSPN ||
        ( nwInfo.iServiceProviderNameDisplayReq & ENWDisplaySPNRequired );
    
    __PRINT(__DBG_FORMAT("XAI: NamePublisher > RefreshL - showSPN  = %d"), showSPN );
    
#if defined(WINSCW) || defined(__WINS__)
    _LIT( KOperator, "Operator" );    
    const TPtrC serviceProviderName( KOperator() );
#else
    const TPtrC serviceProviderName = nwInfo.iSPName;
#endif
        
    if ( ( !IsRoaming() || isInSPDI ) && serviceProviderName.Length() )
        {
        if ( showPLMN )
            {
            if ( showSimultaneously )
            	{
            	//spn & plmn (SPN.And.EONS)
            	HBufC* operatorName = ConstructOperatorNameStringL( serviceProviderName );
            	CleanupStack::PushL( operatorName );

            	iPriority = EAiServiceProviderName;
	            iPrioritizer->TryToPublishL( *iBroadcaster,
	                                        	EAiDeviceStatusContentNetworkIdentity,
	                                        	operatorName->Des(),
	                                        	iPriority );
	            CleanupStack::PopAndDestroy(operatorName);
            	}
            else if( isKeyLockEnabled )
            	{
            	//only plmn
        		ShowNetworkIdentityNameL( ETrue );
            	}
            else
            	{
                __PRINTS("XAI: NamePublisher > RefreshL (showPLMN)- try publish and start delayed operation ");
                //spn & plmn (SPN.And.EONS.No)
                if ( !OperatorNamePriority( iPriority ) )
                    {                    
                    iPriority = EAiServiceProviderName;
                    }
	            
			    __PRINT(__DBG_FORMAT("XAI: NamePublisher > RefreshL - try publish with priority %d"), iPriority );
	            iPrioritizer->TryToPublishL( *iBroadcaster,
	                                        	EAiDeviceStatusContentNetworkIdentity,
	                                        	serviceProviderName,
	                                        	iPriority );
	            StartDelayedPLMNOperation();//to publish PLMN name
            	}
            }
        else
            {
            //only spn
            iPriority = EAiServiceProviderName;
            iPrioritizer->TryToPublishL( *iBroadcaster,
                                            EAiDeviceStatusContentNetworkIdentity,
                                            serviceProviderName,
                                            iPriority );
            iSuccess = ETrue;
            }
        }
    else
        {
        if ( showSPN && serviceProviderName.Length() )
            {
            if ( showSimultaneously )
            	{
            	//spn & plmn (SPN.And.EONS)
            	HBufC* operatorName = ConstructOperatorNameStringL( serviceProviderName );
            	CleanupStack::PushL( operatorName );

            	iPriority = EAiServiceProviderName;
	            iPrioritizer->TryToPublishL( *iBroadcaster,
	                                        	EAiDeviceStatusContentNetworkIdentity,
	                                        	operatorName->Des(),
	                                        	iPriority );
	            CleanupStack::PopAndDestroy(operatorName);
            	}
            else if( isKeyLockEnabled )
            	{
            	//only plmn
        		ShowNetworkIdentityNameL( ETrue );
            	}
            else
            	{
                __PRINTS("XAI: NamePublisher > RefreshL (showSPN)- try publish and start delayed operation ");
	            //spn & plmn (SPN.And.EONS.No)
                if ( !OperatorNamePriority( iPriority ) )
                    {                    
                    iPriority = EAiServiceProviderName;
                    }

      		    __PRINT(__DBG_FORMAT("XAI: NamePublisher > RefreshL - try publish with priority %d"), iPriority );
	            iPrioritizer->TryToPublishL( *iBroadcaster,
	                                        	EAiDeviceStatusContentNetworkIdentity,
	                                        	serviceProviderName,
	                                        	iPriority );
	            StartDelayedPLMNOperation();//to publish PLMN name
            	}
            }
		else
        	{
        	//only plmn
        	ShowNetworkIdentityNameL( ETrue );
        	}
        }
    }

HBufC* CAiOperatorNamePublisher::ConstructOperatorNameStringL( const TDesC& aServiceProviderName )
	{
	ShowNetworkIdentityNameL( EFalse );

    CCoeEnv* coeEnv = CCoeEnv::Static();
    HBufC* opnSpnFormatString = StringLoader::LoadLC( R_AI_OPN_SPN_SEPARATOR_FORMAT,
            													coeEnv );
    HBufC* opnSpnStringTemp = HBufC::NewLC( iNetworkIdentityName.Length()
            									+ opnSpnFormatString->Length()
            									+ aServiceProviderName.Length() );

    HBufC* opnSpnString = HBufC::NewLC( iNetworkIdentityName.Length()
            									+ opnSpnFormatString->Length()
            									+ aServiceProviderName.Length() );

    TPtr opnSpnStringTempPtr = opnSpnStringTemp->Des();
    StringLoader::Format( opnSpnStringTempPtr,
    					*opnSpnFormatString,
    					1,
    					aServiceProviderName );

    TPtr opnSpnStringPtr = opnSpnString->Des();
	StringLoader::Format( opnSpnStringPtr,
						  opnSpnStringTempPtr,
						  0,
						  iNetworkIdentityName );

	CleanupStack::Pop( opnSpnString );
	CleanupStack::PopAndDestroy( opnSpnStringTemp ); //opnSpnStringTemp, opnSpnFormatString
	CleanupStack::PopAndDestroy( opnSpnFormatString );

	return opnSpnString;
	}



void CAiOperatorNamePublisher::ShowNetworkIdentityNameL( TBool aTryToPublish )
	{
    __PRINTS("XAI: NamePublisher > ShowNetworkIdentityNameL");
	const TNWInfo& nwInfo = iListener->NetworkInfo();

	iNetworkIdentityName.Set( KNullDesC );

	iPriority = EAiInvalidPriority;

	// *** Network operator name (CPHS-ONS) ***
    if( nwInfo.iNPName.Length() > 0 &&
    		nwInfo.iOperatorNameInfo.iType != RMmCustomAPI::EOperatorNameFlexiblePlmn  &&
    			nwInfo.iRegistrationStatus == ENWRegisteredOnHomeNetwork )
    	{
        __PRINTS("XAI: NamePublisher > ShowNetworkIdentityNameL - iNPName ");
        
    	//priority
    	iPriority = EAiNetworkOperatorName;
    	//name
    	iNetworkIdentityName.Set( nwInfo.iNPName );
    	//publish network identity name
    	if( aTryToPublish )
    		{
			iPrioritizer->TryToPublishL( *iBroadcaster,
                               	   		EAiDeviceStatusContentNetworkIdentity,
                                    	iNetworkIdentityName,
                                    	iPriority );
    		}

        iSuccess = ETrue;
    	return;
    	}

    // *** Operator name ***
    if( nwInfo.iOperatorNameInfo.iName.Length() > 0 )
    	{
        __PRINTS("XAI: NamePublisher > ShowNetworkIdentityNameL - iOperatorNameInfo.iName ");
        
    	//priority
    	OperatorNamePriority( iPriority );
    	//name
    	iNetworkIdentityName.Set( nwInfo.iOperatorNameInfo.iName );
    	//converted name
	    HBufC* convertedOperatorName = NULL;

	    if ( nwInfo.iOperatorNameInfo.iType == RMmCustomAPI::EOperatorNameFlexiblePlmn )
	        {
	        // Long & short name may be in the same buffer.
	        const TInt separatorPos = iNetworkIdentityName.Locate( KFlexibleNameSeparator );
	        if ( separatorPos != KErrNotFound )
	            {
	            convertedOperatorName = nwInfo.iOperatorNameInfo.iName.Left( separatorPos ).AllocLC();
	            }
	        }
	    else if ( nwInfo.iOperatorNameInfo.iType ==
	                  RMmCustomAPI::EOperatorNameMccMnc ||
	              nwInfo.iOperatorNameInfo.iType ==
	                  RMmCustomAPI::EOperatorNameCountryMcn )
	        {
	        // Perform display language specific conversion.
	        convertedOperatorName = nwInfo.iOperatorNameInfo.iName.AllocLC();
	        TPtr ptr = convertedOperatorName->Des();
	        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
	        }
	    else
	        {
	        // No changes needed.
	        }

		if ( convertedOperatorName )
	        {
	        iNetworkIdentityName.Set( *convertedOperatorName );
	        }

		if( iPriority != EAiInvalidPriority )
			{
			//Publish network identity name
			if( aTryToPublish )
    			{
				iPrioritizer->TryToPublishL( *iBroadcaster,
	                               			EAiDeviceStatusContentNetworkIdentity,
	                                		iNetworkIdentityName,
	                                		iPriority );
    			}

	        iSuccess = ETrue;
			}

		if ( convertedOperatorName )
	        {
	        CleanupStack::PopAndDestroy( convertedOperatorName );
	        convertedOperatorName = NULL;
	        }

		if( iPriority != EAiInvalidPriority )
	    	{
	    	return;
	    	}
	    }

	// *** Network info name ***
	if( iPriority == EAiInvalidPriority )
		{
	    //priority
	    iPriority = EAiOperatorNetInfoName;

	    if ( nwInfo.iLongName.Length() > 0 )
			{
		    iNetworkIdentityName.Set( nwInfo.iLongName );
		    }
		else if ( nwInfo.iShortName.Length() > 0 )
		    {
		    iNetworkIdentityName.Set( nwInfo.iShortName );
		    }
		else if ( nwInfo.iDisplayTag.Length() > 0 )
		    {
		    iNetworkIdentityName.Set( nwInfo.iDisplayTag );
		    }
		else
		   	{
		    iPriority = EAiInvalidPriority;
		    }

		//Publish network identity name
		if( aTryToPublish )
    		{
			iPrioritizer->TryToPublishL( *iBroadcaster,
	                               		EAiDeviceStatusContentNetworkIdentity,
	                                	iNetworkIdentityName,
	                                	iPriority );
    		}

	    iSuccess = ETrue;
	    return;
	    }
	}


void CAiOperatorNamePublisher::CheckServiceProviderDisplayListStatus(
    TInt aMCC,
    TInt aMNC,
    TBool& aShowSPN,
    TBool& aIsInSPDI ) const
    {

    // Not in list by default
    aIsInSPDI = EFalse;
    aShowSPN = EFalse;

    if ( !CurrentNetworkOk() )
        {
        return;
        }


    const TNWInfo& nwInfo = iListener->NetworkInfo();

    TInt bufferLength = nwInfo.iPLMNField.Length();
     if ( !bufferLength )
        {
        return;
        }

    const TUint8* field =
        reinterpret_cast< const TUint8* >( nwInfo.iPLMNField.Ptr() );

    TInt octetIndex = 0;

    // Update SPN showing. (in 3GPP a tag).
    aShowSPN = static_cast<TUint8>( field[ octetIndex ] ) ? EFalse : ETrue ;
    octetIndex++;

    // Number of PLMN pairs. (in 3GPP number of octets).
    const TUint8 length = static_cast<TUint8>( ( field[ octetIndex ] ) );
    octetIndex++;

    // Empty list (always with SIM and sometimes with UICC)
    if ( !( length > 0 ) )
        {
        return;
        }

    // Compare MCC&MNC pairs
    TNWCountryCode mcc;
    TNWIdentity    mnc;

    const TUint numberOfPairs = length;      // each pair has 3 octets

    for( TUint pair = 0; pair < numberOfPairs; pair++ )
        {
        // 1st octet
        AppendDigit( mcc, field[ octetIndex ] & 0x0F );
        AppendDigit( mcc, ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour );
        octetIndex++;

        // 2nd octet
        AppendDigit( mcc, field[ octetIndex ] & 0x0F );
        TInt tmp = ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour;
        octetIndex++;

        // 3rd octet
        AppendDigit( mnc, field[ octetIndex ] & 0x0F );
        AppendDigit( mnc, ( field[ octetIndex ] & 0xF0 ) >> KBitShiftByFour );
        octetIndex++;

        AppendDigit( mnc, tmp ); // mnc 3rd digit.

        TInt imcc = StrToInt( mcc );
        TInt imnc = StrToInt( mnc );

        if ( ( aMNC == imnc ) && ( aMCC == imcc ) )
            {
            // match.
            aIsInSPDI = ETrue;
            break;
            }


        // Reset
        mnc.Zero();
        mcc.Zero();
        }
    }


TBool CAiOperatorNamePublisher::CurrentNetworkOk() const
    {
    TBool ok = iListener->MessageReceived(
                            MNWMessageObserver::ENWMessageNetworkInfoChange ) ||
               iListener->MessageReceived(
                            MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );

    ok = ok & ( iListener->NetworkInfo().iStatus == ENWStatusCurrent );

    return ok;
    }


TBool CAiOperatorNamePublisher::IsRoaming() const
    {
    const TNWInfo& nwInfo = iListener->NetworkInfo();

    return iListener->MessageReceived(
                MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange ) &&
           ( nwInfo.iRegistrationStatus == ENWRegisteredRoaming );
    }



void CAiOperatorNamePublisher::StartDelayedPLMNOperation()
    {
    if( !iPeriodic->IsActive() )
        {
        iPeriodic->Start( KCleanOperationDelay,
                          KCleanOperationDelay,
                          TCallBack( CleanAndShowPLMNOperationCallback, this ) );
        }
    }


TInt CAiOperatorNamePublisher::CleanAndShowPLMNOperationCallback( TAny* aPtr )
    {
    __PRINTS("XAI: NamePublisher > CleanAndShowPLMNOperationCallback ");
    CAiOperatorNamePublisher* self =
                    static_cast<CAiOperatorNamePublisher*>( aPtr );

    if( self )
        {
        self->iPeriodic->Cancel();
		// check if got suspended while timer was active
        if ( !self->iSuspended )
            {
            TRAP_IGNORE
                (
                //clean
                self->DoCleanOperationL();
                //show PLMN name
                self->ShowNetworkIdentityNameL( ETrue );
                );
            }
        }

    return KErrNone;
    }


void CAiOperatorNamePublisher::DoCleanOperationL()
    {
    __PRINTS("XAI: NamePublisher > DoCleanOperationL");
    iPrioritizer->TryToCleanL( *iBroadcaster,
                                EAiDeviceStatusContentNetworkIdentity,
                                iPriority );
    }


TBool CAiOperatorNamePublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    __PRINTS("XAI: NamePublisher > RefreshL with content");
    if ( aContentId == EAiDeviceStatusContentNetworkIdentity )
        {
        iSuspended = EFalse;
        
   	    RefreshL( aClean );
   	    
   	    if ( iSuccess )
   	        {
   	        return ETrue;
   	        }
    	}

    return EFalse;
	}

TBool CAiOperatorNamePublisher::SuspendL( TInt aContentId, TBool /*aClean*/ )
    {
    __PRINTS("XAI: NamePublisher > SuspendL with content");
    if ( aContentId == EAiDeviceStatusContentNetworkIdentity )
        {
        iSuspended = ETrue;
        iPeriodic->Cancel();
        return ETrue;
        }

    return EFalse;    
    }

TBool CAiOperatorNamePublisher::RefreshContentWithPriorityL(
                                            TInt aContentId,
                                            TInt aPriority )
	{
    __PRINTS("XAI: NamePublisher > RefreshContentWithPriorityL");
	if( aContentId == EAiDeviceStatusContentNetworkIdentity && aPriority == EAiServiceProviderName )
        {
	    __PRINTS("XAI: NamePublisher > identity and priority matched ");
	    RefreshL( EFalse );
	    if( iSuccess )
   	        {
   	        return ETrue;
   	        }
        }
    return EFalse;
	}


TBool CAiOperatorNamePublisher::OperatorNamePriority( TInt& aPriority )
	{
	TBool succeeded = ETrue;

	const TNWInfo& nwInfo = iListener->NetworkInfo();

    switch ( nwInfo.iOperatorNameInfo.iType )
    	{
        case RMmCustomAPI::EOperatorNameFlexiblePlmn:
        	aPriority = EAiFlexiblePLMN;
            break;

       	case RMmCustomAPI::EOperatorNameNitzFull:
            aPriority = EAiNITZ;
            break;

        case RMmCustomAPI::EOperatorNameNitzShort:
            aPriority = EAiNITZ;
            break;

        case RMmCustomAPI::EOperatorNameProgrammableUcs2:
            aPriority = EAiUnicodeOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameProgrammableLatin:
            aPriority = EAiLatinOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameHardcodedUcs2:
            aPriority = EAiUnicodeOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameHardcodedLatin:
            aPriority = EAiLatinOperatorName;
            break;

        case RMmCustomAPI::EOperatorNameCountryMcn:
            aPriority = EAiCountryMNC;
            break;

        case RMmCustomAPI::EOperatorNameMccMnc:
            aPriority = EAiMCC_MCN;
            break;

        default:
        	aPriority = EAiInvalidPriority;
        	succeeded = EFalse;
            break;
       	}
    return succeeded;
	}

TBool CAiOperatorNamePublisher::IsKeyLockEnabled()
	{
    TInt value;
    TInt err = RProperty::Get(KPSUidAvkonDomain, KAknKeyguardStatus, value);
    if ( err != KErrNone ) 
    	return EFalse;
    switch( value ) 
    	{
        case EKeyguardLocked:
        case EKeyguardAutolockEmulation:
        	return ETrue;
        case EKeyguardNotActive:
        default:
        	return EFalse;
       	}
  	}

void CAiOperatorNamePublisher::HandleProfileActivatedL( TInt /*aProfileId*/ )
    {
    RefreshL( EFalse );
    }

