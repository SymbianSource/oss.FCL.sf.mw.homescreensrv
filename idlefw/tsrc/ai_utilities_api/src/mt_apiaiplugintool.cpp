/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// CLASS UNDER TEST
#include <caouserafter.h>
#include <aiutility.h>
#include <aiplugintool.h>
#include <mt_apiaiplugintool.h>

#include <aisystemuids.hrh>
//#include <aicontentpublisher.h>

#include <digia/eunit/eunitmacros.h>


// CONSTRUCTION
MT_ApiAiplugintool* MT_ApiAiplugintool::NewL()
    {
    MT_ApiAiplugintool* self = MT_ApiAiplugintool::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_ApiAiplugintool* MT_ApiAiplugintool::NewLC()
    {
    MT_ApiAiplugintool* self = new( ELeave ) MT_ApiAiplugintool();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_ApiAiplugintool::~MT_ApiAiplugintool()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_ApiAiplugintool::MT_ApiAiplugintool()
    {
    }

// Second phase construct
void MT_ApiAiplugintool::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    //CAOUserAfter::AfterLD( 2 * 1000000 );
    }

//  METHODS

void MT_ApiAiplugintool::SetupL(  )
    {
    iAiPluginTool = AiUtility::CreatePluginToolL();
    }

void MT_ApiAiplugintool::Teardown(  )
    {
    if( iPublisher )
    {
    iPublisher->Stop( CHsContentPublisher::ESystemShutdown );
    delete iPublisher;
    iPublisher = NULL;        
    }

    Release( iAiPluginTool );
    }

void MT_ApiAiplugintool::TestContentItemIteratorLL(  )
    {
    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN };
    
    _LIT( KDeviceStatusPluginName, "0x10275101" );
    _LIT8( KNameSpace, "1" );
 
    THsPublisherInfo pubInfo( uid, KDeviceStatusPluginName, KNameSpace );
    iPublisher = CHsContentPublisher::NewL( pubInfo );
 
    MAiContentItemIterator* itemIterator = NULL;
    itemIterator= iAiPluginTool->ContentItemIterator( *iPublisher );

    EUNIT_ASSERT( itemIterator);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_ApiAiplugintool,
    "Test MAiPluginTool implementation returned from AiUtility::CreatePluginToolL",
    "MODULE")

EUNIT_TEST(
    "Test ContentItemIteratorL",
    "ContentItemIteratorL",
    "All",
    "FUNCTIONALITY",
    SetupL, TestContentItemIteratorLL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
