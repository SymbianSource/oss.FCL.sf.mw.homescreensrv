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
* Description:  Active Idle Content Observer interface multiplexer 
*                implementation.
*
*/

#include "aimulticontentobserver.h"
#include "aicontentobserveroptimizer.h"

CAiMultiContentObserver* CAiMultiContentObserver::NewL()
    {
    return new(ELeave) CAiMultiContentObserver;
    }
    
    
CAiMultiContentObserver::~CAiMultiContentObserver()
    {
    iObserverOptimizers.ResetAndDestroy();
    }
    
    
void CAiMultiContentObserver::AddObserverL(MAiContentObserver& aObserver)
    {
    TBool foundExisting = EFalse;
    
    for( TInt i = 0; i < iObserverOptimizers.Count(); ++i )
        {
        if ( &(iObserverOptimizers[i]->Observer()) == &aObserver )
            {
            foundExisting = ETrue;
            break;
            }
        }
    if( !foundExisting )
        {
        CAiContentObserverOptimizer *optimizer = 
            CAiContentObserverOptimizer::NewL( aObserver );
        iObserverOptimizers.AppendL( optimizer );       
        }
    }


TInt CAiMultiContentObserver::StartTransaction( TInt aTxId )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->StartTransaction( aTxId );
   
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    

TInt CAiMultiContentObserver::Commit( TInt aTxId )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Commit( aTxId );
      
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    

TInt CAiMultiContentObserver::CancelTransaction( TInt aTxId )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->CancelTransaction( aTxId );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    
    
TBool CAiMultiContentObserver::CanPublish( CHsContentPublisher& aPlugin,
                                           TInt aContent,
                                           TInt aIndex )
    {
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if( iObserverOptimizers[i]->CanPublish( aPlugin, aContent, aIndex ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
    
    
TInt CAiMultiContentObserver::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       TInt aResource,
                                       TInt aIndex )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Publish( aPlugin, aContent, aResource, aIndex );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    
    
TInt CAiMultiContentObserver::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       const TDesC16& aText,
                                       TInt aIndex )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Publish( aPlugin, aContent, aText, aIndex );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    
    
TInt CAiMultiContentObserver::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       const TDesC8& aBuf,
                                       TInt aIndex )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Publish( aPlugin, aContent, aBuf, aIndex );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    
    
TInt CAiMultiContentObserver::Publish( CHsContentPublisher& aPlugin,
                                       TInt aContent,
                                       RFile& aFile,
                                       TInt aIndex )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Publish( aPlugin, aContent, aFile, aIndex );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }
    
    
TInt CAiMultiContentObserver::Clean( CHsContentPublisher& aPlugin,
                                     TInt aContent,
                                     TInt aIndex )
    {
    TInt result = KErrNone;
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        const TInt r = iObserverOptimizers[i]->Clean( aPlugin, aContent, aIndex );
        if ( r != KErrNone )
            {
            result = r;
            }
        }
    return result;
    }


TAny* CAiMultiContentObserver::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

TBool CAiMultiContentObserver::RequiresSubscription( 
    const THsPublisherInfo& /*aPublisherInfo*/ ) const
    {
    return ETrue;
    }

TInt CAiMultiContentObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
           const TDesC8& /*aElementId*/,
           const TDesC8& /*aPropertyName*/,
           const TDesC8& /*aPropertyValue*/ )
    {
        return KErrNotSupported;
    }
   
TInt CAiMultiContentObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
           const TDesC8& /*aElementId*/,
           const TDesC8& /*aPropertyName*/,
           const TDesC8& /*aPropertyValue*/,  
           MAiContentObserver::TValueType /*aValueType*/)
    {
    return KErrNotSupported;
    }

void CAiMultiContentObserver::ClearBlackList()
    {
    const TInt count = iObserverOptimizers.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iObserverOptimizers[i]->ClearBlackList(); 
        }
    }

CAiMultiContentObserver::CAiMultiContentObserver()
    {
    }

// End of file
