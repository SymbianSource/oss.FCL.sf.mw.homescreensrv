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
#ifndef C_WIDGETDESCRIPTION_H
#define C_WIDGETDESCRIPTION_H

//  INCLUDES
#include <e32base.h>
// CONSTANTS

// FORWARD DECLARATIONS
class CCaInnerEntry;

/**
 *  Widget Parser
 *
 *  @since Series Series60 5.x
 */
class CCaWidgetDescription : public CBase
    {

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCaWidgetDescription* NewL( );
    static CCaWidgetDescription* NewLC( );
    static CCaWidgetDescription* NewLC( CCaInnerEntry* aEntry );

    /**
     * Destructor.
     */
    virtual ~CCaWidgetDescription();

    /**
     * Static method to compare two CCaWidgetDescription objects
     * @param aFirst first item to compare
     * @param aSecond second item to compare
     * @return ETrue if iLibrary value is the same in both objects,
     * otherwise returns EFalse
     */
    static TBool Compare( const CCaWidgetDescription& aFirst,
            const CCaWidgetDescription& aSecond);

    /**
     * Method to compare two CCaWidgetDescription objects
     * @param aToCompare first item to compare
     * @return ETrue if all value is the same in both objects,
     * otherwise returns EFalse
     */
    TBool Compare( const CCaWidgetDescription& aToCompare );

    /*
     * MMC id setter
     * @param aMmcId MMC id
     */
    void SetMmcId( TUint aMmcId );

    /*
     * PackageUid setter
     * @param aPackageUid package UID
     */
    void SetPackageUidL( const TDesC& aPackageUid );

    /*
     * Title setter
     * @param aTitle widget title
     */
    void SetTitleL( const TDesC& aTitle );

    /*
     * Description setter
     * @param aTitle widget title
     */
    void SetDescriptionL( const TDesC& aDescription );

    /*
     * Uri setter
     * @param aUri widget uri
     */
    void SetUriL( const TDesC& aUri );

    /*
     * Icon Uri setter
     * @param aIconUri widget icon uri
     */
    void SetIconUriL( const TDesC& aIconUri );

    /*
     * Library setter
     * @param aLibrary widget library
     */
    void SetLibraryL( const TDesC& aLibrary );

    /*
     * Missing flag setter
     * @param aMissing flag
     */
    void SetMissing( TBool aMissing );

    /*
     * Visible flag setter
     * @param aVisible flag
     */
    void SetVisible( TBool aVisible );

    /*
     * Content arsenal entry id getter
     * @return CA entry id
     */
    TInt GetEntryId( ) const;

    /*
     * MMC id getter
     * @return CA entry id
     */
    TUint GetMmcId( ) const;

    /*
     * Library getter
     * @return widget library
     */
    TPtrC GetLibrary( ) const;

    /*
     * Description getter
     * @return widget Description
     */
    TPtrC GetDescription( ) const;

    /*
     * Uri getter
     * @return widget Uri
     */
    TPtrC GetUri( ) const;

    /*
     * IconUri getter
     * @return widget IconUri
     */
    TPtrC GetIconUri( ) const;

    /*
     * Title getter
     * @return widget IconUri
     */
    TPtrC GetTitle( ) const;

    /*
     * Missing flag
     * @return ETrue if flag missing is set
     */
    TBool IsMissing( ) const;

    /*
     * Entry getter
     * @return entry representing widget
     */
    CCaInnerEntry* GetEntryLC( ) const;

private:

    /*
     * Library name getter
     * @return libraru name
     */
    TPtrC GetLibraryName( ) const;

    /*
     * Flag setter
     * @param aFlag flag
     * @param aValue flag value
     */
    void SetFlag( TInt aFlag, TBool aValue );

private:

    /**
     * C++ default constructor.
     */
    CCaWidgetDescription();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );
    void ConstructL( CCaInnerEntry* aEntry );


private:
    // Data

    /*
     * Content Storage entry id
     */
    TInt iEntryId;

    /*
     * Package UID
     */
    TUint iPackageUid;

    /*
     * MMC id
     */
    TUint iMmcId;

    /*
     * Widget title. Own
     */
    RBuf iTitle;

    /*
     * Widget description. Own
     */
    RBuf iDescription;

    /*
     * Widget uri. Own
     */
    RBuf iUri;

    /*
     * Widget icon uri. Own
     */
    RBuf iIconUri;


    /*
     * Widget library. Own
     */
    RBuf iLibrary;

    /*
     * Widgets flags.
     */
    TInt iFlags;
    };

typedef RPointerArray<CCaWidgetDescription> RWidgetArray;

#endif      // C_WIDGETDESCRIPTION_H
// End of File
