/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used for receive SIM Application name, icon or 
*                visibility information.
*
*/



// INCLUDE FILES

#include <e32property.h>

#include "mcsdef.h"
#include "menueng.h"
#include "menusrvengutils.h"
#include "mcsmenunotifier.h"
#include "mcssatmonitor.h"
#include "mcsmenuitem.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMcsSatHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMcsSatMonitor* CMcsSatMonitor::NewL( CMenuEng& aEng )
    {
    CMcsSatMonitor* self = new( ELeave ) CMcsSatMonitor( aEng );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
CMcsSatMonitor::~CMcsSatMonitor()
    {
    iEng.DequeueOperation( *this );
    delete iMcsSatNotifier2;
    delete iMcsSatNotifier1;
    }

// -----------------------------------------------------------------------------
// CMcsSatHandler::CMcsSatHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMcsSatMonitor::CMcsSatMonitor( CMenuEng& aEng ):
    iEng(aEng)
    {
    iOpStatus = EFalse;
    }

// -----------------------------------------------------------------------------
// CMcsSatHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMcsSatMonitor::ConstructL()
    {
    iMcsSatNotifier1 = CMcsSatNotifier::NewL( this, KCRUidMenu, 
                                             KMenuSatUIName );                                      
    iMcsSatNotifier2 = CMcsSatNotifier::NewL( this, KCRUidMenu, 
                                             KMenuSatUIIconId  );   
    }

// ---------------------------------------------------------
// CMcsSatMonitor::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsSatMonitor::RunMenuEngOperationL()
    {
    TInt root;
    iEng.RootFolderL( root ); 
    TInt parentId(0); 
    TInt id = GetIdL( KSatUid );
    if (id != KErrNotFound)
        {
        do
            {
            iEng.ParentFolderL( id, parentId );
            id = parentId;
            iEng.AppendNotifyL( id, 
                        RMenuNotifier::EItemAttributeChanged );
            }
        while ( id != root );
        }
    iOpStatus = EFalse;
    } 

// ---------------------------------------------------------
// CMcsSatMonitor::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsSatMonitor::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }

// -----------------------------------------------------------------------------
// CMcsSatHandler::SATChangeL
// -----------------------------------------------------------------------------
//
void CMcsSatMonitor::SATChangeL()
    {
    
    // AppArc app scan complete, we have the app list.
    // Now queue this operation to be executed by the Engine.
    if ( !iOpStatus ) 
    	{
    	iOpStatus = ETrue;
    	iEng.QueueOperationL( *this );
    	}
    }

// ---------------------------------------------------------
// CMcsSatHandler::GetParentIdL
// ---------------------------------------------------------
//
TInt CMcsSatMonitor::GetIdL( TUid aAppUid )
    {
    TInt result(KErrNotFound);
    RArray<TMenuItem> array;
    CleanupClosePushL(array);
    TInt root;
    iEng.RootFolderL( root );
    TMenuSrvTypeAttrFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );
    TBuf<KUidChars> uidString;
    TInt appuid( aAppUid.iUid );
    MenuSrvUtil::UidToStringL( appuid, uidString, EFalse, EHex );
    appFilter.SetAttr( KMenuAttrUid(), uidString );
    iEng.GetItemsL( array, root, &appFilter, ETrue );
    if (array.Count())
        {
        if( array[0].Flags() != TMenuItem::EHidden )
            {
            result = array[0].Id();
            }
        }
    CleanupStack::PopAndDestroy(&array);
    return result;
    }   

//  End of File  
