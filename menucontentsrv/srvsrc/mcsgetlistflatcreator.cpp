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

#include <e32cmn.h> 
#include <liwcommon.h>
#include <liwgenericparam.h>
#include <s32mem.h>
#include "mcsgetlistflatcreator.h"

// ---------------------------------------------------------
// CMcsGetlistFlatCreator::NewL
// ---------------------------------------------------------
//
CMcsGetlistFlatCreator* CMcsGetlistFlatCreator::NewL( CMenuSrvEng& aEng )
	{
	CMcsGetlistFlatCreator* self;
	self = new ( ELeave ) CMcsGetlistFlatCreator( aEng );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsGetlistFlatCreator::~CMcsGetlistFlatCreator
// ---------------------------------------------------------
//
CMcsGetlistFlatCreator::~CMcsGetlistFlatCreator()
    {
    }

// ---------------------------------------------------------
// CMcsGetlistFlatCreator::CMcsGetlistFlatCreator
// ---------------------------------------------------------
//
CMcsGetlistFlatCreator::CMcsGetlistFlatCreator( CMenuSrvEng& aEng ):
	CMcsGetListCreatorInterface(aEng)
    {
    }

// ---------------------------------------------------------
// CMcsGetlistFlatCreator::ConstructL
// ---------------------------------------------------------
//
void CMcsGetlistFlatCreator::ConstructL()
    {
    CMcsGetListCreatorInterface::ConstructL();
    }

// ---------------------------------------------------------
// CMcsGetlistFlatCreator::CreateLC
// ---------------------------------------------------------
//
CLiwDefaultList* CMcsGetlistFlatCreator::CreateLC(
	    RArray<TMenuItem>& aIdsArray,
	    CDesC16Array* aRequiredAttributes,
	    CDesC16Array* aIgnoredAttributes)
	{
	CLiwDefaultList* list = CLiwDefaultList::NewLC();

	TInt arrCount = aIdsArray.Count();
	for( TInt index = 0; index < arrCount; ++index )
		{
		CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
		// Build Map
		BuildMapL( aIdsArray[index], aRequiredAttributes,
				aIgnoredAttributes, map );


		TLiwVariant lvMap( map );
		lvMap.PushL();
		list->AppendL( lvMap );
		CleanupStack::PopAndDestroy( &lvMap );
		CleanupStack::PopAndDestroy( map );

		}
	return list;
	}





//  End of File
