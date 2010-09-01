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

#ifndef __MENUENGFILTER_H__
#define __MENUENGFILTER_H__

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class CMenuEngObject;

// CLASS DECLARATION

/**
* Menu Engine Filter interface.
* Instances of this filter can be passed to CMenuEng::GetItemsL(), to get
* a filtered list of menu items.
*/
NONSHARABLE_CLASS( MMenuEngFilter )
    {

public:     // new methods

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    virtual TBool MatchesObject( const CMenuEngObject& aObject ) const = 0;

    };

#endif // __MENUENGFILTER_H__
