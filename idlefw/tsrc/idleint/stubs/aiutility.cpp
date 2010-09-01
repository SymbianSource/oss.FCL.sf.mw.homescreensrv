/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "aiutility.h"
#include "caipspropertyobserver.h"
#include "caistrparser.h"
#include "caicontentitemarrayiterator.h"
#include "aipluginsettingsimpl.h"
#include "caiplugintool.h"
#include "UT_IdleInt.h"

//#include <TelephonyInternalPSKeys.h>
#include <StartupDomainPSKeys.h>


EXPORT_C MAiPSPropertyObserver* AiUtility::CreatePSPropertyObserverL(
                                            TCallBack aCallBack,
                                            TUid aCategory,
                                            TInt aKey )
	{
	CPSPropertyObserver* observerStub = UT_IdleInt::PSPropertyObserver( aCategory, aKey );
	observerStub->SetCallBack( aCallBack );
	return observerStub;
    //return CPSPropertyObserver::NewL( aCallBack, aCategory, aKey );	
	}
	
EXPORT_C MAiStrParser* AiUtility::CreateStrParserL()
	{	
	return NULL;
	}
	
EXPORT_C MAiContentItemIterator* AiUtility::CreateContentItemArrayIteratorL(
                                            const TAiContentItem* /*aArray*/,
                                            TInt /*aCount*/ )
	{
	return NULL;
	}

EXPORT_C MAiPluginSettings* AiUtility::CreatePluginSettingsL()
	{
	return NULL;
	}

EXPORT_C MAiPluginTool* AiUtility::CreatePluginToolL()
	{
	return NULL;
	}
	
   
	
// End of File.
