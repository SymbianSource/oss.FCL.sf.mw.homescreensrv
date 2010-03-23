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


#ifndef C_WIDGETSCANNERPARSER_H
#define C_WIDGETSCANNERPARSER_H

//  INCLUDES
#include <e32base.h>
#include <xmlengdomimplementation.h>
#include <xmlengelement.h>
#include <badesca.h>
#include <f32file.h>
#include "cawidgetdescription.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CDir;

/**
 *  Widget Parser
 *
 *  @since Series Series60 5.x
 */
class CCaWidgetScannerParser : public CBase
    {

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCaWidgetScannerParser* NewL( RFs& aFs );
    static CCaWidgetScannerParser* NewLC( RFs& aFs );

    /**
     * Destructor.
     */
    virtual ~CCaWidgetScannerParser();

    /**
     * WidgetsL
     * @return array containing widgets description
     */
    RWidgetArray& WidgetsScanL( );

private:

    /**
     * Looks for manifests on a drive
     * @param aDriveLetter drive letter
     */
    void ScanOnDriveL( );

    /**
     * Parses all manifest files in a directory
     * @param aDriveLetter drive letter
     * @param aDirectoryName directory name
     */
    void ParseDirectoryL( const TDesC& aDirectoryName );

    /**
    * Parse manifest file
    * @param aFilePath full file path to manifest file
    * @param aPackageUid descriptor containing package uid
    */
    void ParseManifestFileL( const TDesC& aFilePath,
        const TDesC& aPackageUid);

    /**
    * Parse a widget, store parsed object in an internal array
    * @param aFilePath full file path to manifest file
    * @param aElement widget element
    * @param aPackageUid descriptor containing package uid
    */
    void ParseWidgetL( const TDesC& aFilePath,
            TXmlEngElement aElement, const TDesC& aPackageUid );

    /**
     * Gets all directories from import folder
     * @param aDriveLetter drive letter
     * @return directories
     */
    CDir* GetDirectoriesLC( );

    /**
     * Returns import path
     * @param aDriveLetter drive letter
     * @return import path
     */
    HBufC* FullPathLC( );

    /**
     * Returns manifest directory path
     * @param aDriveLetter drive letter
     * @param aDirectoryName directory name
     * @return manifest directory path
     */
    HBufC* GetManifestDirectoryPathLC( const TDesC& aDirectoryName );

    /**
     * Sets widgets URI
     * @param aElement xml element
     * @param aWidget widget to set value
     */
    void SetUriL( TXmlEngElement & aElement, CCaWidgetDescription * aWidget );

    /**
     * Sets widgets library
     * @param aElement xml element
     * @param aPackageUid package uid
     * @param aWidget widget to set value
     */
    void SetLibraryL( TXmlEngElement & aElement, const TDesC & aPackageUid,
            CCaWidgetDescription * aWidget );

    /**
     * Sets widgets title
     * @param aElement xml element
     * @param aWidget widget to set value
     */
    void SetTitleL( TXmlEngElement & aElement, 
            CCaWidgetDescription * aWidget );

    /**
     * Sets widgets description
     * @param aElement xml element
     * @param aWidget widget to set value
     */
    void SetDescriptionL( TXmlEngElement & aElement,
            CCaWidgetDescription * aWidget );

    /**
     * Sets widgets visibility
     * @param aElement xml element
     * @param aWidget widget to set value
     */
    void SetVisibilityL( TXmlEngElement & aElement,
            CCaWidgetDescription * aWidget );

    /**
     * Sets icons URI
     * @param aElement xml element
     * @param aPackageUid package uid
     * @param aWidget widget to set value
     */
    void SetIconUriL( TXmlEngElement & aElement, const TDesC & aPackageUid,
            CCaWidgetDescription * aWidget );

    /**
     * Sets widgets mmc id
     * @param aWidget widget to set value
     */
    void SetMmcIdL( CCaWidgetDescription * aWidget );

private:

    /**
     * C++ default constructor.
     */
    CCaWidgetScannerParser( RFs& aFs );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );

private:
    // Data

    /*
     * Stores current drive letter
     */
    TChar iCurrentDriveLetter;

    /*
     * File server session.Not Own
     */
    RFs iFs;

    /*
     * Path to the private directory (wihout drive).own
     */
    RBuf iImportPath;

    /*
     * Widgets. Own.
     */
    RWidgetArray iWidgets;
    /*
     * Own.
     */
    RXmlEngDOMImplementation iDomImpl;

    };

#endif      // C_WIDGETSCANNERPARSER_H
// End of File
