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
* Description:  Build liw list from a DomDocument.
*
*/


#ifndef _HSPSLIWUTILITIES_H_
#define _HSPSLIWUTILITIES_H_

#include <e32base.h>
#include <liwcommon.h>
#include <hspsdomdocument.h>
#include <hspsdomlist.h>
#include <hspsodt.h>
#include <hspsthememanagement.h>

#include "hspsliwvocabulary.hrh"
/*
* @ingroup group_hsps_sapi
* CHspsLiwUtilies is used to build liw based lists according to Dom Document received 
* from HSPS server. It is also responsible for parsing liw based input parammeter list received from SAPI user.
*/
class CHspsLiwUtilities : public CBase
    {
    // Data types
    // Plugin map items (bit mask)
    enum
        {
        EPluginMapId = 1,               // bit 1
        EPluginMapUid = 2,              // bit 2
        EPluginMapName = 4,             // bit 3
        EPluginMapType = 8,             // bit 4
        EPluginMapActivationState = 16, // bit 5
        EPluginMapDesc = 32,            // bit 6
        EPluginMapLocked = 64           // bit 7
        };
    
    //
    // Construction
    //
    public:	

        virtual ~CHspsLiwUtilities();
        static CHspsLiwUtilities* NewL();
        static CHspsLiwUtilities* NewLC();

    private:

        CHspsLiwUtilities();	
        void ConstructL();

    //
    // New methods
    //
    public:
        /**
         * Adds common status element to the output parameter list
         * @param aStatus Operation status 
         * @param aOutParamList Output parameters
         */
        static void AppendStatusL( 
            TInt32 aStatus,
            CLiwGenericParamList& aOutParamList );

    	/**
    	 * Configuration interface method.
    	 * Gets a plugin from a configuration.
    	 * @param aReturnName A name to code into return list.
    	 * @param aPluginId An idetifier of the plugin that is to be returned.
    	 * @param aDomDocument A document where the plugin is searched from.
    	 * @param aOutParamList Container for the return value.
    	 */
    	void GetConfigurationL( 
            const TDesC8& aReturnName, 
            const TDesC8& aPluginId,
            ChspsDomDocument& aDomDocument,
    		CLiwGenericParamList& aOutParamList );
    
    	 /**
    	 * Configuration interface method.
    	 * Gets a settings from a plugin configuration.
         * @param aReturnName A name to code into return list.
         * @param aPluginId An idetifier of the plugin whose settings are to be returned.
         * @param aDomDocument A document where the plugin is searched from.
         * @param aSettingsDom  value.
         * @return an error code.
         */
    	TInt SetPluginSettingsNodeL(  
    	                    const CLiwList& aItemList,
    	                    TDesC8& aPluginId,
    	                    ChspsDomDocument& aDomDocument,
    	                    ChspsDomDocument& aSettingsDom );
    	        
    	/**
         * Creates GetPlugins method output parameter list
         * @param aList List of plugin header ODT's
         * @param aOutParamList Output parameters
         */
        static void GetPluginsOutputL( 
            CArrayPtrFlat<ChspsODT>& aList,
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates GetPluginList method output parameter list
         * @param aDom Dom document of active configuration
         * @param aType Requested plugin type
         * @param aPluginId Plugin which plugin list is requested
         * @param aOutParamList Output parameters
         */
        static void GetPluginListOutputL( 
            ChspsDomNode& aRootNode,
            TDesC8& aType,
            TDesC8& aPluginId,
            CLiwGenericParamList& aOutParamList );
        
        /**
         * Creates AddPlugin method output parameter list
         * @param aPluginId Id of added plugin
         * @param aOutParamList Output parameters
         */
        static void AddPluginOutputL( 
            TInt aPluginId,
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates RemovePlugin method output parameter list
         * @param aOutParamList Output parameters
         */
        static void RemovePluginOutputL( 
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates ReplacePlugin method output parameter list
         * @param aOutParamList Output parameters
         */
        static void ReplacePluginOutputL( 
            CLiwGenericParamList& aOutParamList );        
        
        /**
         * Creates SetPluginSettings method output parameter list
         * @param aOutParamList Output parameters
         */
        static void SetPluginSettingsOutputL( 
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates GetPluginSettings outputs.
         * @param aDom - reference to settings dom
         * @param aOutParamList - reference to out params list 
         */
        void GetPluginSettingsOutputL( 
        		ChspsDomDocument& aDom, 
        		CLiwGenericParamList& aOutParamList );
        
        /**
         * Creates MovePlugins method output parameter list
         * @param aOutParamList Output parameters
         */
        static void MovePluginsOutputL( 
            CLiwGenericParamList& aOutParamList );

        /**
         * Get list of plugin id's from plugins list
         * @param aPluginsList Plugins list
         * @param aPluginIdLIst List of plugin id's
         */
        static void GetPluginIdListL(
            const CLiwList* aPluginsList,
            CArrayFixFlat<TInt>& aPluginIdList );

        /**
         * Creates GetAppConfs method output parameter list
         * @param aList List of plugin header ODT's
         * @param aOutParamList Output parameters
         */
        static void GetAppConfsOutputL( 
            CArrayPtrFlat<ChspsODT>& aList,
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates SetActiveAppConf method output parameter list
         * @param aOutParamList Output parameters
         */
        static void SetActiveAppConfOutputL( 
            CLiwGenericParamList& aOutParamList );

        /**
         * Creates SetConfState method output parameter list
         * @param aOutParamList Output parameters
         */
        static void SetConfStateOutputL( 
            CLiwGenericParamList& aOutParamList );

        /**
         * Returns value of defined attribute
         * @param aNode node which attribute value is requested
         * @param aAttrId requested attribute
         * @param aAttrValue attribute value
         */
        static void GetAttributeValueL( 
            ChspsDomNode& aNode,
            const TDesC8& aAttrId,
            TPtrC8& aAttrValue );
        
        /**
         * Finds a root node for liw structure from dom document by identifier.
         * @param aNodeIdentifier An Tag.
         * @param aNodeIdentifier An Identifier.
         * @param aNode A Dom Node where the node is searched from.
         * @return Result node.
         */
        static ChspsDomNode& FindRootNodeByIdentifierL( const TDesC8& aNodeTag,
                                                 const TDesC8& aNodeIdentifier,
                                                 ChspsDomNode& aNode );
        
                
        
        /**
         * Finds a root node for liw structure from dom document by the tag.
         * @param aNodeIdentifier An Tag.

         * @param aDomDocument A document where the node is searched from.
         * @return Result node.
         */
        ChspsDomNode& FindRootNodeByTagL(const TDesC8& aNodeTag, 
                                     ChspsDomNode& aDomNode );

        /**
         * Creates SetActivePlugin method output parameter list
         * @param aOutParamList Output parameters
         */
        static void SetActivePluginOutputL( 
            CLiwGenericParamList& aOutParamList );

    private:
        
        /**
         * Copies a key-value pair to a map.
         * @param aMap              A Map where the key-value pair is added to.
         * @param aAttrList         A list where the key is searched from.
         * @param aDomName          Name of the value for the search.
         * @param aLiwName          Key name for the map.
         * @param aDefaultValue     Default value for attribute if not found from dom.
         * @return An error code.
         */
        static TInt AppendAttributeToMapL( CLiwDefaultMap& aMap, 
                         ChspsDomList& aAttrList, 
                         const TDesC8& aDomName,
                         const TDesC8& aLiwName,
                         const TDesC8& aDefaultValue = KHspsLiwEmptyValue() );
    
        /**
         * Builds a plugin map that contains child plugins, settings and resources.
         * @param aConfigurationNode A Dom node from which the liw structure is 
         *                                built from.
         * @param aMap A Map where the plugin is added to.
         * @return An error code.
         */
        TInt BuildConfigurationMapL( ChspsDomNode& aConfigurationNode, 
                              CLiwDefaultMap& aMap );
        
        /**
         * Appends a list of flat (Does not contain child plugins, settings or 
         * resouces) plugins to a map.
         * @param aConfigurationNode A Dom node from which the liw structure is 
         *                           built from.
         * @param aMap A Map where the plugin is added to.
         * @return An error code.
         */
        TInt AppendPluginsFlatL( ChspsDomNode& aConfigurationNode, 
                                 CLiwDefaultMap& aMap );
        
        /**
         * Appends a single (flat) plugin to a map.
         * @param aPluginNode A Dom node from which the liw structure is 
         *                           built from.
         * @param aMap A Map where the plugin is added to.
         * @param aItemMask Bit mask defining which items are included in plugin map
         * @return An error code.
         */
        static TInt BuildPluginMapL(  
            ChspsDomNode& aPluginNode, 
            CLiwDefaultMap& aMap,
            TInt aItemMask );
        
        /**
         * Builds settings list and appends items to it.
         * @param aConfigurationNode A Dom node from which the liw structure is 
         *                           built from.
         * @param aMap A Map where the list is added to.
         * @return An error code.
         */
        TInt AppendSettingsL( ChspsDomNode& aConfigurationNode, 
                              CLiwDefaultMap& aMap );
        
        /**
         * Buils items map and attributes list.
         * @param aItem A Dom node from which the liw structure is 
         *              built from.
         * @param aMap A Map where the plugin is added to.
         * @return An error code.
         */
        TInt BuildItemsMapL( ChspsDomNode& aItem, CLiwDefaultMap& aMap );
        
        /**
         * Buils properties map.
         * @param aProperty A Dom node from which the liw structure is 
         *                  built from.
         * @param aMap A Map where the plugin is added to.
         * @return An error code.
         */
        TInt BuildPropertiesMapL( ChspsDomNode& aProperty, CLiwDefaultMap& aMap );
        
        /**
         * Builds resources list and appends objects to it.
         * @param aConfigurationNode A Dom node from which the liw structure is 
         *                           built from.
         * @param aMap A Map where the list is added to.
         * @return An error code.
         */
        TInt AppendResourcesL( ChspsDomNode& aConfigurationNode, 
                               CLiwDefaultMap& aMap );
        
        /**
         * Buils aObject map.
         * @param aProperty A Dom node from which the liw structure is 
         *                  built from.
         * @param aMap A Map where the plugin is added to.
         * @return An error code.
         */
        TInt BuildObjectMapL( ChspsDomNode& aObject, CLiwDefaultMap& aMap );

        /**
         * Builds item maps from item nodes (child nodes of settings node) 
         * and appends them to the settings list.
         * @param aSettingsNode Settings node
         * @param aList Settings list
         */
        void AppendItemsL(
            ChspsDomNode& aSettingsNode, 
            CLiwDefaultList& aList );

        /**
         * Buils plugin info list
         * @param aList List of plugin ODT headers
         * @param aPluginList List of plugin info maps
         */
        static void BuildPluginInfoListL( 
            CArrayPtrFlat<ChspsODT>& aList, 
            CLiwDefaultList& aPluginInfoList );
        
        /**
         * Buils plugin info map
         * @param aPluginHeader Plugin ODT header
         * @param aPluginInfoMap Plugin info map
         */
        static void BuildPluginInfoMapL( 
            ChspsODT& aPluginHeader, 
            CLiwDefaultMap& aPluginInfoMap );

        /**
         * Converts a configuration type enumeration to the corresponding
         * configuration type string.
         * @param aType
         * @param aTypeStr Configuration type string
         */
        static void GetConfigurationTypeStrL(
            const TUint aType,
            HBufC8** aTypeStr );
        
        
       TInt SetPluginPropertyNodeL( 
                           const CLiwList& aPropertyList,
                           ChspsDomNode& aNode );   
       
       TInt SetPropertyAttributesL(
                TDesC8& aAttrName,
                TDesC8& aAttrValue,
                ChspsDomNode& aNode );

        /**
         * Finds a child node from the child node list by the tag.
         * @param aNodeTag Requested node tag.
         * @param aParentNode Parent node
         * @param aIndex Start index
         * @return Result node and the index where the element was found
         */
        static ChspsDomNode* FindChildNodeByTagL(
            const TDesC8& aNodeTag, 
            ChspsDomNode& aParentNode,
            TInt& aIndex );

        /**
         * Buils application confoguration info list
         * @param aList List of application configuration ODT headers
         * @param aAppConfInfoList List of application configuration info maps
         */
        static void BuildAppConfInfoListL( 
            CArrayPtrFlat<ChspsODT>& aList, 
            CLiwDefaultList& aAppConfInfoList );
        
        /**
         * Buils plugin info map
         * @param aAppConfHeader Application configuration ODT header
         * @param aAppConfInfoMap Application configuration info map
         */
        static void BuildAppConfInfoMapL( 
            ChspsODT& aAppConfHeader, 
            CLiwDefaultMap& aAppConfInfoMap );

        /**
         * Buils plugin list
         * @param aRootNode Root node of active application configuration
         * @param aType Type of requested plugins
         * @param aPluginId Plugin id which plugin list is requested
         * @param aPluginList List of plugin maps
         */
        static void BuildPluginListL( 
            ChspsDomNode& aRootNode,
            TDesC8& aType,
            TDesC8& aPluginId,
            CLiwDefaultList& aPluginList );

        /**
         * Finds a parent node by the tag.
         * @param aNodeTag Requested node tag.
         * @param aChildNode Child node which parent node is requested
         * @return Pointer to parent node or NULL if it is not found
         */
        static ChspsDomNode* FindParentNodeByTagL(
            const TDesC8& aNodeTag, 
            ChspsDomNode& aChildNode );       
    };

#endif /*HSPSLIWUTILITIES_H_*/
