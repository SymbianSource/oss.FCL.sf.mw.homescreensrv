/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of menu item attribute
*
*/

#ifndef MENUSUITE_H_
#define MENUSUITE_H_

#include <e32base.h>

class CSuiteObject;

NONSHARABLE_CLASS( RMenuSrvSuiteArray ) : public RPointerArray<CSuiteObject>
    {

public:

	/**
	 * Find suite by name.
	 * @param aSuiteName suite name.
	 * @return TInt, possition in array if attribute was found, KErrNotFound if not found.
	 */
	TInt FindLastSuite( const TDesC& aSuiteName );

	/**
	 * Find suite by file name.
	 * @param aSuiteName suite name.
	 * @return TInt, possition in array if attribute was found, KErrNotFound if not found.
	 */
	TInt FindByFile( const TDesC& aFileName );

	/*
	 * Chech if Suite exists
	 * */
	TBool Exist( const TDesC& aSuiteName );
 
    /**
    * Remove attributes by suite name.
    * @param aId Id.
    */ 
    void RemoveByFileName( const TDesC& aFileName  );
    
    
    /**
    * Append new item.
    */
    TInt Append(const CSuiteObject* anEntry);

  };

/**
* Push a ResetAndDestroy() on the cleanup stack.
* @param aArray Array.
*/
void CleanupResetAndDestroyPushL( RMenuSrvSuiteArray& aArray );

#endif /* MENUSUITE_H_ */
