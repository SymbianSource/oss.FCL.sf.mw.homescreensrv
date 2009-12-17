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

#include <s32mem.h>
#include <hspsdomdepthiterator.h>
#include <hspsdomnode.h>
#include <hspsdomattribute.h>

#include "hspsliwutilities.h"
#include "hspsxmlelements.h"
#include "hspsliwvocabulary.hrh"

const TInt KMaxTextLength = 16 * 1024; // 16KB.

_LIT8( KMaxChildDefault, "6" );

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------  
CHspsLiwUtilities::CHspsLiwUtilities()
	{
	
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------  
CHspsLiwUtilities::~CHspsLiwUtilities()
	{

	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------  
CHspsLiwUtilities* CHspsLiwUtilities::CHspsLiwUtilities::NewL()
	{
	CHspsLiwUtilities* self = CHspsLiwUtilities::NewLC();
	CleanupStack::Pop( self ); //self
	return self;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------  
CHspsLiwUtilities* CHspsLiwUtilities::CHspsLiwUtilities::NewLC()
	{
	CHspsLiwUtilities* self = new ( ELeave ) CHspsLiwUtilities();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}

// -----------------------------------------------------------------------------
// 2nd stage constructor
// -----------------------------------------------------------------------------  
void CHspsLiwUtilities::ConstructL()
	{	
	}

// -----------------------------------------------------------------------------
// Adds common status element to the output parameter list
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::AppendStatusL( 
    TInt32 aStatus,
    CLiwGenericParamList& aOutParamList )
    {    
    TLiwGenericParam param;
    param.SetNameAndValueL( KHspsLiwStatus, TLiwVariant( aStatus ) );
    aOutParamList.AppendL( param );

    param.Reset();
    
    }

// -----------------------------------------------------------------------------
// Builds liw list from a dom document.
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetConfigurationL( 
	const TDesC8& aReturnName,
    const TDesC8& aPluginId,
    ChspsDomDocument& aDomDocument,
    CLiwGenericParamList& aOutParamList )
	{
	ChspsDomNode* node( NULL );
    TInt err = KErrNotFound;
	//figure out what part of the dom is wanted.
	//we have configuration id.
	if( aPluginId.Length() > 0 )
		{
		//we have plugin id.
		//get view/widget configuration.
		//finds configuration node with plugin id.
		node = &( FindRootNodeByIdentifierL( KPluginElement, aPluginId, *aDomDocument.RootNode() ));
		if ( node )
		    {
    		node = &( FindRootNodeByTagL( KConfigurationElement, 
                    *node ));
		    }
		}
	else
		{
		//get application configuration
		//finds first configuration node.
		node = &( FindRootNodeByTagL( KConfigurationElement, 
								  *aDomDocument.RootNode() ));
		}

	if( node )
		{
	    CLiwDefaultMap* rootMap = CLiwDefaultMap::NewL();
	    CleanupStack::PushL( rootMap ); 

    	err = BuildConfigurationMapL( *node, *rootMap );
    	//put map into the iterable.
    	TLiwVariant rootMapVariant;
    	rootMapVariant.Set( rootMap );
    	//Put iterable into outParamList
    	TLiwGenericParam rootMapParam;
    	rootMapParam.SetNameAndValueL( aReturnName, rootMapVariant );
    	aOutParamList.AppendL( rootMapParam );
    	//Cleanup.
    	CleanupStack::Pop( rootMap );
    	rootMap->DecRef();
    	rootMapParam.Reset();
	    }
	
	User::LeaveIfError( err );
	}

// -----------------------------------------------------------------------------
// Builds plugins list from plugin header list
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetPluginsOutputL( 
    CArrayPtrFlat<ChspsODT>& aList,
    CLiwGenericParamList& aOutParamList )
    {
    CLiwDefaultList* pluginList = CLiwDefaultList::NewL();
    CleanupStack::PushL( pluginList );
    
    BuildPluginInfoListL( aList, *pluginList );
    
    TLiwVariant listVariant;
    listVariant.Set( pluginList );
    TLiwGenericParam param;
    param.SetNameAndValueL( KHspsLiwPlugins, listVariant );
    aOutParamList.AppendL( param );
    
    CleanupStack::Pop( pluginList );
    pluginList->DecRef();
    param.Reset();
    
    }

// -----------------------------------------------------------------------------
// Builds plugins list from plugin node list
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetPluginListOutputL( 
    ChspsDomNode& aRootNode,
    TDesC8& aType,
    TDesC8& aPluginId,
    CLiwGenericParamList& aOutParamList )
    {    
    CLiwDefaultList* pluginList = CLiwDefaultList::NewL();
    CleanupStack::PushL( pluginList );

    BuildPluginListL(
        aRootNode,
        aType,
        aPluginId,
        *pluginList );

    TLiwVariant listVariant;
    listVariant.Set( pluginList );
    TLiwGenericParam param;
    param.SetNameAndValueL( KHspsLiwPlugins, listVariant );
    aOutParamList.AppendL( param );
    
    CleanupStack::Pop( pluginList );
    pluginList->DecRef();
    param.Reset();

    }

// -----------------------------------------------------------------------------
// Creates output parmater list for AddPlugin method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::AddPluginOutputL( 
    TInt aPluginId,
    CLiwGenericParamList& aOutParamList )
    {    
    TBuf8<10>id;
    _LIT8(KFormat8, "%d");
    id.AppendFormat( KFormat8, aPluginId );
    
    TLiwGenericParam param;
    param.SetNameAndValueL( KPluginId, TLiwVariant( id ) );
    aOutParamList.AppendL( param );
    
    param.Reset();
    
    }
// -----------------------------------------------------------------------------
// Creates output parmater list for RemovePlugin method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::RemovePluginOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    
    AppendStatusL( KErrNone, aOutParamList );

    }

// -----------------------------------------------------------------------------
// Creates output parmater list for ReplacePlugin method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::ReplacePluginOutputL( 
    CLiwGenericParamList& aOutParamList )
    {    
    AppendStatusL( KErrNone, aOutParamList );
    }

// -----------------------------------------------------------------------------
// Builds settings Dom document from liw list
// ----------------------------------------------------------------------------- 
TInt CHspsLiwUtilities::SetPluginSettingsNodeL( 
     const CLiwList& aItemList,
     TDesC8& aPluginId,
     ChspsDomDocument& aDomDocument,
     ChspsDomDocument& aSettingsDom )
    {
    TInt error(KErrNone);
    ChspsDomNode* node( NULL );
    ChspsDomNode* settingsNode( NULL );
    ChspsDomNode* itemNode( NULL );
    
    if( aPluginId.Length() > 0 )
        {
        // get plugin node
        node = &( FindRootNodeByIdentifierL( KPluginElement, aPluginId, *aDomDocument.RootNode() ));
        if ( node )
            {
            // get control node of the plugin
            node = &( FindRootNodeByTagL( KControlElement, 
                                        *node ));
            if( node )
                {
                TInt index = 0;
                // get settings node (child of control node)
                settingsNode = FindChildNodeByTagL(
                         KSettingsElement,
                         *node,
                         index );
                }
            }
        
        
        if ( node == NULL || settingsNode == NULL )
            {
            // pluginId Not Found
            error=KErrNotFound;
            } 
        
        TLiwVariant tempVariant;
        tempVariant.PushL();
        TInt itemsCount = aItemList.Count();
        if ( (KErrNone == error) &&
                     (itemsCount > 0) )
            {    
            // Find items for plugin settings
            for ( TInt i = 0; ( i < itemsCount && error == KErrNone ); i++ )
                {
                error = KErrNotFound;
                aItemList.AtL( i, tempVariant );
                const CLiwMap* item = tempVariant.AsMap();
                if ( (item != NULL) &&
                      item->FindL( KHspsLiwItemId, tempVariant ) )
                    {
                    TPtrC8 itemId;
                    itemId.Set( tempVariant.AsData() );
                   
                    itemNode = &( FindRootNodeByIdentifierL( 
                            KItemElement, itemId, *settingsNode ));
                    if( itemNode )
                        {
                        item->FindL(KHspsLiwProperties, tempVariant );
                        const CLiwList* propertyList = tempVariant.AsList();
                        if(propertyList && propertyList->Count() > 0)
                            {
                            if ( SetPluginPropertyNodeL( *propertyList, *itemNode) == KErrNone )
                                {
                                error = KErrNone;                              
                                }
                            }
                        }
                    }               
                tempVariant.Reset();
                }
            }
        else
            {
            error = KErrNotFound;
            }

        CleanupStack::Pop(&tempVariant);
        tempVariant.Reset();
        
        
        if( error == KErrNone )
            {
          
            // Making Settings Dom
            ChspsDomNode* clone = settingsNode->CloneL( aSettingsDom.StringPool() );
            CleanupStack::PushL( clone );
            aSettingsDom.SetRootNode( clone );
            CleanupStack::Pop( clone );
            }
            
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// Parse Properties from liw list
// ----------------------------------------------------------------------------- 
TInt CHspsLiwUtilities::SetPluginPropertyNodeL( const CLiwList& aPropertyList,
        ChspsDomNode& aNode  )
    {
    // Init used local variables.        
    TInt err = KErrNone;
    
    TLiwVariant mapVariant;
    mapVariant.PushL();
    
    TLiwVariant propertyNameVariant;
    propertyNameVariant.PushL();

    TLiwVariant propertyValueVariant;
    propertyValueVariant.PushL();            
    
    const TInt propertiesCount = aPropertyList.Count();
    
    // Loop all items in given list.
    for( TInt i = 0; ( i < propertiesCount && err == KErrNone ); i++ )
        {        
        // Extract map.
        aPropertyList.AtL( i, mapVariant );
        
        const CLiwMap* property = mapVariant.AsMap();
        if( property == NULL ) 
            {
            err = KErrCorrupt;
            }
                       
        // Extract property name.
        if( !err )
            {
            if( !property->FindL( KHspsLiwPropertyName, propertyNameVariant ) )
                {
                err = KErrNotFound;
                }
            }
        
        TPtrC8 name;
        if( !err )
            {
            name.Set( propertyNameVariant.AsData() );
            if( name.Length() > KMaxTextLength )
                {
                err = KErrArgument;
                }
            }
        
        // Extract property value.
        if( !err )
            {
            if( !property->FindL( KHspsLiwPropertyValue, propertyValueVariant ) )
                {
                err = KErrNotFound;
                }
            }
        
        TPtrC8 value;
        if( !err )
            {        
            value.Set( propertyValueVariant.AsData() );
            if( value.Length() > KMaxTextLength )
                {
                err = KErrArgument;
                }
            }
        
        // Set attribute.
        if( !err )
            {
            err = SetPropertyAttributesL( name, value, aNode );
            }
        
        mapVariant.Reset();
        propertyNameVariant.Reset();
        propertyValueVariant.Reset();        
        }
        
    // Cleanup.    
    CleanupStack::Pop( &propertyValueVariant );
    CleanupStack::Pop( &propertyNameVariant );
    CleanupStack::Pop( &mapVariant );
    
    return err;   
    }
//-----------------------------------------------------------------------------
// SetpropertyAttributeL replaces value of property attribute given in parameter 
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::SetPropertyAttributesL(
         TDesC8& aAttrName,
         TDesC8& aAttrValue,
        ChspsDomNode& aNode )
    {
   
   
    TInt error(KErrNotFound);
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aNode );
    CleanupStack::PushL( iter );                                
    ChspsDomNode* node = iter->First();                              
    TBool replaced(EFalse);                     
    
    while( node && !replaced )
        {
        const TDesC8& nodeName = node->Name();
        if( nodeName == KPropertyElement )
            {
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPropertyAttrName) );     
            if ( attr )
                {
                const TDesC8& name = attr->Value(); 
                if (aAttrName.Compare( name )== 0 ) 
                    {
                    ChspsDomAttribute* attr2 = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPropertyAttrValue) );
                    if( attr2 )
                        {
                        attr2->SetValueL( aAttrValue );
                        replaced=ETrue;
                        error = KErrNone;
                        }
                    }
                }
            }
        if( !replaced )
            {
            node = iter->NextL();
            }
        }
                
    CleanupStack::PopAndDestroy( iter );
    
    return error;
   
    }  
// -----------------------------------------------------------------------------
// Creates output parmater list for SetPluginSettings method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::SetPluginSettingsOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    AppendStatusL( KErrNone, aOutParamList );
    }

// -----------------------------------------------------------------------------
// Creates output parmater list for GetPluginSettings method
// ----------------------------------------------------------------------------- 

void CHspsLiwUtilities::GetPluginSettingsOutputL( 
		ChspsDomDocument& aDom, 
		CLiwGenericParamList& aOutParamList )
	{
	TInt pos = 0;
    CLiwDefaultList* list = CLiwDefaultList::NewL();
    CleanupStack::PushL( list );
	ChspsDomNode* node( NULL );

	node = aDom.RootNode();
    if( !node )
    	{
    	User::Leave( KErrNotFound );
    	}
  
    // get ctrl node (child of control node)
    node = FindChildNodeByTagL(
    	KControlElement,
        *node,
        pos );
    
    if( !node )
    	{
    	User::Leave( KErrNotFound );
    	}

    // get settings node (child of control node)
    node = FindChildNodeByTagL(
    	KSettingsElement,
        *node,
        pos );
	    
    if ( node )
        {
        // append item maps to settings list
        AppendItemsL( *node, *list );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
	TLiwVariant listVariant;
	listVariant.Set( list );

    TLiwGenericParam param;
    param.SetNameAndValueL( KHspsLiwSettings, listVariant );
    aOutParamList.AppendL( param );
    
    CleanupStack::Pop( list );
    list->DecRef();
    param.Reset();
  	}


// -----------------------------------------------------------------------------
// Creates output parmater list for MovePlugins method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::MovePluginsOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    
    AppendStatusL( KErrNone, aOutParamList );

    }

// -----------------------------------------------------------------------------
// Gets list of plugin id's from plugins list
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetPluginIdListL( 
    const CLiwList* aPluginsList,
    CArrayFixFlat<TInt>& aPluginIdList )
    {
    
    const TInt pluginsCount = aPluginsList->Count();
    TLiwVariant entry;
    entry.PushL();
    for( TInt i = 0; i < pluginsCount; i++ )
        {
        aPluginsList->AtL( i, entry );
        TInt id = 0;
        TPtrC8 pluginId = entry.AsData();
        TLex8 lex( pluginId );
        User::LeaveIfError( lex.Val( id ) );
        if ( id < 1 )
            {
            User::Leave( KErrArgument );
            }

        // Plugin id's must be unique
        const TInt pluginIdCount = aPluginIdList.Count();
        for ( TInt j = 0; j < pluginIdCount; j++ )
            {
            if ( id == aPluginIdList.At( j ) )
                {
                User::Leave( KErrArgument );
                }
            }
        
        aPluginIdList.AppendL( id );
        entry.Reset();
        }
    CleanupStack::Pop( &entry );

    }

// -----------------------------------------------------------------------------
// Builds plugins list from plugin header list
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetAppConfsOutputL( 
    CArrayPtrFlat<ChspsODT>& aList,
    CLiwGenericParamList& aOutParamList )
    {
    CLiwDefaultList* pluginList = CLiwDefaultList::NewL();
    CleanupStack::PushL( pluginList );
    
    BuildAppConfInfoListL( aList, *pluginList );
    
    TLiwVariant listVariant;
    listVariant.Set( pluginList );
    TLiwGenericParam param;
    param.SetNameAndValueL( KHspsLiwAppConfs, listVariant );
    aOutParamList.AppendL( param );
    
    CleanupStack::Pop( pluginList );
    pluginList->DecRef();
    param.Reset();
    
    }

// -----------------------------------------------------------------------------
// Creates output parmater list for SetActiveAppConf method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::SetActiveAppConfOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    
    AppendStatusL( KErrNone, aOutParamList );

    }

// -----------------------------------------------------------------------------
// Creates output parmater list for SetConfState method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::SetConfStateOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    
    AppendStatusL( KErrNone, aOutParamList );

    }

// -----------------------------------------------------------------------------
// Returns value of defined attribute
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::GetAttributeValueL( 
    ChspsDomNode& aNode,
    const TDesC8& aAttrId,
    TPtrC8& aAttrValue )
    {
    
    // Get configutation node's ID attribute
    ChspsDomList& attrList = aNode.AttributeList();    
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                                  attrList.FindByName( aAttrId ) );
    if ( !attr )
        {
        User::Leave( KErrNotFound );
        }

    aAttrValue.Set( attr->Value() );
    
    }

// -----------------------------------------------------------------------------
// Finds a node from a dom document.
// Looks for configuration elements and plugin elements by the identifier.
// -----------------------------------------------------------------------------
ChspsDomNode& CHspsLiwUtilities::FindRootNodeByIdentifierL( 
        const TDesC8& aNodeTag,
        const TDesC8& aNodeIdentifier,
		ChspsDomNode& aNode )
	{
	ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aNode );
	CleanupStack::PushL( iter );
	ChspsDomNode* targetNode( NULL );
	ChspsDomNode* node = iter->First();
	TBool found = EFalse;
	while( !found && node )
		{
	    const TDesC8& name = node->Name();
	    if ( name.Compare( aNodeTag ) == 0)
	    	{
	    	ChspsDomList& attrList = node->AttributeList();
	    	
	    	ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
	    							      attrList.FindByName( KAttrId ));
	    	const TDesC8& value = attr->Value();
	        if( value.Compare( aNodeIdentifier ) == 0 )
	        	{
	        	found = ETrue;
	        	targetNode = node;
	        	}
	    	}
	    node = iter->NextL();
		}	
    CleanupStack::PopAndDestroy( iter );
	return *targetNode;
	}

// -----------------------------------------------------------------------------
// Finds a node from a dom document.
// Looks for the next node tag.
// -----------------------------------------------------------------------------
ChspsDomNode& CHspsLiwUtilities::FindRootNodeByTagL( 
		const TDesC8& aNodeTag,
		 ChspsDomNode& aDomNode )
	{
	ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aDomNode );
	CleanupStack::PushL( iter );
	ChspsDomNode* targetNode( NULL );
	ChspsDomNode* node = iter->First();
	TBool found = EFalse;
	while( !found && node )
		{
	    const TDesC8& name = node->Name();
	    if ( name.Compare( aNodeTag ) == 0 )
	        {  
        	found = ETrue;
        	targetNode = node;
	        }
	    node = iter->NextL();
		}	
    CleanupStack::PopAndDestroy( iter );
	return *targetNode;
	}

// -----------------------------------------------------------------------------
// Appends found attributes into the LIW map instance
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::AppendAttributeToMapL( CLiwDefaultMap& aMap, 
		                              ChspsDomList& aAttrList, 
		                              const TDesC8& aDomName,
		                              const TDesC8& aLiwName,
		                              const TDesC8& aDefaultValue )
	{
	TInt err = KErrNone;
	ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
											aAttrList.FindByName(aDomName) );	            
	if( attr )
        {
        const TDesC8& value = attr->Value(); 
        aMap.InsertL( aLiwName, TLiwVariant( value ) );                                  
        }   
    else
    	{
    	aMap.InsertL( aLiwName, TLiwVariant( aDefaultValue ) );
   	}
    return err;
	}

// -----------------------------------------------------------------------------
// Builds a plugin map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::BuildConfigurationMapL( ChspsDomNode& aConfigurationNode, 
		                                   CLiwDefaultMap& aMap )
	{
	TInt err = KErrNone;	
	//Get plugin info.
	const TDesC8& name = aConfigurationNode.Name();
    ChspsDomList& attrList = aConfigurationNode.AttributeList();
    if ( name == KConfigurationElement )
        {    
        //Add id
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrId, KHspsLiwId );
        //Add pluginUid
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrUid, KHspsLiwUid );
        //Add type
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrType, KHspsLiwType );
        //Add interface
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrInterface, 
        													KHspsLiwInterface );
        //Add name
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrName, KHspsLiwName );

        //Add multiinstance
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrMultiInstance,
            KHspsLiwMultiInstance );
        
        //Add desc
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrDescription,
                KHspsLiwDescription );

        // Add state
        AppendAttributeToMapL( aMap, attrList, KConfigurationAttrState, KHspsLiwState );

        // Add max_child
        AppendAttributeToMapL( aMap,
                attrList,
                KConfigurationAttrMaxChild,
                KHspsLiwMaxChild,
                KMaxChildDefault );        
        
        //Add list of plugins.
       	//create list where maps are added.
      	AppendPluginsFlatL( aConfigurationNode, aMap );      	     	
        //Add list of settings.
        AppendSettingsL( aConfigurationNode, aMap );
        //Add list of resources.
        AppendResourcesL( aConfigurationNode, aMap );
        }
    else
    	{
    	//should be always configuration element.
    	}
	return err;
	}

// -----------------------------------------------------------------------------
// Builds and appends a plugins to a map. Plugins are appended without chidren 
// plugins, settings or resources.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::AppendPluginsFlatL( ChspsDomNode& aConfigurationNode, 
						 CLiwDefaultMap& aMap )
	{
	TInt err = KErrNone;
	//create a list for plugins.
	CLiwDefaultList* list = CLiwDefaultList::NewL();
	CleanupStack::PushL( list );
	
	ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( 
														   aConfigurationNode );
	CleanupStack::PushL( iter );
 	TBool found = EFalse;
 	ChspsDomNode* node = iter->First();

	//find plugins element.
	while( !found && node )
		{
		const TDesC8& name = node->Name();
		if ( name.Compare( KPluginsElement ) == 0)
			{
	    	//iterate plugin information
	    	ChspsDomList& children = node->ChildNodes();
	    	TInt length = children.Length();
	    	for( TInt i = 0; i < length; i++ )
	    		{
	    		ChspsDomNode* plugin;
	    		plugin = static_cast<ChspsDomNode*>( children.Item( i ) );
	    		//create a plugin map and add it to the list.
	    		CLiwDefaultMap* map = CLiwDefaultMap::NewL();
	    		CleanupStack::PushL( map );

                TInt pluginMapItems = ( 
                    CHspsLiwUtilities::EPluginMapId +
                    CHspsLiwUtilities::EPluginMapUid +
                    CHspsLiwUtilities::EPluginMapActivationState +
                    CHspsLiwUtilities::EPluginMapLocked );
                BuildPluginMapL( *plugin, *map, pluginMapItems );
                
                //append map to the list                
                TLiwVariant mapVariant;
                mapVariant.Set( map );
                list->AppendL( mapVariant );
                
                CleanupStack::Pop( map );
                map->DecRef();
                }
            found = ETrue;
            }
        node = iter->NextL();
        }
     
    //append list to a map.
    TLiwVariant listVariant;
    listVariant.Set( list );
    aMap.InsertL( KHspsLiwPlugins, listVariant );
    
    CleanupStack::PopAndDestroy( iter );
    CleanupStack::Pop( list );
    list->DecRef();    
    
    return err; 
    }

// -----------------------------------------------------------------------------
// Builds and appends settings to a map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::AppendSettingsL( 
    ChspsDomNode& aConfigurationNode, 
    CLiwDefaultMap& aMap )
    {
    TInt err( KErrNone );
    TInt index( 0 );
    
    //create list
    CLiwDefaultList* list = CLiwDefaultList::NewL();
    CleanupStack::PushL( list );
    //get control (child of configuration node)
    ChspsDomNode* node( NULL );
    node = FindChildNodeByTagL( 
        KControlElement, 
        aConfigurationNode,
        index );

    if( node )
        {
        // get settings node (child of control node)
        node = FindChildNodeByTagL(
            KSettingsElement,
            *node,
            index );
        if ( node )
            {
            // append item maps to settings list
            AppendItemsL( *node, *list );
            }
        else
            {
            // there should always be a settings element
            err = KErrNotFound;
            }
        }
    else
        {
        //there should always be a control element.
        err = KErrNotFound;
        }
    
    User::LeaveIfError( err );
    
    //append list to a map.
    TLiwVariant listVariant;
    listVariant.Set( list );
    aMap.InsertL( KHspsLiwSettings, listVariant );
    //cleanup.
    CleanupStack::Pop( list );
    list->DecRef();    
    
    return err;
    }

// -----------------------------------------------------------------------------
// Builds and appends resources to a map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::AppendResourcesL( ChspsDomNode& aConfigurationNode, 
                                            CLiwDefaultMap& aMap )
    {    
    TInt err = KErrNone;
    //create list.
    CLiwDefaultList* list = CLiwDefaultList::NewL();
    CleanupStack::PushL( list );    
    //find resources.
    ChspsDomList& children = aConfigurationNode.ChildNodes();
    TInt length = children.Length();
    for( TInt i = 0; i < length; i++ )
        {
        ChspsDomNode* child;
        child = static_cast<ChspsDomNode*>( children.Item( i ) );
        const TDesC8& childName = child->Name();
        if( childName.Compare( KResourcesElement ) == 0)
            {
            ChspsDomList& objects = child->ChildNodes();
            TInt length = objects.Length();
            for( TInt i = 0; i < length; i++ )
                {
                //put object elements to the list.
                ChspsDomNode* object;
                object = static_cast<ChspsDomNode*>( objects.Item( i ) );
                const TDesC8& objectName = object->Name();
                if( objectName.Compare( KObjectElement ) == 0)
                    {
                    CLiwDefaultMap* map = CLiwDefaultMap::NewL();
                    CleanupStack::PushL( map );
                    
                    BuildObjectMapL( *object, *map );
                    
                    ///append object to the list                
                    TLiwVariant mapVariant;
                    mapVariant.Set( map );
                    list->AppendL( mapVariant );
                    
                    CleanupStack::Pop( map );
                    map->DecRef();
                    }                
                }
            //There should be only one.
            break;
            }
        }
    //append list to a map.
    TLiwVariant listVariant;
    listVariant.Set( list );
    aMap.InsertL( KHspsLiwResources, listVariant );    
    //cleanup.
    CleanupStack::Pop( list );
    list->DecRef();    
    return err;
    }

// -----------------------------------------------------------------------------
// Builds plugin map without resources, settings or child plugins.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::BuildPluginMapL( 
    ChspsDomNode& aPluginNode, 
    CLiwDefaultMap& aMap,
    TInt aItemMask )
    {
    TInt err( KErrNotFound );

    // Get plugin info.
    const TDesC8& name = aPluginNode.Name();
    ChspsDomList& attrList = aPluginNode.AttributeList();
    
    if ( name == KPluginElement )
        {
        if ( aItemMask & CHspsLiwUtilities::EPluginMapId )
            {
            //Add plugin id
            AppendAttributeToMapL( aMap, attrList, KPluginAttrId, KHspsLiwId );
            }
        if ( aItemMask & CHspsLiwUtilities::EPluginMapUid )
            {
            //Add plugin uid
            AppendAttributeToMapL( aMap, attrList, KPluginAttrUid, KHspsLiwUid );
            }
        if ( aItemMask & CHspsLiwUtilities::EPluginMapType ||
             aItemMask & CHspsLiwUtilities::EPluginMapName )
            {
            // Get plugin type and name from Configuration node attribute list
            TInt index = 0;
            ChspsDomNode* confNode = FindChildNodeByTagL( 
                KConfigurationElement,
                aPluginNode,
                index );
            if ( confNode )
                {
                ChspsDomList&confNodeAttrList = confNode->AttributeList();
                if ( aItemMask & CHspsLiwUtilities::EPluginMapType )
                    {
                    // Add plugin type
                    AppendAttributeToMapL( aMap, confNodeAttrList, KConfigurationAttrType, KHspsLiwType );
                    }
                if ( aItemMask & CHspsLiwUtilities::EPluginMapName )
                    {
                    // Add plugin type
                    AppendAttributeToMapL( aMap, confNodeAttrList, KConfigurationAttrName, KHspsLiwName );
                    }
                }
            }
        if ( aItemMask & CHspsLiwUtilities::EPluginMapActivationState )
            {
            //Add plugin activation state
            AppendAttributeToMapL( aMap, attrList, KPluginAttrActive, KHspsLiwActivationState );
            }
        if ( aItemMask & CHspsLiwUtilities::EPluginMapDesc )
            {
            //Add plugin description
            AppendAttributeToMapL( aMap, attrList, KConfigurationAttrDescription, KHspsLiwDescription );
            }
        if ( aItemMask & CHspsLiwUtilities::EPluginMapLocked )
            {
            //Add plugin locked status - locked/removable/none(default)
            AppendAttributeToMapL( aMap,
                    attrList,
                    KConfigurationAttrLockingStatus,
                    KHspsLiwLockingStatus,
                    KHspsLiwLockingNone );
            }        
        
        err = KErrNone;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// Builds items map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::BuildItemsMapL( ChspsDomNode& aItem, 
                                          CLiwDefaultMap& aMap )
    {
    TInt err = KErrNone;
    //Append attributes
    //create attribute list
    ChspsDomList& attrList = aItem.AttributeList();
    //Add itemId
    AppendAttributeToMapL( aMap, attrList, KItemAttrId, KHspsLiwItemId );
    //Add name
    AppendAttributeToMapL( aMap, attrList, KItemAttrName, KHspsLiwItemName );
    
    //create list
    CLiwDefaultList* list = CLiwDefaultList::NewL();
    CleanupStack::PushL( list );
    //get children
    ChspsDomList& properties = aItem.ChildNodes();
    TInt length = properties.Length();
    for( TInt i = 0; i < length; i++ )
        {
        ChspsDomNode* property;
        property = static_cast<ChspsDomNode*>( properties.Item( i ) );
        const TDesC8& name = property->Name();
        if( name.Compare( KPropertyElement ) == 0)
            {
            CLiwDefaultMap* map = CLiwDefaultMap::NewL();       
            CleanupStack::PushL( map );
        
            //build attribute.
            BuildPropertiesMapL( *property, *map );
        
            //append attribute to the list              
            TLiwVariant mapVariant;
            mapVariant.Set( map );
            list->AppendL( mapVariant );
        
            CleanupStack::Pop( map );
            map->DecRef();              
            }
        }
    //append list to a map.
    TLiwVariant listVariant;
    listVariant.Set( list );
    aMap.InsertL( KHspsLiwProperties, listVariant );
    //cleanup
    CleanupStack::Pop( list );
    list->DecRef();        
    return err;
    }

// -----------------------------------------------------------------------------
// Builds properties map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::BuildPropertiesMapL( ChspsDomNode& aProperty, 
                                               CLiwDefaultMap& aMap )
    {
    TInt err = KErrNone;
    //Append attributes
    //create attribute list
    ChspsDomList& attrList = aProperty.AttributeList();
    //Add name
    AppendAttributeToMapL( aMap, attrList, KPropertyAttrName, KHspsLiwPropertyName );
    //Add value
    AppendAttributeToMapL( aMap, attrList, KPropertyAttrValue, KHspsLiwPropertyValue );
    
    return err;
    }

// -----------------------------------------------------------------------------
// Builds object map.
// -----------------------------------------------------------------------------
//
TInt CHspsLiwUtilities::BuildObjectMapL( ChspsDomNode& aObject, 
                                           CLiwDefaultMap& aMap )
    {
    TInt err = KErrNone;
    
    ChspsDomList& attrList = aObject.AttributeList();
    //Add name
    AppendAttributeToMapL( aMap, attrList, KObjectAttrName, KHspsLiwObjectName );
    //Add path
    AppendAttributeToMapL( aMap, attrList, KObjectAttrPath, KHspsLiwObjectPath );
    //Add mediatype
    AppendAttributeToMapL( aMap, attrList, KObjectAttrMediatype, KHspsLiwObjectMediatype );
    //Add tags
    AppendAttributeToMapL( aMap, attrList, KObjectAttrTag, KHspsLiwObjectTag );
    
    return err;
    }

// -----------------------------------------------------------------------------
// Buils plugin info list
// -----------------------------------------------------------------------------
//
void CHspsLiwUtilities::BuildPluginInfoListL( 
    CArrayPtrFlat<ChspsODT>& aList, 
    CLiwDefaultList& aPluginInfoList )
    {
    for ( TInt i = 0; i < aList.Count(); i++ )
        {
        CLiwDefaultMap* pluginInfoMap = CLiwDefaultMap::NewL();
        CleanupStack::PushL( pluginInfoMap );
        BuildPluginInfoMapL( *( aList[ i ] ), *pluginInfoMap );
        TLiwVariant pluginInfoMapVariant;
        pluginInfoMapVariant.Set( pluginInfoMap );
        aPluginInfoList.AppendL( pluginInfoMapVariant );
        CleanupStack::Pop( pluginInfoMap );
        pluginInfoMap->DecRef();
        }
    }

// -----------------------------------------------------------------------------
// Buils plugin info map
// -----------------------------------------------------------------------------
//
void CHspsLiwUtilities::BuildPluginInfoMapL( 
    ChspsODT& aPluginHeader, 
    CLiwDefaultMap& aPluginInfoMap )
    {
    // pluginInfo.uid ( 10 -> uid 32 bit as a hex string, 0xXXXXXXXX )
    TBuf8<10> uid;
    uid.Append( '0' );
    uid.Append( 'x' );
    uid.AppendNum( aPluginHeader.ThemeUid(), EHex );
    aPluginInfoMap.InsertL( KHspsLiwUid, TLiwVariant( uid ) );
    // pluginInfo.interface ( 10 -> uid 32 bit as a hex string, 0xXXXXXXXX )
    TBuf8<10> interface;
    interface.Append( '0' );
    interface.Append( 'x' );
    interface.AppendNum( aPluginHeader.RootUid(), EHex );
    aPluginInfoMap.InsertL( KHspsLiwInterface, TLiwVariant( interface ) );
    // pluginInfo.type ( enumeration to 8 bit string )
    HBufC8* type;
    GetConfigurationTypeStrL( aPluginHeader.ConfigurationType(), &type );
    CleanupStack::PushL( type );
    TPtr8 typePtr = type->Des();
    aPluginInfoMap.InsertL( KHspsLiwType, TLiwVariant( typePtr ) );
    CleanupStack::PopAndDestroy( type );
    // pluginInfo.name ( 16 bit string to 8 bit string )
    HBufC8* name = HBufC8::NewL( ( aPluginHeader.ThemeFullName() ).Length() );
    CleanupStack::PushL( name );
    TPtr8 namePtr = name->Des();
    namePtr.Copy( aPluginHeader.ThemeFullName() );
    aPluginInfoMap.InsertL( KHspsLiwName, TLiwVariant( namePtr ) );
    CleanupStack::PopAndDestroy( name );

    TBuf8<10> multiInstance;
    multiInstance.AppendNum( aPluginHeader.MultiInstance());
    aPluginInfoMap.InsertL( KHspsLiwMultiInstance,
        TLiwVariant( multiInstance ) );
    
    HBufC8* desc = HBufC8::NewL( ( aPluginHeader.Description() ).Length() );
    CleanupStack::PushL( desc );
    TPtr8 descPtr = desc->Des();
    descPtr.Copy( aPluginHeader.Description() );
    aPluginInfoMap.InsertL( KHspsLiwDescription, TLiwVariant( descPtr ) );
    CleanupStack::PopAndDestroy( desc );    
    
    if ( aPluginHeader.LogoFile().Length() )
        {
        HBufC8* buf8 = HBufC8::NewLC( ( aPluginHeader.LogoFile() ).Length() );        
        buf8->Des().Copy( aPluginHeader.LogoFile() );
        aPluginInfoMap.InsertL( KHspsLiwLogo, TLiwVariant( *buf8 ) );
        CleanupStack::PopAndDestroy( buf8 );
        }
    if ( aPluginHeader.PreviewFile().Length() )
        {
        HBufC8* buf8 = HBufC8::NewLC( ( aPluginHeader.PreviewFile() ).Length() );        
        buf8->Des().Copy( aPluginHeader.PreviewFile() );
        aPluginInfoMap.InsertL( KHspsLiwPreview, TLiwVariant( *buf8 ) );
        CleanupStack::PopAndDestroy( buf8 );
        }
    }

//----------------------------------------------------------------------------
// CHspsLiwUtilities::GetConfigurationTypeL
// ----------------------------------------------------------------------------
//
void CHspsLiwUtilities::GetConfigurationTypeStrL(
    const TUint aType,
    HBufC8** aTypeStr )
    {
    if ( aType == EhspsAppConfiguration )
        {
        *aTypeStr = HBufC8::NewL( KHspsLiwApplicationConf().Length() );
        TPtr8 typePtr = ( *( aTypeStr ) )->Des();
        typePtr.Copy( KHspsLiwApplicationConf );
        }
    else if ( aType == EhspsViewConfiguration )
        {
        *aTypeStr = HBufC8::NewL( KHspsLiwViewConf().Length() );
        TPtr8 typePtr = ( *( aTypeStr ) )->Des();
        typePtr.Copy( KHspsLiwViewConf );
        }
    else if ( aType == EhspsWidgetConfiguration )
        {
        *aTypeStr = HBufC8::NewL( KHspsLiwWidgetConf().Length() );
        TPtr8 typePtr = ( *( aTypeStr ) )->Des();
        typePtr.Copy( KHspsLiwWidgetConf );
        }
    else if ( aType == EhspsTemplateConfiguration )
        {
        *aTypeStr = HBufC8::NewL( KHspsLiwTemplateConf().Length() );
        TPtr8 typePtr = ( *( aTypeStr ) )->Des();
        typePtr.Copy( KHspsLiwTemplateConf );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }


//----------------------------------------------------------------------------
// CHspsLiwUtilities::FindChildNodeByTagL
// ----------------------------------------------------------------------------
//
ChspsDomNode* CHspsLiwUtilities::FindChildNodeByTagL(
    const TDesC8& aNodeTag, 
    ChspsDomNode& aParentNode,
    TInt& aIndex )
    {
    ChspsDomNode* node( NULL );
    ChspsDomList& items = aParentNode.ChildNodes();
    TInt length = items.Length();
    node = NULL;
    for ( TInt i = aIndex; i < length && node == NULL; i++ )
        {
        node = static_cast<ChspsDomNode*>( items.Item( i ) );
        const TDesC8& name = node->Name();
        if( name.Compare( aNodeTag ) != 0 )
            {
            node = NULL;
            }
        else
            {
            aIndex = i;
            }
        }
    
    return node;
    }

//----------------------------------------------------------------------------
// CHspsLiwUtilities::AppendItemsL
// ----------------------------------------------------------------------------
//
void CHspsLiwUtilities::AppendItemsL(
    ChspsDomNode& aSettingsNode,
    CLiwDefaultList& aList )
    {

    ChspsDomNode* itemNode( NULL );
    TInt index = 0;
    do
        {
        itemNode = FindChildNodeByTagL(
            KItemElement,
            aSettingsNode,
            index );
        if ( itemNode )
            {
            CLiwDefaultMap* map = CLiwDefaultMap::NewL();        
            CleanupStack::PushL( map );

            //build item.
            BuildItemsMapL( *itemNode, *map );

            //append item to the list                
            TLiwVariant mapVariant;
            mapVariant.Set( map );
            aList.AppendL( mapVariant );

            CleanupStack::Pop( map );
            map->DecRef();
            // Find next item element
            index++;
            }
        } while( itemNode );
    }

// -----------------------------------------------------------------------------
// Builds app conf info list
// -----------------------------------------------------------------------------
//
void CHspsLiwUtilities::BuildAppConfInfoListL( 
    CArrayPtrFlat<ChspsODT>& aList, 
    CLiwDefaultList& aAppConfInfoList )
    {
    for ( TInt i = 0; i < aList.Count(); i++ )
        {
        CLiwDefaultMap* appConfInfoMap = CLiwDefaultMap::NewL();
        CleanupStack::PushL( appConfInfoMap );
        BuildAppConfInfoMapL( *( aList[ i ] ), *appConfInfoMap );
        TLiwVariant appConfInfoMapVariant;
        appConfInfoMapVariant.Set( appConfInfoMap );
        aAppConfInfoList.AppendL( appConfInfoMapVariant );
        CleanupStack::Pop( appConfInfoMap );
        appConfInfoMap->DecRef();
        }
    }

// -----------------------------------------------------------------------------
// Buils app conf info map
// -----------------------------------------------------------------------------
//
void CHspsLiwUtilities::BuildAppConfInfoMapL( 
    ChspsODT& aAppConfHeader, 
    CLiwDefaultMap& aAppConfInfoMap )
    {
    // appConfInfo.uid ( 10 -> uid 32 bit as a hex string, 0xXXXXXXXX )
    TBuf8<10> uid;
    uid.Append( '0' );
    uid.Append( 'x' );
    uid.AppendNum( aAppConfHeader.ThemeUid(), EHex );
    aAppConfInfoMap.InsertL( KHspsLiwUid, TLiwVariant( uid ) );
    // pluginInfo.name ( 16 bit string to 8 bit string )
    HBufC8* name = HBufC8::NewL( ( aAppConfHeader.ThemeFullName() ).Length() );
    CleanupStack::PushL( name );
    TPtr8 namePtr = name->Des();
    namePtr.Copy( aAppConfHeader.ThemeFullName() );
    aAppConfInfoMap.InsertL( KHspsLiwName, TLiwVariant( namePtr ) );
    CleanupStack::PopAndDestroy( name );
    }

//----------------------------------------------------------------------------
// CHspsLiwUtilities::BuildPluginListL
// ----------------------------------------------------------------------------
//
void CHspsLiwUtilities::BuildPluginListL(
    ChspsDomNode& aRootNode,
    TDesC8& aType,
    TDesC8& aPluginId,
    CLiwDefaultList& aPluginList )
    {
    
    TBool checkPluginId( EFalse );
    TBool checkType( EFalse );
    if ( aPluginId.Length() )
        {
        checkPluginId = ETrue;
        }
    if ( aType.Length() )
        {
        checkType = ETrue;
        }
    
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aRootNode );
    CleanupStack::PushL( iter );
    ChspsDomNode* node = iter->First();

    TBool pluginIdFound( EFalse );
    while( node && !pluginIdFound )
        {
        const TDesC8& name = node->Name();
        if ( name.Compare( KPluginsElement ) == 0)
            {
            if ( checkPluginId )
                {
                ChspsDomNode* pluginNode = FindParentNodeByTagL( 
                    KPluginElement,
                    *node );
                if ( pluginNode )
                    {
                    TPtrC8 pluginId;
                    GetAttributeValueL( 
                        *pluginNode, 
                        KPluginAttrId, 
                        pluginId );
                    if ( aPluginId.Compare( pluginId ) == 0 )
                        {
                        pluginIdFound = ETrue;
                        }
                    }
                }
            if ( !checkPluginId || pluginIdFound )
                {
                ChspsDomList& children = node->ChildNodes();
                TInt length = children.Length();
                for( TInt i = 0; i < length; i++ )
                    {
                    ChspsDomNode* plugin;
                    plugin = static_cast<ChspsDomNode*>( children.Item( i ) );
                    TBool typeMatch( EFalse );
                    if ( checkType )
                        {
                        // Check plugin type
                        TInt index = 0;
                        ChspsDomNode* confNode = FindChildNodeByTagL(
                            KConfigurationElement,
                            *plugin,
                            index );
                        if ( confNode )
                            {
                            TPtrC8 type;
                            GetAttributeValueL( 
                                *confNode, 
                                KConfigurationAttrType, 
                                type );
                            if ( aType.Compare( type ) == 0 )
                                {
                                typeMatch = ETrue;
                                }
                            }
                        }
                    
                    if ( typeMatch || !checkType )
                        {
                        CLiwDefaultMap* map = CLiwDefaultMap::NewL();
                        CleanupStack::PushL( map );
                        TInt pluginMapItems = ( 
                            CHspsLiwUtilities::EPluginMapId +
                            CHspsLiwUtilities::EPluginMapType +
                            CHspsLiwUtilities::EPluginMapName +
                            CHspsLiwUtilities::EPluginMapActivationState +
                            CHspsLiwUtilities::EPluginMapDesc +
                            CHspsLiwUtilities::EPluginMapLocked );
                        BuildPluginMapL( *plugin, *map, pluginMapItems );
                        
                        TLiwVariant mapVariant;
                        mapVariant.Set( map );
                        aPluginList.AppendL( mapVariant );
                        
                        CleanupStack::Pop( map );
                        map->DecRef();
                        }
                    }
                }
            }
        node = iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter );
    }

//----------------------------------------------------------------------------
// CHspsLiwUtilities::FindParentNodeByTagL
// ----------------------------------------------------------------------------
//
ChspsDomNode* CHspsLiwUtilities::FindParentNodeByTagL(
    const TDesC8& aNodeTag, 
    ChspsDomNode& aChildNode )
    {

    TBool found( EFalse );
    ChspsDomNode* parentNode = aChildNode.Parent();
    while ( !found && parentNode )
        {
        if ( aNodeTag.Compare( parentNode->Name() ) == 0 )
            {
            found = ETrue;
            }
        else
            {
            parentNode = parentNode->Parent();
            }
        }
    
    return parentNode;
    }

// -----------------------------------------------------------------------------
// Creates output parmater list for SetActivePlugin method
// ----------------------------------------------------------------------------- 
void CHspsLiwUtilities::SetActivePluginOutputL( 
    CLiwGenericParamList& aOutParamList )
    {
    
    AppendStatusL( KErrNone, aOutParamList );

    }
