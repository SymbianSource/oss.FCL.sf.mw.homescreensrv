/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Base class for settings views.
*
*/


#ifndef GSBASEVIEW_H
#define GSBASEVIEW_H

// INCLUDES
#include    <gsplugininterface.h>

#include    <aknnavi.h>
#include    <aknview.h>
#include    <bldvariant.hrh>
#include    <ConeResLoader.h>
#include    <eikclb.h>

// FORWARD DECLARATIONS
class CGSBaseContainer;
class CAknNavigationDecorator;

// CLASS DECLARATION

/**
*  CGSBaseView view class
*  Base class for GS plugin views.
*
*  @lib GSFramework.lib
*  @since Series60_3.1
*/
class CGSBaseView : public CGSPluginInterface , public MEikListBoxObserver
    {
    public:

        // Panic code used in this class
        enum KGSViewPanicCodes
            {
            EGSViewPanicNullPtr = 1
            };

    public: // Constructors and destructor

        /**
        * C++ default constructor
        */
        IMPORT_C CGSBaseView();

        /**
        * Symbian OS default constructor.
        */
        IMPORT_C virtual void ConstructL()=0;

        /**
        * Destructor.
        */
        IMPORT_C ~CGSBaseView();

    public: // New

        /**
        * This function is used to set the current item in the listbox.
        * @param aIndex Current item. Default value is 0 i.e. the first item
        *        of the list.
        */
        IMPORT_C virtual void SetCurrentItem( TInt aIndex = 0 );

        /**
        * Returns the container this view has.
        * @return pointer to container, does not transfer ownership.
        */
        IMPORT_C CGSBaseContainer* Container();

        /**
        * Opens the nearest localized resourcefile using aResourceLoader.
        * aResourceLoader should be opened only once before closing it.
        * Otherwise it will raise a panic. Therefore it is better to have
        * multiple resource loaders. Uses KDC_RESOURCE_FILES_DIR.
        *
        * @param aResourceFileName Drive and name of resource file in format
        *                          <path>:<rsc_file_name>
        * @param aResourceLoader Resource loader for the resource. The loader
        *                        must not be opened allready or it will raise
        *                        a panic. It is caller's responsibility to
        *                        close the resource loader after using the
        *                        resource.
        */
        IMPORT_C void OpenLocalizedResourceFileL(
            const TDesC& aResourceFileName,
            RConeResourceLoader& aResourceLoader );

    public: // From CGSPluginInterface

        /**
        * See base class.
        */
        IMPORT_C virtual void ResetSelectedItemIndex();

    public: // From CAknView

        /**
        * See base class.
        */
        IMPORT_C void DoActivateL( const TVwsViewId& aPrevViewId,
                                   TUid /*aCustomMessageId*/,
                                   const TDesC8& /*aCustomMessage*/ );

        /**
        * See base class.
        */
        IMPORT_C void DoDeactivate();

    protected: // New

        /**
        * Sets the correct navi pane for the view (eg. tabs, string etc.).
        */
        IMPORT_C virtual void SetNaviPaneL();

        /**
        * Creates context for view's navi pane.
        * @param aResourceId Resource id for navi pane context's resource.
        */
        IMPORT_C virtual void CreateNaviPaneContextL( TInt aResourceId );

        /**
        * Creates container,
        */
        IMPORT_C virtual void CreateContainerL();

        /**
        * Functionality for creating a container. Called by DoActivateL().
        * Implemented by sub-class.
        */
        IMPORT_C virtual void NewContainerL() = 0;

    protected: // From CAknView

        /**
        * See base class.
        */
        void HandleClientRectChange();

    protected: // From MEikListBoxObserver

        /**
        * See base class.
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
            TListBoxEvent aEventType );

    private: // New

        /**
        * Handles listbox selection. Called by HandleListBoxEventL.
        * Implement by sub-class.
        */
        virtual void HandleListBoxSelectionL() = 0;
        
        /**
        *  Creates navi control container when needed
        */
        CAknNavigationControlContainer* NaviControlContainerL();

    public: // Data

        TVwsViewId iPrevViewId; // Previous view UID

    protected: // Data

        // Pointer to application UI.
        CAknViewAppUi* iAppUi;

        // Pointer to the container
        CGSBaseContainer* iContainer;

        // Navi pane
        CAknNavigationDecorator* iNaviPaneContext;

        // Pointer to the navi control container
        CAknNavigationControlContainer* iNaviControlContainer;

        // Currently selected listbox item
        TInt iCurrentItem;

        // First item in the listbox
        TInt iTopItemIndex;

        // European look and feel
        TBool iElaf;

        // Resource loader
        RConeResourceLoader iResourceLoader;

    private: // Data

    };

#endif // GSBASEVIEW_H
// End of File
