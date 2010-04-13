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
* Description:   Container for GSParentPlugin.
*
*/


#ifndef GSPARENTCONTAINER_H
#define GSPARENTCONTAINER_H

#include    "gsparentplugin.h"
#include    <bldvariant.hrh>
#include    <AknIconArray.h>
#include    <AknsUtils.h>     // For loading icons
#include    <coecntrl.h>
#include    <eiklbx.h>
#include    <eiktxlbx.h>

// CLASS DECLARATION
class CAknViewAppUi;
class CGSPluginInterface;
class CAknSingleLargeStyleListBox;
class MGSWatchDog;

/**
*  Container class for the GS Main view.
*  @lib GSFramework.lib
*  @since Series60_3.1
*/
class CGSParentContainer : public CCoeControl, public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS constructor.
        * Required by the base class.
        */
        /*void ConstructL(
            const TRect& aRect,
            CAknViewAppUi* aAppUi,
            CArrayPtrFlat<CGSPluginInterface>* aPluginArray,
            TInt aTitleRscId );*/

        /**
        * Symbian OS constructor.
        * Required by the base class.
        */
        void ConstructL(
            const TRect& aRect,
            CAknViewAppUi* aAppUi,
            CArrayPtrFlat<CGSPluginInterface>* aPluginArray,
            TInt aTitleRscId,
            CGSParentPlugin* aParentPlugin,
            TGSListboxTypes aListBoxType =
                EGSListBoxTypeSingleLarge );

	      /**
        * Destructor.
        */
        ~CGSParentContainer();

    public: // New

        /**
        * Updates iListBox. Listbox is updated depending on the type of the
        * listbox. Plugins are inserted to listbox only if they are visible.
        */
        void UpdateListBoxL();

        /**
        * @return Pointer to owned listbox
        */
        CEikListBox* ListBox();

        /**
        * @return Plugin which is selected in the lbx or NULL if no plugins
        *         exist.
        */
        CGSPluginInterface* SelectedPlugin();

        /**
        * Sets selected lbx item using the UID of the item. If the item is not
        * existing or visible anymore, selected item is the first item in the
        * lbx.
        */
        void SetSelectedItem( TUid aSelectedItemUid );
        
        /**
        * @return Plugin which is top in the lbx or NULL if no plugins
        *         exist.
        */
        CGSPluginInterface* TopPlugin();
        
        /**
        * Sets top lbx item using the UID of the item.
        */
        void SetTopItem( TUid aTopItemUid );

        /**
         * Set the empty text of list box.
         */
        void SetListBoxEmptyTextL(const TDes& aEmpty );
        
        /**
         * @return the exact position of the listbox containing current item index,
         * iVerticalOffset and  TopItemindex
         */
        void GetPositionL(RArray<TInt>& aPosition);

        /**
         * set listbox's exact position.
         */
        void SetPosition(const RArray<TInt>& aPosition, TBool aChangeMode);   
        

    public: // From CCoeControl

        /**
        * Set focus on the selected listbox. For animated skins feature.
        */
        IMPORT_C void FocusChanged( TDrawNow aDrawNow );

        /**
        * See base class.
        */
        void HandleResourceChange( TInt aType );

        /**
        * See base class.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    public: //New

        void HandleSelectionKeyL();

    protected: // From MEikListBoxObserver

        /**
        * Handles listbox events.
        * @param aListBox Listbox where the event occurred.
        * @param aEventType Event type.
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
            TListBoxEvent aEventType );

    private: // New

        /**
        * Creates a listbox.
        */
        void CreateListBoxL( TGSListboxTypes aListBoxType );

        /**
        * Adds plugin data to listbox.
        * @param aPlugin A plugin whose data is to be used.
        * @param aItemTextArray Array for plugin texts and format strings used
        *        by lbx.
        * @param aIconArray Array for icons used in lbx.
        * @param aIconCounter Counter for inserted icons, will be updated.
        */
        void AddPluginDataToLbxL( CGSPluginInterface* aPlugin,
                                  CDesCArray* aItemTextArray,
                                  CAknIconArray* aIconArray,
                                  TInt& aIconCounter );

    private: // From CCoeControl

        /**
        * See base class.
        */
        void SizeChanged();

        /**
        * See base class.
        */
        TInt CountComponentControls() const;

        /**
        * See base class.
        */
        CCoeControl* ComponentControl(TInt /*aIndex*/) const;

        /**
        * See base class.
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType );

    private: // Data

        // Application UI. Not owned.
        CAknViewAppUi* iAppUi;

        // Owned listbox.
        CEikListBox* iListBox;

        // Type of iListBox.
        TGSListboxTypes iListBoxType;

        // Pointer to listbox model's item texts. Note that the indexes of
        // listbox items are mapped directly to the indexes in plugin array.
        // Plugin which maps to selected listbox item index in plugin array is
        // activated when item is selected in listbox.
        //
        // Owned by iListBox.
        //
        CDesCArray* iItemTextArray;

        // Array of the child plugins. Owned by CGSParentPlugin.
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;

        // Array of pointers to iPluginArray plugins which are visible.
        // This is the actual array which is displayed by the listbox.
        // It is needed because iPluginArray also contains non-visible plugins
        // and therefore the indexes do not match to lbx items.
        CArrayPtrFlat<CGSPluginInterface>* iVisiblePlugins;

        // Pointer to parent plugin. Not owned.
        CGSParentPlugin* iParentPlugin;

        // Pointer to GSWatchDog owned by GS Application document. Not owned.
        MGSWatchDog* iGSWatchDog;
       
    };


#endif // GSPARENTCONTAINER_H
// End of File
