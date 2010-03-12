/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle Content Observer optimizer
*
*/


#ifndef C_AIMULTICONTENTOBSERVEROPTIMIZER_H
#define C_AIMULTICONTENTOBSERVEROPTIMIZER_H

// System includes
#include <e32base.h>

// User includes
#include "aicontentobserver.h"

// Forward declarations
class CHsContentPublisher;

/**
 *  @ingroup group_devicestatusplugin
 *
 * Active Idle Content Observer optimizer. 
 * 
 * Optimizes the Commit calls to different UI controllers. In 
 * case nothing is published nothing is committed.
 *
 * @see MAiContentObserver
 * @since S60 3.2
 */
NONSHARABLE_CLASS( CAiContentObserverOptimizer ) : public CBase      
    {
private:
    struct TAiPublishBlackList
        {
        TInt iContentId;
        
        TInt iIndex;              
        };

public:
    static CAiContentObserverOptimizer* NewL(MAiContentObserver& aObserver);
    ~CAiContentObserverOptimizer();
    
    /**
     * Starts a transaction
     * 
     * @return KErrAlreadyExists in case transaction already ongoing. 
     *         KErrNotSupported in case transaction not supported
     *         KErrNone in case transaction succesfuly started
     */
    TInt StartTransaction( TInt aTxId );
    
    /**
     * Commits a started transaction. In case no committing is needed
     * (no publish calls has been made) the transaction is cancelled.
     * 
     * @return KErrNotReady in case no transaction is ongoing.
     *         KErrNotSupported in case transaction not supported
     *         KErrNone in case succesfully committed.
     */         
    TInt Commit( TInt aTxId );
    
    /**
     * Cancels a transactions in case a transaction was started.
     * 
     * @return KErrNotReady in case no transaction is ongoing.
     *         KErrNotSupported in case transactions are not supported
     *         KErrNone in case transaction succesfully cancelled     
     */
    TInt CancelTransaction( TInt aTxId );
    
    /*
     * @see MAiContentObserver
     */
    TBool CanPublish( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );
    
    /**
     * @see MAiContentObserver
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  TInt aResource, TInt aIndex );

    /**
     * @see MAiContentObserver
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  const TDesC16& aText, TInt aIndex );
    /**
     * @see MAiContentObserver
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  const TDesC8& aBuf, TInt aIndex );

    /**
     * @see MAiContentObserver
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  RFile& aFile, TInt aIndex );
    /**
     * @see MAiContentObserver
     */
    TInt Clean( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );

    /**
     * Returns the actual content observer.
     */
    MAiContentObserver& Observer() const;
    
    /**
     * Clears blacklist
     */
    void ClearBlackList();
    
private:
    CAiContentObserverOptimizer(MAiContentObserver& aObserver);

    TInt AddToBlackList( TInt aContentId, TInt aIndex );

    TBool IsInBlackList( TInt aContentId, TInt aIndex ) const;

private: // data

    /**
     * Handle to the observer that receives
     * the publish calls.
     */
    MAiContentObserver &iObserver;
    
    TBool iCommitNeeded;

    TBool iTransactionStarted;
    
    RArray<TAiPublishBlackList> iBlackList;
    };


#endif // C_AIMULTICONTENTOBSERVEROPTIMIZER_H
