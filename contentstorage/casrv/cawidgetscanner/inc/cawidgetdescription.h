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
    void SetMmcIdL( const TDesC& aMmcId );

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
     * Path setter
     * @param aLibrary widget library
     */
    void SetPathL( const TDesC& aPath );

    /*
     * Path getter
     * @return widget library
     */
    TPtrC GetPath( ) const;

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
     * Used flag setter
     * @param aUsed flag
     */
    void SetUsed( TBool aUsed );

    /*
     * Content arsenal entry id getter
     * @return CA entry id
     */
    TInt GetEntryId( ) const;

    /*
     * MMC id getter
     * @return CA entry id
     */
    TPtrC GetMmcId( ) const;

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
     * Used flag
     * @return ETrue if flag used is set
     */
    TBool IsUsed( ) const;
    

    /*
     * Used flag
     * @return ETrue if flag visible is set
     */
    TBool IsVisible( ) const;

    /*
     * Entry getter
     * @return entry representing widget
     */
    CCaInnerEntry* GetEntryLC( ) const;

    /*
     * Modification time setter
     * @param aModificationTime
     */
    void SetModificationTimeL( const TDesC& aModificationTime );

    /*
     * Modification time getter
     * @return widget Modification time
     */
    TPtrC GetModificationTime() const;

    /*
     * Remove MMC id.
     */
    void RemoveMmcId();
    /*
     * Set service fw xml filename
     * @param aServiceXml Service xml filename
     */
    void SetServiceXmlL(const TDesC& aServiceXml);
    /*
     * Get service fw xml filename
     * @return Service xml filename
     */
    TPtrC GetServiceXml() const;

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
    RBuf iMmcId;

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
    RBuf iPath;

    /*
     * Widget library. Own
     */
    RBuf iLibrary;

    /*
     * Widgets flags.
     */
    TInt iFlags;

    /*
     * Modification time.
     */
    RBuf iModificationTime;
    /*
     * Service fw xml file
     */
    RBuf iServiceXml;
    };

typedef RPointerArray<CCaWidgetDescription> RWidgetArray;

#endif      // C_WIDGETDESCRIPTION_H
// End of File
