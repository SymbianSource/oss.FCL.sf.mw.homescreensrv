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
#include "ut_aicpscommandbuffer.h"

#include "caicpscommandbuffer.h"
            

// Constants
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType,   "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT( KWRTPublisher, "wrt_publisher");
_LIT( KTemplateWidget,"ai3templatedwidget");
_LIT( KContentIdValue,   "com.accuweather.widget.touchNG" );
_LIT8( KAction,   "active" );
_LIT( KPubData,        "publisher" );

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsCommandBuffer* UT_AiCpsCommandBuffer::NewL()
    {
    UT_AiCpsCommandBuffer* self = UT_AiCpsCommandBuffer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsCommandBuffer* UT_AiCpsCommandBuffer::NewLC()
    {
    UT_AiCpsCommandBuffer* self = new ( ELeave ) UT_AiCpsCommandBuffer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::~UT_AiCpsCommandBuffer()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsCommandBuffer::~UT_AiCpsCommandBuffer()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::UT_AiCpsCommandBuffer()
//
// ----------------------------------------------------------------------------
//
UT_AiCpsCommandBuffer::UT_AiCpsCommandBuffer()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::SetupL()
    {
    iCommandBuffer = CAiCpsCommandBuffer::NewL();
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::Teardown()
    {
    delete iCommandBuffer;
    iCommandBuffer = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::CreateAndDeleteL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::CreateAndDelete()
    {
    //nothing here
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::GetCPsServiceInsterfaceL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::GetCPsServiceInsterfaceL()
    {
    iCommandBuffer->GetCPSInterfaceL();
    iCommandBuffer->GetCPSInterfaceL();
    MLiwInterface* cpsInterface = iCommandBuffer->CpsInterface();
    CLiwServiceHandler* handler = iCommandBuffer->ServiceHandler();
    if ( !cpsInterface || !handler )
        {
        User::Leave( KErrArgument );
        }
    }

// ----------------------------------------------------------------------------
// UT_AiCpsCommandBuffer::GetCPsServiceInsterfaceL()
//
// ----------------------------------------------------------------------------
//
void UT_AiCpsCommandBuffer::AddCommandAndFlushL()
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KTemplateWidget ));
    filter->InsertL( KContentId, TLiwVariant( KContentIdValue ));
    
    iCommandBuffer->AddCommand( _L("1"), KPubData, filter, KAction );
    iCommandBuffer->AddCommand( _L("1"), KPubData, filter, KAction );
    iCommandBuffer->Flush();
    
    // let destructor do flush
    iCommandBuffer->AddCommand( _L("1"), KPubData, filter, KAction );
    iCommandBuffer->AddCommand( _L("2"), KPubData, filter, KAction );
        
    CleanupStack::PopAndDestroy( filter );
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiCpsCommandBuffer,
    "Unit test suite for AiCpsCommandBuffer",
    "UNIT" )

    EUNIT_TEST(
        "Create And Delete",
        "AiCpsCommandBuffer",
        "",
        "FUNCTIONALITY",
        SetupL, CreateAndDelete, Teardown )

    EUNIT_TEST(
        "Get CPS service interface",
        "AiCpsCommandBuffer",
        "",
        "FUNCTIONALITY",
        SetupL, GetCPsServiceInsterfaceL, Teardown )

    EUNIT_TEST(
        "Add command and flush",
        "AddCommandL",
        "",
        "FUNCTIONALITY",
        SetupL, AddCommandAndFlushL, Teardown )
    
EUNIT_END_TEST_TABLE

// End of file
