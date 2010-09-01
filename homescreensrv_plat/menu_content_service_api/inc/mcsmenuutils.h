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


#include <e32base.h>

NONSHARABLE_CLASS( MenuUtils )
    {
    public:
    /**
    * Gets TUint from a aStr - HEX or Decimal
    *
    * @return error code
    */
    IMPORT_C static TInt GetTUint( const TDesC& aStr, TUint& aInt );   
    
    /**
    * Sets TUint in a aStr - HEX
    *    
    */
    IMPORT_C static void SetTUint( TDes& aStr, TUint aInt );     
    };