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
#include "aioperatornamepublisher.h"
#include "UT_DevStaPlg.h"
#include "aicontentpublisher.h"
#include "aidevicestatuscontentmodel.h"
#include "caouserafter.h"
#include "logotools.h"
#include "testingtools.h"
#include "contentobserver.h"
#include "ainativeuiplugins.h"
#include <aicontentrequest.h>

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/ceunitalloctestcasedecorator.h>
#include <profile.hrh>
#include <mprofilechangeobserver.h>
#include <e32cmn.h>
#include <gulicon.h>

#include "aidevicestatusplugin.h"

//  INTERNAL INCLUDES

const TInt KCountryCode = 244;
const TInt KNetworkId = 91;

// CONSTRUCTION
UT_DevStaPlg* UT_DevStaPlg::NewL()
    {
    UT_DevStaPlg* self = UT_DevStaPlg::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_DevStaPlg* UT_DevStaPlg::NewLC()
    {
    UT_DevStaPlg* self = new( ELeave ) UT_DevStaPlg();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_DevStaPlg::~UT_DevStaPlg()
    {
    }

// Default constructor
UT_DevStaPlg::UT_DevStaPlg()
    {
    }

// Second phase construct
void UT_DevStaPlg::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }


void UT_DevStaPlg::SetupL(  )
    {
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

void UT_DevStaPlg::SetupEmptyL()
    {
//    __UHEAP_MARK;
    }




void UT_DevStaPlg::Teardown(  )
    {
    if( iPublisher )
        {
        iPublisher->Stop( EAiSystemStartup );
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
    REComSession::FinalClose();
	#if _BullseyeCoverage
	    cov_write();
    #endif
    }



void UT_DevStaPlg::TestL()
    {
    CAiOperatorNamePublisher* namePub = CAiOperatorNamePublisher::NewL();
    /*
    namePub->Subscribe( *this,
                        *static_cast<MAiPropertyExtension*>(iPublisher->Extension( KExtensionUidProperty )),
                        *this,
                        *this );
    */
    namePub->Subscribe( *this,
                        *iPublisher,
                        *this,
                        *this );
    
    MAiNetworkInfoObserver* name = namePub;
    name->HandleNetworkInfoChange( MNWMessageObserver::ENWMessageNetworkInfoChange,
                                      *CStubDataHolder::Instance()->NWInfo(),
                                      ETrue );
    name->HandleNetworkInfoChange( MNWMessageObserver::ENWMessageNetworkInfoChange,
                                      *CStubDataHolder::Instance()->NWInfo(),
                                      EFalse );
    _LIT( KProfSilent, "Silent" );
    _LIT( KProfOffline, "Offline" );

    //when publisher is resumed, profile is updated
    //wait for a while

    CStubDataHolder::InstanceL()->SetCurrentProfileL( KProfSilent, EProfileSilentId );

    //when publisher is resumed, profile update should be published
    iPublisher->Resume( EAiSystemStartup );

    CAOUserAfter::AfterLD( 100000 );

    //iBuf should now contain TAiDeviceStatusContentProfile structure

    CContentCache* cache = iCache->GetContent( EAiDeviceStatusContentSilentIndicator );
    EUNIT_ASSERT( cache  );

    iCache->Reset();

    //change profile
    CStubDataHolder::InstanceL()->SetCurrentProfileL( KProfOffline, EProfileOffLineId );
    
    RArray<TProfileObserver>& obs = CStubDataHolder::InstanceL()->ProfileChangeObservers();
    const TInt count = obs.Count();
    for( TInt i( 0 ); i < count; i++ )
        {
        obs[i].iObserver->HandleActiveProfileEventL( EProfileActiveProfileModified , EProfileOffLineId );
        }
    

    //iBuf should now contain updated TAiDeviceStatusContentProfile structure

    cache = iCache->GetContent( EAiDeviceStatusContentNetworkIdentity );

    if( !cache )
        {
        User::Leave( KErrNoMemory );
        }
    

    EUNIT_ASSERT( KProfOffline() == *cache->iText );

    iCache->Reset();
    }
         
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_DevStaPlg,
    "Device Status Plugin test cases",
    "MODULE" )

EUNIT_TEST(
    "Test",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestL, Teardown)

/*
EUNIT_ALLOC_TEST(
    "(Alloc) Test initializing",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupEmptyL, TestL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test plugin property",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestPropertyL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test profile publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestProfilePublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator logo publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorLogoPublisherL, Teardown)


EUNIT_ALLOC_TEST(
    "(Alloc) Test operator prog name publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorProgNamePublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator netinfo name publisher - long",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorNetinfoNamePublisher_LongL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator netinfo name publisher - short",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorNetinfoNamePublisher_ShortL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator netinfo name publisher - tag",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorNetinfoNamePublisher_TagL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator provider name publisher - in SPDI don't show PLMN",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorProviderNamePublisher_InSPDINoPLMNL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test operator provider name publisher - in SPDI show PLMN",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestOperatorProviderNamePublisher_InSPDIPLMNL, Teardown)


EUNIT_ALLOC_TEST(
    "(Alloc) Test BTSAP publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestBTSAPPublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test SimReg publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestSimRegPublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test HCZ publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestHCZPublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test CUG publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestCUGPublisherL, Teardown)

EUNIT_ALLOC_TEST(
    "(Alloc) Test MCN publisher",
    "Add tested class name here",
    "Add tested function name here",
    "ERRORHANDLING",
    SetupL, TestMCNPublisherL, Teardown)
*/

EUNIT_END_TEST_TABLE

//  END OF FILE
