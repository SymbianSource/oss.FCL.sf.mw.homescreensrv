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


#ifndef __MCSDRMSCANNER_H__
#define __MCSDRMSCANNER_H__

#include <e32base.h>
#include "menuengoperation.h"
#include "mcsmenuitem.h"

class CMenuSrvEngUtils;
class CMenuSrvEng;
class TAppItem;
/**
 * Interface for updating Now Playing entry after Publish&Subscribe events.
 *
 * @since S60 v3.2
 */
class MMcsDrmAppScanner
    {
    
public:
        
    virtual void HandleDrmAppEvent() = 0;  
    };


/**
 *  DRM Scanner Active Object.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsDrmScanner ) : 
    public CActive,
    public MMenuEngOperation
    {
    
public:

    /**
     * Creates an instance of CMcsDrmScanner implementation.
     *
     * @since S60 v5.0
     */
    static CMcsDrmScanner* NewL( 
        MMcsDrmAppScanner& aObserver, 
        CMenuSrvEng& aSrvEng,
        CMenuSrvEngUtils& aUtils );

    /**
     * Destructor.
     */
    virtual ~CMcsDrmScanner();

    const RArray<TUid>& DrmExpiredAppArray();
    const RArray<TUid>& DrmProtectedAppArray();
    
    void Scan();
    
    void RemoveObserverAndScanL( TUid aUid );
    
private:
   
   /**
    * Execute engine operation:
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
    /**
     */
    void ScanDrmApplicationsL();
    
    /**
     */
    void GetMcsAppItemsL( RArray<TMenuItem>& aArray );

private:
    /**
     * Constructor.
     *
     * @since S60 v5.0
     */
    CMcsDrmScanner( 
        MMcsDrmAppScanner& aObserver, 
        CMenuSrvEng& aSrvEng,
        CMenuSrvEngUtils& aUtils );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // From CActive.
    
    /**
     * From CActive.
     */
    void DoCancel();
        
    /**
     * From CActive.
     */
    void RunL();      
    
    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    TInt RunError( TInt aError );
    
private: 

    void GetAppItemsL( 
        RArray<TMenuItem>& aItemArray, 
        RArray<TAppItem>& aAppItemArray );
        
    void GetUidsL( 
        RArray<TAppItem>& aAppItemArray, 
        RArray<TUid>& aUidArray );
        
    TBool RemoveNotExistingApps( 
        RArray<TUid>& aArrayToModify, 
        const RArray<TUid>& aUidArray );

    TBool RemoveChangedApps( );
    
    TBool RemoveMissingApps( const RArray<TUid>& aUidArray );
    
    TBool CheckDrmAppsL( const RArray<TAppItem>& aUidArray );
        
private:    

  
        
private:

	
    /**
     * Interface for notifying changes.
     * Not own.
     */
    MMcsDrmAppScanner& iObserver;     
    
    
    CMenuSrvEng& iSrvEng;
    
    CMenuSrvEngUtils& iUtils;
    
    RArray<TUid> iNotProtectedAppArray;
    RArray<TUid> iDrmExpiredAppArray;
    RArray<TUid> iDrmProtectedAppArray;

    RArray<TUid> iChangedAppArray;

    
    TBool iOpStatus;

    };

NONSHARABLE_CLASS( TAppItem )
    {
    public:
    TAppItem( TInt aId, TUid aUid ): iId(aId), iUid(aUid) {};
    
    TInt Id() const
    {
        return iId;
    };
    
    TUid Uid() const
    {
        return iUid;
    };
    
    private:
    TInt iId;
    TUid iUid;
    
    };


#endif // __MCSDRMSCANNER_H__

// End of File
