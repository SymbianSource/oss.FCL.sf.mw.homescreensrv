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


#include "aicontentobserveroptimizer.h"

CAiContentObserverOptimizer* CAiContentObserverOptimizer::NewL(MAiContentObserver& aObserver)
    {
    return new(ELeave) CAiContentObserverOptimizer( aObserver );
    }
    
CAiContentObserverOptimizer::~CAiContentObserverOptimizer()
    {
    iBlackList.Close();
    }

TInt CAiContentObserverOptimizer::StartTransaction( TInt aTxId )
    {
    TInt err = KErrAlreadyExists;
    if ( iTransactionStarted )
        {
        return err;
        }
    iCommitNeeded = EFalse;
    
    err = iObserver.StartTransaction( aTxId );
    if ( err == KErrNone )
        {
        iTransactionStarted = ETrue;
        }
    return err;   
    }
    

TInt CAiContentObserverOptimizer::Commit( TInt aTxId )
    {
    TInt err = KErrNotReady;
    if ( iTransactionStarted )
        {
        if ( iCommitNeeded )
            {
            iCommitNeeded = EFalse;
            err = iObserver.Commit( aTxId );
            }
        else
            {
            err = CancelTransaction( aTxId );
            }
        }
    iTransactionStarted = EFalse;
    return err;
    }
    

TInt CAiContentObserverOptimizer::CancelTransaction( TInt aTxId )
    {
    TInt err = KErrNotReady;
    if ( iTransactionStarted )
        {
        err = iObserver.CancelTransaction( aTxId );
        }
    iTransactionStarted = EFalse;
    return err;
    }
    
    
TBool CAiContentObserverOptimizer::CanPublish( CHsContentPublisher& aPlugin,
                                           TInt aContent,
                                           TInt aIndex )
    {
    return iObserver.CanPublish( aPlugin, aContent, aIndex );
    }
  
TInt CAiContentObserverOptimizer::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       TInt aResource,
                                       TInt aIndex )
    {
    if ( IsInBlackList( aContent, aIndex ) )
        {
        return KErrNotFound;
        }   
    TInt err = iObserver.Publish( aPlugin, aContent, aResource, aIndex );
    // Publish went through OK, we need to commit the transaction
    if ( err == KErrNone && iTransactionStarted )
        {
        iCommitNeeded = ETrue;
        }
    // publish failed because the ui declaration doesn't
    // include this content => add to black list and 
    // don't try to publish again    
    else if ( err == KErrNotFound || err == KErrNotSupported )
        {
        AddToBlackList( aContent, aIndex );
        }
    return err;
    }
    
    
TInt CAiContentObserverOptimizer::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       const TDesC16& aText,
                                       TInt aIndex )
    {
    if ( IsInBlackList( aContent, aIndex ) )
        {
        return KErrNotFound;
        }   
    TInt err = iObserver.Publish( aPlugin, aContent, aText, aIndex );
    // Publish went through OK, we need to commit the transaction
    if ( err == KErrNone && iTransactionStarted )
        {
        iCommitNeeded = ETrue;
        }
    // publish failed because the ui declaration doesn't
    // include this content => add to black list and 
    // don't try to publish again    
    else if ( err == KErrNotFound || err == KErrNotSupported )
        {
        AddToBlackList( aContent, aIndex );
        }
    return err;
    }
    
    
TInt CAiContentObserverOptimizer::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       const TDesC8& aBuf,
                                       TInt aIndex )
    {
    if ( IsInBlackList( aContent, aIndex ) )
        {
        return KErrNotFound;
        }
    TInt err = iObserver.Publish( aPlugin, aContent, aBuf, aIndex );
    // Publish went through OK, we need to commit the transaction
    if ( err == KErrNone && iTransactionStarted )
        {
        iCommitNeeded = ETrue;
        }
    // publish failed because the ui declaration doesn't
    // include this content => add to black list and 
    // don't try to publish again    
    else if ( err == KErrNotFound || err == KErrNotSupported )
        {
        AddToBlackList( aContent, aIndex );
        }
    return err;
    
    }
    
    
TInt CAiContentObserverOptimizer::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       RFile& aFile,
                                       TInt aIndex )
    {
    if ( IsInBlackList( aContent, aIndex ) )
        {
        return KErrNotFound;
        }    
    TInt err = iObserver.Publish( aPlugin, aContent, aFile, aIndex );
    // Publish went through OK, we need to commit the transaction
    if ( err == KErrNone && iTransactionStarted )
        {
        iCommitNeeded = ETrue;
        }
    // publish failed because the ui declaration doesn't
    // include this content => add to black list and 
    // don't try to publish again
    else if ( err == KErrNotFound || err == KErrNotSupported )
        {
        AddToBlackList( aContent, aIndex );
        }
    return err;
    }
    
    
TInt CAiContentObserverOptimizer::Clean( CHsContentPublisher& aPlugin,
                                     TInt aContent,
                                     TInt aIndex )
    {
    return iObserver.Clean( aPlugin, aContent, aIndex );    
    }

MAiContentObserver& CAiContentObserverOptimizer::Observer() const
    {
    return iObserver;
    }

TInt CAiContentObserverOptimizer::AddToBlackList( TInt aContentId, TInt aIndex )
    {
    TInt err = KErrNone;
    if ( !IsInBlackList( aContentId, aIndex ) )
        {
        TAiPublishBlackList entry;
        entry.iContentId = aContentId;
        entry.iIndex = aIndex;
        err = iBlackList.Append( entry );
        }
    return err;
    }

TBool CAiContentObserverOptimizer::IsInBlackList( TInt aContentId, TInt aIndex ) const 
    {
    for (TInt i = 0; i < iBlackList.Count(); ++i )
        {       
        if ( iBlackList[i].iContentId == aContentId &&
             iBlackList[i].iIndex == aIndex )
            {
            return ETrue;                   
            }
        }
    return EFalse;
    }

CAiContentObserverOptimizer::CAiContentObserverOptimizer(MAiContentObserver& aObserver): 
    iObserver( aObserver )
    {
    }

// end of file
