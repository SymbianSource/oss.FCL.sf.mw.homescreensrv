/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class header for plugins containing other plugins.
*
*/


#ifndef GSPARENTPLUGIN_H
#define GSPARENTPLUGIN_H

// INCLUDES
#include    "gspluginloader.h"
#include    "gsplugininterface.h"
#include    "gschildviewmanager.h"

#include    <data_caging_path_literals.hrh>
#include    <ConeResLoader.h>
#include    <babitflags.h>

// CONSTANTS
_LIT( KGSParentPluginResourceFileName, "z:GSParentPluginRsc.rsc" );

/**
* Used by CGSParentPlugin::ListBoxType(). These enumerations are used to define
* the desired type of the listbox. Enumerations here are currently implemented
* by CGSParentPlugin. Additional types can be defined later as long as the
* values for previous types are not changed.
*/
enum TGSListboxTypes
    {
    // CAknSingleLargeStyleListBox for
    // list_single_large_pane
    EGSListBoxTypeSingleLarge = 1, // Default
    // CAknSettingStyleListBox for
    // list_setting_pane and
    // list_big_single_setting_pane
    EGSListBoxTypeSettings,
    // CAknDouble2LargeStyleListBox for
    // list_double2_large_graphic_pane
    EGSListBoxTypeDouble2Large,
    // CAknDoubleLargeStyleListBox for
    // list_double_large_graphic_pane
    // Note: Only different layout than EGSListBoxTypeDouble2Large
    EGSListBoxTypeDoubleLarge
    };

/**
* Indexes to flags that are used for different customisation options for parent
* plugin. The flags define for example optimization options for loading child
* plugins.
*/
enum TGSOptionFlagIndexes
    {
    // On:  Will load child plugins when parent plugin is activated.
    // Off: Will load child plugins on initialization of the parent plugin.
    EGSLoadChildrenOnActivation,
    // On:  Will delete child plugins when parent plugin is deactivated.
    // Off: Will delete child plugins when parent plugin is destroyed.
    EGSDeleteChildrenOnDeactivation // CURRENTLY NOT IMPLEMENTED!
    };

// FORWARD DECLARATIONS
class CGSParentContainer;

// CLASS DECLARATION

/**
* CGSParentPlugin view class. Thus is a base class for plugins owning other
* plugins as their sub-views.
*
* @lib GSFramework.lib
* @since Series60_3.1
*/
class CGSParentPlugin : public CGSPluginInterface,
                        public MGSPluginLoadObserver,
                        public MGSChildViewManager
{
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C ~CGSParentPlugin();

    public: // New

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

        /**
        * Symbian OS default constructor. Call this from sub classes.
        *
        * @param aViewTitleRscId    Id for the view's resource.
        * @parem aTitleRscId        Id for the view's title resource.
        */
        IMPORT_C void BaseConstructL( TInt aViewRscId, TInt aTitleRscId );

        /**
        * @return UID of the upper level view which launched this view. Note
        *         that it is not always the previous view because a view can
        *         be activated also by returning from another view.
        */
        IMPORT_C virtual TUid UpperLevelViewUid();

        /**
        * Defines the type of the used listbox visualising the child plugins.
        * Classes deriving from CGSParentContainer can override this function
        * to define the used type of the listbox. Otherwise default value
        * EGSListBoxTypeSingleLarge is used.
        *
        * @return Type of the listbox. See TGSListboxTypes for
        *         types.
        */
        IMPORT_C virtual TGSListboxTypes ListBoxType();

        /**
        * Updates plugin's view. This can be used when child plugin's values
        * are changed.
        */
        IMPORT_C virtual void UpdateView();

        /**
        * Defines custom options for parent plugin. Use TGSOptionFlagIndexes to
        * enable a combination of bit flags.
        *
        * @param aOptionFlags Option combination created by combining binary
        *        option flags.
        */
        IMPORT_C virtual void SetOptionFlags( TBitFlags& aOptionFlags );

        /**
        * Gets currently used option flags.
        *
        * @return Binary option flags used by this plugin.
        */
        IMPORT_C virtual const TBitFlags& OptionFlags() const;

        /**
        * Provides help context. This should be implemented by CGSParentPlugin
        * sub class in case help is needed. Normally this is defined in
        * CCoeControl derived class but since all parent plugin implementations
        * have only CGSParentContainer class as the only CCoeControl derived
        * class, and need still their own unique help contexts, this function
        * is needed in view level class.
        */
        IMPORT_C virtual void GetHelpContext( TCoeHelpContext& aContext );

        /**
        * @return Plugin which is selected in the lbx or NULL if no plugins
        *         or iContainer exist.
        */
        IMPORT_C CGSPluginInterface* SelectedPlugin();

        /**
        * Attempts to change child plugin load priority on next RunL cycle.
        * Use this if parent needs to change plugin loader priority. Normally
        * when parent plugin view is opened, it is advisable to increase loader
        * priority to get child plugins loaded faster.
        *
        * @param aPriority Desired priority.
        */
        void RequestPriority( CActive::TPriority aPriority );

    public: // From CGSPluginInterface

        /**
        * See base class.
        */
        IMPORT_C void ResetSelectedItemIndex();

    public: // From MGSPluginLoadObserver

        /**
        * See base class.
        */
        IMPORT_C void HandlePluginLoaded( KGSPluginLoaderStatus aStatus );

    public: //New

        /**
        * Checks if the MSK label needs to be adjusted.
        */
        void CheckMiddleSoftkeyLabelL();

        /**
        * Adds given resource text as MSK to CBA. Use either this or
        * SetMiddleSoftKeyLabelTextL in in case text is used deirectly instead
        * of resource.
        *
        * @param aResourceId middle softkey label.
        * @param aCommandId command that should be performed when MSK
        *        is pressed.
        * @since S60 v3.1
        */
        void SetMiddleSoftKeyLabelL( const TInt aResourceId, const TInt aCommandId );

        /**
        * Adds given text as MSK to CBA.
        *
        * @param aMskLabel middle softkey label.
        * @param aCommandId command that should be performed when MSK
        *        is pressed.
        * @since S60 v3.1
        */
        void SetMiddleSoftKeyLabelTextL( const TPtr aMskLabel, const TInt aCommandId );

    public: // From MGSChildViewManager

        IMPORT_C void TransferDynamicPluginL( CGSPluginInterface* aPlugin );

    protected: // From CAknView

        /**
        * See base class.
        */
        IMPORT_C void DoActivateL( const TVwsViewId& aPrevViewId,
                                   TUid aCustomMessageId,
                                   const TDesC8& aCustomMessage );

        /**
        * See base class.
        */
        IMPORT_C void DoDeactivate();

        /**
        * See base class.
        */
        IMPORT_C void HandleCommandL( TInt aCommand );

        /**
        * See base class.
        */
        IMPORT_C CGSParentPlugin();

    protected: // From MEikMenuObserver

        /**
        * See base class.
        */
        IMPORT_C void DynInitMenuPaneL( TInt aResourceId,
                                        CEikMenuPane* aMenuPane );

    private: // New

        /**
        * Constructs container for this view.
        */
        void ConstructContainerL();

        /**
        * Remove command from MSK.
        * @param aMskCommandId ID of the command to be removed.
        */
        void RemoveCommandFromMSK( TInt aMskCommandId );

        /**
         * Used in lazy loading. Starts loading the plugins.
         */
        void StartAsyncPluginLoadL();

    protected: // Data

        // Previous view UID.
        TVwsViewId iPrevViewId;

    private: // Data

        // Application UI.
        CAknViewAppUi* iAppUi;

        // Owned container.
        CGSParentContainer* iContainer;

        // GS plug-ins owned by this view.
        // CGSPluginLoader defines the ordering of plugins.
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;

        // Asynchronous loader for the GS plug-ins.
        CGSPluginLoader* iPluginLoader;

        // Resource loader.
        RConeResourceLoader iResourceLoader;

        // Resource id for the title.
        TInt iTitleRscId;

        // UID of the selected plugin in the container's lbx. Uid is used
        // instead of simple index because it is unambiguous whereas index
        // could refer to wrong plugin in case plugin order or count is
        // changed.
        TUid iSelectedPluginUid;

        // Combination of binary flags that can be set to alter parent plugin
        // behaviour.
        TBitFlags iOptionFlags;

        // This stores the desired MSK command ID.
        TInt iMskCommandId;
        
        // UID of the top plugin in the container's lbx. Uid is used
        // instead of simple index because it is unambiguous whereas index
        // could refer to wrong plugin in case plugin order or count is
        // changed.
        TUid iTopPluginUid;
        
        //keep the exact position of the listbox including current item index,
        //iVerticalOffset and  TopItemindex
        RArray<TInt> iPosition;
        
        //record the screen mode, portrait/landscape
        TBool iScreenMode;
};


#endif // GSPARENTPLUGIN_H
// End of File
