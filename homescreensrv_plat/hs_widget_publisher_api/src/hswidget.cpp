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

#include "hswidget.h"
#include "hswidgetitem.h"
#include "hsexception.h"
#include <e32err.h>

using namespace Hs;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void HsWidget::setItem( std::string aItemName, int aValue )
	{
	HsWidgetItem* item = getWidgetItem( aItemName );
	if( !item )
		{
		item  = new HsWidgetItem( aItemName, aValue );
		if( !item )
			{
			throw HsException( KErrNoMemory );
			}
		mItems.push_back( item );
		}
	item->setValue( aValue );
	}	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void HsWidget::setItem( std::string aItemName, std::string aValue )
	{
	HsWidgetItem* item = getWidgetItem( aItemName );
	if( !item )
		{
		item  = new HsWidgetItem( aItemName, aValue );
		if( !item )
			{
			throw HsException( KErrNoMemory );
			}
		mItems.push_back( item );
		}
	item->setValue( aValue );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void HsWidget::removeItem( std::string aItemName )
	{
	HsWidgetItem* ret( 0 );
	int count = mItems.size();
	for (int index = 0; index < count; index++)
		{
		HsWidgetItem* const item = mItems.at( index );
		if( !aItemName.compare( item->getItemName() ) )
			{
			ret = item;
			mItems.erase( mItems.begin() + index );
			break;
			}
		}
	if( !ret )
		{
		throw HsException( KErrNotFound );
		}
	delete ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidget::HsWidget( std::string& aTemplateName, 
	std::string& aWidgetName, std::string& aIdentifier,
	std::string& aDescription, std::string& aIconLocation ):
	mWidgetName(aWidgetName), mTemplateName(aTemplateName), 
	mIdentifier(aIdentifier), mDescription( aDescription ),
	mIconLocation( aIconLocation )
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidget::~HsWidget()
	{
   	int count = mItems.size();
   	for( int index = 0; index < count; index++ )
   		{
   		HsWidgetItem* const item = mItems.at( index );
   		delete item;
   		}
   	mItems.erase( mItems.begin(), mItems.end() );	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidgetItem* HsWidget::getWidgetItem( 
	std::string& aItemName )
	{
	HsWidgetItem* ret( 0 );
	int count = mItems.size();
	for (int index = 0; index < count; index++)
		{
		HsWidgetItem* const item = mItems.at( index );
		if( !aItemName.compare( item->getItemName() ) )
			{
			ret = item;
			break;
			}
		} 
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidget::getIdentifier()
	{
	return mIdentifier;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidget::getWidgetName()
	{
	return mWidgetName;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidget::getTemplateName()
	{
	return mTemplateName;	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
int HsWidget::itemsCount()
	{
	return mItems.size();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidgetItem* HsWidget::getWidgetItem( int aIndex )
	{
	return mItems.at( aIndex ); 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
bool HsWidget::checkIfWidgetItemExist( 
	std::string& aItemName )
	{
	bool itemExist( false );
	int count = mItems.size();
	for (int index = 0; index < count; index++)
		{
		HsWidgetItem* const item = mItems.at( index );
		if( !aItemName.compare( item->getItemName() ) )
			{
			itemExist = true;
			break;
			}
		}
	return itemExist;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidget::getDescription() const
    {
    return mDescription;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const std::string& HsWidget::getIconLocation() const
    {
    return mIconLocation;
    }
