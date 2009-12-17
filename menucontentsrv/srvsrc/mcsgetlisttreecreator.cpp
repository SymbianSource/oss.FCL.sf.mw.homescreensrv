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
#include "mcsgetlisttreecreator.h"

// ---------------------------------------------------------
// CMcsGetlistTreeCreator::NewL
// ---------------------------------------------------------
//
CMcsGetlistTreeCreator* CMcsGetlistTreeCreator::NewL( CMenuSrvEng& aEng )
	{
	CMcsGetlistTreeCreator* self;
	self = new ( ELeave ) CMcsGetlistTreeCreator( aEng );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsGetlistTreeCreator::~CMcsGetlistTreeCreator
// ---------------------------------------------------------
//
CMcsGetlistTreeCreator::~CMcsGetlistTreeCreator()
    {
    }

// ---------------------------------------------------------
// CMcsGetlistTreeCreator::CMcsGetlistTreeCreator
// ---------------------------------------------------------
//
CMcsGetlistTreeCreator::CMcsGetlistTreeCreator( CMenuSrvEng& aEng ):
	CMcsGetListCreatorInterface(aEng)
    {
    }

// ---------------------------------------------------------
// CMcsGetlistTreeCreator::ConstructL
// ---------------------------------------------------------
//
void CMcsGetlistTreeCreator::ConstructL()
    {
    CMcsGetListCreatorInterface::ConstructL();
    }

// ---------------------------------------------------------
// CMcsGetlistTreeCreator::CreateLC
// ---------------------------------------------------------
//
CLiwDefaultList* CMcsGetlistTreeCreator::CreateLC(
	RArray<TMenuItem>& aIdsArray,
	CDesC16Array* aRequiredAttributes,
	CDesC16Array* aIgnoredAttributes )
	{
	CLiwDefaultList* list = CLiwDefaultList::NewLC();
	iRequiredAttributes = aRequiredAttributes;
	iIgnoredAttributes = aIgnoredAttributes;
	TInt index = 0;
	if ( aIdsArray.Count() )
		{
		BuildTreeListL( aIdsArray, *list, index, aIdsArray[0].Parent() );
		}

	return list;
	}

// ---------------------------------------------------------------------------
// Build List for Output paremeter in a tree format
// ---------------------------------------------------------------------------
//
void CMcsGetlistTreeCreator::BuildTreeListL( RArray<TMenuItem>& aIdsArray,
		CLiwDefaultList& aList,	TInt& aIndex, TInt aParentId )
	{
	TBool buildlist = ETrue ;
	TInt  prevId = 0;
	TInt  arrCount = aIdsArray.Count();

	for( TInt index = aIndex; index < arrCount &&  buildlist; ++index )
		{
		CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

		if( aParentId == aIdsArray[index].Parent() )
			{
			buildlist = ETrue;
			// Build Map
			BuildMapL( aIdsArray[index], iRequiredAttributes,
						iIgnoredAttributes, map );
			aList.AppendL(TLiwVariant(map));

			}
		else
			{
			if( prevId ==  aIdsArray[index].Parent() )
				{
				// Recursive Child List
				CLiwDefaultList *childlist = CLiwDefaultList::NewL();
				CleanupStack::PushL( childlist );
				// Build Child List
				BuildTreeListL( aIdsArray, *childlist, index, aIdsArray[index].Parent() );
				InsertChildListL(aList, childlist);
				CleanupStack::Pop( childlist );
				childlist->DecRef();
				}
			else
				{
				// Return from recursive list
				buildlist = EFalse;
				aIndex = index-1;
				}
			}


		prevId = aIdsArray[index].Id();

		CleanupStack::PopAndDestroy( map );
		}
	}

// ---------------------------------------------------------------------------
// Insert Children list to last item in tree list
// ---------------------------------------------------------------------------
//
void CMcsGetlistTreeCreator::InsertChildListL(CLiwDefaultList& aList,
		CLiwDefaultList* aChildList)
	{
	TInt index = aList.Count() - 1;

	// Get last item from aList
	TLiwVariant livVar;
	livVar.PushL();
	aList.AtL(index, livVar);
	CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
	livVar.Get(*map);

	// Insert children list to that item
	map->InsertL(KChildren, TLiwVariant(aChildList));

	// Remove last item form a list and add a new one
	aList.Remove(index);
	aList.AppendL(TLiwVariant(map) );

	CleanupStack::Pop(map);
	map->DecRef();
	CleanupStack::PopAndDestroy(&livVar);
	}
//  End of File
