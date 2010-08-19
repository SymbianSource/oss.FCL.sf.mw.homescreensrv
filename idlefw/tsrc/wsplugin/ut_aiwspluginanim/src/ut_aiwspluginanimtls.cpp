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

// System includes

// User includes
#include "ut_aiwspluginanimtls.h"

// Constants

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::CreateL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnimTls::CreateL()
    {
    if ( !Instance() )
        {
        UT_AiWsPluginAnimTls* self = new( ELeave ) UT_AiWsPluginAnimTls;
        CleanupStack::PushL( self );
        User::LeaveIfError( Dll::SetTls( self ) );
        CleanupStack::Pop( self );
        }
    }
  
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::Instance()
//
// ----------------------------------------------------------------------------
//   
UT_AiWsPluginAnimTls* UT_AiWsPluginAnimTls::Instance()
    {
    return static_cast<UT_AiWsPluginAnimTls*>( Dll::Tls() );
    }
    
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::Destroy()
//
// ----------------------------------------------------------------------------
//    
void UT_AiWsPluginAnimTls::Destroy()
    {
    delete Instance();
    Dll::SetTls( NULL );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::UT_AiWsPluginAnimTls()
//
// ----------------------------------------------------------------------------
//
UT_AiWsPluginAnimTls::UT_AiWsPluginAnimTls() 
    {
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::~UT_AiWsPluginAnimTls
//
// ----------------------------------------------------------------------------
//    
UT_AiWsPluginAnimTls::~UT_AiWsPluginAnimTls() 
    {
    iEventHandlers.Close();
    iFeatures.Close();
    }
    
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::AddEventHandlerL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnimTls::AddEventHandlerL( 
    T_AiWsPlugin::CTestEventHandler& aHandler )
    {
    iEventHandlers.AppendL( &aHandler );
    }
    
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::EventHandlers()
//
// ----------------------------------------------------------------------------
//
TArray<T_AiWsPlugin::CTestEventHandler*> UT_AiWsPluginAnimTls::EventHandlers() const
    {
    return iEventHandlers.Array();
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::IncFeatMgrCount()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnimTls::IncFeatMgrCount()
    {
    ++iFeatMgrCount;
    }
    
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::DecFeatMgrCount()
//
// ----------------------------------------------------------------------------
//    
void UT_AiWsPluginAnimTls::DecFeatMgrCount()
    {
    --iFeatMgrCount;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::FeatMgrCount()
//
// ----------------------------------------------------------------------------
//    
TInt UT_AiWsPluginAnimTls::FeatMgrCount() const
    {
    return iFeatMgrCount;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::FeatureSupported()
//
// ----------------------------------------------------------------------------
//    
TBool UT_AiWsPluginAnimTls::FeatureSupported( TInt aFeatureId ) const
    {
    const TBool* value = iFeatures.Find( aFeatureId );
    
    return (value ? *value : EFalse);
    }
 
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnimTls::SetFeatureSupportedL()
//
// ----------------------------------------------------------------------------
//    
void UT_AiWsPluginAnimTls::SetFeatureSupportedL( TInt aFeatureId, 
    TBool aValue )
    {
    iFeatures.InsertL( aFeatureId, aValue );
    }

// End of file
