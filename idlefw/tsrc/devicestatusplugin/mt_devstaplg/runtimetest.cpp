/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "runtimetest.h"


CRuntimeTest::CRuntimeTest()
	{
	}
CRuntimeTest::~CRuntimeTest()
	{
	delete iName;
	}

void CRuntimeTest::BaseConstructL( const TDesC& aName )
	{
	iName = CreateTestNameL( aName );
	}

const TDesC& CRuntimeTest::Name() const
	{
	return *iName;
	}
HBufC* CRuntimeTest::CreateTestNameL( const TDesC& aName ) const
	{
	HBufC* name = HBufC::NewL( aName.Length() + 200 ); //200 characters for rest of data
	TPtr ptr( name->Des() );
	ptr.Copy( aName );

	ptr.Append( _L("\t") );
	ptr.Append( _L("") );   //class name

	ptr.Append( _L("\t") );
	ptr.Append( _L("") );   //tested method

	ptr.Append( _L("\t") );
	ptr.Append( _L("FUNCTIONALITY") );   //test type


	return name;
	}
//  END OF FILE
