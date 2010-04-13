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
* Description:   Main view.
*
*/



#ifndef GSMAINVIEW_H
#define GSMAINVIEW_H

// INCLUDES
#include    "gspluginloader.h"
#include    "gschildviewmanager.h"
#include    <data_caging_path_literals.hrh>
#include    <ConeResLoader.h>
#include    <gstabbedviewowner.h>
#include    <aknview.h>
#include    <eikmenup.h>

// CONSTANTS
_LIT( KGSMainViewResourceFileName, "z:GSApp.rsc" );

// FORWARD DECLARATIONS
class CGSMainContainer;
class CGSPluginInterface;

// CLASS DECLARATION

/**
* CGSMainView view class
*
* Main view of GS
*
* This view is responsible for visualizing the first level of settings
* plugins.
*
*/
class CGSMainView :
    public CAknView,
    public MGSPluginLoadObserver,
    public MGSTabbedViewOwner,
    public MGSChildViewManager
    {
    public: // Constructors and destructor

        CGSMainView();

        /**
        * Symbian OS two-phased constructor
        * @return GS main view.
        */
        static CGSMainView* NewL();

        /**
        * Symbian OS two-phased constructor
        * @return GS main view.
        */
        static CGSMainView* NewLC();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor
        */
        ~CGSMainView();

    public: // New

        /**
        * Returns pointer to the owned plugins.
        */
        CArrayPtrFlat<CGSPluginInterface>* Plugins();

        /**
        * Updates plugin's view. This can be used when child plugin's values
        * are changed.
        */
        void UpdateView();

    public: // From CAknView

        /**
        * See base view.
        */
        TUid Id() const;

    public: // From MGSPluginLoadObserver

        /**
        * See base view.
        */
        void HandlePluginLoaded( KGSPluginLoaderStatus aStatus );

    public: // From MGSChildViewManager

        /**
        * See base view.
        */
        void TransferDynamicPluginL( CGSPluginInterface* aPlugin );

    public: // From MGSTabbedViewOwner

        /**
        * See base view.
        */
        CArrayPtrFlat<CGSPluginInterface>* TabbedViews();

        /**
        * See base class.
        */
        void TabChangedL( TUid selectedTabUid );

    protected: // From CAknView

        /**
        * See base view.
        */
        void DoActivateL(
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
        * See base view.
        */
        void DoDeactivate();

        /**
        * See base view.
        */
        void HandleCommandL( TInt aCommand );
    
    protected: // From MEikMenuObserver

        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:

        /**
        * Opens the nearest localized resourcefile.
        */
        void OpenLocalizedResourceFileL( const TDesC& aResourceFileName );

        /**
        * Creates new container.
        */
        void NewContainerL();

    private: // Data

        // Pointer to Application UI. Not owned.
        CAknViewAppUi* iAppUi;

        // Pointer to owned container.
        CGSMainContainer* iContainer;

        // GS plug-ins owned by this view. CGSPluginLoader defines the
        // ordering of items.
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;

        // Asynchronous loader for the GS plug-ins.
        CGSPluginLoader* iPluginLoader;

        // Resource loader.
        RConeResourceLoader iResourceLoader;

        // UID of the selected plugin in the container's lbx.
        TUid iSelectedPluginUid;
        
        //keep the exact position of the listbox including current item index,
        //iVerticalOffset and  TopItemindex
        RArray<TInt> iPosition;
        
        //record the screen mode, portrait/landscape
        TBool iScreenMode;
    };

#endif  // GSMAINVIEW_H
// End of File
