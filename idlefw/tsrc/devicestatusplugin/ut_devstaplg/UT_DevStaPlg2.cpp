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


//  CLASS HEADER
#define private public
#include "aioperatornamepublisher.h"
#undef private
#define protected public
#include "aibtsappublisher.h"
#include "aimulticontentobserver.h"
#include "aicontentobserveroptimizer.h"
#include "aioperatorlogopublisher.h"
#define private public
#include "aiprofilepublisher.h"
#include "aidatepublisher.h"
#undef private
#undef protected
#include "UT_DevStaPlg2.h"
#include <hscontentpublisher.h>
#include "aidevicestatuscontentmodel.h"
#include "caouserafter.h"
#include "logotools.h"
#include "testingtools.h"
#include "contentobserver.h"
#include "ainativeuiplugins.h"
#include <aicontentrequest.h>
#include "ainwidpriorities.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/ceunitalloctestcasedecorator.h>
#include <settingsinternalcrkeys.h>
#include <profile.hrh>
#include <mprofilechangeobserver.h>
#include <e32cmn.h>
#include <gulicon.h>

#include "aidevicestatusplugin.h"

//  INTERNAL INCLUDES


const TInt KCountryCode = 244;
const TInt KNetworkId = 91;




// CONSTRUCTION
UT_DevStaPlg2* UT_DevStaPlg2::NewL()
    {
    UT_DevStaPlg2* self = UT_DevStaPlg2::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_DevStaPlg2* UT_DevStaPlg2::NewLC()
    {
    UT_DevStaPlg2* self = new( ELeave ) UT_DevStaPlg2();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_DevStaPlg2::~UT_DevStaPlg2()
    {
    }

// Default constructor
UT_DevStaPlg2::UT_DevStaPlg2()
    {
    }

// Second phase construct
void UT_DevStaPlg2::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }


void UT_DevStaPlg2::SetupL(  )
    {
    //    __UHEAP_MARK;
    CStubDataHolder::InstanceL();
    iCache = this;

    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN };
    
    _LIT8( KNameSpace, "1" );
    
    THsPublisherInfo pubInfo( uid, KDeviceStatusPluginName, KNameSpace );
    
    iPublisher = CHsContentPublisher::NewL( pubInfo );
    
    RAiSettingsItemArray settings;
    iPublisher->ConfigureL( settings );
    iPublisher->SubscribeL( *iCache );
    iPublisher->Start( CHsContentPublisher::ESystemStartup );    
    /*
//    __UHEAP_MARK;
    CStubDataHolder::InstanceL();
    iCache = this;

    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN };
    iPublisher = CAiContentPublisher::NewL( uid );
    //User::LeaveIfError( err );

    TAny* temp = NULL;
    MAiPropertyExtension* extension = NULL;
    TRAPD( err, temp = iPublisher->Extension( KExtensionUidProperty ) );

    if( temp /*&& err == KErrNone )
        {
        TAiPublisherInfo publisher;
        publisher.iUid = KDeviceStatusPluginUid;
        publisher.iName.Copy( KDeviceStatusPluginName );
        
        extension = static_cast<MAiPropertyExtension*>(temp);
        extension->SetPropertyL( EAiPublisherInfo, (TAny*)&publisher );
        
        const TAiPublisherInfo* result = NULL;
        
        TRAPD( err2, result = extension->PublisherInfoL() );
        
        }
        
    */
        
    /*
    else
        {
        EUNIT_PRINT( _L("Publisher extension not found, cannot set logger") );
        User::Leave( KErrNotFound );
        }
*/
    iPublisher->SubscribeL( *iCache );

    iLogoPublisher = CAiOperatorLogoPublisher::NewL();
        
    iNamePublisher = CAiOperatorNamePublisher::NewL();

    iMultiObserver = CAiMultiContentObserver::NewL();

    iBTSAP = CAiBTSAPPublisher::NewL();

    iDatePublisher = CAiDatePublisher::NewL();
    }

void UT_DevStaPlg2::SetupEmptyL()
    {
//    __UHEAP_MARK;
    }




void UT_DevStaPlg2::Teardown(  )
    {
    delete iDatePublisher;
    
    delete iMultiObserver;

    delete iBTSAP;

    iInternalCache.ResetAndDestroy();

    delete iLogoPublisher;
        
    delete iNamePublisher;

    if( iPublisher )
        {
        iPublisher->Stop( CHsContentPublisher::ESystemShutdown );
        delete iPublisher;
        iPublisher = NULL;        
        }
        
    /*if( iUtils )
        {
        delete iUtils;
        iUtils =NULL;
        }*/
        
    if( iCache )
        {
        iCache->Reset();
        iCache = NULL;        
        }

    CStubDataHolder::Release();
    #if _BullseyeCoverage
        cov_write();
    #endif
    }
    
void UT_DevStaPlg2::TestOperatorLogoPublisherL()
    {
    iLogoPublisher->Subscribe( *this,
                        *iPublisher,
                        *this,
                        *this );    	

    RFbsSession::Connect();

    // StoreOTALogoL( KCountryCode, KNetworkId );
    
    //only OTA logo
    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
  
	nwInfo->iStatus = ENWStatusCurrent;
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );
    
    /*
    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    TPckgBuf<CGulIcon*> logoBuf;
    logoBuf.Copy( *cache->iData );

    EUNIT_ASSERT( logoBuf()->Bitmap() );
    EUNIT_ASSERT( logoBuf()->Mask() );
    */
        
	nwInfo->iRegistrationStatus = ENWNotRegisteredNoService;
	nwInfo->iStatus = ENWStatusCurrent;
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

	//TODO Assert

	nwInfo->iRegistrationStatus = ENWRegistrationUnknown ;
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    
	//TODO Assert

    RFbsSession::Disconnect();
    }    
    
void UT_DevStaPlg2::TestOperatorProgNamePublisherL()
    {
#if defined(WINSCW) || defined(__WINS__)    
    _LIT( KOperatorName, "Operator" );
#else
    _LIT( KOperatorName, "ProgName" );
#endif
    
    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iOperatorNameInfo.iName.Copy( KOperatorName );
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplayPLMNRequired;
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplaySPNRequired;
    nwInfo->iStatus = ENWStatusCurrent;

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    
    /*
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }
    */

    // EUNIT_ASSERT( *cache->iText == KOperatorName );
        
    CAOUserAfter::AfterLD( 100000 );

    nwInfo->iNPName.Copy( _L("NpName") );
    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    CAOUserAfter::AfterLD( 100000 );
    
    nwInfo->iNPName.Copy( _L("") );
    nwInfo->iSPName.Copy( _L("OpName") );

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    CAOUserAfter::AfterLD( 100000 );

    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    
    CAOUserAfter::AfterLD( 100000 );

    nwInfo->iServiceProviderNameDisplayReq &= !ENWDisplayPLMNRequired;
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    CAOUserAfter::AfterLD( 100000 );
    
    nwInfo->iOperatorNameInfo.iName.Copy( _L("") );

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameUpdating );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );


	//wait for publish
	CAOUserAfter::AfterLD( 2 * 1000000 );
	
    TInt prio;
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzShort;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameProgrammableUcs2;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameProgrammableLatin;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedUcs2;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedLatin;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameCountryMcn;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameMccMnc;
    EUNIT_ASSERT( iNamePublisher->OperatorNamePriority( prio ) );

    nwInfo->iOperatorNameInfo.iType = (RMmCustomAPI::TOperatorNameType)0xdeafdead;
    EUNIT_ASSERT( !iNamePublisher->OperatorNamePriority( prio ) );      
    }
    
void UT_DevStaPlg2::TestMultiObserverL()
    {
    	/*
    RFs fss;
    fss.Connect();
    RFile file;
    iMultiObserver->AddObserverL( *this );
    iMultiObserver->AddObserverL( *this );
    iMultiObserver->Publish( *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )), 0,
                             file, 0 );
    iMultiObserver->Publish( *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )), 0,
                             _L8("dkorks"), 0 );
    iMultiObserver->Extension( TUid::Uid( 0 ) );
    iMultiObserver->CanPublish( *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )),
                                0, 0 );
    fss.Close();
    EUNIT_ASSERT( iBufPublished )
    EUNIT_ASSERT( iFilePublished )
    */
    }

void UT_DevStaPlg2::TestBTSAPPublisherL()
    {
    	/*
    iBTSAP->Subscribe( *this,
                       *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )),
                       *this,
                       *this );
    iBTSAP->RefreshL( 0, ETrue );
    iBTSAP->RefreshL( EAiDeviceStatusContentNetworkIdentity, EFalse );
    iBTSAP->RefreshL( EAiDeviceStatusContentNetworkIdentity, ETrue );
    iBTSAP->RefreshL( 0, EFalse );
    iBTSAP->RefreshL( ETrue );
    iBTSAP->RefreshL( EFalse );
    iBTSAP->RefreshContentWithPriorityL( EAiDeviceStatusContentNetworkIdentity,
                                         EAiBTSAP );
    iBTSAP->RefreshContentWithPriorityL( 0,
                                         EAiBTSAP );
    iBTSAP->RefreshContentWithPriorityL( EAiDeviceStatusContentNetworkIdentity,
                                         0 );
                                         */
    }

/*
void UT_DevStaPlg2::TestProfilePublisherL()
    {

    iProfilePublisher = CAiProfilePublisher::NewL();

    CStubDataHolder::Instance()->SetCurrentProfileL(_L("General"),EProfileGeneralId);
    iProfilePublisher->Subscribe( *this,
                                  *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )),
                                  *this,
                                  *this );
    iProfilePublisher->RefreshL( ETrue );
    iProfilePublisher->RefreshL( EFalse );
    
    iProfilePublisher->RefreshProfileL( ETrue );
    iProfilePublisher->RefreshProfileL( EFalse );
    
    iProfilePublisher->RefreshL( EAiDeviceStatusContentProfileName, ETrue );
    iProfilePublisher->RefreshL( EAiDeviceStatusContentGeneralProfileName, EFalse );
    iProfilePublisher->RefreshL( EAiDeviceStatusContentNetworkIdentity, ETrue );
    
    iProfilePublisher->RefreshContentWithPriorityL( EAiDeviceStatusContentNetworkIdentity,
                                         EAiOfflineProfile );
    iProfilePublisher->RefreshContentWithPriorityL( 0,
                                         EAiOfflineProfile );
    iProfilePublisher->RefreshContentWithPriorityL( EAiDeviceStatusContentNetworkIdentity,
                                                    0 );
    delete iProfilePublisher;

    }
    */    
    
void UT_DevStaPlg2::TestDatePublisherL()
    {
    	/*
    iDatePublisher->Subscribe( *this,
                                  *static_cast<CHsContentPublisher*>(iPublisher->Extension( KExtensionUidProperty )),
                                  *this,
                                  *this );
    HBufC* string;
    string = iDatePublisher->GetDayNameStringLC( EMonday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( ETuesday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( EWednesday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( EThursday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( EFriday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( ESaturday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    string = iDatePublisher->GetDayNameStringLC( ESunday, *CCoeEnv::Static() );
    EUNIT_ASSERT( string )
    CleanupStack::PopAndDestroy( string );
    */
    }

void UT_DevStaPlg2::TestPluginL()
    {
    iLogoPublisher->Subscribe( *this,
                        *iPublisher,
                        *this,
                        *this );
    iDatePublisher->Subscribe( *this,
                                  *iPublisher,
                                  *this,
                                  *this );
    iNamePublisher->Subscribe( *this,
                        *iPublisher,
                        *this,
                        *this );
    
    iPublisher->SubscribeL( *this );
    iPublisher->Resume( CHsContentPublisher::EForeground );

    EUNIT_ASSERT( !iPublisher->Extension( TUid::Uid(0xdeafd0de) ) );
    // EUNIT_ASSERT( iPublisher->Extension( KExtensionUidProperty ) );
    
    EUNIT_ASSERT( iPublisher->GetProperty( CHsContentPublisher::EPublisherResources ) );
    
    iPublisher->SetProperty( CHsContentPublisher::EPublisherResources, NULL );
    iLogoPublisher->RefreshL( 0xdee, ETrue );
    iLogoPublisher->RefreshL( 0xdee, EFalse );
    iLogoPublisher->RefreshL( EAiDeviceStatusContentNetworkIdentity, EFalse );
    iLogoPublisher->RefreshL( EAiDeviceStatusContentNetworkIdentity, ETrue );
    iLogoPublisher->HandleNotifyInt( 0xd00, 6 );
    iLogoPublisher->HandleNotifyInt( 0, 6 );
    iLogoPublisher->HandleNotifyInt( KSettingsDisplayOperatorLogo, 6 );

    _LIT( KOperatorName, "ProgName" );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iOperatorNameInfo.iName.Copy( KOperatorName );
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplayPLMNRequired;
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplaySPNRequired;
    nwInfo->iStatus = ENWStatusCurrent;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    
    nwInfo->iNPName.Copy(_L("NPN"));
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    
    nwInfo->iNPName.Copy(_L(""));
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameFlexiblePlmn;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameMccMnc;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameCountryMcn;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameProgrammableUcs2;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameProgrammableLatin;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedUcs2;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );

    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameHardcodedLatin;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameMccMnc;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    nwInfo->iOperatorNameInfo.iType = (RMmCustomAPI::TOperatorNameType)0xdeafdead;
    iNamePublisher->ShowNetworkIdentityNameL( ETrue );
    }
        
#include "contentobserver.h"


void UT_DevStaPlg2::Reset()
    {
    iInternalCache.ResetAndDestroy();
    }


CContentCache* UT_DevStaPlg2::GetContent( TInt aContentId, TBool aClean )
    {
    const TInt count( iInternalCache.Count() );
    for( TInt i( 0 ); i < count; i++ )
        {
        if( iInternalCache[i]->iId == aContentId )
            {
            if( !aClean && iInternalCache[i]->iClean)
                {
                continue;
                }
            return iInternalCache[i];
            }
        }

    return NULL;
    }


CContentCache* UT_DevStaPlg2::GetLastContent( TInt aContentId, TBool aClean )
    {
    const TInt count( iInternalCache.Count() );
    for( TInt i( count - 1 ); i >= 0; i-- )
        {
        if( iInternalCache[i]->iId != aContentId )
            {
            return NULL;
            }
        if( !aClean && iInternalCache[i]->iClean)
            {
            continue;
            }
        return iInternalCache[i];
        }
    return NULL;
    }


TInt UT_DevStaPlg2::StartTransaction(TInt /*aTxId*/)
    {
    return 0;
    }


TInt UT_DevStaPlg2::Commit(TInt /*aTxId*/)
    {
    return 0;
    }


TInt UT_DevStaPlg2::CancelTransaction(TInt /*aTxId*/)
    {
    return 0;
    }
    

TBool UT_DevStaPlg2::CanPublish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/ )
    {
    return EFalse;
    }

    void UT_DevStaPlg2::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                TInt aContent, 
                                TInt aResource, 
                                TInt aPriority )
        {
        Publish( *((CHsContentPublisher*)(0x0)),
                 aContent, 
                 aResource, 
                 aPriority );
        }
    void UT_DevStaPlg2::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                TInt aContent, 
                                const TDesC16& aText, 
                                TInt aPriority )
        {
        Publish( *((CHsContentPublisher*)(0x0)),
                 aContent, 
                 aText, 
                 aPriority );
        }
    void UT_DevStaPlg2::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                TInt aContent, 
                                const TDesC8& aBuf, 
                                TInt aPriority )
        {
        Publish( *((CHsContentPublisher*)(0x0)),
                 aContent, 
                 aBuf, 
                 aPriority );
        }
    void UT_DevStaPlg2::TryToCleanL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                TInt aContent, 
                                TInt aPriority )
        {
        Clean( *((CHsContentPublisher*)(0x0)),
                 aContent, 
                 aPriority );
        }

TInt UT_DevStaPlg2::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, TInt aResource, TInt aIndex )
    {
    RDebug::Print( _L("Publish(%d, %d, %d)"), aContent, aResource, aIndex );
    CContentCache* cache = new( ELeave )CContentCache;
    CleanupStack::PushL( cache );
    cache->iId = aContent;
    cache->iResource = aResource;
    cache->iIndex = aIndex;
    User::LeaveIfError( iInternalCache.Append( cache ) );
    CleanupStack::Pop( cache );
    return 0;
    }

TInt UT_DevStaPlg2::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, const TDesC16& aText, TInt aIndex )
    {
    RDebug::Print( _L("Publish(%d, \"%S\", %d)"), aContent, &aText, aIndex );
    CContentCache* cache = new( ELeave )CContentCache;
    CleanupStack::PushL( cache );
    cache->iId = aContent;
    cache->iText = aText.AllocL();
    cache->iIndex = aIndex;
    User::LeaveIfError( iInternalCache.Append( cache ) );
    CleanupStack::Pop( cache );
    return 0;
    }


TInt UT_DevStaPlg2::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, const TDesC8& aBuf, TInt aIndex )
    {
    iBufPublished = ETrue;
    RDebug::Print( _L("Publish(%d, Buf.Len=%d, %d)"), aContent, aBuf.Length(), aIndex );
    CContentCache* cache = new( ELeave )CContentCache;
    CleanupStack::PushL( cache );
    cache->iId = aContent;
    cache->iData = aBuf.AllocL();
    cache->iIndex = aIndex;
    User::LeaveIfError( iInternalCache.Append( cache ) );
    CleanupStack::Pop( cache );
    return 0;
    }


TInt UT_DevStaPlg2::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, RFile& /*aFile*/, TInt aIndex )
    {
    iFilePublished = ETrue;
    CContentCache* cache = new( ELeave )CContentCache;
    CleanupStack::PushL( cache );
    cache->iId = aContent;
    cache->iIndex = aIndex;
    User::LeaveIfError( iInternalCache.Append( cache ) );
    CleanupStack::Pop( cache );
    return 0;
    }


TInt UT_DevStaPlg2::Clean(CHsContentPublisher& /*aPlugin*/, TInt aContent, TInt aIndex)
    {
    RDebug::Print( _L("Clean(%d, %d)"), aContent, aIndex );
    CContentCache* cache = new( ELeave )CContentCache;
    CleanupStack::PushL( cache );
    cache->iId = aContent;
    cache->iIndex = aIndex;
    cache->iClean = ETrue;
    User::LeaveIfError( iInternalCache.Append( cache ) );
    CleanupStack::Pop( cache );
    return 0;
    }
    
// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt UT_DevStaPlg2::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/ )
{
	return KErrNone;
}				

// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt UT_DevStaPlg2::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/,
									          MAiContentObserver::TValueType /*aValueType*/ )
{
	return KErrNone;
}				    


TAny* UT_DevStaPlg2::Extension(TUid /*aUid*/)
    {
    return NULL;
    }

TBool UT_DevStaPlg2::RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const
    {
    return EFalse;
    }


//  END OF FILE

         
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_DevStaPlg2,
    "Device Status Plugin test cases",
    "MODULE" )

EUNIT_TEST(
    "Test op logo publish",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestOperatorLogoPublisherL, Teardown)

EUNIT_TEST(
    "Test op name publish",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestOperatorProgNamePublisherL, Teardown)

EUNIT_TEST(
    "Test multiobserver",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestMultiObserverL, Teardown)

EUNIT_TEST(
    "Test BTSAP",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestBTSAPPublisherL, Teardown)
  
EUNIT_TEST(
    "Test date publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestDatePublisherL, Teardown)

EUNIT_TEST(
    "Test date plugin",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestPluginL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
