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

// INCLUDE FILES
#include "widgetscannerutils.h"
#include "cainnerentry.h"
#include "cadef.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WidgetScannerUtils::IsFileOnDrive(const TDesC& aFileName,
    const DriveInfo::TDefaultDrives& aDefaultDrive) 
{
    if (aFileName.Length()) {
        TInt drive;
        TInt err = DriveInfo::GetDefaultDrive(aDefaultDrive, drive);
        if (!err) {
            TInt fileDrive;
            err = RFs::CharToDrive(aFileName[0], fileDrive);
            if (!err && fileDrive == drive) {
                return ETrue;
            }
        }
    }
    return EFalse;
}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint WidgetScannerUtils::CurrentMmcId( RFs& aFs )
    {
    TUint mmcId = 0;
    TInt mmcDrive;
    if ( DriveInfo::GetDefaultDrive(
            DriveInfo::EDefaultRemovableMassStorage, mmcDrive ) == KErrNone )
        {
        TVolumeInfo volumeInfo;
        if( aFs.Volume( volumeInfo, mmcDrive ) == KErrNone )
            {
            mmcId = volumeInfo.iUniqueID;
            }
        }
    return mmcId;
    }

//  End of File  
