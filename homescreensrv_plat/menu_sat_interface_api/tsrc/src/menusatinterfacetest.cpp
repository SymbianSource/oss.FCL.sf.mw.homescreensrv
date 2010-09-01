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
#include <Stiftestinterface.h>
#include <MenuSatInterface.h>
#include "menusatinterfacetest.h"

//MCS
#include <mcsdef.h>

//  CONSTANTS
_LIT( KSatName, "Plus Mega" );

// ============================ MEMBER FUNCTIONS ===============================


TInt CMenuSatInterfaceTest::Test_SatUiVisibleL(CStifItemParser& )
    {
    TInt result (KErrGeneral);
    
    TBool visible = ETrue;
    CMenuSATInterface* interface = new(ELeave) CMenuSATInterface();
    CleanupStack::PushL( interface );    
    interface->MakeSatUiVisible( visible, KSatName );
    CleanupStack::PopAndDestroy( interface );
    
    TBuf<256> name; 
    TInt error = RProperty::Get( KCRUidMenu, KMenuSatUIName, name );
    
  	if( name == KSatName() 
  			&&  error == KErrNone )
  		{
  		result = KErrNone;
  		}
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMenuSatInterfaceTest::Test_SatUiVisibleIconIdL(CStifItemParser& )
    {
    TInt result (KErrGeneral);
    
    TBool visible = ETrue;
    TUint8 iconInfoId = 21;
    CMenuSATInterface* interface = new(ELeave) CMenuSATInterface();
    CleanupStack::PushL( interface );    
    interface->MakeSatUiVisible( visible, KSatName, iconInfoId );
    CleanupStack::PopAndDestroy( interface );
    
    TInt iconInfoIdTest = -1;
    RProperty::Get( KCRUidMenu, KMenuSatUIIconId, iconInfoIdTest );
    
  	if( iconInfoIdTest == iconInfoId )
  		{
  		result = KErrNone;
  		}
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMenuSatInterfaceTest::Test_SatInterfaceConstructL(CStifItemParser& )
    {
    TInt result (KErrGeneral);
    
    CMenuSATInterface* interface = new(ELeave) CMenuSATInterface();
  	if( interface )
  		{
  		result = KErrNone;
  		}
    return result;
    }


//  End of File
