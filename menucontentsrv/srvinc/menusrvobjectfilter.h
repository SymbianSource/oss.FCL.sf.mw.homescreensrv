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

#ifndef __MENUSRVOBJECTFILTER_H__
#define __MENUSRVOBJECTFILTER_H__

//  INCLUDES

#include "menuengfilter.h"

// FORWARD DECLARATION

class CMenuFilter;

// CLASS DECLARATION

/**
* Wrap a CMenuFilter as MMenuEngFilter.
*/
NONSHARABLE_CLASS( TMenuSrvObjectFilter ): public MMenuEngFilter
    {

public:     // construction

    /**
    * Constructor.
    * @param aFilter Filter.
    */
    TMenuSrvObjectFilter( CMenuFilter& aFilter ): iFilter( aFilter ) {}

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

private:    // data

    CMenuFilter& iFilter; ///< Filter.

    };

#endif // __MENUSRVOBJECTFILTER_H__
            
// End of File
