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


#ifndef WIDGETSCANNERUTILS_H
#define WIDGETSCANNERUTILS_H

//  INCLUDES
#include <e32base.h>
#include <driveinfo.h>

// CONSTANTS
_LIT(KAttrWidgetPath, "widget:path");
_LIT(KAttrWidgetLibrary, "widget:library");
_LIT(KAttrWidgetUri, "widget:uri");
_LIT(KSlash, "/");
_LIT(KDllExt, ".dll");
_LIT(KNoLibrary, "NoLibrary");

// FORWARD DECLARATIONS
class CCaInnerEntry;
/**
 *  Widget Scanner Utils
 *
 *  @since Series Series60 5.x
 */
class WidgetScannerUtils 
    {
public:

    /**
     * Checks if file is present on a drive
     * @param aFileName file name
     * @param aDriveLetter drive letter
     * @return ETrue if file is present, EFalse if not
     */
    static TBool IsFileOnDrive(const TDesC& aFileName,
        const DriveInfo::TDefaultDrives& aDefaultDrive);

    /**
     * Returns current MMC id
     * @param aFs file server session reference
     * @return current mmc id
     */  
    static TUint CurrentMmcId( RFs& aFs );

    };

#endif      // WIDGETSCANNERUTILS_H
// End of File
