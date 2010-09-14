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
* Description:  Defines the XML elements and values of a configuration file.
*
*/


#ifndef HSPSCONFIGURATION_H_
#define HSPSCONFIGURATION_H_
		
// Xmluiml element
_LIT8( KXmlUimlElement,              	"xmluiml" );      		// Root element
_LIT8( KXmlUimlAttrXmlns,				"xmlns");				// Not required or used in HSPS

// Configuration element
_LIT8( KConfigurationElement,        	"configuration" );
_LIT8( KConfigurationAttrId,         	"id" );					// Unique id for the configuration elements 
_LIT8( KConfigurationAttrType,       	"type" );				// root/view/widget/template
_LIT8( KConfigurationAttrInterface,  	"interface" );			// Uid of Xuikon technology
_LIT8( KConfigurationAttrUid,        	"uid" );				// Globally unique uid, identifies specific configuration
_LIT8( KConfigurationAttrName,        	"name" );				// Localized name of the configuration
_LIT8( KConfigurationAttrNameEntity,  	"_name" );				// Logical string id of the Name attribute (DTD entity)
_LIT8( KConfigurationAttrState,         "state" );              // State of the configuration
_LIT8( KConfigurationAttrVersion,       "version" );            // Configuration version
_LIT8( KConfigurationAttrMultiInstance, "multiinstance" );      // Multiinstance flag
_LIT8( KConfigurationAttrDesc,          "desc" );               // Localized description
_LIT8( KConfigurationAttrDescEntity,    "_desc" );              // Logical string id of the Description attr (DTD entity)
_LIT8( KConfigurationAttrMaxChild,      "max_child" );          // Maximum child count
_LIT8( KConfigurationAttrLocking,       "locking_status" );     // Locking status


// Configuration element type attribute values
_LIT8( KConfTypeApp,                    "application" );        // Application configuration
_LIT8( KConfTypeView,                   "view" );               // View configuration
_LIT8( KConfTypeWidget,                 "widget" );             // Widget configuration
_LIT8( KConfTypeTemplate,               "template" );           // Template configuration

// Configuration element state attribute values
_LIT8( KConfStateNotConfirmed,          "NotConfirmed" );       // Not confirmed state
_LIT8( KConfStateWaitForConfirmation,   "WaitForConfirmation" );// Wait for confirmation state
_LIT8( KConfStateConfirmed,             "Confirmed" );          // Confirmed state
_LIT8( KConfStateError,                 "Error" );              // Error state

// Options for the KConfigurationAttrLocking 
_LIT8( KConfLockingLocked,              "locked" );             // Locked state
_LIT8( KConfLockingPermanent,           "permanent" );          // Permanent state (not removable, not movable)
_LIT8( KConfLockingNone,                "none" );               // Default

// Control element
_LIT8( KControlElement, 				"control" );
_LIT8( KControlAttrName, 				"name" );				// Localized name of the configuration
_LIT8( KControlAttrNameEntity,  		"_name" );				// DTD entity, used to localize the name attribute from DTD files

// Plugins element
_LIT8( KPluginsElement, 				"plugins" );

// Plugin element
_LIT8( KPluginElement, 					"plugin" );				
_LIT8( KPluginAttrId, 					"id" );					// Unique id for the plugin elements
_LIT8( KPluginAttrUid, 					"uid" );				// Globally unique uid, identifies specific configuration
_LIT8( KPluginAttrInterface, 			"interface" );			// uid of AI3/etc
_LIT8( KPluginAttrName, 				"name" );				// Localized name of the plugin
_LIT8( KPluginAttrNameEntity,  			"_name" );				// DTD entity, used to localize the name attribute from DTD files
_LIT8( KPluginAttrActive,               "active" );             // Plugin active state 

// Plugin element active state attribute values.
_LIT8( KPluginActiveStateNotActive,     "0" );                  // Not active
_LIT8( KPluginActiveStateActive,        "1" );                  // Active

// Settings element
_LIT8( KSettingsElement, 				"settings" );
_LIT8( KInitialSettingsElement,         "initial_settings");

// Item
_LIT8( KItemElement, 					"item" );				// 
_LIT8( KItemAttrId, 					"id" );					// Name of the attributes container
_LIT8( KItemAttrName, 					"name" );				// Localized name
_LIT8( KItemAttrNameEntity,				"_name" );				// DTD entity, used to localize the name attribute from DTD files

// Property
_LIT8( KPropertyElement,				"property" );
_LIT8( KPropertyAttrName,				"name" );				// Property name
_LIT8( KPropertyAttrValue,				"value" );				// Property value

// Resources
_LIT8( KResourcesElement, 				"resources" );

// Object
_LIT8( KObjectElement, 					"object" );
_LIT8( KObjectAttrFilename,				"name" );
_LIT8( KObjectAttrMediatype,			"mediatype" );
_LIT8( KObjectAttrPath,					"path" );
_LIT8( KObjectAttrTag,                  "tag" );

// Tags for resource objects
_LIT8( KObjectAttrTagLogo,              "logo");  
_LIT8( KObjectAttrTagPreview,           "preview");

#endif /*HSPSCONFIGURATION_H_*/

// End of File
