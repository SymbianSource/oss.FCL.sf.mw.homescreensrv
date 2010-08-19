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
#include "MT_DevStaPlg.h"
#include "hscontentpublisher.h"
#include "aidevicestatuscontentmodel.h"
#include "caouserafter.h"
#include "logotools.h"
#include "testingtools.h"
#include "contentobserver.h"
#include "ainativeuiplugins.h"
#include "aidevicestatusplugin.h"
#include <aicontentrequest.h>
#include <aidevstaplgres.rsg>

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/ceunitalloctestcasedecorator.h>
#include <profile.hrh>
#include <mprofilechangeobserver.h>
#include <e32cmn.h>
#include <gulicon.h>

//  INTERNAL INCLUDES

const TInt KCountryCode = 244;
const TInt KNetworkId = 91;

// CONSTRUCTION
MT_DevStaPlg* MT_DevStaPlg::NewL()
    {
    MT_DevStaPlg* self = MT_DevStaPlg::NewLC();
    CleanupStack::Pop();

    return self;
    }

MT_DevStaPlg* MT_DevStaPlg::NewLC()
    {
    MT_DevStaPlg* self = new( ELeave ) MT_DevStaPlg();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
MT_DevStaPlg::~MT_DevStaPlg()
    {
	//iLibrary.Close();
    }

// Default constructor
MT_DevStaPlg::MT_DevStaPlg()
    {
    }

// Second phase construct
void MT_DevStaPlg::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    //User::LeaveIfError( iLibrary.Load( _L("AiDevStaPlg.dll") ) );

    //create test cases;

    }


void MT_DevStaPlg::SetupL(  )
    {
//    __UHEAP_MARK;
    CStubDataHolder::InstanceL();
    iCache = &CContentObserver::InstanceL();

    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN };
    
    _LIT8( KNameSpace, "1" );
    
    THsPublisherInfo pubInfo( uid, KDeviceStatusPluginName, KNameSpace );
    
    iPublisher = CHsContentPublisher::NewL( pubInfo );
    
    RAiSettingsItemArray settings;
    iPublisher->ConfigureL( settings );
    iPublisher->SubscribeL( *iCache );
    iPublisher->Start( CHsContentPublisher::ESystemStartup );
    }

void MT_DevStaPlg::SetupEmptyL()
    {
//    __UHEAP_MARK;
    }




void MT_DevStaPlg::Teardown(  )
    {
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
        iCache->Release();
        iCache = NULL;        
        }

    CStubDataHolder::Release();
    //delete iTestObserver;
    //REComSession::FinalClose();
	#if _BullseyeCoverage
	    cov_write();
    #endif
    }

void MT_DevStaPlg::TestPropertyL()
    { 
   	EUNIT_ASSERT( iPublisher );

    TAny* result = iPublisher->GetProperty( CHsContentPublisher::EPublisherContent );
    EUNIT_ASSERT( result );
    
    MAiContentItemIterator* iter = static_cast<MAiContentItemIterator*>( result );
    EUNIT_ASSERT( iter );
    }

void MT_DevStaPlg::TestOperatorLogoPublisherL()
    {
    RFbsSession::Connect();
    
    // StoreOTALogoL( KCountryCode, KNetworkId );
    
    //only OTA logo
    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;
    //TNWInfo nwInfo;
    
    // CStubDataHolder::Instance()->SetNWMessageObserver(*iTestObserver, nwInfo);
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );   
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    
    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

    //CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    // EUNIT_ASSERT(cache);

    /*
    TPckgBuf<CGulIcon*> logoBuf;
    logoBuf.Copy( *cache->iData );
    
    //EUNIT_ASSERT( cache.iId );
    EUNIT_ASSERT( logoBuf()->Bitmap() );
    EUNIT_ASSERT( logoBuf()->Mask() );
    */
    
    RFbsSession::Disconnect();
    }


void MT_DevStaPlg::TestOperatorProgNamePublisherL()
    {
    _LIT( KOperatorName, "ProgName" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iOperatorNameInfo.iName.Copy( KOperatorName );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;
    //TNWInfo nwInfo;
    //CStubDataHolder::Instance()->SetNWMessageObserver(*iTestObserver, nwInfo);
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );   
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );

    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

                                                 
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );
    //CContentCache* cache = iCache->GetContent( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );

    EUNIT_ASSERT(cache);

    EUNIT_ASSERT( *cache->iText == KOperatorName );
    }


void MT_DevStaPlg::TestOperatorNetinfoNamePublisher_LongL()
    {
    // BUG IN THE CODE, WILL NOT WORK UNTIL IT'S FIXED
    _LIT( KOperatorNameLong, "OPERATOR II" );
    _LIT( KOperatorName, "KEKS" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iLongName.Copy( KOperatorName );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;
    //nwInfo->iOperatorNameInfo.iName=KOperatorNameLong;

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );   
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );

    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT(cache);

    EUNIT_ASSERT( *cache->iText == KOperatorNameLong );
    }

void MT_DevStaPlg::TestOperatorNetinfoNamePublisher_ShortL()
    {
    // BUG IN THE CODE, WILL NOT WORK UNTIL IT'S FIXED
    _LIT( KOperatorNameShort, "OPERATOR" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iShortName.Copy( KOperatorNameShort );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;
    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );   
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    
    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT(cache);
    
    EUNIT_ASSERT( *cache->iText == KOperatorNameShort );
    }

void MT_DevStaPlg::TestOperatorNetinfoNamePublisher_TagL()
    {
    // BUG IN THE CODE, WILL NOT WORK UNTIL IT'S FIXED
    _LIT( KOperatorNameTag, "OP2" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iDisplayTag.Copy( KOperatorNameTag );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredRoaming;

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );   
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );

    //wait for publish
    CAOUserAfter::AfterLD( 2 * 1000000 );

    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT(cache);

    EUNIT_ASSERT( *cache->iText == KOperatorNameTag );
    }
    
    
void MT_DevStaPlg::TestOperatorProviderNamePublisher_InSPDINoPLMNL()
    {    
    _LIT( KOperatorProviderName, "Provider" );

    // Delete logo, so it will not be published.
    // DeleteOTALogoL(KCountryCode, KNetworkId);
    
    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iSPName.Copy( KOperatorProviderName );
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplaySPNRequired;
//    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplayPLMNRequired;
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    EncodePLMNField( nwInfo->iPLMNField, ETrue, KCountryCode, KNetworkId );
    

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    //wait for publish
    CAOUserAfter::AfterLD( 500000 );

    //CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity );
    
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT( cache );

    EUNIT_ASSERT( *cache->iText == KOperatorProviderName );
    
    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
void MT_DevStaPlg::TestOperatorProviderNamePublisher_InSPDIPLMNL()
    {    
    _LIT( KOperatorProviderName, "Provider" );

    // Delete logo, so it will not be published.
    // DeleteOTALogoL(KCountryCode, KNetworkId);
    
    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iSPName.Copy( KOperatorProviderName );
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplaySPNRequired;
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplayPLMNRequired;
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    EncodePLMNField( nwInfo->iPLMNField, ETrue, KCountryCode, KNetworkId );
    

    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );

    //wait for publish
    CAOUserAfter::AfterLD( 500000 );

    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );    

    EUNIT_ASSERT(cache);

    // EUNIT_ASSERT( *cache->iText == KOperatorProviderName );
    
    iCache->Reset();
    
    //wait for clean
    CAOUserAfter::AfterLD( 3 * 1000000 );

    /*
    cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity, ETrue );
    EUNIT_ASSERT( cache->iClean );
    */
    
    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
    
    
    
void MT_DevStaPlg::TestBTSAPPublisherL()
    {    
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    //fake BTSAP change
    
    ChangeBTSAPValueL( EBTSapConnected );
    
    CAOUserAfter::AfterLD( 1000000 );
                               
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT( cache );

    EUNIT_ASSERT( cache->iResource == R_ACTIVEIDLE_BT_SIM_ACCESS_PROFILE_STRING );

    iCache->Reset();
                                
    DeleteBTSAPKey();
                                
    CAOUserAfter::AfterLD( 1000000 );
    
    cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity, ETrue );

    EUNIT_ASSERT( cache );

    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
    
void MT_DevStaPlg::TestSimRegPublisherL()
    {
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    EmulateSimRegFailEvent();
    
    CAOUserAfter::AfterLD( 1000000 );
                               
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentSIMRegStatus );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    EUNIT_ASSERT( cache->iResource == EAiDeviceStatusResourceSIMRegFail );
    
    iCache->Reset();
                                
    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
void MT_DevStaPlg::TestHCZPublisherL()
    {    
    _LIT( KZoneName, "ZoneName" );
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    EmulateHCZEvent( KZoneName );
    
    CAOUserAfter::AfterLD( 1000000 );
                               
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentVHZText );


    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    EUNIT_ASSERT( *cache->iText == KZoneName );
    
    iCache->Reset();
                                
    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
void MT_DevStaPlg::TestCUGPublisherL()
    {    
    _LIT( KCUGMessage, "Group 7" );
    const TInt KCUGIndex = 7;
    
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    EmulateCUGEvent( KCUGIndex );
    
    CAOUserAfter::AfterLD( 1000000 );
                               
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentCUGIndicator );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    EUNIT_ASSERT( *cache->iText == KCUGMessage );
    
    iCache->Reset();
                                
    iPublisher->Suspend( CHsContentPublisher::EBackground );
    }
    
void MT_DevStaPlg::TestMCNPublisherL()
    {
    _LIT( KMCNMessage, "MCN" );
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    EmulateMCNEvent( KMCNMessage );
    
    CAOUserAfter::AfterLD( 1000000 );
                               
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentMCNIndicator );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    EUNIT_ASSERT( *cache->iText == KMCNMessage );                               
    
    iCache->Reset();    
    
    iPublisher->Suspend( CHsContentPublisher::EBackground );    
    }

/*
void MT_DevStaPlg::TestContentRequest1L()
    {    
    _LIT( KProfile, "Outdoor" );
    
    RFbsSession::Connect();
    
    iPublisher->Resume( CHsContentPublisher::EForeground );
    
    CStubDataHolder::Instance()->SetCurrentProfileL( KProfile, EProfileGeneralId );

    StoreOTALogoL( KCountryCode, KNetworkId );
    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
	
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );    
    
    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }    
		/*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );

		
    iCache->Reset();
    RFbsSession::Disconnect();
    }
*/

void MT_DevStaPlg::TestContentRequest2L()
    {
    _LIT( KOperatorProviderName, "Provider" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iSPName.Copy( KOperatorProviderName );
    nwInfo->iServiceProviderNameDisplayReq |= ENWDisplaySPNRequired;
    nwInfo->iCountryCode.Format( _L("%i"), KCountryCode );
    nwInfo->iNetworkId.Format( _L("%i"), KNetworkId );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    EncodePLMNField( nwInfo->iPLMNField, ETrue, KCountryCode, KNetworkId );
    
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
        */
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest3L()
    {
    _LIT( KOperatorName, "ProgName" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iOperatorNameInfo.iName.Copy( KOperatorName );
    nwInfo->iStatus = ENWStatusCurrent;
    nwInfo->iRegistrationStatus = ENWRegisteredOnHomeNetwork;
    
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    // shouldn't have content...
    // EUNIT_ASSERT( !cache );

    // ...until now:
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkInfoChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange );
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageServiceProviderNameChange );    
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange );
    
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );    
    
    // request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );
    
    EUNIT_ASSERT( cache );

    // EUNIT_ASSERT( *cache->iText == KOperatorName );
    iCache->Reset();
    }
         
/*
void MT_DevStaPlg::TestContentRequest4L()
    {
    _LIT( KOperatorNameTag, "NetInfo" );
    _LIT( KProfGeneral, "General" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iDisplayTag.Copy( KOperatorNameTag );
    
    CStubDataHolder::Instance()->SetCurrentProfileL( KProfGeneral, EProfileGeneralId );
        
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    EUNIT_ASSERT( cache );

    iCache->Reset();
    }
         */

void MT_DevStaPlg::TestContentRequest5L()
    {    
    _LIT( KOperatorName, "ProgName" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    TNWInfo* nwInfo = CStubDataHolder::Instance()->NWInfo();
    nwInfo->iOperatorNameInfo.iType = RMmCustomAPI::EOperatorNameNitzFull;
    nwInfo->iOperatorNameInfo.iName.Copy( KOperatorName );
    
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    */
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    // shouldn't have content...
    // EUNIT_ASSERT( !cache );

    // ...until now:
    CStubDataHolder::Instance()->NWMessageObserver()->HandleNetworkMessage( MNWMessageObserver::ENWMessageNetworkProviderNameChange  );
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );

    cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest6L()
    {
    _LIT( KProfile, "Outdoor" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    CStubDataHolder::Instance()->SetCurrentProfileL( KProfile, EProfileOutdoorId );
        
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentProfileName );
    */
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentProfileName );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest7L()
    {    
    _LIT( KProfile, "General" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    CStubDataHolder::Instance()->SetCurrentProfileL( KProfile, EProfileGeneralId );

		/*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentGeneralProfileName );
    */
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentGeneralProfileName );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest8L()
    {
    _LIT( KProfile, "Offline" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    CStubDataHolder::Instance()->SetCurrentProfileL( KProfile, EProfileOffLineId );
        
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    */
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest9L()
    {    
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    */
    iPublisher->Resume( CHsContentPublisher::EForeground );

    request->RefreshContent( EAiDeviceStatusContentDate ); 
    // request->RefreshContent( EAiDeviceStatusContentDate );
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentDate );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest10L()
    {
    _LIT( KProfile, "Silent" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    CStubDataHolder::Instance()->SetCurrentProfileL( KProfile, EProfileSilentId );
    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    */
    
    // EAiDeviceStatusContentProfileName and EAiDeviceStatusContentSilentIndicator
    // are published in same class
    // request->RefreshContent( EAiDeviceStatusContentProfileName );
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentSilentIndicator );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest13L()
    {    
    _LIT( KZoneName, "ZoneName" );

    iPublisher->Resume( CHsContentPublisher::EForeground );

    EmulateHCZEvent( KZoneName );
    
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );    
    request->RefreshContent( EAiDeviceStatusContentVHZIndicator );     
    
    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentVHZText );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest14L()
    {    
    _LIT( KCUGMessage, "Group 7" );
    const TInt KCUGIndex = 7;

    iPublisher->Resume( CHsContentPublisher::EForeground );

    EmulateCUGEvent( KCUGIndex );
    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentCUGIndicator );
    */
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );    
    request->RefreshContent( EAiDeviceStatusContentCUGIndicator );
    
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentCUGIndicator );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    EUNIT_ASSERT( *cache->iText == KCUGMessage );

    iCache->Reset();
    }
         

void MT_DevStaPlg::TestContentRequest15L()
    {    
    iPublisher->Resume( CHsContentPublisher::EForeground );

    ChangeBTSAPValueL( EBTSapConnected );
    
    /*
    MAiPropertyExtension* ext = static_cast<MAiPropertyExtension*>( iPublisher->Extension( KExtensionUidProperty ) ); 
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ext->GetPropertyL( EAiContentRequest ) );
    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    */
    TAny* ptr = iPublisher->GetProperty( CHsContentPublisher::EContentRequest );
    MAiContentRequest* request = static_cast<MAiContentRequest*>( ptr );    
    request->RefreshContent( EAiDeviceStatusContentNetworkIdentity );
    
    CContentCache* cache = iCache->GetLastContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }

    iCache->Reset();
    DeleteBTSAPKey();
    }
         
         
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_DevStaPlg,
    "Device Status Plugin test cases",
    "MODULE" )

/*
EUNIT_TEST(
    "Test initializing",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupEmptyL, TestL, Teardown)
*/
    
EUNIT_TEST(
    "Operator log publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestOperatorLogoPublisherL, Teardown)    

EUNIT_TEST(
    "Test plugin property",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestPropertyL, Teardown)

/*
EUNIT_TEST(
    "Publisher - silent & offline",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestProfilePublisher2L, Teardown)
*/
    
EUNIT_TEST(
    "Test SimReg publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestSimRegPublisherL, Teardown)

EUNIT_TEST(
    "Test HCZ publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestHCZPublisherL, Teardown)

EUNIT_TEST(
    "Test CUG publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestCUGPublisherL, Teardown)

EUNIT_TEST(
    "Test MCN publisher",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestMCNPublisherL, Teardown)
       
EUNIT_TEST(
    "Cont. Request 2",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest2L, Teardown)
    
EUNIT_TEST(
    "Cont. Request 3",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest3L, Teardown)   
    
EUNIT_TEST(
    "Cont. Request 5",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest5L, Teardown)  
    
EUNIT_TEST(
    "Cont. Request 9",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest9L, Teardown)   

EUNIT_TEST(
    "Cont. Request 13",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest13L, Teardown) 
    
EUNIT_TEST(
    "Cont. Request 14",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest14L, Teardown) 
    
EUNIT_TEST(
    "Cont. Request 15",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestContentRequest15L, Teardown)     

EUNIT_TEST(
    "SPDI show PLMN",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestOperatorProviderNamePublisher_InSPDIPLMNL, Teardown)    

EUNIT_END_TEST_TABLE

//  END OF FILE
