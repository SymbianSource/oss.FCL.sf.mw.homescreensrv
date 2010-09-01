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

#include "AiFwTestContentPlugin.h"
#include "AiFwTestContentPlugin2.h"
#include "AiFwTestContentPlugin3.h"
#include "AiFwTestContentPlugin4.h"
#include "AiFwTestContentPlugin5.h"
#include "AiFwTestContentPlugin6.h"
#include "AiFwTestContentPlugin7.h"
#include "AiFwTestContentPlugin8.h"
#include "AiFwTestContentPlugin9.h"
#include "AiFwTestControllerPlugin.h"
#include "AiFwTestControllerPlugin2.h"
#include "AiFwTestDefines.h"
#include <ecom/ImplementationProxy.h>

using namespace AiTestUiController;
using namespace AiTestUiController2;

TAiTestTLSStorage::TAiTestTLSStorage() :
    iContentPlugin(NULL),
    iControllerPlugin(NULL)
    {
    }

TAiTestTLSStorage* TAiTestTLSStorage::CreateInstanceL()
    {
   	TAiTestTLSStorage* inst = NULL;
    TAny* ptr = Dll::Tls();

    if ( !ptr )
		{
		inst = new(ELeave) TAiTestTLSStorage;
    	User::LeaveIfError(Dll::SetTls( inst ));
		}
		else
	    {
	    inst = static_cast<TAiTestTLSStorage*>( ptr );
	    }
	return inst;
    }
    
void TAiTestTLSStorage::DestroyInstance()
    {
    TAny* ptr = Dll::Tls();
   	TAiTestTLSStorage* tlsSet = static_cast<TAiTestTLSStorage*>( ptr );

    if ( tlsSet )
		{
		delete tlsSet;
    	Dll::SetTls( NULL );
		}
    }

const TImplementationProxy KImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin, CAiFwTestContentPlugin::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiFwTestUiController, CAiFwTestControllerPlugin::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiFwTestUiController2, CAiFwTestControllerPlugin2::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin2, CAiFwTestContentPlugin2::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin3, CAiFwTestContentPlugin3::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin4, CAiFwTestContentPlugin4::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin5, CAiFwTestContentPlugin5::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin6, CAiFwTestContentPlugin6::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin7, CAiFwTestContentPlugin7::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin8, CAiFwTestContentPlugin8::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KImplementationUidAiTestContentPlugin9, CAiFwTestContentPlugin9::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
}

//  END OF FILE






