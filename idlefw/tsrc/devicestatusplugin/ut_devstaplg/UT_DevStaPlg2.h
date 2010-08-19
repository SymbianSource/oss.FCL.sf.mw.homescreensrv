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


#ifndef __UT_DEVSTAPLG_H__
#define __UT_DEVSTAPLG_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <hscontentpublisher.h>

//  INTERNAL INCLUDES
#include "aicontentobserver.h"
#include "stubdataholder.h"
#include "ccontentcache.h"
#include "aiprioritizer.h"
#include "aipublisherbroadcaster.h"
#include "ccontentcache.h"

//  FORWARD DECLARATIONS
class CAiContentPublisher;
class CContentObserver;
class CStubDataHolder;
class CAiOperatorLogoPublisher;
class CAiOperatorNamePublisher;
class CAiMultiContentObserver;
class CAiBTSAPPublisher;
class CAiProfilePublisher;
class CAiDatePublisher;


#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif


//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class.
 *
 */
NONSHARABLE_CLASS( UT_DevStaPlg2 )
     : public CEUnitTestSuiteClass,
       public MAiPublishPrioritizer,
       public MAiPublisherBroadcaster,
       public MAiContentObserver
       
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_DevStaPlg2* NewL();
        static UT_DevStaPlg2* NewLC();
        /**
         * Destructor
         */
        ~UT_DevStaPlg2();

    private:    // Constructors and destructors

        UT_DevStaPlg2();
        void ConstructL();

    public:  // MAiPublishPrioritizer

    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                TInt aResource, 
                                TInt aPriority );
    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                const TDesC16& aText, 
                                TInt aPriority );
    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                const TDesC8& aBuf, 
                                TInt aPriority );
    void TryToCleanL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                TInt aPriority );

    
    TInt NextPriority() const        
        {
        return 0;
        };

    public: // MAiPublisherBroadcaster
    
    TBool RefreshPriorizedPublishersL( TInt , TInt  )
        {
        return ETrue;
        };
    
    public: // MAiContentObserver
    void Reset();
    CContentCache* GetContent( TInt aContentId, TBool aClean = EFalse );
    CContentCache* GetLastContent( TInt aContentId, TBool aClean = EFalse );
            
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
        
		    TInt SetProperty( CHsContentPublisher& aPlugin,
		        const TDesC8& aElementId,
		        const TDesC8& aPropertyName,
		        const TDesC8& aPropertyValue );

		    TInt SetProperty( CHsContentPublisher& aPlugin,
		        const TDesC8& aElementId,
		        const TDesC8& aPropertyName,
		        const TDesC8& aPropertyValue,  
		        MAiContentObserver::TValueType aValueType);         
                
    private:    // New methods

         void SetupL();
         void SetupEmptyL();

         void Teardown();

         void TestOperatorLogoPublisherL();

         void TestOperatorProgNamePublisherL();
         
         void TestMultiObserverL();

         void TestBTSAPPublisherL();

         // void TestProfilePublisherL();
         
         void TestDatePublisherL();

         void TestPluginL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE;

        CHsContentPublisher* iPublisher;

        UT_DevStaPlg2* iCache;
        
        CStubDataHolder* iStubDataHolder;
        
        CAiOperatorLogoPublisher* iLogoPublisher;
        
        CAiOperatorNamePublisher* iNamePublisher;

		CAiMultiContentObserver* iMultiObserver;

		CAiBTSAPPublisher* iBTSAP;

		CAiProfilePublisher* iProfilePublisher;
		
		CAiDatePublisher* iDatePublisher;

        RPointerArray<CContentCache> iInternalCache;
        
        TBool iFilePublished;
        
        TBool iBufPublished;

    };

#endif      //  __UT_DEVSTAPLG_H__

// End of file
