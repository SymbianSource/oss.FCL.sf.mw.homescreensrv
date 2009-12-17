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


#ifndef __SUITEOBJECTFACTORY_H__
#define __SUITEOBJECTFACTORY_H__

#include <e32base.h>
// FORWARD DECLARATION

class CGECODefaultObjectFactory;
class MGECOAttributeProvider;

/**
* Menu Engine Object factory.
*/
NONSHARABLE_CLASS( CSuiteObjectFactory ): public CGECODefaultObjectFactory
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CSuiteObjectFactory();

    /**
    * Two-phased constructor.
    * @return The created object.
    */
    static CSuiteObjectFactory* NewL( );

protected:  // construction

    /**
    * Constructor.
    */
    CSuiteObjectFactory();

public:    // from CGECOObjectFactoryBase

    /**
    * Initializes the current object with attribute provider data.
    * @param aAttributeProvider Attribute provider for data initialization.
    */
    void InitializeObjectL(
        MGECOAttributeProvider& aAttributeProvider );

    };

#endif // __SUITEOBJECTFACTORY_H__
