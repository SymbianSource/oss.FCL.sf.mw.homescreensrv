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
* Description:  Active Idle Content Observer interface multiplexer interface.
*
*/


#ifndef C_AIMULTICONTENTOBSERVER_H
#define C_AIMULTICONTENTOBSERVER_H

// System includes
#include <e32base.h>

// User includes
#include "aicontentobserver.h"

// Forward declarations
class CAiContentObserverOptimizer;
class CHsContentPublisher;

/**
 *  @ingroup group_devicestatusplugin
 *
 * Active Idle Content Observer interface multiplexer.
 *
 * @see MAiContentObserver
 * @since S60 3.2
 */
NONSHARABLE_CLASS( CAiMultiContentObserver ) : public CBase,         
    public MAiContentObserver
    {
public:
    static CAiMultiContentObserver* NewL();
    ~CAiMultiContentObserver();
    
    /**
     * Adds a new observer to this multiplexer.
     */
    void AddObserverL(MAiContentObserver& aObserver);

//from base class MAiContentObserver
    TInt StartTransaction( TInt aTxId );
    TInt Commit( TInt aTxId );
    TInt CancelTransaction( TInt aTxId );
    TBool CanPublish( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  TInt aResource, TInt aIndex );
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  const TDesC16& aText, TInt aIndex );
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  const TDesC8& aBuf, TInt aIndex );
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent,
                  RFile& aFile, TInt aIndex );
    TInt Clean( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );
    TAny* Extension( TUid aUid );
    TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const;

    TInt SetProperty( CHsContentPublisher& aPlugin,
               const TDesC8& aElementId,
               const TDesC8& aPropertyName,
               const TDesC8& aPropertyValue );
       
    TInt SetProperty( CHsContentPublisher& aPlugin,
               const TDesC8& aElementId,
               const TDesC8& aPropertyName,
               const TDesC8& aPropertyValue,  
               MAiContentObserver::TValueType aValueType);

private:
    CAiMultiContentObserver();
    void ConstructL();

private: // data
    /**
     * Array of observers.
     * Own.
     */
    RPointerArray<CAiContentObserverOptimizer> iObserverOptimizers;

    };


#endif // C_AIMULTICONTENTOBSERVER_H
