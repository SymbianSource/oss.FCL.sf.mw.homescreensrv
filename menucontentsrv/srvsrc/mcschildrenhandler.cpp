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

#include <e32base.h>

#include "mcschildrenhandler.h"

#include "mcsdef.h"
#include "mcsmenunotifier.h"
#include "menueng.h"
#include "mcscachehandler.h"
#include "menusrveng.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMcsChildrenHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CMcsChildrenHandler* CMcsChildrenHandler::NewL( 
    CMenuSrvEng& aSrvEng,
    CMcsCacheHandler& aCacheHandler )
	{
	CMcsChildrenHandler* self = new( ELeave ) 
				CMcsChildrenHandler( aSrvEng, aCacheHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
//
CMcsChildrenHandler::~CMcsChildrenHandler()
	{
	delete iScanner;
	}

// -----------------------------------------------------------------------------
// CMcsChildrenHandler::CMcsChildrenHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMcsChildrenHandler::CMcsChildrenHandler( 
    CMenuSrvEng& aSrvEng,
    CMcsCacheHandler& aCacheHandler  ) :
    iSrvEng( aSrvEng ),
    iCacheHandler( aCacheHandler )
	{

	}

// -----------------------------------------------------------------------------
// CMcsDrmHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CMcsChildrenHandler::ConstructL()
	{
	iScanner = CMcsChildrenScanner::NewL(*this, iSrvEng);
	}

// -----------------------------------------------------------------------------    
// 
// -----------------------------------------------------------------------------
//
void CMcsChildrenHandler::HandleChildrenEvent(TInt aId, TUint aOldCount, TUint aNewCount)
	{
	if (aOldCount != aNewCount)
		{
		iCacheHandler.HandleRemoveAttribute(aId, KChildrenCount);

		// Send Notification
		if (aOldCount == 0 || aNewCount == 0)
			{
			TRAP_IGNORE( iSrvEng.Engine().ModifiableObjectL( 
					aId, RMenuNotifier::EItemAttributeChanged ));
			}
		}
	}


// -----------------------------------------------------------------------------    
// 
// -----------------------------------------------------------------------------
//
void CMcsChildrenHandler::EngineEvents( TInt aFolder, TInt aEvents )
	{
    if ( aEvents & ( RMenuNotifier::EItemsAddedRemoved 
                   | RMenuNotifier::EItemAttributeChanged ) )
        {
        iScanner->Scan(aFolder);
        }    
	}

//  End of File  

