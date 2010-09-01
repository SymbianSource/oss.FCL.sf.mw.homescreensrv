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
* Description:  This class implements MMcsGetListCreatorInterface. It builds flat 
*   output list for getlist operation 
*
*
*/


#ifndef __MCSGETLISTFLATCREATOR_H__
#define __MCSGETLISTFLATCREATOR_H__

#include <e32base.h>
#include "mcsgetlistcreatorinterface.h"

//forward declarations
class CMenuSrvEng;

/*
 *  GetList flat list creator
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsGetlistFlatCreator ): public CMcsGetListCreatorInterface
    {
public:
	/**
	*   Factory method.
	*/
    static CMcsGetlistFlatCreator* NewL( CMenuSrvEng& aEng );

    /**
    *   Destructor.
    */
    ~CMcsGetlistFlatCreator();

//from MMcsGetListCreatorInterface
	CLiwDefaultList* CreateLC( RArray<TMenuItem>& aIdsArray,
			CDesC16Array* aRequiredAttributes,
			CDesC16Array* aIgnoredAttributes );

private:
	/**
	*   Constructor.
	*/
	CMcsGetlistFlatCreator( CMenuSrvEng& aEng );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();
    };
	
#endif // __MCSGETLISTFLATCREATOR_H__
