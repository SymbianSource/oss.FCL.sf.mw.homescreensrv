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
#include <sacls.h>
#include <javaregistry.h>
#include <javaregistrypackageentry.h>
#include <javaregistryapplicationentry.h>
#include <javadomainpskeys.h>
#include "mcsdrmhandler.h"
#include "mcsdrmscanner.h"
#include "mcsmenunotifier.h"
#include "menueng.h"
#include "menusrveng.h"
#include "menusrvengutils.h"
#include "mcscachehandler.h"
#include "mcsdef.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMcsDrmHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CMcsDrmHandler* CMcsDrmHandler::NewL( 
    CMenuSrvEng& aSrvEng,
    CMenuSrvEngUtils& aUtils,
    CMcsCacheHandler& aCacheHandler )
    {
    CMcsDrmHandler* self = new( ELeave ) 
            CMcsDrmHandler( aSrvEng, aUtils, aCacheHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------    
// Destructor
// -----------------------------------------------------------------------------
CMcsDrmHandler::~CMcsDrmHandler()
    {
    delete iInstallNotifier;
    delete iScanner;
    iObserversManager.Close();
    }

// -----------------------------------------------------------------------------
// CMcsDrmHandler::CMcsDrmHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMcsDrmHandler::CMcsDrmHandler( 
    CMenuSrvEng& aSrvEng,
    CMenuSrvEngUtils& aUtils,
    CMcsCacheHandler& aCacheHandler ): 
    iObserversManager( aUtils ), iSrvEng( aSrvEng ),
    iUtils( aUtils ), iCacheHandler( aCacheHandler )
    {
    }

// -----------------------------------------------------------------------------
// CMcsDrmHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMcsDrmHandler::ConstructL()
    {
    iScanner = CMcsDrmScanner::NewL( *this, iSrvEng, iUtils );
	iInstallNotifier = CMcsInstallNotifier::NewL(*this, KPSUidJavaLatestInstallation);

    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//    
TInt CMcsDrmHandler::AppFolderIdL( TUid aUid )
    {
    RArray<TMenuItem> mcsItems; 
    CleanupClosePushL( mcsItems );
    
    TInt root;
    iSrvEng.Engine().RootFolderL( root );
    TMenuSrvTypeAttrFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );   
    
    TBuf<KUidChars> uidString;
    MenuSrvUtil::UidToStringL( aUid.iUid, uidString, 0, EHex );
    
    appFilter.SetAttr( KMenuAttrUid(), uidString );
    iSrvEng.Engine().GetItemsL( mcsItems, root, &appFilter, ETrue );
    
    TInt folderId = KErrNotFound;
    if (mcsItems.Count())
        {
        folderId = mcsItems[0].Parent();
        iCacheHandler.HandleRemoveAttribute(mcsItems[0].Id(), KMenuAttrDrmProtection);
        }
    
    CleanupStack::PopAndDestroy( &mcsItems );
    return folderId;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//     
void CMcsDrmHandler::HandleDrmEvent( TUid aUid )
    {
    TRAP_IGNORE(
    		iSrvEng.Engine().AppendNotifyL( AppFolderIdL( aUid ), 
    				RMenuNotifier::EItemAttributeChanged );
    		iScanner->RemoveObserverAndScanL( aUid );
				);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//     
void CMcsDrmHandler::HandleDrmAppEvent()
    {
    TRAP_IGNORE( AddRemoveObserversL(
        iScanner->DrmProtectedAppArray() ) );     
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//     
void CMcsDrmHandler::AddRemoveObserversL( 
	const RArray<TUid>& aDrmProtectedArray )
    {
    iObserversManager.RemoveMissingObservers( aDrmProtectedArray );    
    iObserversManager.CreateObserversL( aDrmProtectedArray, *this );
    }    

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//        
void CMcsDrmHandler::EngineEvents( TInt /*aFolder*/, TInt aEvents )
    {
    if ( aEvents & RMenuNotifier::EItemsAddedRemoved )
        {
        iScanner->Scan();
        }    
    }

// ---------------------------------------------------------
// CMcsDrmHandler::HandleInstallNotifyL
// ---------------------------------------------------------
//
void CMcsDrmHandler::HandleInstallNotifyL( TInt aPackageUid )
	{
	RArray<TUid> uids;
	CleanupClosePushL( uids );

	Java::CJavaRegistry* javaRegistry;
	javaRegistry = Java::CJavaRegistry::NewLC();

	Java::CJavaRegistryEntry* regEntry = 
		javaRegistry->RegistryEntryL( TUid::Uid(aPackageUid) );

	if( regEntry )
		{
	CleanupStack::PushL( regEntry );

	Java::TJavaRegistryEntryType entryType = regEntry->Type();
	
	if ( ( entryType >= Java::EGeneralPackage ) && 
			(entryType < Java::EGeneralApplication) )
		{
		//package entry
		Java::CJavaRegistryPackageEntry* regPackageEntry = 
					static_cast<Java::CJavaRegistryPackageEntry*>( regEntry );
			regPackageEntry->GetEmbeddedEntries( uids );
			}
		else 
			{
			//application entry
			uids.AppendL( regEntry->Uid() );
			}
		
		CleanupStack::PopAndDestroy( regEntry );
	    for ( TInt i = 0; i< uids.Count(); i++ )
	        {
	        if ( ( iScanner->DrmExpiredAppArray()).Find(uids[i]) != KErrNotFound )
	            {
	            HandleDrmEvent( uids[i] );
	            }
	        else
	            {
	            iObserversManager.RefreshObserverL( iScanner->DrmProtectedAppArray(), 
	                    uids[i], *this );
	            }
	        }
		}
	else
	    {
	    for ( TInt i = 0; i< iScanner->DrmProtectedAppArray().Count(); i++ )
	        {
            iObserversManager.RefreshObserverL( iScanner->DrmProtectedAppArray(), 
        		   iScanner->DrmProtectedAppArray()[i], *this );
	        }
	    for ( TInt i = 0; i< iScanner->DrmExpiredAppArray().Count(); i++ )
	        {
	        HandleDrmEvent( iScanner->DrmExpiredAppArray()[i] );
	        }	    
	    }
	
	
	CleanupStack::PopAndDestroy( javaRegistry );
	CleanupStack::PopAndDestroy( &uids );
    }


//  End of File  
