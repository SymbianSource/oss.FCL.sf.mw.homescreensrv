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
* Description:  The API supports attributes not present in MCS from SAT Api
*
*/


#include "mcsdrmscanner.h"
#include "menueng.h"
#include "menusrveng.h"
#include "menuengobject.h"
#include "menusrvengutils.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CMcsDrmScanner implementation
// ---------------------------------------------------------------------------
//
CMcsDrmScanner* CMcsDrmScanner::NewL( 
    MMcsDrmAppScanner& aObserver, 
    CMenuSrvEng& aSrvEng,
    CMenuSrvEngUtils& aUtils )
    {
    CMcsDrmScanner* self = new (ELeave) CMcsDrmScanner( 
        aObserver, aSrvEng, aUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMcsDrmScanner::~CMcsDrmScanner()
    {
    Cancel();
    iSrvEng.Engine().DequeueOperation( *this );
    iNotProtectedAppArray.Close();
    iDrmExpiredAppArray.Close();
    iDrmProtectedAppArray.Close();
    iChangedAppArray.Close();
    }
    
// -----------------------------------------------------------------------------
// CMcsDrmScanner::CMcsDrmScanner
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMcsDrmScanner::CMcsDrmScanner( 
    MMcsDrmAppScanner& aObserver, 
    CMenuSrvEng& aSrvEng,
    CMenuSrvEngUtils& aUtils ): 
    CActive( EPriorityNormal ), iObserver( aObserver ), 
    iSrvEng( aSrvEng ), iUtils( aUtils )
    {    
    CActiveScheduler::Add( this );
    iOpStatus = EFalse;
    }

// -----------------------------------------------------------------------------
// CMcsDrmScanner::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMcsDrmScanner::ConstructL()
    {
    Scan();
    }

// ---------------------------------------------------------------------------
// CMcsDrmScanner::DoCancel
// ---------------------------------------------------------------------------
//
void CMcsDrmScanner::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CMcsDrmScanner::RunL
// ---------------------------------------------------------------------------
//
void CMcsDrmScanner::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    iSrvEng.Engine().QueueOperationL( *this );    
    }    

// ---------------------------------------------------------
// CMcsDrmScanner::RunError()
// ---------------------------------------------------------
//
TInt CMcsDrmScanner::RunError( TInt /*aError*/ )
    {
    // Ignore the error (what else could we do?).
    return KErrNone;
    }

// ---------------------------------------------------------
// CMcsDrmScanner::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsDrmScanner::RunMenuEngOperationL()
    {
    ScanDrmApplicationsL();
    iOpStatus = EFalse;
    }  
    
// ---------------------------------------------------------
// CMcsDrmScanner::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsDrmScanner::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    }          

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMcsDrmScanner::GetAppItemsL( 
    RArray<TMenuItem>& aItemArray, 
    RArray<TAppItem>& aAppItemArray )
    {
    //for every item in apaAndCrItems array
    CMenuEng& eng = iSrvEng.Engine();
    for ( TInt i = 0; i<aItemArray.Count(); i++ )
        {        
        TUid appUid;
        if ( KErrNone == 
            CMenuSrvEngUtils::GetAppUid( 
            	eng.ObjectL(aItemArray[i].Id()), appUid) )
            {
            TAppItem appItem( aItemArray[i].Id(), appUid );
            aAppItemArray.AppendL(appItem);        
            }        
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CMcsDrmScanner::GetUidsL( 
    RArray<TAppItem>& aAppItemArray, 
    RArray<TUid>& aUidArray )
    {
    for ( TInt i = 0; i<aAppItemArray.Count(); i++ )
        {  
        aUidArray.AppendL( aAppItemArray[i].Uid() );               
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CMcsDrmScanner::RemoveNotExistingApps( 
    RArray<TUid>& aArrayToModify, 
    const RArray<TUid>& aUidArray )
    {  
    TInt index = 0;
    TBool changed = EFalse;
    while ( index < aArrayToModify.Count() )
        {
        if ( aUidArray.Find( aArrayToModify[index] ) == KErrNotFound )
            {
            aArrayToModify.Remove( index );
            changed = ETrue;
            }
        else
            {
            index++;
            }       
        }
    return changed;
    }        


// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CMcsDrmScanner::RemoveChangedApps( )
    {   
    TBool changed = EFalse;
    for( TInt i = 0; i< iChangedAppArray.Count(); i++ )
    	{
    	TInt index = iDrmExpiredAppArray.Find( iChangedAppArray[i] );
        if ( index != KErrNotFound )
               {
               iDrmExpiredAppArray.Remove( index );
               changed = ETrue;
               }
    	index = iDrmProtectedAppArray.Find( iChangedAppArray[i] );
        if ( index != KErrNotFound )
               {
               iDrmProtectedAppArray.Remove( index );
               changed = ETrue;
               }   	
    	}
    iChangedAppArray.Reset();
    return changed;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CMcsDrmScanner::RemoveMissingApps( const RArray<TUid>& aUidArray )
    {    
    RemoveNotExistingApps( iNotProtectedAppArray, aUidArray );    
    
    TBool expiredChanged = 
        RemoveNotExistingApps( iDrmExpiredAppArray, aUidArray );    
        
    TBool protectedChanged = 
        RemoveNotExistingApps( iDrmProtectedAppArray, aUidArray );    

    TInt changed = EFalse;
    if ( expiredChanged || protectedChanged )
        {
        changed = ETrue;
        }
    return changed;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CMcsDrmScanner::CheckDrmAppsL( const RArray<TAppItem>& aUidArray )
    {    
    TInt changed = EFalse;
    
    for ( TInt i = 0; i<aUidArray.Count(); i++ )
        {                        
        if ( iNotProtectedAppArray.Find( aUidArray[i].Uid() ) == KErrNotFound )
            {
            if ( iDrmProtectedAppArray.Find( aUidArray[i].Uid() ) == KErrNotFound &&
                iDrmExpiredAppArray.Find( aUidArray[i].Uid() ) == KErrNotFound )
                {
		        TBool attrExists;
		        RBuf attrVal;
        	    attrVal.CleanupClosePushL();
        	    attrVal.CreateL(KMenuMaxAttrValueLen);
        	    
                iSrvEng.GetAttributeL( 
                    aUidArray[i].Id(), 
                    KMenuAttrDrmProtection,
                    attrExists,
                    attrVal );

                if ( attrVal == KMenuDrmProtected )
                    {                       
                    iDrmProtectedAppArray.AppendL( aUidArray[i].Uid() );
                    changed = ETrue;
                    }
                else if ( attrVal == KMenuDrmRightsExpired )
                    {       
                    iDrmExpiredAppArray.AppendL( aUidArray[i].Uid() );
                    changed = ETrue;    
                    }
                else
                    {
                    iNotProtectedAppArray.AppendL( aUidArray[i].Uid() );        
                    }     
                CleanupStack::PopAndDestroy( &attrVal );                   
                }                                    
            }
        }
    
    return changed;
    }
    
// ---------------------------------------------------------
// CMcsDrmScanner::GetMcsItemsL
// ---------------------------------------------------------
//
void CMcsDrmScanner::ScanDrmApplicationsL()
    {    
    RArray<TMenuItem> mcsItems; 
    CleanupClosePushL( mcsItems );
    GetMcsAppItemsL( mcsItems );
          
    RArray<TAppItem> appArray;
    CleanupClosePushL( appArray );
    
    GetAppItemsL( mcsItems, appArray );
    
    RArray<TUid> uidArray;
    CleanupClosePushL( uidArray );
    
    GetUidsL( appArray, uidArray );
    
    TBool removeChanged = RemoveChangedApps( );     
    
    TBool removeMissing = RemoveMissingApps( uidArray );     
    
    TBool drmChanged = CheckDrmAppsL( appArray );
    
    CleanupStack::PopAndDestroy( &uidArray );        
    CleanupStack::PopAndDestroy( &appArray );
    CleanupStack::PopAndDestroy( &mcsItems );
    
    if ( removeChanged || removeMissing || drmChanged )
        {
        iObserver.HandleDrmAppEvent();    
        }        
    iOpStatus = EFalse;
    }

// ---------------------------------------------------------
// CMcsDrmScanner::GetMcsItemsL
// ---------------------------------------------------------
//
void CMcsDrmScanner::GetMcsAppItemsL( RArray<TMenuItem>& aArray )
    {
    TInt root;
    iSrvEng.Engine().RootFolderL( root );
    TMenuSrvTypeFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );
    iSrvEng.Engine().GetItemsL( aArray, root, &appFilter, ETrue );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const RArray<TUid>& CMcsDrmScanner::DrmExpiredAppArray()
    {
    return iDrmExpiredAppArray;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const RArray<TUid>& CMcsDrmScanner::DrmProtectedAppArray()
    {
    return iDrmProtectedAppArray;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMcsDrmScanner::Scan()
    {
    if ( !IsActive() && !iOpStatus )
        {
        iOpStatus = ETrue;
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );
        } 
    }  

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMcsDrmScanner::RemoveObserverAndScanL( TUid aUid )
    {
    iChangedAppArray.AppendL( aUid );
    Scan( );
    }
    
// End of File
