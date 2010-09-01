/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "contentprioritymap.h"

using namespace AiUtility;


// ======== MEMBER FUNCTIONS ========

CContentPriorityMap::CContentPriorityMap()
    {
    }

EXPORT_C CContentPriorityMap* CContentPriorityMap::NewL()
    {
    CContentPriorityMap* self = new( ELeave ) CContentPriorityMap;
    return self;
    }

CContentPriorityMap::~CContentPriorityMap()
    {
    iPriorityMap.ResetAndDestroy();
    }


EXPORT_C TInt CContentPriorityMap::CurrentPriority( const TDesC8& aUiElementId ) const
    {
    // Find current priority value for aUiElementId
    const TInt* currentPriority = iPriorityMap.Find( aUiElementId );
    
    // Return current priority value or KErrNotFound if it has not been set
    return currentPriority ? *currentPriority : KErrNotFound;
    }

EXPORT_C TInt CContentPriorityMap::SetCurrentPriority( const TDesC8& aUiElementId,
                                                       TInt aPriority )
    {
    TInt error = KErrNone;
    TInt* currentPriority = iPriorityMap.Find( aUiElementId );
    
    if ( currentPriority )
        {
        // Update old value
        *currentPriority = aPriority;
        }
    else
        {
        // Insert new <key,value> pair
        TRAP( error,
            {
            HBufC8* key = aUiElementId.AllocLC();
            TInt* value = new( ELeave ) TInt( aPriority );
            CleanupStack::PushL( value );
            iPriorityMap.InsertL( key, value );
            CleanupStack::Pop( 2, key );
            } );
        }
    
    return error;
    }

EXPORT_C void CContentPriorityMap::ClearPriority( const TDesC8& aUiElementId )
    {
    TInt* currentPriority = iPriorityMap.Find( aUiElementId );
    
    if ( currentPriority )
        {
        // Clear old value. Use KErrNotFound to avoid future reallocations of
        // the same key.
        *currentPriority = KErrNotFound;
        }
    }

EXPORT_C void CContentPriorityMap::Reset()
    {
    iPriorityMap.ResetAndDestroy();
    }

EXPORT_C TBool CContentPriorityMap::OverrideContent( const TDesC8& aUiElementId,
                                                     TInt aNewPriority ) const
    {
    return ( aNewPriority >= CurrentPriority( aUiElementId ) );
    }

