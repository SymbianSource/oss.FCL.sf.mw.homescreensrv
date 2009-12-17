/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include <xcfwtree.h>
#include <gecoobjectbase.h>
#include <gecodefaultobject.h>

#include "mcssuiteparser.h"
#include "mcssuiteobject.h"
#include "mcsdef.h"
#include "mcssuiteobjectfactory.h"


// ---------------------------------------------------------
// CSuiteObject::CSuiteParser
// ---------------------------------------------------------
//
CSuiteParser::CSuiteParser()
	{
	// No implementation required
	}

// ---------------------------------------------------------
// CSuiteObject::~CSuiteParser
// ---------------------------------------------------------
//
CSuiteParser::~CSuiteParser()
	{
	iFileName.Close();
    delete iContentEngine;
    delete iObjectFactory;
    delete iActiveScheduler;
    delete iContentTree;
	}

// ---------------------------------------------------------
// CSuiteObject::NewLC
// ---------------------------------------------------------
//
CSuiteParser* CSuiteParser::NewLC( const TDesC& aFileName )
	{
	CSuiteParser* self = new (ELeave) CSuiteParser();
	CleanupStack::PushL(self);
	self->ConstructL( aFileName );
	return self;
	}

// ---------------------------------------------------------
// CSuiteObject::NewL
// ---------------------------------------------------------
//
CSuiteParser* CSuiteParser::NewL( const TDesC& aFileName )
	{
	CSuiteParser* self = CSuiteParser::NewLC( aFileName );
	CleanupStack::Pop( self ); // self;
	return self;
	}

// ---------------------------------------------------------
// CSuiteObject::ConstructL
// ---------------------------------------------------------
//
void CSuiteParser::ConstructL( const TDesC& aFileName )
	{
	iFileName.CreateL( aFileName );
	iActiveScheduler = new ( ELeave ) CActiveSchedulerWait();
    iObjectFactory = CSuiteObjectFactory::NewL();
    iContentEngine = CXCFWEngine::NewL( this );
    iContentEngine->RegisterObjectFactoryL( iObjectFactory );
	}

// -----------------------------------------------------------------------------
// CSuiteParser::GetSuitsArrayL
// -----------------------------------------------------------------------------
//
TInt CSuiteParser::GetSuitsArrayL(  RPointerArray<CSuiteObject>& aSuitesArray  )
    {
    TInt err;
    iContentTree = CXCFWTree::NewL( );
    iContentEngine->LoadL( *iContentTree, iFileName );

    iActiveScheduler->Start( );

    if ( iCompletionStatus == 0 /*TSuiteParsingStatus::ECompleted*/ )
        {
        //this trap ensures stoping the ActiveSheduler
        //in case a leave occurs when updating data
        TRAP( err, HandleEntriesL( aSuitesArray ) );
        }

    delete iContentTree;
    iContentTree = NULL;
    return err;
    }

// ---------------------------------------------------------
// CSuiteObject::HandleEngineEventL
// ---------------------------------------------------------
//
void CSuiteParser::HandleEngineEventL( TXCFWEngineEvent aEvent )
    {
    // handle completion
    if ( aEvent == EEvtParsingComplete )
        {
        TSuiteParsingStatus status = ECompleted;
        HandleCompletionL( status );
        }
    // else => ignore
    }

// ---------------------------------------------------------
// CSuiteObject::HandleEngineErrorL
// ---------------------------------------------------------
//
void CSuiteParser::HandleEngineErrorL( TInt /*aErrorCode*/)
    {
    HandleCompletionL( EFailed );
    }

// ---------------------------------------------------------
// CSuiteObject::HandleCompletionL
// ---------------------------------------------------------
//
void CSuiteParser::HandleCompletionL(
		TSuiteParsingStatus aStatus )
    {
    iCompletionStatus = aStatus;
    iActiveScheduler->AsyncStop( );
    }


// ---------------------------------------------------------
// CSuiteObject::HandleEntriesL
// ---------------------------------------------------------
//
void CSuiteParser::HandleEntriesL( RPointerArray<CSuiteObject>& aSuitesArray )
    {
    MXCFWNode* root = iContentTree->Root( );
    // get children 
    RNodeArray suites;
    CleanupClosePushL( suites );
    iContentTree->GetChildNodesL( root, suites );
    for ( TInt i = 0; i < suites.Count( ); i++ )
        {
        CSuiteObject* suite = CreateSuiteLC( suites[i] );
        aSuitesArray.AppendL( suite );
        CleanupStack::Pop( suite );
        }
    
    // clean up
    CleanupStack::PopAndDestroy( &suites );
    }

// ---------------------------------------------------------
// CSuiteObject::CreateSuiteLC
// ---------------------------------------------------------
//
CSuiteObject* CSuiteParser::CreateSuiteLC( MXCFWNode* aNode )
	{
	TInt lastBackSlash = iFileName.Length() -
        iFileName.LocateReverseF( KBackSlashSuite ) - 1;
	CSuiteObject* suite = 
        CSuiteObject::NewLC( iFileName.Right( lastBackSlash ) );
	
    CGECODefaultObject* node=
            static_cast<CGECODefaultObject*>( aNode->Data() );

    TPtrC name;
    TPtrC value;
    TInt attrCount = node->NumAttributes();
    TBool isLocalized = EFalse;

    for( TInt i = 0; i < attrCount; i++ )
        {
		node->AttributeDetailsL( i, name, value, isLocalized );
        if( name == KMenuAttSuiteName() )
        	{
        	suite->SetSuiteNameL( value );
        	}
			suite->SetInitialAttributeL( name, value, isLocalized );
        }
    
    return suite;
	}

