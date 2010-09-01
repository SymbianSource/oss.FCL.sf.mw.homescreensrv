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
* Description: EUnit unit test class for CAiStateProvider
*
*/

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>
#include <liwservicehandler.h>

// User includes
#include "ut_aicpsexecuteparam.h"

#include "aicpsexecuteparam.h"

// Constants
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType,   "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT( KWRTPublisher, "wrt_publisher");
_LIT( KTemplateWidget,"ai3templatedwidget");
_LIT( KContentIdValue,   "com.accuweather.widget.touchNG" );
_LIT8( KAction,   "active" );
_LIT( KAction16,   "active" );
_LIT( KPubData,        "publisher" );
_LIT8( KFilter, "filter" );
_LIT8( KActionTrigger, "action_trigger" );
_LIT8( KType,          "type");

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsExecuteParam* UT_AiCpsExecuteParam::NewL()
    {
    UT_AiCpsExecuteParam* self = UT_AiCpsExecuteParam::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsExecuteParam* UT_AiCpsExecuteParam::NewLC()
    {
    UT_AiCpsExecuteParam* self = new ( ELeave ) UT_AiCpsExecuteParam();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::~UT_AiCpsExecuteParam()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsExecuteParam::~UT_AiCpsExecuteParam()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::UT_AiCpsExecuteParam()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsExecuteParam::UT_AiCpsExecuteParam()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsExecuteParam::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsExecuteParam::SetupL()
    {
    iExecuteParam = CAiCpsExecuteParam::NewL();
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsExecuteParam::Teardown()
    {
    delete iExecuteParam;
    iExecuteParam = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::PluginId()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsExecuteParam::PluginId()
    {
    _LIT( KPluginId, "123" );
    iExecuteParam->SetPluginIdL( KPluginId );
    EUNIT_ASSERT_EQUALS( iExecuteParam->PluginId(), KPluginId );
    }

// ----------------------------------------------------------------------------
// UT_AiCpsExecuteParam::InParamMapL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsExecuteParam::InParamMapL()
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    iExecuteParam->SetFilterL(filter);
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KTemplateWidget ));
    filter->InsertL( KContentId, TLiwVariant( KContentIdValue ));
    
    iExecuteParam->SetPluginIdL(_L("1"));
    iExecuteParam->SetRegistryTypeL( KPubData );
    iExecuteParam->SetFilterL(filter);
    iExecuteParam->AddActionL( KAction );

    CLiwDefaultMap* map = iExecuteParam->InParamMapLC();
    
    TLiwVariant tempVariant;
    tempVariant.PushL();
    
    // check map
    if ( map->FindL( KType, tempVariant ) )
        {
        EUNIT_ASSERT_EQUALS( tempVariant.AsDes(), KPubData );
        map->Remove( KType );
        }
    if ( map->FindL( KFilter, tempVariant ) )
        {
        CLiwDefaultMap* filter2 = CLiwDefaultMap::NewLC();
        if ( tempVariant.Get( *filter2 ) )
            {
            if ( filter2->FindL( KPublisherId, tempVariant ) )
                {
                EUNIT_ASSERT_EQUALS( tempVariant.AsDes(), KWRTPublisher );
                filter2->Remove( KPublisherId );
                }
            if ( filter2->FindL( KContentType, tempVariant ) )
                {
                EUNIT_ASSERT_EQUALS( tempVariant.AsDes(), KTemplateWidget );
                filter2->Remove( KContentType );
                }
            if ( filter2->FindL( KContentId, tempVariant ) )
                {
                EUNIT_ASSERT_EQUALS( tempVariant.AsDes(), KContentIdValue );
                filter2->Remove( KContentId );
                }
            if ( filter2->FindL( KActionTrigger, tempVariant ) )
               {
               CLiwDefaultList* actionsToLaunch = CLiwDefaultList::NewLC();
               if ( tempVariant.Get( *actionsToLaunch ) )
                   {
                   if ( actionsToLaunch->AtL( 0, tempVariant ) )
                       {
                       EUNIT_ASSERT_EQUALS( tempVariant.AsData(), KAction );
                       actionsToLaunch->Remove( 0 );
                       //make sure actionsToLaunch is empty
                       EUNIT_ASSERT_EQUALS ( actionsToLaunch->Count(), 0 );
                       }
                   
                   }
               CleanupStack::PopAndDestroy( actionsToLaunch );
               filter2->Remove( KActionTrigger );
               }
            }
        //make sure filter is empty
        EUNIT_ASSERT_EQUALS ( filter2->Count(), 0 );
        CleanupStack::PopAndDestroy( filter2 );
        
        map->Remove( KFilter );
        }
    
    //make sure map is empty
    EUNIT_ASSERT_EQUALS ( map->Count(), 0 );
    
    CleanupStack::Pop(&tempVariant);
    tempVariant.Reset();
    CleanupStack::PopAndDestroy( 2,filter );
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiCpsExecuteParam,
    "Unit test suite for CAiCpsExecuteParam",
    "UNIT" )

    EUNIT_TEST(
        "PluginId",
        "CAiCpsExecuteParam",
        "",
        "FUNCTIONALITY",
        SetupL, PluginId, Teardown )
    
    EUNIT_TEST(
        "InParamMap",
        "CAiCpsExecuteParam",
        "",
        "FUNCTIONALITY",
        SetupL, InParamMapL, Teardown )
        
EUNIT_END_TEST_TABLE

// End of file
