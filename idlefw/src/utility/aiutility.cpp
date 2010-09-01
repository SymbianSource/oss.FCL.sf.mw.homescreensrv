/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

EXPORT_C MAiPSPropertyObserver* AiUtility::CreatePSPropertyObserverL(
                                            TCallBack aCallBack,
                                            TUid aCategory,
                                            TInt aKey )
	{	
	return CPSPropertyObserver::NewL( aCallBack, aCategory, aKey );	
	}
	
EXPORT_C MAiStrParser* AiUtility::CreateStrParserL()
	{	
	return CStrParser::NewL();	
	}
	
EXPORT_C MAiContentItemIterator* AiUtility::CreateContentItemArrayIteratorL(
                                            const TAiContentItem* aArray,
                                            TInt aCount )
	{
	return CAiContentItemArrayIterator::NewL( aArray, aCount );
	}

EXPORT_C MAiPluginSettings* AiUtility::CreatePluginSettingsL()
	{
	return CAiPluginSettings::NewL();
	}

EXPORT_C MAiPluginTool* AiUtility::CreatePluginToolL()
	{
	return CAiPluginTool::NewL();
	}
	
   
	
// End of File.
