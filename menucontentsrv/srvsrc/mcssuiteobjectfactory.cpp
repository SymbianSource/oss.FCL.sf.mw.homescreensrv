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
#include <gecodefaultobject.h>
#include <gecodefaultobjectfactory.h>

#include "mcssuiteobjectfactory.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSuiteObjectFactory::~CSuiteObjectFactory
// ---------------------------------------------------------
//
CSuiteObjectFactory::~CSuiteObjectFactory()
    {
    }

// ---------------------------------------------------------
// CSuiteObjectFactory::NewL
// ---------------------------------------------------------
//
CSuiteObjectFactory* CSuiteObjectFactory::NewL()
    {
    CSuiteObjectFactory* factory =
        new (ELeave) CSuiteObjectFactory();
    return factory;
    }

// ---------------------------------------------------------
// CSuiteObjectFactory::CSuiteObjectFactory
// ---------------------------------------------------------
//
CSuiteObjectFactory::CSuiteObjectFactory()
    {
    }


// ---------------------------------------------------------
// CSuiteObjectFactory::InitializeObjectL
// ---------------------------------------------------------
//
void CSuiteObjectFactory::InitializeObjectL
( MGECOAttributeProvider& aAttributeProvider )
    {
    if ( iContext )
        {
        //Cast context to GECODefaultObject
        CGECODefaultObject* temp = 
            reinterpret_cast<CGECODefaultObject*>( iContext );

        TInt counter = aAttributeProvider.NumAttributes() - 1;
        TPtrC name;
        TPtrC value;
        TBool islocalized = EFalse;
        while ( counter >= 0 )
            {
            aAttributeProvider.AttributeDetailsL( counter,name,value, islocalized );
            temp->SetAttributeL( name, value, islocalized );
            counter--;            
            }
        }
    }

//  End of File  
