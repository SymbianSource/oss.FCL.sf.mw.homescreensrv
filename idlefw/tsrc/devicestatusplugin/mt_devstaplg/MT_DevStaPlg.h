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


#ifndef __MT_DEVSTAPLG_H__
#define __MT_DEVSTAPLG_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "aicontentobserver.h"
#include "stubdataholder.h"
#include "ccontentcache.h"

//  FORWARD DECLARATIONS
class CHsContentPublisher;
class CContentObserver;

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
NONSHARABLE_CLASS( MT_DevStaPlg )
     : public CEUnitTestSuiteClass//, public MAiContentObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_DevStaPlg* NewL();
        static MT_DevStaPlg* NewLC();
        /**
         * Destructor
         */
        ~MT_DevStaPlg();

    private:    // Constructors and destructors

        MT_DevStaPlg();
        void ConstructL();

    public:     // From observer interface

/*
        TInt StartTransaction(TInt aTxId);
        TInt Commit(TInt aTxId);
        TInt Rollback(TInt aTxId);
        TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, TInt aResource, TInt aIndex );
        TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex );
        TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );
        TInt Publish(MAiPropertyExtension& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );
        TInt Clean(MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex);
        TAny* Extension(TUid aUid);
        */


    private:    // New methods

         void SetupL();
         void SetupEmptyL();

         void Teardown();

         // void TestL();

         void TestPropertyL();
         // void TestProfilePublisher1L();
         // void TestProfilePublisher2L();
         void TestOperatorLogoPublisherL();
         void TestOperatorProgNamePublisherL();
         void TestOperatorNetinfoNamePublisher_LongL();
         void TestOperatorNetinfoNamePublisher_ShortL();
         void TestOperatorNetinfoNamePublisher_TagL();
         void TestOperatorProviderNamePublisher_InSPDINoPLMNL();
         void TestOperatorProviderNamePublisher_InSPDIPLMNL();
         void TestBTSAPPublisherL();
         void TestSimRegPublisherL();
         void TestHCZPublisherL();
         void TestCUGPublisherL();
         void TestMCNPublisherL();
         void TestContentRequest1L();
         void TestContentRequest2L();
         void TestContentRequest3L();
         void TestContentRequest4L();
         void TestContentRequest5L();
         void TestContentRequest6L();
         void TestContentRequest7L();
         void TestContentRequest8L();
         void TestContentRequest9L();
         void TestContentRequest10L();
         void TestContentRequest13L();
         void TestContentRequest14L();
         void TestContentRequest15L();


    private:    // Data

        EUNIT_DECLARE_TEST_TABLE;

        CHsContentPublisher* iPublisher;

        CContentObserver* iCache;

        RLibrary iLibrary;
        


    };

#endif      //  __MT_DEVSTAPLG_H__

// End of file
