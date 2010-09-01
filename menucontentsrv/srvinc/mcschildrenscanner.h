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
#ifndef MCSCHILDRENSCANNER_H_
#define MCSCHILDRENSCANNER_H_


#include <e32base.h>
#include "menuengoperation.h"
#include "mcsmenuitem.h"

class CMenuSrvEng;

/**
 * Interface for updating Now Playing entry after Publish&Subscribe events.
 *
 * @since S60 v3.2
 */
class MMcsChildrenScanner
    {
    
public:
    virtual void HandleChildrenEvent(TInt aId, TUint oldCount, TUint newCount) = 0;
    };


/**
 *  Children Scanner Active Object.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsChildrenScanner ) : 
    public CActive,
    public MMenuEngOperation
    {
    
public:

    /**
     * Creates an instance of CMcsChildrenScanner implementation.
     *
     * @since S60 v5.0
     */
    static CMcsChildrenScanner* NewL( 
    	MMcsChildrenScanner& aObserver, 
        CMenuSrvEng& aSrvEng);

    /**
     * Destructor.
     */
    virtual ~CMcsChildrenScanner();
    
    void Scan(TInt aId);
    
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
    
    void ScanFolderL();
    

private:
    /**
     * Constructor.
     *
     * @since S60 v5.0
     */
	CMcsChildrenScanner ( 
		MMcsChildrenScanner& aObserver, 
        CMenuSrvEng& aSrvEng);

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
    
    void AddId(TInt aId);
    
    void RemoveId(TInt aId);
        
private:

	
    /**
     * Interface for notifying changes.
     * Not own.
     */
	MMcsChildrenScanner& iObserver;     
    
	/**
	 * Menu engine.
	 * Not own.
	 */
    CMenuSrvEng& iSrvEng;
    
    TBool iOpStatus;
    
    RArray<TInt> iIdBuff;

    };

#endif /*MCSCHILDRENSCANNER_H_*/
