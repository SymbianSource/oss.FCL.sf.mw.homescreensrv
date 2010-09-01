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
* Description: CCPSApi DLL source
*
*/

//  Include Files  

#include "hswidgetpublisher.h"
#include "hswidgetpublisherimpl.h"
#include "hswidget.h"


//  Member Functions

using namespace Hs;
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HsWidgetPublisher::HsWidgetPublisher( IHsDataObserver* aDataObserver):
	mImpl( new HsWidgetPublisherImpl() )
	{
	mImpl->SetDataObserver( aDataObserver );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HsWidgetPublisher::~HsWidgetPublisher()
	{
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void HsWidgetPublisher::publishHsWidget( HsWidget& aTemplate )
	{
	mImpl->PublishWidget( aTemplate );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HsWidget& HsWidgetPublisher::createHsWidgetWithDesc( 
        std::string aTemplateName, std::string aWidgetName,
        std::string aIdentifier, std::string aDescription,
        std::string aIconLocation )
    {
    return mImpl->CreateWidget( aTemplateName, aWidgetName, aIdentifier,
            aDescription, aIconLocation );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HsWidget& HsWidgetPublisher::createHsWidget( std::string aTemplateName, 
	std::string aWidgetName,
	std::string aIdentifier )
	{
	std::string emptyString("");
	return mImpl->CreateWidget( aTemplateName, aWidgetName, aIdentifier,
	        emptyString, emptyString );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void HsWidgetPublisher::removeHsWidget( std::string aTemplateName, 
	std::string aWidgetName,
	std::string aIdentifier )
	{
	mImpl->RemoveWidget( aTemplateName, aWidgetName, aIdentifier );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HsWidget& HsWidgetPublisher::getHsWidget( 
	std::string aTemplateName, 
	std::string aWidgetName,
	std::string aIdentifier )
	{
	return mImpl->GetWidget( aTemplateName, aWidgetName, aIdentifier );
	}
