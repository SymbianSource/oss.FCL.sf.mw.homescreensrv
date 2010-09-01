/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CONTENTOBSERVER_H
#define C_CONTENTOBSERVER_H

#include <e32base.h>
#include <aicontentobserver.h>

#include "ccontentcache.h"

class CContentObserver : public CBase, public MAiContentObserver
    {
public:

        static CContentObserver& InstanceL();
        static void Release();
        static void Reset();
        CContentCache* GetContent( TInt aContentId, TBool aClean = EFalse );
        CContentCache* GetLastContent( TInt aContentId, TBool aClean = EFalse );
        
		    /**
		     *
		     */
		    TInt SetProperty( CHsContentPublisher& aPlugin,
		        const TDesC8& aElementId,
		        const TDesC8& aPropertyName,
		        const TDesC8& aPropertyValue );
		    
		    /**
		     *
		     */
		    TInt SetProperty( CHsContentPublisher& aPlugin,
		        const TDesC8& aElementId,
		        const TDesC8& aPropertyName,
		        const TDesC8& aPropertyValue,  
		        MAiContentObserver::TValueType aValueType);           

private:

        ~CContentObserver();
        TInt StartTransaction(TInt aTxId);
        TInt Commit(TInt aTxId);
        TInt CancelTransaction(TInt aTxId);
        TBool CanPublish(CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );
        TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex );
        TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex );
        TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );
        TInt Publish(CHsContentPublisher& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );
        TInt Clean(CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex);
        TAny* Extension(TUid aUid);
        TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const;

private:

        RPointerArray<CContentCache> iCache;
    };


#endif      //  C_CONTENTOBSERVER_H

// End of file
