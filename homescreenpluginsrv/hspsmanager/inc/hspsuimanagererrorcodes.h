/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Includes the error codes of UI Manager.
*
*/


#ifndef __hspsUIMANAGERERRORCODES_H__
#define __hspsUIMANAGERERRORCODES_H__

/**
 *
 * Error code definitions used in UI Manager
 *
 */


// CONSTANTS
/**
 * Wrong manifest file.
 */
const TInt KErrWrongManifestFile                    = -32274;

/**
 * Application UID element is not found from the manifest file.
 */
const TInt KErrAppUidDefinitionMissing              = -32275;

/**
 * Provider UID element is not found from the manifest file.
 */
const TInt KErrProviderUidDefinitionMissing         = -32276;

/**
 * Theme UID element is not found from the manifest file.
 */
const TInt KErrThemeUidDefinitionMissing            = -32277;

/**
 * Parsing of the manifest file failed.
 */
const TInt KErrManifestFileCorrupted                = -32278;

/**
 * Theme full name element not found from the manifest file.
 */
const TInt KErrThemeFullNameDefinitionMissing       = -32279;

/**
 * Theme short name element not found from the manifest file.
 */
const TInt KErrThemeShortDefinitionNameMissing      = -32280;

/**
 * Theme version element not found from the manifest file.
 */
const TInt KErrThemeVersionDefinitionMissing        = -32281;

/**
 * The XML file name element not found from the manifest file.
 */
const TInt KErrXmlFileDefinitionMissing             = -32282;

/**
 * The XML file not found.
 */
const TInt KErrXmlFileNotFound                      = -32283;

/**
 * The CSS file name element not found from the manifest file.
 */
const TInt KErrCssFileDefinitionMissing             = -32284;

/**
 * The CSS file not found.
 */
const TInt KErrCssFileNotFound                      = -32285;

/**
 * The resource file not found.
 */
const TInt KErrResourceFileNotFound                 = -32286;

/**
 * The DTD file name element is not found from the main manifest file.
 */
const TInt KErrDtdFileNotFound                      = -32287;

/**
 * The application UID specified in the manifest file is not within 
 * the correct range (0x00000000 - 0xFFFFFFFF).
 */
const TInt KErrWrongAppUid                          = -32288;

/**
 * The installation is not allowed for some reason.
 */
const TInt KErrIllegalInstallation                  = -32289;

/**
 * The DAT file not found.
 */
const TInt KErrDatFileNotFound                      = -32290;

/**
 * The installation failed because there was a locked theme with
 * the same identity on ROM.
 */
const TInt KErrThemeStatusLocked                      = -32291;

/**
 * The interface UID specified in the manifest file is not within 
 * the correct range (0x00000000 - 0xFFFFFFFF).
 */
const TInt KErrWrongIfUid                          = -32292;

/**
 * Unspecified Xuikon error.
 */
const TInt KErrOtherXuikonError                     = -322122;


#endif      // __hspsUIMANAGERERRORCODES_H__   
            
// End of File
