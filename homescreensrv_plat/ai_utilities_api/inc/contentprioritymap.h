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


#ifndef C_CONTENTPRIORITYMAP_H
#define C_CONTENTPRIORITYMAP_H

#include <e32base.h>
#include <e32hashtab.h>

namespace AiUtility {

/**
 *  Content priority map for UI controllers.
 *
 *  @lib aiutils_hs_hs.lib
 *  @since S60 3.2
 */
class CContentPriorityMap : public CBase
    {

public:
    /**
     * Creates and returns a new instance of this class.
     *
     * @post CurrentPriority(*) == KErrNotFound
     */
    IMPORT_C static CContentPriorityMap* NewL();

    virtual ~CContentPriorityMap();

    /**
     * Returns the current priority assigned to UI element.
     *
     * @param aUiElementId  textual identifier of the UI element.
     * @return Current priority set to ui element or KErrNotFound if not set.
     */
    IMPORT_C TInt CurrentPriority( const TDesC8& aUiElementId ) const;

    /**
     * Sets the current priority assigned to UI element.
     *
     * @param aUiElementId  textual identifier of the UI element.
     * @param aPriority     priority value to set.
     * @return KErrNone if priority was set succesfully, otherwise one of
     *         the system-wide error codes.
     * @post CurrentPriority(aUiElementId) == aPriority
     */
    IMPORT_C TInt SetCurrentPriority( const TDesC8& aUiElementId, TInt aPriority );

    /**
     * Clears the current priority assigned to UI element.
     *
     * @param aUiElementId  textual identifier of the UI element.
     * @post CurrentPriority(aUiElementId) == KErrNotFound
     */
    IMPORT_C void ClearPriority( const TDesC8& aUiElementId );

    /**
     * Clears all priority assignments of this map.
     */
    IMPORT_C void Reset();
    
    /**
     * Tests if content with given priority may override
     * current content in given ui element.
     *
     * @param aUiElementId  textual identifier of the UI element.
     * @param aNewPriority  priority of the new content.
     */
    IMPORT_C TBool OverrideContent( const TDesC8& aUiElementId, TInt aNewPriority ) const;

private:

    CContentPriorityMap();

private: // data

    RPtrHashMap<TDesC8,TInt> iPriorityMap;

    };


} // namespace AiUtility

#endif // C_CONTENTPRIORITYMAP_H
