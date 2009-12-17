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
* Description:  Defines the XML elements and values of a manifest file.
*
*/


#ifndef HSPSMANIFEST_H_
#define HSPSMANIFEST_H_

// XML elements from the manifest.dat file
_LIT8(KPackage,						"package");

// Attributes for KPackage
_LIT8(KPackageVersion, 				"version");

_LIT8(KFamily,                      "family");
_LIT8(KConfigurationType,			"type");
_LIT8(KInterfaceUid,				"interfaceuid");
_LIT8(KProviderUid,					"provideruid");
_LIT8(KThemeUid,					"configurationuid");
_LIT8(KThemeStatus,					"status");
_LIT8(KThemeFullName,				"fullname");
_LIT8(KThemeShortName,				"shortname");
_LIT8(KThemeVersion,				"version");
_LIT8(KThemeDesc,                   "description");
_LIT8(KFileLogo,                    "filelogo");
_LIT8(KFilePreview,                 "filepreview");
_LIT8(KFileXML,           			"filexml");
_LIT8(KFileDTD,						"filedtd");
_LIT8(KLocalized,                   "localization");
_LIT8(KFileResource,      			"fileresource");
_LIT8(KMultiInstance,               "multiinstance");

// Options for KFamily
_LIT8(KFamilyQvga,                  "qvga");
_LIT8(KFamilyQvga2,                 "qvga2");
_LIT8(KFamilyVga,                   "vga");
_LIT8(KFamilyVga3,                  "vga3");
_LIT8(KFamilyQhd,                   "qhd");
_LIT8(KFamilyQhd_tch,               "qhd_tch");
_LIT8(KFamilyVga_tch,               "vga_tch");

// Options for KConfigurationType
_LIT8(KManifestTypeApp,				"application");
_LIT8(KManifestTypeView,			"view");
_LIT8(KManifestTypeWidget,			"widget");
_LIT8(KManifestTypeTemplate,		"template");

// Options for KThemeStatus
_LIT8(KStatusNone,					"statusnone");
_LIT8(KStatusLicenceeDefault, 		"statuslicenceedefault");
_LIT8(KStatusLicenceeRestorable, 	"statuslicenceerestorable");
_LIT8(KStatusOperatorDefault, 		"statusoperatordefault");
_LIT8(KStatusUserDefault,			"statususerdefault");
_LIT8(KStatusMakeActive,			"statusmakeactive");
_LIT8(KStatusLocked,				"statuslocked");

// Additional values for KMulitiInstance
_LIT8( KMultiInstanceUnlimited, "unlimited" ); 
_LIT8( KMultiInstanceHidden, "hidden" );
const TInt32 KMultiInstanceUnlimitedValue = -1;
const TInt32 KMultiInstanceHiddenValue = 0;
const TInt32 KMultiInstanceDefaultValue = 1;
const TInt32 KMultiInstanceMinimumCountValue = 0;
const TInt32 KMultiInstanceMaximumCountValue = 32767;



// Attributes for KFileResource
_LIT8(KMediaType,					"mediatype");
_LIT8(KTag,                         "tag");

#endif /*HSPSMANIFEST_H_*/

// End of File
