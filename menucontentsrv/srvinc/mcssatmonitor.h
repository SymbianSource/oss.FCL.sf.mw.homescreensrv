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


#ifndef __MCSSATMONITOR_H__
#define __MCSSATMONITOR_H__

#include "mcssatnotifier.h"
#include "menuengoperation.h"

//Forward declatations
class MMenuEngObserver;
class CMenuEng;
class CMcsSatNotifier;

/**
 *  SAT Handler.
 *  @lib mcssathandler.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsSatMonitor ): public CBase, 
    public MMcsSATNotifierCallback, MMenuEngOperation
    {
public:
    
    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed object.
    */
    IMPORT_C static CMcsSatMonitor* NewL( CMenuEng& aEng );    
    
    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    virtual ~CMcsSatMonitor();
    
private:

    /**
    * Constructor.
    */
    CMcsSatMonitor( CMenuEng& aEng );    

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation:
    * Do the application scan and update menu accordingly.
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
    
    /**
    * Inherited from MNotifierMMcsSATNotifierCallbackCallback
    * 
    * @since Series 60 3.1
    * @return void
    */    
    void SATChangeL();   
    
    TInt GetIdL( TUid aAppUid );
    
private:    // data
    
    CMenuEng& iEng; ///< Engine.

    CMcsSatNotifier* iMcsSatNotifier1;
    
    CMcsSatNotifier* iMcsSatNotifier2;
    TBool iOpStatus;
    };

#endif // __MCSSATHANDLER_H__
