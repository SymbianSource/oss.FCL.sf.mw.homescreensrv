/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for this component
*
*/



// INCLUDE FILES
#include    "hspsdefinitionengine.h"
#include    "hspsxmlhandler.h"
#include    "hspsodt.h"
#include    "hspslocaleparser.h"
#include    "hspsdomdocument.h"
#include    "hspsdomnode.h"
#include    "hspsdomattribute.h"
#include    "hspsdomlist.h"
#include    "hspsdomdepthiterator.h"
#include    "hspsdefinitionengineinterface.h"

#include    <f32file.h>




//CONSTANT DEFINITIONS
_LIT8(KRef,     "ref");
_LIT8(KId,      "id");

// ============================ LOCAL FUNCTIONS ===============================

    
// -----------------------------------------------------------------------------
// DoBuildRefNodesL Find referred node and build subtree if any.
// -----------------------------------------------------------------------------
//
static void DoBuildRefNodesL( ChspsDomDocument& aDom, ChspsDomNode& aNode )
    {
    ChspsDomAttribute* refAttr = 
        static_cast<ChspsDomAttribute*>( aNode.AttributeList().FindByName( KRef ) );
    
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *aDom.RootNode() );
    CleanupStack::PushL( iter );
    
    ChspsDomNode* node = iter->First();
    TBool found( EFalse );
    
            
    while ( node && !found )
        {
        ChspsDomAttribute* attr = 
            static_cast<ChspsDomAttribute*>( node->AttributeList().FindByName( KId ) );
        if ( refAttr && attr )
            {
            
            if ( refAttr->Value().Compare( attr->Value() ) == 0 &&
                aNode.Name().Compare( node->Name() ) == 0 &&
                aNode.Namespace().Compare( node->Namespace() ) == 0 )
                {
                //If referred node is a referrer parent leave because of illegal structure
                ChspsDomNode* parent = aNode.Parent();
                while ( parent )
                    {
                    if ( node == parent )
                        {
                        User::Leave( KErrArgument );
                        }
                    parent = parent->Parent();    
                    }
                
                //reference to self is also forbidden
                if ( node == &aNode )
                    {
                    User::Leave( KErrArgument );
                    }
                
                //Build a sub-tree aggregation of ref nodes if any    
                TInt childCount( node->ChildNodes().Length() );
                ChspsDomNode* refNode = NULL;
                ChspsDomNode* referredNode = NULL;
                for ( TInt i=0; i<childCount; i++ )
                    {
                    referredNode = 
                        static_cast<ChspsDomNode*>( node->ChildNodes().Item( i ) );
                    refNode = referredNode->CreateRefNodeL();
                    CleanupStack::PushL( refNode );
                    aNode.AddChildL( refNode );
                    CleanupStack::Pop( refNode );
                    }
                found = ETrue;
                }
            }
        node = iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter ); 
    }
    
// -----------------------------------------------------------------------------
// BuildRefNodesL Goes through the DOM and creates a ref node to match referred 
// sub-tree if any.
// -----------------------------------------------------------------------------
//
static void BuildRefNodesL( ChspsDomDocument& aDom )
    {
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *aDom.RootNode() );
    CleanupStack::PushL( iter );
    
    ChspsDomNode* node = iter->First();
    ChspsDomNode* current = node;
    while ( node )
        {
        // Iterator need to continue iterating previous context, 
        // not from added content. If current node is a ref node, it can only 
        // have siblings before referred sub tree is created. 
        current = node;
        node = iter->NextL(); 
        
        if ( current->IsRefNode() )
            {
            DoBuildRefNodesL( aDom, *current );
            }
        }
    CleanupStack::PopAndDestroy( iter ); 
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDefinitionEngine::ChspsDefinitionEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDefinitionEngine::ChspsDefinitionEngine()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDefinitionEngine::ConstructL()
    {
    ihspsXMLHandler = ChspsXMLHandler::NewL();
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDefinitionEngine* ChspsDefinitionEngine::NewL()
    {
    ChspsDefinitionEngine* self = new( ELeave ) ChspsDefinitionEngine;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
ChspsDefinitionEngine::~ChspsDefinitionEngine()
    {
    delete ihspsXMLHandler;
    }


// -----------------------------------------------------------------------------
// ChspsDefinitionEngine::CreateDOM
// Parse the given XML file, insert all nodes to DOM tree,
// assign all properties to the nodes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ChspsDefinitionEngineInterface::TError ChspsDefinitionEngine::CreateDOM(
    RFs& aFsSession,
    const TDesC& aXMLFileName,
    ChspsODT& ahspsOdt )
    {
    ChspsDomDocument& dom = ahspsOdt.DomDocument();
    ChspsDefinitionEngineInterface::TError errCode;
    errCode.iDefEngError = KErrNone;
    errCode.iSubComponentError = KErrNone;
    
    TRAPD( err,ihspsXMLHandler->ParseXmlDocL( aFsSession, aXMLFileName, dom ) );

    if ( err != KErrNone )
        {
        	
        errCode.iDefEngError = KErrXmlParsing;
        errCode.iSubComponentError = err;
        return errCode;
        }
        
    TRAP( err, BuildRefNodesL( dom ) );
    if ( err != KErrNone )
        {
        errCode.iDefEngError = KErrRefBuilding;
        errCode.iSubComponentError = err;
        return errCode;
        }
    
    return errCode;

    }

// -----------------------------------------------------------------------------
// ChspsDefinitionEngine::ApplyLanguageL
// Parse the given XML file, insert all nodes to DOM tree,
// assign all properties to the nodes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsDefinitionEngine::ApplyLanguageL(
    RFs& aFsSession,
    const TDesC& aDTDFileName,
    ChspsODT& ahspsOdt )
    {
    ChspsLocaleParser* localeParser =  ChspsLocaleParser::NewL();
    CleanupStack::PushL( localeParser );
    localeParser->ApplyDtdToDomL( aFsSession, aDTDFileName, ahspsOdt.DomDocument() );
    CleanupStack::PopAndDestroy( localeParser );   
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
