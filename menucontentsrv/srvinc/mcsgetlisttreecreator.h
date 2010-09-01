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
* Description:  This class implements MMcsGetListCreatorInterface. It builds tree 
*   output list for getlist operation 
*
*
*/


#ifndef __MCSGETLISTTREECREATOR_H__
#define __MCSGETLISTTREECREATOR_H__

#include <e32base.h>
#include "mcsgetlistcreatorinterface.h"

//forward declarations
class CMenuSrvEng;
class CLiwDefaultList;

/*
 *  GetList tree list creator
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsGetlistTreeCreator ): public	CMcsGetListCreatorInterface
    {
public:
	/**
	*   Factory method.
	*/
    static CMcsGetlistTreeCreator* NewL( CMenuSrvEng& aEng );

    /**
    *   Destructor.
    */
    ~CMcsGetlistTreeCreator();

//from MMcsGetListCreatorInterface
	CLiwDefaultList* CreateLC( RArray<TMenuItem>& aIdsArray,
			CDesC16Array* aRequiredAttributes,
			CDesC16Array* aIgnoredAttributes );

private:
	/**
	*   Constructor.
	*/
	CMcsGetlistTreeCreator( CMenuSrvEng& aEng );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

	/**
	 * Builds output as a tree for GetList operation
	 * @param aIdsArray array containing items to build tree with
	 * @param aList list to store result
	 * @param aIndex index to start with 
	 * @param aParentInt parent id
	 */	
	void BuildTreeListL( RArray<TMenuItem>& aIdsArray, CLiwDefaultList& aList,
			TInt& aIndex, TInt aParentId );
	

	/**
	 * Inserts list with children to aList
	 * @param aList list to store result
	 * @param aChildList list containing children
	 */	
	void InsertChildListL(CLiwDefaultList& aList, 
			CLiwDefaultList* aChildList);
private:
	/*
	 * not own
	 */
	CDesC16Array* iRequiredAttributes;

	/*
	 * not own
	 */
	CDesC16Array* iIgnoredAttributes;
    };
	
#endif // __MCSGETLISTTREECREATOR_H__
