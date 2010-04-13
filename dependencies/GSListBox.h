/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     class description for CGSListBox & CGSListBoxItemTextArray.
*
*/



#ifndef GS_LIST_BOX_H
#define GS_LIST_BOX_H

#include    <e32base.h>
#include    <e32std.h>
#include    <barsread.h>
#include    <coemain.h>

class CAknRadioButtonSettingPage;
class CEikListBox;


/*================================================
* CGSListboxEntry: class to map feature->item index when selected
* from a listbox. This is required when there are features which are
* dynamically shown/hidden.
================================================*/
class CGSListboxEntry : public CBase
	{
	public:
		IMPORT_C static CGSListboxEntry* NewLC( TResourceReader& aReader );
		IMPORT_C static CGSListboxEntry* NewLC( TInt aFeatureId );
		IMPORT_C static CGSListboxEntry* NewLC( 
            TUint32 aResId, CCoeEnv& aEnv, TInt aFeatureId );
			IMPORT_C ~CGSListboxEntry();

	public: // Accessors
		IMPORT_C TInt FeatureId() const;
		IMPORT_C const TDesC& Caption() const;
		IMPORT_C TInt Type() const;

    public: // Manipulators
		IMPORT_C void SetDynamicTextL( const TDesC& aText );
		IMPORT_C void SetDynamicTextDoublePaneL( const TDesC& aText );

	private:
		CGSListboxEntry();
		void ConstructL( TResourceReader& aReader );
		void ConstructL( TUint32 aResId, CCoeEnv& aEnv, TInt aFeatureId );
		
	private:
		inline TInt StaticTextLength() const { return iStaticTextLength; }

	private:

		/** 
		 * The length of the static text which does not change
		 */
		TInt iStaticTextLength;

		/**
		 * The static text and dynamic text combined. We can always
		 * change the dynamic part due to the fact that we know
		 * the length (prefix) of the static part.
		 */
		HBufC* iCaption;

		/**
		 * One of either EGSListBoxItemTypeIsAlwaysVisible or
		 * EGSListBoxItemTypeIsDynamic
		 */
		TInt iType;

		/**
		 * The id of this feature
		 */
		TInt iFeatureId;
		
		/**
		* Maximum Length of the Dynamic text which doesnot change.. 
		* This has been added after inclusion Double Pane List Box
		*/
		TInt iDynamicTextSize;
	};



/*================================================
================================================*/
class CGSItemTextArray : public CBase, public MDesCArray
    {
	public:
	    enum TVisibility
		    {
		    EInvisible = 0,
		    EVisible = 1
		    };

    public:
        IMPORT_C static CGSItemTextArray* NewL(
            TUint32 aResId, 
            CCoeEnv& aEnv );
		~CGSItemTextArray();

	protected:
		CGSItemTextArray();
		void ConstructL( TUint32 aResId, CCoeEnv& aEnv );
		void ConstructL( TUint32 aResId, CCoeEnv& aEnv, TInt aCount ); 
		
    public: // From MDesCArray
		IMPORT_C TInt MdcaCount() const;
		IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

	public: // API
		
		/** 
		 * Used to add dynamic text to the end of an existing model item
		 */
		IMPORT_C void SetDynamicTextL( TInt aFeatureId, 
            const TDesC& aAdditionalText );

        /** 
		 * Used to add dynamic text to the end of an existing model item 
		 * When Double Pane List box is used CAknDoubleStyleListBox
		 */
		IMPORT_C void SetDynamicTextDoublePaneL( TInt aFeatureId, 
            const TDesC& aAdditionalText );
            
		/**
		 * Specify that this item should be visible / invisible in the
		 * iCurrentlyDisplayedEntries array
		 */
		IMPORT_C void SetItemVisibilityL( TInt aFeatureId, 
            TVisibility aVisibility );

		/**
		 * Access the id of the currently selected feature
		 */
        IMPORT_C virtual TInt CurrentFeature() const;

		/**
		 * Set the currently selected feature id
		 */
        IMPORT_C TInt IndexForFeatureIdL(TInt aFeatureId) const;

		/**
		 * Returns the feature id corresponding to an index
		 */
        IMPORT_C TInt FeatureIdByIndex(TInt aIndex) const;

        /**
		 * 
		 */
		IMPORT_C const CGSListboxEntry* FeatureByIdL( TInt aFeatureId ) const;

        /**
		 * 
		 */
        IMPORT_C static HBufC* GetItemCaptionFromFeatureIdLC(TUint32 aResourceId, TInt aFeatureId, CCoeEnv& aCone);


	private:
		static TBool CompareEntries( const CGSListboxEntry& aLeft, 
			const CGSListboxEntry& aRight );


	protected:

		/**
		 * All the entries which may potentially be contained within the listbox
		 * model. This array is owned and destroyed by this class.
		 */
		RPointerArray< CGSListboxEntry > iAllEntries;

		/**
		 * The entries which are currently displayed within the listbox. These
		 * pointers are actually owned in the iAllEntries array and therefore
		 * this array must not be 'ResetAndDestroyed()' (but rather just Reset())
		 * from within this class' destructor.
		 */
		RPointerArray< CGSListboxEntry > iCurrentlyDisplayedEntries;
    };


/*================================================
================================================*/
class CGSRadioButtonSettingPageItemTextArray : public CGSItemTextArray
    {
	public:
		IMPORT_C static CGSRadioButtonSettingPageItemTextArray* NewL(
            TUint32 aResId, 
            CCoeEnv& aEnv,
			CAknRadioButtonSettingPage* aSettingPage = NULL
            );

	private:
		CGSRadioButtonSettingPageItemTextArray( CAknRadioButtonSettingPage* aSettingPage  );

    public: // FROM CGSItemTextArray
    	IMPORT_C TInt CurrentFeature() const;

    public: // New API
        IMPORT_C void SetRadioButtonSettingPage( CAknRadioButtonSettingPage& aPage );

	private:

		/**
		 * Underlying setting page
		 */
		CAknRadioButtonSettingPage* iSettingPage;
    };



/*================================================
* CGSListBoxItemTextArray: This class acts as a wrapper class for
* CGSListboxEntry. Instance of this class is created from the
* calling function.
================================================*/
class CGSListBoxItemTextArray : public CGSItemTextArray
	{
	public:
		IMPORT_C static CGSListBoxItemTextArray* NewL(
            TUint32 aResId, 
			CEikListBox& aListBox,
            CCoeEnv& aEnv );
        IMPORT_C static CGSListBoxItemTextArray* NewL( 
            TUint32 aResId, 
            CEikListBox& aListBox, 
            CCoeEnv& aEnv, 
            TInt aCount );
            
 	private:
		CGSListBoxItemTextArray( CEikListBox& aListBox );

    public: // FROM CGSItemTextArray
    	IMPORT_C TInt CurrentFeature() const;

	private:

		/**
		 * Underlying listbox
		 */
		CEikListBox& iListBox;
	};

#endif //GS_LIST_BOX_H
