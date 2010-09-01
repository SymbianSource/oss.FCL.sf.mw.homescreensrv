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
// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "menusatinterfacetest.h"

// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CMenuSatInterfaceTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMenuSatInterfaceTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {          		
	  	ENTRY( "Test_SatUiVisibleL",              CMenuSatInterfaceTest::Test_SatUiVisibleL),	 
	  	ENTRY( "Test_SatUiVisibleIconIdL",        CMenuSatInterfaceTest::Test_SatUiVisibleIconIdL),
	  	ENTRY( "Test_SatInterfaceConstructL",     CMenuSatInterfaceTest::Test_SatInterfaceConstructL),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

//  End of File
