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
     * Returns the list of available Home screen widgets
     * @param aArray List of widgets
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt WidgetListL( CHsContentInfoArray& aArray ) = 0;

    /**
     * Returns the list of Home screen widgets included in an application 
     * configuration or a view
     * @param aInfo Content info defining the application configuration 
     *              or the view which widget list is requested
     * @param aArray List of widgets
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt WidgetListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray ) = 0;

    /**
     * Returns the list of available Home screen views
     * @param aArray List of views
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ViewListL( CHsContentInfoArray& aArray ) = 0;

    /**
     * Returns the list of available Home screen views included in an
     * application configuration
     * @param aInfo Content info defining the application configuration
     * @param aArray List of views
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ViewListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray ) = 0;

    /**
     * Returns the list of available Home screen application configurations
     * @param aArray List of application configurations
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt AppListL( CHsContentInfoArray& aArray ) = 0;

    /**
     * Adds a widget to the active Home screen view.
     * @param aInfo Widget request to be added
     * @return KErrNone on success, any of system wide error codes,
     *         KHsErrorViewFull, KHsErrorMaxInstanceCountExceeded or
     *         KHsErrorDoesNotFit
     */
    virtual TInt AddWidgetL( CHsContentInfo& aInfo ) = 0;

    /**
     * Removes a widget from the configuration.
     * @param aInfo Widget request to be removed
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt RemoveWidgetL( CHsContentInfo& aInfo ) = 0;

    /**
     * Adds a view to the active Home screen application configuration.
     * @param aInfo View request to be added
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt AddViewL( CHsContentInfo& aInfo ) = 0;

    /**
     * Removes a view from the configuration.
     * @param aInfo View request to be removed
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt RemoveViewL( CHsContentInfo& aInfo ) = 0;

    /**
     * Activates the Home screen view
     * @param aInfo View request to be activated
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ActivateViewL( CHsContentInfo& aInfo ) = 0;

    /**
     * Activates the Home screen application configuration
     * @param aInfo Application configuration request to be activated
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ActivateAppL( CHsContentInfo& aInfo ) = 0;

    /**
     * Returns the active Home screen view
     * @param aInfo Active view
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ActiveViewL( CHsContentInfo& aInfo ) = 0;

    /**
     * Returns the active Home screen application configuration
     * @param aInfo Active application configuration
     * @return KErrNone on success, any of system wide error codes
     */
    virtual TInt ActiveAppL( CHsContentInfo& aInfo ) = 0;

    };


#endif // HSCONTENTCOTROLLER_H

// End of file
