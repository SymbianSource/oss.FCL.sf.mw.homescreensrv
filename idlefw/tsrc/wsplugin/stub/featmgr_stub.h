/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Feature manager API
*
*/


#ifndef _FEATUREMANAGER_STUB_H
#define _FEATUREMANAGER_STUB_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CFeatMgrTlsData;

/**
 * Feature manager API stub.
 */
NONSHARABLE_CLASS( FeatureManager ) : public CBase
    {
    public: 
        /**
        * This must be called in the scope of the thread before calling
        * any other methods. It sets up TLS. Uninitialization is done
        * by calling the UnInitializeLibL() function.
        *
        * Leave conditions: out of memory.
        */
        static void InitializeLibL();

        /**
        * This must be called in the scope of the thread after calling
        * InitializeLibL(). It frees the allocated TLS.
        */
        static void UnInitializeLib();
        
		/**
        * Fetches information whether a certain feature is supported.
        *
        * @since 1.2
        * @param aFeature feature ID
        * @return feature support status
        */
        static TBool FeatureSupported( TInt aFeature );
    
    private:

        /**
        * C++ default constructor.
        * Prohibits instantiation of this class.
        */
        FeatureManager();
    };

#endif  // _FEATUREMANAGER_STUB_H
            
