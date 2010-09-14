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
#include "hswidgetitem.h"
#include "hsexception.h"
#include <e32err.h>


using namespace Hs;
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidgetItem::HsWidgetItem(std::string& aItemName, std::string& aValue):
	mItemName(aItemName), mItemString(aValue), mItemInt(0), 
	mIsString(true),mTrigger(false)
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidgetItem::HsWidgetItem( std::string& aItemName, int aValue ):
	mItemName(aItemName), mItemString(), mItemInt(aValue), 
	mIsString(false), mTrigger(false)
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidgetItem::~HsWidgetItem()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetItem::setValue( int aValue )
	{
	mItemInt = aValue;
	mIsString = false;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetItem::setValue( std::string& aValue )
	{
	mItemString = aValue;
	mIsString = true;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidgetItem::getItemName()
	{
	return mItemName;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidgetItem::getItemValue()
	{
	return mItemString;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
int HsWidgetItem::getItemValueInt()
	{
	return mItemInt;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
bool HsWidgetItem::isStringValue()
	{
	return mIsString;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetItem::triggerAdded()
    {
    mTrigger = true;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
bool HsWidgetItem::isTriggerAdded()
    {
    return mTrigger;
    }
