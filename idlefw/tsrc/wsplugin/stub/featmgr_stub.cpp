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

// System includes

// User includes
#include "featmgr_stub.h"

#include "ut_aiwspluginanimtls.h"

// Constants

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// FeatureManager::InitializeLibL()
//
// ----------------------------------------------------------------------------
//
void FeatureManager::InitializeLibL()
    {
    UT_AiWsPluginAnimTls::Instance()->IncFeatMgrCount();
    }

// ----------------------------------------------------------------------------
// FeatureManager::UnInitializeLib()
//
// ----------------------------------------------------------------------------
//
void FeatureManager::UnInitializeLib()
    {
    UT_AiWsPluginAnimTls::Instance()->DecFeatMgrCount();
    }

// ----------------------------------------------------------------------------
// FeatureManager::FeatureSupported()
//
// ----------------------------------------------------------------------------
//
TBool FeatureManager::FeatureSupported( TInt aFeature )
    {
    return UT_AiWsPluginAnimTls::Instance()->FeatureSupported(aFeature);
    }
    
// End of file
