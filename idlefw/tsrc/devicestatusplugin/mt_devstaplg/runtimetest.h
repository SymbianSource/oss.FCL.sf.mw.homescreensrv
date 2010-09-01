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
* Description:
*
*/


#ifndef C_RUNTIMETEST_H
#define C_RUNTIMETEST_H

#include <digia/eunit/meunittestcase.h>
#include <e32base.h>

class CRuntimeTest : public CBase, public MEUnitTestCase
    {
protected:

        CRuntimeTest();
        virtual ~CRuntimeTest();

        void BaseConstructL( const TDesC& aName );
        virtual const TDesC& Name() const;
        HBufC* CreateTestNameL( const TDesC& aName ) const;

        virtual void SetupL() = 0;
        virtual void RunTestL() = 0;
        virtual void Teardown() = 0;

private:

	    HBufC* iName;
    };


#endif      //  C_RUNTIMETEST_H

// End of file
