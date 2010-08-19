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


#ifndef _UT_AIWSPLUGINANIMTLS_H
#define _UT_AIWSPLUGINANIMTLS_H

// System includes 
#include <e32base.h>
#include <e32hashtab.h>

// User incldues
#include "testeventhandler_stub.h"

// Forward declarations

/**
 * TLS shared test data holder for CAiWsPluginAnim unit tests.
 */
NONSHARABLE_CLASS( UT_AiWsPluginAnimTls )
	: public CBase
    {
public:
    static void CreateL();
    static UT_AiWsPluginAnimTls* Instance();
    static void Destroy();

private:
    UT_AiWsPluginAnimTls();
    void ConstructL();
    ~UT_AiWsPluginAnimTls();

public:    // New methods
    template<class EventHandler_t>
    EventHandler_t* FindEventHandler() const;
    
    void AddEventHandlerL( T_AiWsPlugin::CTestEventHandler& aHandler );
    TArray<T_AiWsPlugin::CTestEventHandler*> EventHandlers() const;
    
    void  IncFeatMgrCount();
    void  DecFeatMgrCount();
    TInt  FeatMgrCount() const;
    TBool FeatureSupported( TInt aFeatureId ) const;
    void  SetFeatureSupportedL( TInt aFeatureId, TBool aValue );

private:    // Data
    RPointerArray<T_AiWsPlugin::CTestEventHandler> iEventHandlers;
    TInt iFeatMgrCount;
    RHashMap< TInt, TBool > iFeatures;
    };


template<class EventHandler_t>
EventHandler_t* UT_AiWsPluginAnimTls::FindEventHandler() const
    {
    EventHandler_t* result = NULL;
    const TInt count = iEventHandlers.Count();
    for( TInt i = 0; i < count && !result; ++i )
        {
        T_AiWsPlugin::CTestEventHandler* handler = iEventHandlers[i];
        result = dynamic_cast<EventHandler_t*>( handler );
        }
    return result;
    }

#endif  //  _UT_AIWSPLUGINANIMTLS_H

