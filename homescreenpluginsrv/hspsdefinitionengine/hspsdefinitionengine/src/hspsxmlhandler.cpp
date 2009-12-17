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
* Description:  Main class for XML parsing
*
*/



// INCLUDE FILES
#include    "hspsxmlhandler.h"

#include    "hspsdomattribute.h"
#include    "hspsdomlist.h"
#include    "hspsdomdocument.h"
#include    "hspsdomnode.h"
#include    "hspsdomstringpool.h"

#include    <parserfeature.h>
#include    <matchdata.h>


// LOCAL CONSTANTS AND MACROS
_LIT8(KXmlMimeType,"text/xml");
_LIT8(KVariant,"libxml2");

_LIT8(KAnd,"&");
_LIT8(KSemicolon, ";" );
_LIT8(KRefAttr, "ref");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsXMLHandler::ChspsXMLHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsXMLHandler::ChspsXMLHandler()    
    {
    }

// -----------------------------------------------------------------------------
// ChspsXMLHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::ConstructL()
    {
    MContentHandler* contentHandler = this;
     
    CMatchData* matchData = CMatchData::NewL();
    CleanupStack::PushL(matchData);
    matchData->SetMimeTypeL(KXmlMimeType);
    matchData->SetVariantL(KVariant);

    iXmlParser = Xml::CParser::NewL( *matchData, *contentHandler );
    iXmlParser->EnableFeature( EReplaceIntEntityFromExtSubsetByRef );
    iNodeStack = new (ELeave)RPointerArray<ChspsDomNode>;
    CleanupStack::PopAndDestroy(matchData);

    }

// -----------------------------------------------------------------------------
// ChspsXMLHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsXMLHandler* ChspsXMLHandler::NewL()
    {
    ChspsXMLHandler* self = new( ELeave ) ChspsXMLHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
ChspsXMLHandler::~ChspsXMLHandler()
    {
    delete iXmlParser;
    if ( iNodeStack )
        {
        iNodeStack->Close();
        delete iNodeStack;
        }
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::ParseXmlDocL
// Parses the given XML file, creates nodes and attributes and inserting
// them to aDomDocument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::ParseXmlDocL(
    RFs& aFsSession, 
    const TDesC& aFileName,
    ChspsDomDocument& aDomDocument )    
    {
    iError = KErrNone;
    iDomDocument = &aDomDocument;
    Xml::ParseL(*iXmlParser, aFsSession, aFileName );
    User::LeaveIfError( iError );
    }    
  
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnStartDocumentL
// This method is a callback to indicate the start of the document
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
    {
    }

// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnEndDocumentL
// This method is a callback to indicate the end of the document
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnStartElementL
// This method is a callback to indicate the start of an element
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 void ChspsXMLHandler::OnStartElementL(
    const RTagInfo& aElement, 
    const RAttributeArray& aAttributes, 
    TInt /*aErrorCode*/)
    {
    const TDesC8& localName = aElement.LocalName().DesC();
    const TDesC8& uri = aElement.Uri().DesC();
        
    ChspsDomNode* currentNode = iDomDocument->CreateElementNSL( localName, uri );
    CleanupStack::PushL( currentNode );
    
    // set hierarchy
    if ( iParentNode  )
        {
        iParentNode->AddChildL( currentNode );
        }
    else
        {
        iDomDocument->SetRootNode( currentNode );
        iParentNode = currentNode;  // root node
        }
    CleanupStack::Pop( currentNode );   //Now parent or document has a ownership of the new node
        
    currentNode->SetNodeId( iCurrentNodeId++ );
    // "Push" parent for this node into stack. When an end element for this node is encountered, 
    // "Pop" parent node from stack. 
    iNodeStack->AppendL( iParentNode );     
    iParentNode = currentNode;                  //Current node will be a parent for a new element
    
    
    // add attributes
    TInt attrCount( aAttributes.Count() );
    TBool refSet ( EFalse );
    for( TInt i=0; i<attrCount; i++ )
        {
        const TDesC8& attrNameDes8 = aAttributes[i].Attribute().LocalName().DesC();
        const TDesC8& attValueDes8 = aAttributes[i].Value().DesC();
        
        if ( !refSet && KRefAttr().Compare( attrNameDes8 ) == 0 )
            {
            currentNode->SetRefNode();
            refSet = ETrue;
            }
        ChspsDomStringPool& stringPool = currentNode->StringPool();
        ChspsDomAttribute* newAttr = ChspsDomAttribute::NewL( attrNameDes8, stringPool );
        CleanupStack::PushL( newAttr );
        newAttr->SetValueL( attValueDes8 );
        currentNode->AttributeList().AddItemL( newAttr );    //takes ownership
        CleanupStack::Pop( newAttr );
        }
    }

// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnEndElementL
// This method is a callback to indicate the end of an element
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::OnEndElementL(const RTagInfo& /*aElement*/, TInt /*aErrorCode*/)
    {
  //  const TDesC8& localName = aElement.LocalName().DesC();
    TInt stackCount( iNodeStack->Count() );
    if ( stackCount > 0)
        {
        iParentNode = (*iNodeStack)[ stackCount-1 ];
        iNodeStack->Remove( stackCount-1 );
        }
    }

// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnContentL
// This method is a callback that sends the content of the element.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
#ifdef _DEBUG    
    const TDesC8& currentNodeName = iParentNode->Name();
    TContentType type = iParentNode->ContentType();
#endif
    if ( aErrorCode == KErrNone && iParentNode->ContentType() == EPCData )
        {
        //iParentNode holds the current node
        iParentNode->AppendPCDataL( aBytes );
        }
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnStartPrefixMappingL(
// This method is a notification of the beginning of the scope of a prefix-URI mapping.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsXMLHandler::OnStartPrefixMappingL(
    const RString& /*aPrefix*/, 
    const RString& /*aUri*/, 
    TInt /*aErrorCode*/)
    {
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnEndPrefixMappingL
// This method is a notification of the end of the scope of a prefix-URI mapping.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsXMLHandler::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
    {
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnIgnorableWhiteSpaceL
// This method is a notification of ignorable whitespace in element content.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsXMLHandler::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
    {
    }    
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnProcessingInstructionL
// This method is a receive notification of a processing instruction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsXMLHandler::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/, 
    const TDesC8& /*aData*/, 
	TInt /*aErrorCode*/)
    {
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnSkippedEntityL
// This method is a notification of a skipped entity.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsXMLHandler::OnSkippedEntityL(const RString& aName, TInt aErrorCode) 
    {
#ifdef _DEBUG    
    const TDesC8& currentNodeName = iParentNode->Name();
    const TDesC8& entity = aName.DesC(); 
    TContentType type = iParentNode->ContentType();
#endif
    if ( aErrorCode == KErrNone && iParentNode->ContentType() == EPCData )
        {
        //iParentNode holds the current node
        iParentNode->AppendPCDataL( KAnd );
        iParentNode->AppendPCDataL( aName.DesC() );
        iParentNode->AppendPCDataL( KSemicolon );
        }    
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::OnError
// This method indicates an error has occurred.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsXMLHandler::OnError(TInt aErrorCode)
    {
    iError = aErrorCode;
    }
// -----------------------------------------------------------------------------
// ChspsXMLHandler::GetExtendedInterface
// This method obtains the interface matching the specified uid.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TAny* ChspsXMLHandler::GetExtendedInterface(const TInt32 /*aUid*/)
    {
    return NULL;    
    }    

       

//  End of File  
