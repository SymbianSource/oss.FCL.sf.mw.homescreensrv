/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: unit test for the sapidataobserver
*
*/


//  CLASS HEADER
#include "ut_sapidataobserver.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <e32cmn.h>

//  INTERNAL INCLUDES
#include "sapidata.h"
#include "sapidataobserver.h"
#include "sapidataplugin.h"
#include "sapidatapluginconst.h"

// CONSTRUCTION

// ---------------------------------------------------------------------------
// UT_SapiDataObserver* UT_SapiDataObserver::NewL()
// ---------------------------------------------------------------------------
//
UT_SapiDataObserver* UT_SapiDataObserver::NewL()
    {
    UT_SapiDataObserver* self = UT_SapiDataObserver::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// UT_SapiDataObserver* UT_SapiDataObserver::NewLC()
// ---------------------------------------------------------------------------
//
UT_SapiDataObserver* UT_SapiDataObserver::NewLC()
    {
    UT_SapiDataObserver* self = new(ELeave) UT_SapiDataObserver();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_SapiDataObserver::~UT_SapiDataObserver()
    {
    
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_SapiDataObserver::UT_SapiDataObserver()
    {
    
    }

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ---------------------------------------------------------------------------
// Auxiliary method for the obtaining of the MLiwInterface
// ---------------------------------------------------------------------------
//
MLiwInterface* UT_SapiDataObserver::GetMessagingInterfaceL()
	{
	if( !iServiceHandler )
		{
		iServiceHandler = CLiwServiceHandler::NewL();
		}
	
	CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( inParam );
	CLiwGenericParamList* outParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( outParam );
	CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, KCPInterface,
		KCPService  );
	crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
	
	RCriteriaArray a;
	a.AppendL(crit);    
	
	iServiceHandler->AttachL(a);
	iServiceHandler->ExecuteServiceCmdL( *crit, *inParam, *outParam ); 
	
	delete crit;
	a.Reset();
	
	// find service interface
	TInt pos = 0;
	MLiwInterface* msgInterface = NULL;
	outParam->FindFirst( pos, KCPInterface );
	if ( pos != KErrNotFound ) 
	    {
	    msgInterface = (*outParam)[pos].Value().AsInterface(); 
	    }
	
	outParam->Reset();
	inParam->Reset();
	CleanupStack::PopAndDestroy( outParam );
	CleanupStack::PopAndDestroy( inParam );
	
	return msgInterface;	
	}

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::SetupL()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::SetupL()
    {
    iInterface = GetMessagingInterfaceL();    
    iPlugin = CSapiDataPlugin::NewL();    
    iData = CSapiData::NewL( iPlugin );
    iObserver = CSapiDataObserver::NewL( iInterface, iData );
    
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KPublisherId ));	
	filter->InsertL( KOperation, TLiwVariant( KAddUpdateDelete ) );
	iObserver->RegisterL( filter, KCpData(), 
                                 KExtendedNotifications );
	CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::Teardown()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::Teardown()
    {
    if ( iObserver )
        {
		iObserver->ReleaseL();
        delete iObserver;
        iObserver = NULL;
        }
    if( iInterface )
    	{
		iInterface->Close();
		iInterface = NULL;
    	}
   	if( iServiceHandler )
    	{
    	iServiceHandler->Reset();
    	delete iServiceHandler;
    	iServiceHandler = NULL;
    	}
    if ( iData )
        {
        delete iData;
        iData = NULL;
        }
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::HandleSessionEventL()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::ConstructTestL()
    {
    if ( iObserver )
        EUNIT_ASSERT( ETrue )
    else
        EUNIT_ASSERT( EFalse );
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::RegisterTestL()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::RegisterTestL()
    {
    CLiwDefaultMap* conRegFilter = iData->CreateFilterLC();
    conRegFilter->InsertL( KOperation, TLiwVariant( KAddUpdateDelete ) );
    
    EUNIT_ASSERT_NO_LEAVE( iObserver->RegisterL( conRegFilter, KCpData(), KExtendedNotifications ));
    
    CleanupStack::PopAndDestroy( conRegFilter );
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::ReleaseTestL()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::ReleaseTestL()
    {
    EUNIT_ASSERT_NO_LEAVE( iObserver->ReleaseL() );
    
    delete iObserver;
    iObserver = NULL;
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataObserver::HandleNotifyTestL()
// ---------------------------------------------------------------------------
//
void UT_SapiDataObserver::HandleNotifyTestL()
    {
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewL();
    TInt id = 999;
    TInt retValue = 0;
    
    retValue = iObserver->HandleNotifyL( id, 0, *paramList, *paramList );
    
    if ( retValue == id )    
        EUNIT_ASSERT( ETrue )
    else
        EUNIT_ASSERT( EFalse );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE
    (
    UT_SapiDataObserver,
    "Sapidata Observer tests",
    "UNIT"
    )

EUNIT_TEST
    (
    "Tests ConstructL()",
    "CSapiDataObserver",
    "ConstructL",
    "FUNCTIONALITY",
    SetupL, ConstructTestL, Teardown
    )

EUNIT_TEST
    (
    "Tests RegisterL()",
    "CSapiDataObserver",
    "RegisterL",
    "FUNCTIONALITY",
    SetupL, RegisterTestL, Teardown
    )

EUNIT_TEST
    (
    "Tests ReleaseL()",
    "CSapiDataObserver",
    "ReleaseL",
    "FUNCTIONALITY",
    SetupL, ReleaseTestL, Teardown
    )
    
EUNIT_TEST
    (
    "Tests HandleNotify()",
    "CSapiDataObserver",
    "HandleNotifyL",
    "FUNCTIONALITY",
    SetupL, HandleNotifyTestL, Teardown
    )
 
EUNIT_END_TEST_TABLE

//  END OF FILE
