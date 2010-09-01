/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for XCFW Tree Node
*
*/



// INCLUDE FILES
#include    "xcfwnode.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXCFWNode::CXCFWNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXCFWNode::CXCFWNode()
    {
    }

// -----------------------------------------------------------------------------
// CXCFWNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXCFWNode::ConstructL( 
    CGECOObjectBase* aData )
    {
    SetData(aData);
    }

// -----------------------------------------------------------------------------
// CXCFWNode::NewL
// Two-phased constructor with Node Data initializer.
// -----------------------------------------------------------------------------
//
CXCFWNode* CXCFWNode::NewL( 
    CGECOObjectBase* aData )
    {
    CXCFWNode* self = new( ELeave ) CXCFWNode;
    
    CleanupStack::PushL( self );
    self->ConstructL(aData);
    CleanupStack::Pop( self );

    return self;
    }

   
// Destructor
CXCFWNode::~CXCFWNode()
    {
    delete iData; //delete only owned pointer. Others are not owned.
    iParent = NULL;
    iFirstChild = NULL;
    iLastChild = NULL;
    iPrevSibling = NULL;
    iNextSibling = NULL;
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetParent
// Assign given node to non-owned member pointer variable
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetParent(
    MXCFWNode* aNode )
    {
    iParent = aNode;    
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetFirstChild
// Assign given node to non-owned member pointer variable
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetFirstChild(
    MXCFWNode* aNode )
    {
    iFirstChild = aNode;    
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetLastChild
// Assign given node to non-owned member pointer variable
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetLastChild(
    MXCFWNode* aNode )
    {
    iLastChild = aNode;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetNextSibling
// Assign given node to non-owned member pointer variable
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetNextSibling(
    MXCFWNode* aNode )
    {
    iNextSibling = aNode;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetPrevSibling
// Assign given node to non-owned member pointer variable
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetPrevSibling(
    MXCFWNode* aNode )
    {
    iPrevSibling = aNode;
    }

// -----------------------------------------------------------------------------
// CXCFWNode::SetData
// Assign given data to member data variable. Owned. User should take care that
// the previous data object is deleted.
// -----------------------------------------------------------------------------
//
void CXCFWNode::SetData(
    CGECOObjectBase* aData )
    {
    iData = aData;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::Parent
// return internal pointer
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWNode::Parent()
    {
    return iParent;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::FirstChild
// return internal pointer
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWNode::FirstChild()
    {
    return iFirstChild;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::LastChild
// return internal pointer
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWNode::LastChild()
    {
    return iLastChild;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::NextSibling
// return internal pointer
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWNode::NextSibling()
    {
    return iNextSibling;        
    }

// -----------------------------------------------------------------------------
// CXCFWNode::PrevSibling
// return internal pointer
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWNode::PrevSibling()
    {
    return iPrevSibling;        
    }


// -----------------------------------------------------------------------------
// CXCFWNode::Data
// return internal data object as CGECOObjectBase pointer. User needs to do
// appropriate casting operations to get access to object-specific functionality
// -----------------------------------------------------------------------------
//
EXPORT_C CGECOObjectBase* CXCFWNode::Data()
    {
    return iData;        
    }


//  End of File  
