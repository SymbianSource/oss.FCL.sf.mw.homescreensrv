/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utility class for creating tab groups.
*
*/


#ifndef GSTABHELPER_H
#define GSTABHELPER_H

#include "gsplugininterface.h"
#include "gstabbedviewowner.h"

#include <AknTabObserver.h>
#include <e32base.h>
#include <e32cmn.h>


// Forward declarations
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTabGroup;
class MGSTabbedView;


/**
* CGSTabHelper is used to create dynamic tabs from either
*   - CGSPluginInterface* array (plugins)
*   - MGSTabbedView* interface Array (local views owned by plugin).
*
* How to use:
*
* 1. Call CreateTabGroupL() from the plugin's/view's DoActivate(). Providing an
*    array of all the plugins/views which should be in the tab group including
*    the plugin/view itself.
*
* 2. Call RemoveTabGroup() from the plugin's/view's DoDeactivate().
*
* 3. It is recommended to use CGSTabHelper* as an member variable owned by the
*    plugin/view class.
*
* @lib GSFramework.lib
* @since Series60_3.1
*/
class CGSTabHelper : public CBase, public MAknTabObserver
    {
    public: // Constructors & destructors

        /**
        * Symbian OS two-phased constructor
        * @return
        */
        IMPORT_C static CGSTabHelper* NewL();

        /**
        * Symbian OS two-phased constructor
        * @return
        */
        IMPORT_C static CGSTabHelper* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C ~CGSTabHelper();

    public: // New

        /**
        * This function is for views implementing MGSTabbedViewOwner. Such may
        * be needed in case view needs to update its selected item according to
        * tab selection.
        *
        * Creates and adds a tab group to navipane. The number of visible tabs
        * is 4 unless there are less tabbed views in which case the number of
        * tabs equal the number of tabbed views.
        *
        * @param aActiveViewUid UID of the view whose tab should be activated
        *                       when the tab group is added to the navi pane.
        * @param aTabOwner Reference to the owner of the tabbed views.
        */
        IMPORT_C void CreateTabGroupL( TUid aActiveViewUid,
                                       MGSTabbedViewOwner* aTabOwner );

        /**
        * This function is for plugin implementation.
        *
        * Creates and adds a tab group to navipane. The number of visible tabs
        * is 4 unless there are less tabbed views in which case the number of
        * tabs equal the number of tabbed views.
        *
        * @param aActiveViewUid UID of the view whose tab should be activated
        *                       when the tab group is added to the navi pane.
        * @param aTabbedViews References to the views which belong to the tab
        *                     group.
        */
        IMPORT_C void CreateTabGroupL(
            TUid aActiveViewUid,
            CArrayPtrFlat<CGSPluginInterface>* aTabbedViews );

       /**
        * This function is for local view implementation.
        *
        * Creates and adds a tab group to navipane. The number of visible tabs
        * is 4 unless there are less tabbed views in which case the number of
        * tabs equal the number of tabbed views.
        *
        * @param aActiveViewUid UID of the view whose tab should be activated
        *                       when the tab group is added to the navi pane.
        * @param aTabbedViews References to the views which belong to the tab
        *                     group.
        */
        IMPORT_C void CreateTabGroupL(
            TUid aActiveViewUid,
            CArrayPtrFlat<MGSTabbedView>* aTabbedViews );

        /**
        * Removes the topmost tab group from navi pane.
        */
        IMPORT_C void RemoveTabGroup();

    public: // From MAknTabObserver

        /**
        * See base class.
        */
        IMPORT_C void TabChangedL( TInt aIndex );

    protected: // New

        /**
        * C++ default constructor.
        */
        CGSTabHelper();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data

        // Container for navigation control object.
        CAknNavigationControlContainer* iNaviContainer;

        // Navigation decorator object. Created and owned.
        CAknNavigationDecorator* iNaviDecorator;

        // Tab group.
        CAknTabGroup* iTabGroup;

        // Pointer to App UI. Not owned.
        CAknViewAppUi* iAppUi;

        // Pointer to owner (parent) of the tabbed views.
        MGSTabbedViewOwner* iTabOwner;
    };

#endif // GSTABHELPER_H
// End of File
