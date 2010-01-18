/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HSCONTENTCOTROLLER_H
#define HSCONTENTCOTROLLER_H

// System includes
#include <e32base.h>

// Forward declarations
class CHsContentInfo;
class CHsContentInfoArray;

// Constants

/**
 * unable to add widget because active page is full.
 * No widgets can be added to this page before removing some first.
 */
const TInt KHsErrorViewFull = -87001;

/**
 * Unable to add this widget because the maximum instance count
 * per page will exceed
 */
const TInt KHsErrorMaxInstanceCountExceeded = -87002; 

/**
 * unable to add widget because widget is too large to fit.
 * A smaller widget might fit though.
 */
const TInt KHsErrorDoesNotFit = -87003;



/**
 *  Content controller interface
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
class MHsContentController
    {
public:
    /**
     * Fills an array of CHsContentInfo. Both widget and template types are appended.
     * @param aArray Array of CHsContentInfo
     */
    virtual TInt WidgetListL( CHsContentInfoArray& aArray ) = 0;

    /**
     * Fills an array of CHsContentInfo. View types are appended.
     * @param aArray Array of CHsContentInfo
     */
    virtual TInt ViewListL( CHsContentInfoArray& aArray ) = 0;

    /**
     *
     */
    virtual TInt AppListL( CHsContentInfoArray& aArray ) = 0;

    /**
     * Adds a widget to the active view.
     * @param aInfo Content info object
     * @return KErrNone on success, any of system wide error codes,
     *         KHsErrorViewFull, KHsErrorMaxInstanceCountExceeded or
     *         KHsErrorDoesNotFit
     */
    virtual TInt AddWidgetL( CHsContentInfo& aInfo ) = 0;

    /**
     * Removes a widget from the configuration.
     * @param aInfo Content info object
     */
    virtual TInt RemoveWidgetL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt AddViewL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt RemoveViewL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt ActivateViewL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt ActivateAppL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt ActiveViewL( CHsContentInfo& aInfo ) = 0;

    /**
     *
     */
    virtual TInt ActiveAppL( CHsContentInfo& aInfo ) = 0;

    };


#endif // HSCONTENTCOTROLLER_H

// End of file
