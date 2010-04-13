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
* Description:   Base class for GS plugin containers.
*
*/


#ifndef GSBASECONTAINER_H
#define GSBASECONTAINER_H

// INCLUDES
#include    <coeccntx.h>
#include    <eikclb.h>

// CLASS DECLARATION

/**
*  Base class for GS view containers. Use not necessary.
*
*  @lib GSFramework.lib
*  @since Series60_3.1
*/
class CGSBaseContainer : public CCoeControl
{
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        IMPORT_C virtual void ConstructL( const TRect& aRect ) = 0;

        /**
        * Constructs all common parts of the container.
        *
        * @param aRect Listbox control's rect.
        * @param aResTitleId Resource id for title.
        * @param aResLbxId Resource id for listbox.
        */
        IMPORT_C void BaseConstructL(   const TRect& aRect,
                                        TInt aResTitleId,
                                        TInt aResLbxId );

        /**
        * Constructor.
        */
        IMPORT_C CGSBaseContainer();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CGSBaseContainer();

    public: // New

        /**
        * Sets selected item in the listbox.
        * @param aIndex Index of the item.
        */
        IMPORT_C virtual void SetSelectedItem( TInt aIndex );

        /**
        * Returns selected item in the listbox.
        * @return Seleced item index or 0 if no valid lbx.
        */
        IMPORT_C virtual TInt SelectedItem();

        /**
        * Set focus on the selected listbox. For animated skins feature.
        */
        IMPORT_C void FocusChanged( TDrawNow aDrawNow );

    public: // From CCoeControl

        /**
        * See base class.
        */
        IMPORT_C void HandleResourceChange( TInt aType );

    protected: // New

        /**
        * Constructs listbox.
        * @param aResLbxId Resource id for listbox.
        */
        virtual void ConstructListBoxL( TInt aResLbxId );

    private: // From CCoeControl

        /**
        * See base class.
        */
        IMPORT_C void SizeChanged();

        /**
        * See base class.
        */
        IMPORT_C TInt CountComponentControls() const;

        /**
        * See base class.
        */
        IMPORT_C CCoeControl* ComponentControl( TInt /*aIndex*/ ) const;

        /**
        * See base class.
        */
        IMPORT_C TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType );

    public: // Data

        // Pointer to newly created list box.
        CEikTextListBox* iListBox;

    protected: // Data

        // European look and feel.
        TBool iElaf;
};

#endif // GSBASECONTAINER_H
// End of File
