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


#ifndef __MCSDRMHANDLER_H__
#define __MCSDRMHANDLER_H__

#include "mcsinstallnotifier.h"

#include "mcsdrmobserver.h"
#include "mcsdrmscanner.h"
#include "mcsdrmobservermanager.h"

class CMcsCacheHandler;
class MMcsInstallListener;
/**
 *  DRM Handler.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsDrmHandler ): 
    public CBase,
    public MMcsInstallListener,
    public MMcsDrmObserver,
    public MMcsDrmAppScanner
    {
public:
    
    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed object.
    */
    
    static CMcsDrmHandler* NewL( 
        CMenuSrvEng& aSrvEng,
        CMenuSrvEngUtils& aUtils,
        CMcsCacheHandler& aCacheHandler );
    
    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    virtual ~CMcsDrmHandler();
    
    void HandleDrmEvent( TUid aUid );
    
    void HandleDrmAppEvent();
    
    void EngineEvents( TInt aFolder, TInt aEvents );

    /**
     * Handle Install Event
     */
    void HandleInstallNotifyL(TInt aPackageUid);    
    
private:

    /**
    * Constructor.
    */
    CMcsDrmHandler( 
        CMenuSrvEng& aSrvEng,
        CMenuSrvEngUtils& aUtils,
        CMcsCacheHandler& aCacheHandler  );

    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    
    void AddRemoveObserversL( 
    		const RArray<TUid>& aDrmProtectedArray );
    
    TInt AppFolderIdL( TUid aUid );
    
private:    // data
    
    RMcsDrmObserverManager iObserversManager;
    
    
    CMenuSrvEng& iSrvEng; ///< Menu Engine. Not own.
    
    CMenuSrvEngUtils& iUtils;
    
    CMcsCacheHandler& iCacheHandler;
    
    CMcsDrmScanner* iScanner;
    
    CMcsInstallNotifier* iInstallNotifier;
    
    };

#endif // __MCSDRMHANDLER_H__
