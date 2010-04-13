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
* Description:   Header file for CGSPluginInterface class.
*
*/


#ifndef GSPLUGININTERFACE_H
#define GSPLUGININTERFACE_H

// System includes
#include <GSFrameworkRsc.rsg> // For default icons
#include <gsfwicon.mbg>

#include <aknview.h>
#include <aknViewAppUi.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <aknViewAppUi.h>
#include <data_caging_path_literals.hrh>

// Constant for plugin interface:
const TUid KGSPluginInterfaceUid        = { 0x10207236 };

// Constant for listbox icon type:
const TUid KGSIconTypeLbxItem           = { 0x10207357 };
// Constant for listbox settings item icon type:
const TUid KGSIconTypeLbxSettingsItem   = { 0x102750C5 };
// Constant for tab icon type:
const TUid KGSIconTypeTab               = { 0x10207358 };
// Constant for icon in D-Column:
const TUid KGSIconTypeDColumn           = { 0x10207359 };


// Constant for indexing (iOrder):
const TInt KGSPluginNotIndexed      = -1;

_LIT( KGSDefaultIconFileName, "Z:Gsfwicon.mbm" );

/**
* This enum is used for defining custom operations types.
*/
enum TGSCustomOperationType
    {
    // This operation type returns current status of the plugin's view.
    // If the plugin's view is active, ETrue will be returned, otherwise
    // EFalse. Currently supported by GSStandbyPlugin.
    EGSCustomOperationViewActive = 1
    };

/**
* Used by GetValue(). These are the keys for retrieving a specific
* value. This enum can be extended to provide other values as well as
* long as the original keys are not changed.
*/
enum TGSPluginValueKeys
    {
    // Value for this key is the localized string to be shown in the second row
    // of a listbox item. Plugins providing second row of text should provide
    // value for this key. Second row of list item text is used for following
    // listbox types:
    //  - EGSListBoxTypeSingleLarge
    //  - EGSListBoxTypeDouble2Large.
    // For plugins listed in these listbox types, second row value can either
    // be defined or left empty.
    EGSPluginKeySettingsItemValueString = 1,
    // Localized string to be used in case of custom MSK and menu activation
    // item. See TGSMenuActivationItems.
    EGSCustomMenuActivationText
    };

/**
* Used by PluginItemType(). These enumerations define the desired appearance
* of the plugin in a parent plugin's listbox. Please notice that the parent
* listbox type cannot be altered by a child plugin so plugins using these must
* be aware of the parent plugin listbox type. Safest alternative is to leave
* default implementation for ItemType() in which case EGSItemTypeSingleLarge is
* used. This should fit most of the listbox types. Additionall types can be
* implemented and defined as long as the old enumerations will not be changed.
*
* Different type of items are handled differently:
* - In case of CAknView type items, such as EGSItemTypeSingleLarge and
*   EGSItemTypeSetting, the plugin's DoActivate() is called when user selects
*   the item.
* - In case of items supporting dialogs, such as EGSItemTypeSettingDialog and
*   EGSItemTypeSettingIconDialog, HandleSelection() is called instead of
*   DoActivate().
*
* Item types can be described in a matrix:
* X: Normal list tems with large icon,
*    settings items,
*    settings items with icon
* Y: Items providing CAknView - activated by DoActivate(),
*    items providing dialog - activated by HandleSelection()
*
* All combinations of these are not allowed/supported.
*/
enum TGSListboxItemTypes
    {
    // CAknSingleLargeStyleListBox item:
    // Caption of the item is a descriptor from GetCaptionL().
    // Icon is from CreateIconL( KGSIconTypeLbxItem ).
    EGSItemTypeSingleLarge = 1, // Default
    // CAknSettingStyleListBox item providing a CAknView:
    // 1st row content is a descriptor from GetCaptionL()
    // 2nd row content is a descriptor (can be empty string id needed) from
    //    GetValue( ..., EGSPluginKeySettingsItemValueString ).
    EGSItemTypeSetting,
    // CAknSettingStyleListBox item providing a CAknView:
    // 1st row content is a descriptor from GetCaptionL()
    // 2nd row content is a icon from CreateIconL( KGSIconTypeLbxSettingsItem )
    // GetValue(, EGSPluginKeySettingsItemValueString ).
    EGSItemTypeSettingIcon,
    // CAknSettingStyleListBox item launching a dialog:
    // 1st row content is a descriptor from GetCaptionL()
    // 2nd row content is a descriptor (can be empty string id needed) from
    //    GetValue( ..., EGSPluginKeySettingsItemValueString ).
    EGSItemTypeSettingDialog,
    // CAknSingleLargeStyleListBox item launching a dialog:
    // Caption of the item is a descriptor from GetCaptionL().
    // Icon is from CreateIconL( KGSIconTypeLbxItem ).
    EGSItemTypeSingleLargeDialog
    };

/**
* These are used to define the type of the menu item which opens the selected
* plugin. By default, parent plugin's menu has 'Open' item for child plugins.
* If parent plugin's menu must have 'Change' instead of 'Open' for activating
* the plugin, use EGSMenuActivationItemChange. This functionality is just for
* the visual aspects for views in GS - requested by UI design. Plugins are
* handled logically similarly regardless this value.
*/
enum TGSMenuActivationItems
    {
    // Default - will use the item defined in menu resource.
    EGSMenuActivationItemDefault = 0,
    // Menu should contain 'Open' Item.
    EGSMenuActivationItemOpen = 1,
    // Menu should contain 'Change' item.
    EGSMenuActivationItemChange,
    // Menu should contain custom text which is defined by GetValue() function:
    // GetValue( EGSCustomMenuActivationText, ... );
    // NOTE: When this custom menu item is selected, used menu command ID is
    // EGSCmdAppChange. This might be checked in for example plugin's
    // HandleCommand(). If text is empy, MSK is empty and there's no
    // corresponding item in options menu.
    EGSMenuActivationItemCustom
    };

/**
* Selection types. Used in HandleSelection().
*/
enum TGSSelectionTypes
    {
    // User selected the item by pressing selection key.
    EGSSelectionBySelectionKey = 1,
    // User selected the item by selecting a command from the menu.
    EGSSelectionByMenu
    };

/**
* Interface class for GS plugin. All GS plugins will implement this class.
*
* The main functionality GS framework will use from CAknView is:
* -DoActivate()
* -DoDeactivate()
* -Id()
* functions.
*
* Id() function must return the value of the plugin implementation UID. This
* means that the main view of the plugin will have the same UID as the plugin
* implementation. This will prevent multiple plugins from having same view
* UIDs as their main view. If plugin has more views, it is plugin's
* responsibility to ensure that the UIDs of the other views are unique. This
* can be done for example reserving a unique UID from Symbian.
*
* Most of the functions have implementation using default values. Override
* functions if different values or implementations are desired.
*
* CGSPluginInterface UID = 0x10207236
*
* See GSFWViewUIDs.h for plugin UIDs.
*
* Plugin implementation UID will be used when defining the parent view of a
* plugin. If plugin belongs to application settings view, set the
* ApplicationSettingsView plugin implementation Uid as a value in the plugin's
* default_data field in plugin's implementation info resource file.
*
* Needed libraries (at least):
* GSEcomPlugin.lib
* GSFramework.lib
* egul.lib
* aknskins.lib
* efsrv.lib
*
* @lib GSFramework.lib
* @since Series60_3.1
*/
class CGSPluginInterface: public CAknView
    {

    // CGSPluginLoader accesses iOrder which should not be accessed outside.
    friend class CGSPluginLoader;

    public: // Constructors & destructors

        /**
        * Creates new GS plugin having the given UID.
        * Uses Leave code KErrNotFound if implementation is not found.
        *
        * @param aImplementationUid Implementation UID of the plugin to be
        *        created.
        * @param aInitParams Plugin's initialization parameters. Make sure you know
        *        what the plugin expects as initialization parameters. This
        *        should be an agreement between the plugin client and the
        *        plugin. Parameter can be used for example as sharing a common
        *        data model between multiple plugins.
        */
        IMPORT_C static CGSPluginInterface* NewL(
            const TUid aImplementationUid,
            TAny* aInitParams );

        /**
        * Destructor
        */
        IMPORT_C ~CGSPluginInterface();

    public: // New

        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        *
        * @param aCaption pointer to Caption variable
        */
        virtual void GetCaptionL( TDes& aCaption ) const = 0;

        /**
        * Function for getting plugin's value for a certain key.
        * Override to provide own functionality.
        *
        * @param aKey Key for the value to be retrieved.
        * @parem aValue Value for the given gey in TDes format.
        */
        IMPORT_C virtual void GetValue( const TGSPluginValueKeys aKey,
                                        TDes& aValue );

        /**
        * This function is called in case plugin is an item in a settings
        * listbox and user selects the item. Override this if plugin needs to
        * provide functionality for item selection.
        *
        * Default implementation activates the plugin.
        * @param aSelectionType Defines how user selected the plugin. See
        *        TGSSelectionTypes.
        */
        IMPORT_C virtual void HandleSelection(
            const TGSSelectionTypes aSelectionType );

        /**
        * This defines the appearance of the plugin in a parent plugin listbox.
        * Default value is EGSItemTypeSingleLarge. Please notice that the
        * parent listbox type cannot be defined by a child plugin so plugins
        * using these must be aware of the parent plugin listbox type.
        *
        * @return Desired listbox presentation and functional type for the
        *         plugin.
        */
        IMPORT_C virtual TGSListboxItemTypes ItemType();

        /**
        * This function is only used for child plugins. Defines the dynamic
        * menu item that activates this plugin from parent plugin. This menu
        * item will override the item defined in resource file. Normally
        * 'Open' is used to open the child plugin. Override this to change the
        * activation to for example 'Change'. TGSMenuActivationItems defines the
        * different possible menu items that can be used to open this plugin.
        *
        * Note: This also defines MSK. Menu activation item and MSK have
        *       identical label and behaviour. An exception to this is
        *       when custom text is empty: in this case MSK is empty but there
        *       is no item in options menu (no empty item in menu).
        *
        * @ return Type of the menu item and MSK that should activate this
        *          child plugin from parent plugin.
        */
        IMPORT_C virtual TGSMenuActivationItems MenuActivationItem();

        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used for KGSIconTypeLbxItem. Other
        * icons are empty. Ownership of the created icon is transferred to the
        * caller.
        * NOTE: Return NULL if icon is not to be displayed.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem            - ListBox item icon.
        * KGSIconTypeLbxSettingsItem    - Settings item icon.
        * KGSIconTypeTab                - Tab icon.
        * KGSIconTypeDColumn            - Small icon in D-column.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon or NULL.
        */
        IMPORT_C virtual CGulIcon* CreateIconL( const TUid aIconType );

        /**
        * Method for reading the ID of the plugin provider category. See
        * TGSPluginProviderCategory. PluginProviderCategory can be used for
        * sorting plugins.
        *
        * Default value is EGSPluginProvider3rdParty. Override this function
        * to change the category.
        *
        * @return Plugin provider category ID defined by
        *         TGSPluginProviderCategory
        */
        IMPORT_C virtual TInt PluginProviderCategory() const;

        /**
        * Reserved for future use/plugin's custom functionality. This can be
        * overwritten if plugin needs to have custom functionality which cannot
        * be fulfilled otherwise.
        *
        * Use TGSCustomOperationType enumeration as aParam1 to
        * define operation type.
        */
        IMPORT_C virtual TAny* CustomOperationL( TAny* aParam1, TAny* aParam2 );

        /**
        * Method for checking, if plugin should be visible and used in GS FW.
        * (for example shown in listbox of the parent view).
        *
        * On default plugin is visible. Overwrite this function to enable or
        * disable your plugin dynamically.
        *
        * @return ETrue if plugin should be visible in GS.
        * @return EFalse if plugin should not be visible in GS.
        */
        IMPORT_C virtual TBool Visible() const;

        /**
        * Resets plugin's selected item index if this is supported. Default
        * implementation does nothing. This is needed when navigating from
        * child plugin back to parent plugin. In this case child plugin should
        * reset selected index in its listbox.
        */
        IMPORT_C virtual void ResetSelectedItemIndex();

        /**
        * Sets the Index of the plugin in listbox. Used for CGSPluginLoader. Default
        * value is KGSPluginNotIndexed which means not ordered. This value is
        * read, if defined, from the opaque_data field of the plugin's resource
        * definition. Index starts from 0.
        */
        IMPORT_C void SetOrder( TInt aOrder );

        /**
        * In case plug-in is loaded outside GS application and CGSPluginLoader,
        * cleaning up plug-in's ECOM resources must be done manually.
        *
        * Example:
        * CGSPluginInterface* myPlugin = CGSPluginInterface::NewL(...);
        * REComSession::DestroyedImplementation( myPlugin->GetEcomDestructorKey() );
        * delete myPlugin;
        *
        * @return UID of the plug-in instance which can be used to free dependent ECOM resources.
        */
        IMPORT_C TUid GetEcomDestructorKey();

    protected: // New

        /**
        * C++ constructor.
        */
        IMPORT_C CGSPluginInterface();

    public: // Enumerations

        /**
        * Category of the plugin provider. Provider's type affects the sorting
        * of the plugins. Type EGSPluginProvider3rdParty plugins are allowed
        * to be loaded only by Applications-plugin. Other categories may exist
        * but they should be used only internally.
        **/
        enum TGSPluginProviderCategory
            {
            EGSPluginProviderOEM        = 1,
            EGSPluginProviderOperator   = 2,
            EGSPluginProvider3rdParty   = 3
            };

    private: // Data

        // ECOM plugin instance UID.
        TUid iDtor_ID_Key;

        /**
        * Index of the plugin in listbox. Used for CGSPluginLoader. Default
        * value is KGSPluginNotIndexed which means not ordered. This value is
        * read, if defined, from the opaque_data field of the plugin's resource
        * definition. Index starts from 0.
        */
        TInt iOrder;

    };

#endif // GSPLUGININTERFACE_H
//End of file
