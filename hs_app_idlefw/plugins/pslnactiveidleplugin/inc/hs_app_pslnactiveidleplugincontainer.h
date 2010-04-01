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
* Description:  Container for Application shell theme view.
*
*/


#ifndef C_PSLNACTIVEIDLEPLUGINCONTAINER_H
#define C_PSLNACTIVEIDLEPLUGINCONTAINER_H

// INCLUDES
#include <pslnfwbasecontainer.h>

// CONSTANTS
// Download item index
const TInt KPslnDownloadItemIndex = 0;
// View item text length
const TInt KPslnAsItemMaxTextLength = 128;

// FORWARD DECLARATION
class CXnODT;
class CPslnActiveIdlePlugin;

// CLASS DECLARATION

/**
*  CPslnActiveIdlePluginContainer container class
*  @since S60 v3.1
*/
NONSHARABLE_CLASS( CPslnActiveIdlePluginContainer ) :
    public CPslnFWBaseContainer
    {
    public:

        CPslnActiveIdlePluginContainer(
                    TBool aGSCalling,
                    CPslnActiveIdlePlugin* aPlugin);
        /**
        * Symbian OS constructor.
        * @param aRect Listbox's rect.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CPslnActiveIdlePluginContainer();

        /**
        * Return currently active selection from list.
        * @return currently selected skin index from list view.
        */
        TInt GetCurrentlySelectedIndex() const;

        /**
        * Adds new item to the listbox.
        * @param aItemIndex index to add.
        * @param ODT of the theme to add.
        * @since 3.2
        */
        void AddNewItemToListL(
            const TInt aItemIndex,
            CXnODT* aSkinODT, TInt aUiController );

        /*
        * Removes number of themes from listbox.
        * @param aDeleteFrom start point index where to start deleting.
        *        All themes from this on are deleted.
        * @since 3.2
        */
        void RemoveThemes( const TInt aDeleteFrom );
        
        /**
         * Method for telling to update the listing.
         */
        void RefreshList();
        
    private:

        /* From CPslnFWBaseContainer. */
        void ConstructListBoxL( TInt aResLbxId );

        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /**
        * Adds icons to setting item list.
        */
        void AddIconsToListL();

    private: // data

        /**
        * Listbox array for items.
        * Not own.
        */
        CDesCArray* iItemArray;

        /**
        * List of items from resources.
        * Own.
        */
        CDesCArrayFlat* iItems;

        /**
        * Index of currently selected item.
        */
        TInt iCurrentlySelected;
        
        /**
         * Pointer to psln plugin.
         * Not own.
         */
        CPslnActiveIdlePlugin* iPlugin;

        /**
         * Indicate if gs started us
         */
        TBool iGSCalling;
    
    };

#endif //C_PSLNACTIVEIDLEPLUGINCONTAINER_H

// End of File
