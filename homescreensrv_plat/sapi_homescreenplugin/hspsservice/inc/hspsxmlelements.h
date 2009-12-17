/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef HSPSXMLELEMENTS_H_
#define HSPSXMLELEMENTS_H_
		
// Xmluiml element
_LIT8( KXmlUimlElement,              	"xmluiml" );      		// Root element
_LIT8( KXmlUimlAttrXmlns,				"xmlns");				// Not required

//common elements
_LIT8( KAttrId,                   "id" );
_LIT8( KAttrName,                 "name" );

// Configuration element
_LIT8( KConfigurationElement,        	"configuration" );
_LIT8( KConfigurationAttrId,         	"id" );					// Unique for this configuration 
_LIT8( KConfigurationAttrType,       	"type" );				// root/view/widget
_LIT8( KConfigurationAttrInterface,  	"interface" );			// AI3/?
_LIT8( KConfigurationAttrUid,        	"uid" );				// Globally unique uid, identifies specific configuration
_LIT8( KConfigurationAttrName,        	"name" );		
_LIT8( KConfigurationAttrState,         "state" );       
_LIT8( KConfigurationAttrMaxChild,      "max_child" );
_LIT8( KConfigurationAttrMultiInstance, "multiinstance" );      // Multiinstance flag
_LIT8( KConfigurationAttrDescription,   "desc" );               // Widget's description 
_LIT8( KConfigurationAttrLockingStatus, "locking_status" );     // Locking status

// Configuration element state attribute values
_LIT8( KConfStateNotConfirmed,          "NotConfirmed" );       // Not confirmed state
_LIT8( KConfStateWaitForConfirmation,   "WaitForConfirmation" );// Wait for confirmation state
_LIT8( KConfStateConfirmed,             "Confirmed" );          // Confirmed state
_LIT8( KConfStateError,                 "Error" );              // Error state

// Control element
_LIT8( KControlElement, 				"control" );
_LIT8( KControlAttrName, 				"name" );				// Localized name of the configuration

// Plugins element
_LIT8( KPluginsElement, 				"plugins" );

// Plugin element
_LIT8( KPluginElement, 					"plugin" );				
_LIT8( KPluginAttrId, 					"id" );					// Unique for this configuration
_LIT8( KPluginAttrUid, 					"uid" );				// Globally unique uid, identifies specific configuration
_LIT8( KPluginAttrInterface, 			"interface" );			// AI3/?
_LIT8( KPluginAttrName, 				"name" );				// Localized name of the plugin
_LIT8( KPluginAttrActive,               "active" );             // Plugin activation state

// Settings element
_LIT8( KSettingsElement, 				"settings" );
_LIT8( KSettingsAttrId, 				"id" );					// Identifies the personalized plugin

// Item
_LIT8( KItemElement, 					"item" );				// 
_LIT8( KItemAttrId, 					"id" );					// Name of the attributes container
_LIT8( KItemAttrName, 					"name" );				// Localized name

// Attribute
_LIT8( KAttributeElement,				"attribute" );			
_LIT8( KAttributeAttrKey,				"key" );				// References personalized property
_LIT8( KAttributeAttrName,				"name" );				// Localized key value

// Property
_LIT8( KPropertyElement,				"property" );
_LIT8( KPropertyAttrName,				"name" );				// Property name
_LIT8( KPropertyAttrValue,				"value" );				// Property value

// Resources
_LIT8( KResourcesElement, 				"resources" );

// Object
_LIT8( KObjectElement, 					"object" );
_LIT8( KObjectAttrFilename,				"filename" ); 
_LIT8( KObjectAttrName, 				"name" );
_LIT8( KObjectAttrPath, 				"path" );
_LIT8( KObjectAttrMediatype,			"mediatype" ); 
_LIT8( KObjectAttrTag,                  "tag" );
_LIT8( KObjectAttrType, 				"type" );

// Tag values
_LIT8( KObjectAttrTagLogo,              "logo");
_LIT8( KObjectAttrTagPreview,           "preview");
#endif /*HSPSXMLELEMENTS_H_*/
