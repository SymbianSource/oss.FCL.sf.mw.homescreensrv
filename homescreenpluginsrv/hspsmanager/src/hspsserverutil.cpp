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
* Description:  Utility for common code.
*
*/


#include "hspsthememanagement.h"
#include "hspsserverutil.h"
#include "hspsodt.h"
#include "hspsdomdepthiterator.h"
#include "hspsdomnode.h"
#include "hspsdomdocument.h"
#include "hspsconfiguration.h"
#include "hspsdomlist.h"
#include "hspsresource.h"
#include "hspsdomattribute.h"
#include "hspsmanifest.h"
#include "bautils.h" 
#include "sysutil.h"


_LIT(KHspsFolder, "\\200159c0\\themes\\" );
_LIT(KSourcesFolder, "\\sources\\");
_LIT( KThemesFolder, "\\themes\\" );
_LIT( KDoubleBackSlash, "\\" );
_LIT8( KHexPrefix8, "0x" );

// -----------------------------------------------------------------------------
// hspsServerUtil::GenerateConfigurationAttributesL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GenerateConfigurationAttributesL( ChspsODT& aOdt )
    {
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );   
    CleanupStack::PushL( iter );
                        
    ChspsDomNode* prevNode = NULL;
    ChspsDomNode* node = iter->First();
    while( node && prevNode != node )
        {           
        const TDesC8& name = node->Name();
        
        // Configuration element 
        if ( name == KConfigurationElement )
            {                                                   
            // Add attributes for the configuration node        
            // NOTE! ID attribute is not set here           
            TPtrC8 ptr = KManifestTypeApp().Ptr();
            switch ( aOdt.ConfigurationType() )
                {
                case EhspsViewConfiguration:
                    {
                    ptr.Set( KManifestTypeView );
                    }
                    break;
                case EhspsWidgetConfiguration:
                    {
                    ptr.Set( KManifestTypeWidget );
                    }
                    break;
                case EhspsTemplateConfiguration:
                    {
                    ptr.Set( KManifestTypeTemplate );
                    }
                    break;
                default:
                    {                   
                    }
                }
                    
            AddAttributeDescL( *node, KConfigurationAttrType, ptr );
            
            AddAttributeNumericL( *node, KConfigurationAttrInterface, aOdt.RootUid(), EHex );
            
            AddAttributeNumericL( *node, KConfigurationAttrUid, aOdt.ThemeUid(), EHex );
            
            // Create "name" and "_name" attributes, of which latter holds the entity reference 
            // (logical key for finding localizad strings)
            HBufC8* nameBuf = HBufC8::NewLC( aOdt.ThemeFullName().Length() );
            TPtr8 namePtr( nameBuf->Des() );
            namePtr.Copy( aOdt.ThemeFullName() );
            AddAttributeDescL( *node, KConfigurationAttrName, namePtr ); // will be updated when localized
            AddAttributeDescL( *node, KConfigurationAttrNameEntity, namePtr ); // logical id
            CleanupStack::PopAndDestroy( nameBuf );                        

            // Create theme version attribute
            HBufC8* tv = HBufC8::NewLC( aOdt.ThemeVersion().Length() );
            TPtr8 tvPtr( tv->Des() );
            tvPtr.Copy( aOdt.ThemeVersion() );
            AddAttributeDescL( *node, KConfigurationAttrVersion, tvPtr );
            CleanupStack::PopAndDestroy( tv );
            
            AddAttributeNumericL ( *node, KConfigurationAttrMultiInstance,
                aOdt.MultiInstance(), EDecimal );

            AddAttributeDescL( *node, KConfigurationAttrState, KConfStateNotConfirmed );
            
            const TInt descLength = aOdt.Description().Length();
            if ( descLength )
                {
                // Add description of the widget (16 > 8bit conversion) 
                HBufC8* buf = HBufC8::NewLC( descLength );
                TPtr8 bufPtr( buf->Des() );
                bufPtr.Copy( aOdt.Description() );
                AddAttributeDescL( *node, KConfigurationAttrDesc, bufPtr ); // will be updated when localized
                AddAttributeDescL( *node, KConfigurationAttrDescEntity, bufPtr ); // logical id
                CleanupStack::PopAndDestroy( buf );
                }
            else
                {
                AddAttributeDescL( *node, KConfigurationAttrDesc, KNullDesC8 );
                }
            
            }       
        
        prevNode = node;    
        node = iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter );
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::GenerateObjectAttributesL()
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GenerateObjectAttributesL( ChspsODT& aOdt )
    {
    // Find the configuration node
    ChspsDomDocument& dom = aOdt.DomDocument();     
    ChspsDomNode* configurationNode = dom.RootNode();
    if ( !configurationNode )
        {
#ifdef _hsps_DEBUG_       
        RDebug::Print( _L("hspsServerUtil::GenerateObjectAttributesL(): - invalid ODT!") );
#endif      
        User::Leave( KErrGeneral );
        }
    
    // Find resources node from the XML definition (child of the configuration node)
    ChspsDomList& childsList = configurationNode->ChildNodes(); 
    ChspsDomNode* resourcesNode = (ChspsDomNode *)childsList.FindByName( KResourcesElement );
    TInt popResources = EFalse;
    if ( !resourcesNode )
        {
        // Create a node and add it into the configuration node
        resourcesNode = dom.CreateElementNSL( 
            KResourcesElement,      
            configurationNode->Namespace()          
            );                              
        CleanupStack::PushL( resourcesNode );
        popResources = ETrue;
        configurationNode->AddChildL( resourcesNode );
        resourcesNode->SetParent( configurationNode );
        }    
                              
    // Loop ODT's resources (parsed ealier from the manifest file)
    const TInt resourceCount = aOdt.ResourceCount();
    TBool addResource = EFalse;
    for( TInt resourceIndex=0; resourceIndex < resourceCount; resourceIndex++ )
        {                
        ChspsResource& resource = aOdt.ResourceL( resourceIndex );                                
        
        addResource = EFalse;
        
        // If resource is located under the sources folder
        if ( resource.FileName().FindF( KSourcesFolder ) > 0 )        
            {                        
            // If resource is for the active device language or it's a locale independent resource
            addResource = ( resource.Language() == aOdt.OdtLanguage() || resource.Language() == ELangNone );                                     
            }
                
        if ( addResource )        	
        	{          	
	         // Create an object node and add the object into the resources list                    
	        ChspsDomNode* objectNode = dom.CreateElementNSL( 
	            KObjectElement,         
	            resourcesNode->Namespace()          
	            );                              
	        CleanupStack::PushL( objectNode );
	        resourcesNode->AddChildL( objectNode );
	        objectNode->SetParent( resourcesNode );
	        
	        // Name (16->8bit conversion)
	        HBufC8* nameBuf = HBufC8::NewLC( resource.ResourceId().Length() );
	        TPtr8 namePtr( nameBuf->Des() );
	        namePtr.Copy( GetFixedOdtName( resource.ResourceId() ) );            
	        
	        AddAttributeDescL( *objectNode, KObjectAttrFilename, namePtr );
	        CleanupStack::PopAndDestroy( nameBuf );
	        
	        // Media type
	        TPtrC8 mimePtr( resource.MimeType().Des8() );           
	        if ( mimePtr.Length() )
	            {
	            AddAttributeDescL( *objectNode, KObjectAttrMediatype, mimePtr );
	            }
	        
	        // Tag
            TPtrC tagPtr( resource.Tags() );           
            if ( tagPtr.Length() )
                {
                // (16->8bit conversion)
                HBufC8* buf = HBufC8::NewLC( tagPtr.Length() );
                TPtr8 bufPtr( buf->Des() );
                bufPtr.Copy( tagPtr );
                AddAttributeDescL( *objectNode, KObjectAttrTag, bufPtr );
                CleanupStack::PopAndDestroy( buf );
                }
	                
	        // Path 
	        TInt pos = resource.FileName().FindF( KHspsFolder );
	        if ( pos > 0 )
	            {
	            // Remove filename and extension from the path
	            TParsePtrC parserPtr( resource.FileName() );
	            TFileName path( parserPtr.DriveAndPath() );
	            	            
	            // Remove path to the Definition repository 
	            path.Copy( path.Mid( pos + KHspsFolder().Length() ) );
	            
	            // Fix path references for localized resources
	            GetLocaleIndependentResourcePath( resource.Language(), path );	            	            
	            
	            // 16->8bit conversion	            
	            HBufC8 *pathBuf = HBufC8::NewLC( path.Length() );                  	            
	            pathBuf->Des().Copy( path );
	            	            	            
	            AddAttributeDescL( *objectNode, KObjectAttrPath, pathBuf->Des() );
	            CleanupStack::PopAndDestroy( pathBuf );
	            }
	        
	        // Now the document has an ownership of the objectnode
	        CleanupStack::Pop( objectNode );
	        
        	}
        
        } // for loop

    if ( popResources )
        {
        // Now the document has an ownership of the resourcesNode
        CleanupStack::Pop( resourcesNode );
        }
    }


TFileName hspsServerUtil::GetFixedOdtName( 
        const TDesC& aNameAndExtension )
    {        
    TParsePtrC parsePtr( aNameAndExtension );                      
    TPtrC fileExtension = parsePtr.Ext();
    if ( fileExtension.Length() > 2 && fileExtension.Left(2).CompareF( _L(".o") ) == 0 )
        {            
        // Strip the first letter            
        TInt odtIndex(0);                  
        TLex lex( fileExtension.Mid(3) );
        if ( lex.Val( odtIndex ) == KErrNone && odtIndex >= 0 )
            {                
            fileExtension.Set( _L(".o0000") );
            }
        }
    
    TFileName fileName( parsePtr.Name() );
    fileName.Append( fileExtension );
    return fileName;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetLocaleIndependentResourcePath
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GetLocaleIndependentResourcePath(
		const TLanguage& aResourceLanguage,		 
		TFileName& aPath )
	{
	TBool isLocaleSpecificResource = ( aResourceLanguage != ELangNone );		
	
	// If locale specific resource
    if ( isLocaleSpecificResource )
    	{
    	// Remove locale specific subfolder from the path
    	TInt pos = aPath.FindF( KSourcesFolder );
    	if ( pos )
    		{    		    		    		    	
    		aPath.Copy( aPath.Left( pos + KSourcesFolder().Length() ) );
    		}
    	}    
	}

// -----------------------------------------------------------------------------
// hspsServerUtil::GetRelativeResourcePath
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GetRelativeResourcePath(
        const TFileName& aSourceFile,
        TPath& aRelativePath )
    {    
    // find last part (structure after "/themes/") 
    TInt pos = aSourceFile.FindF( KThemesFolder );
    if( pos != KErrNotFound )
        {
        pos += KThemesFolder().Length();        
        TInt len( aSourceFile.Length() - pos );               
        aRelativePath.Copy( aSourceFile.Right( len ) );                     
        }    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::AddAttributeNumericL
// -----------------------------------------------------------------------------
//
TInt hspsServerUtil::AddAttributeNumericL(
        ChspsDomNode& aNode,
        const TDesC8& aAttrName, 
        const TInt aValue,
        const TRadix aFormat )
    {
    // Format change
    // Value string, 10 -> Max decimal 4294967295 , max hexadecimal 0xXXXXXXXX
    const TInt KMaxLength = 10;
    TBuf8<KMaxLength> attValueDes8; 
    
    if ( aFormat == EHex )
        {
        _LIT8( KFormat8, "%X" );
        _LIT8( KHexPrefix, "0x" );
        attValueDes8.Append( KHexPrefix );
        attValueDes8.AppendFormat( KFormat8, aValue );
        }
    else // EDecimal
        {
        _LIT8( KFormat8, "%d" );
        attValueDes8.AppendFormat( KFormat8, aValue );
        }
    
    ChspsDomList& attrList = aNode.AttributeList();
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(aAttrName) );
    if ( attr )
        {               
        // Replace value of the attribute
        attr->SetValueL( attValueDes8 );
        }
    else
        {              
        // Add an attribute
        ChspsDomAttribute* attr = ChspsDomAttribute::NewL( aAttrName, aNode.StringPool() );    
        CleanupStack::PushL( attr );            
        attr->SetValueL( attValueDes8 );
        ChspsDomList& attrList = aNode.AttributeList();
        attrList.AddItemL( attr );    //takes ownership
        CleanupStack::Pop( attr );
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::AddAttributeDescL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::AddAttributeDescL(
        ChspsDomNode& aNode,
        const TDesC8& aAttrName, 
        const TDesC8& aValue )
    {
    ChspsDomList& attrList = aNode.AttributeList();
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(aAttrName) );
    if ( attr )
        {               
        // Replace value of the attribute
        attr->SetValueL( aValue );
        }
    else
        {              
        // Add an attribute
        ChspsDomAttribute* attr = ChspsDomAttribute::NewL( aAttrName, aNode.StringPool() );    
        CleanupStack::PushL( attr );            
        attr->SetValueL( aValue );
        ChspsDomList& attrList = aNode.AttributeList();
        attrList.AddItemL( attr );    //takes ownership
        CleanupStack::Pop( attr );
        }        
    }


// -----------------------------------------------------------------------------
// Finds a configuration node with the provided id attribute
// -----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::FindConfigurationNodeL(
		ChspsODT& aOdt,		
		const TInt aConfigurationId )
	{
	__UHEAP_MARK;
	
    ChspsDomNode *configurationNode = NULL;
    
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );

    // Find a configuration node with an id attribute that matches the provided id
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )    
        {                
        const TDesC8& name = node->Name();
        
        // An element was found 
        if ( name == KConfigurationElement )
            {           
            ChspsDomList& attrList = node->AttributeList();                    
            ChspsDomAttribute* idAttr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KConfigurationAttrId) );            
            if ( !idAttr )
                {
                // Return with NULL
                jobDone = ETrue;                                                 
                }
            else
                {            
                TInt id(0);            
                const TDesC8& idValue = idAttr->Value();                        
                TLex8 lex( idValue );                        
                lex.Val( id );            
                if ( aConfigurationId == id )
                    {
                    configurationNode = node;        
                    jobDone = ETrue;
                    }
                }
            }
        
        prevNode = node;        
        node = iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter );

#ifdef _hsps_DEBUG_
    if ( !configurationNode )
        {
        RDebug::Print( _L("hspsServerUtil::FindConfigurationNodeL(): failed to find the configuration node") );
        }
#endif
    
    __UHEAP_MARKEND;
    
    return configurationNode;
    }

// -----------------------------------------------------------------------------
// Finds a plugin node with the provided id
// -----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::FindPluginNodeL(
        ChspsODT& aOdt,        
        const TInt aPluginId )
    {            
    __UHEAP_MARK;
    
    ChspsDomNode* pluginNode = NULL;
    
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );

    // Find a plugin node with the provided id attribute
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )
        {                
        const TDesC8& name = node->Name();
        
        // Plugin element was found 
        if ( name == KPluginElement )
            {           
            ChspsDomList& attrList = node->AttributeList();                    
            ChspsDomAttribute* idAttr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrId) );            
            if ( !idAttr )
                {
                // Mandatory information is missing for some reason (should be set at installation handler)!
                // Exit with NULL
                jobDone = ETrue;                
                }
            else
                {            
                TInt id(0);            
                const TDesC8& idValue = idAttr->Value();                        
                TLex8 lex( idValue );                        
                lex.Val( id );            
                if ( aPluginId == id )
                    {
                    pluginNode = node;
                    jobDone = ETrue;
                    }
                }
            }
        
        prevNode = node;        
        node = iter->NextL();        
        }
    CleanupStack::PopAndDestroy( iter );

#ifdef _hsps_DEBUG_
    if ( !pluginNode )
        {
        RDebug::Print( _L("hspsServerUtil::FindPluginNodeL(): failed to find the plugin node") );
        }
#endif
    
    __UHEAP_MARKEND;
    
    return pluginNode;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetConfigurationNameFromDomL
// -----------------------------------------------------------------------------
//
TPtrC8 hspsServerUtil::FindConfigurationAttrL( 
    const ChspsODT& aOdt,
    const TDesC8& aAttr )
    {
    TPtrC8 ptr;
    
    // Get ODT's DOM and find the 1st configuration node
    ChspsDomNode* confNode = aOdt.DomDocument().RootNode();
    if( !confNode || confNode->Name().CompareF( KConfigurationElement) != 0 )
        {            
        User::Leave( KErrGeneral );            
        }                
        
    // Find the name attribute and return it's value
    ChspsDomList& attrList = confNode->AttributeList();
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(aAttr) );                
    if ( !attr )
        {
        User::Leave( KErrGeneral );
        }            
    ptr.Set( attr->Value() );
            
    return ptr;
    }    

// -----------------------------------------------------------------------------
// hspsServerUtil::CopyResourceFileL
// -----------------------------------------------------------------------------
//
TInt hspsServerUtil::CopyResourceFileL(
        RFs& aFs,
        CFileMan& aFilemanager,
        const TPath& aTargetPath,
        const TFileName& aSourceFile )
    {
    // Make target folder
    TInt error = aFs.MkDirAll( aTargetPath );            
    if( error == KErrAlreadyExists )
        {
        // lets ignore error if directory already exists                
        error = KErrNone;
        }       
    if( !error )
        {
        // Default to 'copying was not neccessary'
        error = KErrAlreadyExists;
        
        // Check whether the resource needs to be copied
        if ( hspsServerUtil::ResourceCopyIsRelevantL( 
                aSourceFile,
                aTargetPath,
                aFs ) 
            )
            {        
            // Slowish operation
            error = aFilemanager.Copy( 
                    aSourceFile, 
                    aTargetPath );
            if( !error )
                {
                // Clear readonly file attribs that might be inherited from the source file                
                aFilemanager.Attribs( 
                    aTargetPath,
                    0,
                    KEntryAttReadOnly,
                    TTime( 0 ) ); // TTime(0) = preserve original time stamp.                                                                      
                }
            
            } // copy op        
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::ResourceCopyIsRelevant
// -----------------------------------------------------------------------------
//
TBool hspsServerUtil::ResourceCopyIsRelevantL( 
    const TDesC& aSource,
    const TDesC& aTarget,
    RFs& aFs )
    {
    // Basic sanity check.
    if( aSource.Length() == 0 || aTarget.Length() == 0 )
        {
        return EFalse;
        }

    // Collect data from files.
    
    TEntry sourceEntry;
    TInt entryError = aFs.Entry( aSource, sourceEntry ); 
    if( entryError != KErrNone )
        {
        // Problem. Do not copy.
        return EFalse;
        }            

    TEntry targetEntry;
    entryError = aFs.Entry( aTarget, targetEntry ); 
    if( entryError == KErrNotFound )
        {
        // Target does not exist. Copy needed.
        return ETrue;
        }           
    else if( entryError != KErrNone ) 
        {
        // All other errors handled here. Better not to copy.
        return EFalse;
        }
    
    TParse sourceParser;
    sourceParser.Set( aSource, NULL, NULL );
    
    TParse targetParser;
    targetParser.Set( aTarget, NULL, NULL );
            
    // We have tdesc of target drive but SysUtils need TDriveNumber instead
    // so extract it from tdesc. default to C.
    TInt targetDriveNumber = EDriveC;
        
    // Sanity checks before accessing descriptor (prevent "out of bounds" panic).
    if( targetParser.DrivePresent() &&
        targetParser.Drive().Length() > 0 )
        {
        // Use tmp variable so that CharToDrive does not mess our fallback
        // value. (it should not do that in case of error... but better
        // to do fool-proof.)
        TInt tmpDriveNumber = EDriveC;
        
        // Convert from TDesC to enumeration of drives.
        if( RFs::CharToDrive( targetParser.Drive()[0], tmpDriveNumber ) == KErrNone )
            {
            targetDriveNumber = tmpDriveNumber;
            }        
        }   

    const TInt KTargetExists = BaflUtils::FileExists( aFs, aTarget ); 
           
    // Target exists + size and time stamp identical?
    if( KTargetExists &&
        sourceEntry.iSize == targetEntry.iSize &&
        sourceEntry.iModified == targetEntry.iModified )
        {
        return EFalse;
        }    
    
    // Check required disk space.
    TInt requiredDiskSpace = 0;
    
    if( KTargetExists && sourceEntry.iSize > targetEntry.iSize )
        {
        requiredDiskSpace = sourceEntry.iSize - targetEntry.iSize; 
        }
    else if( !KTargetExists )
        {
        requiredDiskSpace = sourceEntry.iSize;
        }
    
    if( requiredDiskSpace != 0 )
        {
        if( SysUtil::DiskSpaceBelowCriticalLevelL( &aFs, requiredDiskSpace, targetDriveNumber ) )
            {
            return EFalse;
            }
        }
        
    // All tests passed.
    return ETrue;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::UpdateConfigurationStateL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::UpdateConfigurationStateL( 
    ChspsODT& aOdt,
    TDesC8& aConfState,
    TDesC8& aNextConfState,
    TBool& aOdtUpdated )
    {
    aOdtUpdated = EFalse;
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );   
    CleanupStack::PushL( iter );                  
    ChspsDomNode* prevNode = NULL;
    ChspsDomNode* node = iter->First();
    
    while( node && prevNode != node )
        {           
        const TDesC8& name = node->Name();
        
        // Configuration element 
        if ( name == KConfigurationElement )
            {
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                attrList.FindByName( KConfigurationAttrState ) );               
            if ( attr && attr->Value().CompareF( aConfState ) == 0 )
                {
                attr->SetValueL( aNextConfState );
                aOdtUpdated = ETrue;
                }           
            }       
        
        prevNode = node;    
        node = iter->NextL();
        }

    CleanupStack::PopAndDestroy( iter );
    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::UpdateAppConfigurationStateL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::UpdateAppConfigurationStateL( 
        ChspsODT& aAppOdt,
        const TDesC8& aConfState,
        const TDesC8& aNextConfState )
    {
    __ASSERT_DEBUG( aConfState.Length() > 0, User::Leave( KErrArgument) );
    __ASSERT_DEBUG( aNextConfState.Length() > 0, User::Leave( KErrArgument) );
        
    // Update application configuration state
    ChspsDomNode* appConfNode = hspsServerUtil::FindNodeByAttributeL(
        aAppOdt,
        KConfigurationElement,
        KConfigurationAttrType,
        KConfTypeApp );
    ChspsDomList& attrList = appConfNode->AttributeList();
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
        attrList.FindByName( KConfigurationAttrState ) );               
    if ( attr && attr->Value().CompareF( aConfState ) == 0 )
        {
        attr->SetValueL( aNextConfState );
        }
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::FindNodeByAttributeL
// -----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::FindNodeByAttributeL( 
    ChspsODT& aOdt,
    const TDesC8& aNodeName,
    const TDesC8& aAttrName, 
    const TDesC8& aAttrValue)
    {
    __UHEAP_MARK;
    
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );   
    CleanupStack::PushL( iter );
                        
    ChspsDomNode* foundNode( NULL );
    ChspsDomNode* prevNode( NULL );
    ChspsDomNode* node = iter->First();
    while( node && 
           prevNode != node &&
           foundNode == NULL )
        {           
        const TDesC8& name = node->Name();        
        if ( name.CompareF( aNodeName ) == 0 )
            {
            // Node name match
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                attrList.FindByName( aAttrName ) );               
            if ( attr && attr->Value().CompareF( aAttrValue ) == 0 )
                {
                // Attribute name and value match
                foundNode = node;
                }           
            }       
        // Get next node
        prevNode = node;    
        node = iter->NextL();
        }

    CleanupStack::PopAndDestroy( iter );
    
    __UHEAP_MARKEND;
    
    return foundNode;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::FindUniquePluginsL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::FindUniquePluginsL( 
        ChspsODT& aOdt, 
        RArray<TInt>& aPluginArray )
    {    
    aPluginArray.Reset();          
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );
    
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    while( node && prevNode != node )
       {                
       const TDesC8& name = node->Name();
       
       // Plugin element was found 
       if ( name == KPluginElement )
           {           
           ChspsDomList& attrList = node->AttributeList();                    
           ChspsDomAttribute* uidAttr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrUid) );            
           if ( uidAttr )
               {     
               // Convert from hex to int
               const TUid pluginUid = ConvertDescIntoUid( uidAttr->Value() );               
               if ( pluginUid.iUid > 0 )
                   {
                   TBool isUnique = ETrue;
                   for( TInt i=0; isUnique && i<aPluginArray.Count();i++ )
                       {
                       if ( aPluginArray[i] == pluginUid.iUid )
                           {
                           isUnique=EFalse;
                           }
                       }
                   if ( isUnique )
                       {
                       aPluginArray.Append( pluginUid.iUid );
                       }
                   }               
               }
           }
           
           prevNode = node;        
           node = iter->NextL();        
           }
   CleanupStack::PopAndDestroy( iter );      
}

//----------------------------------------------------------------------------
// CHspsServiceUtilities::HexString2Uint
// ----------------------------------------------------------------------------
//
TInt hspsServerUtil::HexString2Uint(
    const TDesC8& aStr,
    TUint& aTrg )
    {
    // Assign to lexer.
    TLex8 lex( aStr );
    
    // Ignore preceding "0x" if it exists. TLex does not know how to handle that
    // and returns just zero.
    if( aStr.Length() >= KHexPrefix8().Length() &&
        aStr.FindF( KHexPrefix8() ) == 0 )
        {
        lex.Inc( KHexPrefix8().Length() );
        }
    
    // Actual conversion.
    TInt status = KErrNone;
    TUint val = 0;
    status = lex.Val( val, EHex );
    
    // Error check.
    if( status == KErrNone )
        {
        aTrg = val;
        }
    
    return status;
    }

//----------------------------------------------------------------------------
// CHspsServiceUtilities::DecString2Int
// ----------------------------------------------------------------------------
//
TInt hspsServerUtil::DecString2Int(
    const TDesC8& aStr )
    {
    TLex8 lex( aStr );
    TUint value;
    lex.Mark();
    
    while ( lex.Peek().IsDigit() )
        {
        lex.Inc();
        }
    TPtrC8 uidToken = lex.MarkedToken();
    TLex8 uidLex( uidToken );
    TInt err = uidLex.Val( value, EDecimal );
    
    return value;
    }
// -----------------------------------------------------------------------------
// Returns a count of plugin instances.
// -----------------------------------------------------------------------------
//
void hspsServerUtil::PluginInstanceCountL(
        const ChspsODT& aAppODT,        
        const TInt aPluginUid,
        TInt& aInstanceCount )              
    {       
    aInstanceCount = 0;
    
    ChspsDomDocument& dom = aAppODT.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );

    // Find a plugin node with the provided id attribute
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )
        {               
        const TDesC8& name = node->Name();
        
        // Plugin element was found 
        if ( name == KPluginElement )
            {           
            ChspsDomList& attrList = node->AttributeList();                 
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrUid) );           
            if ( !attr )
                {
                // Mandatory information is missing for some reason (should be set at installation handler)!
                // Exit with NULL
                jobDone = ETrue;                
                }
            else
                {
                // Convert from (hex?) string into TUid presentation
                const TUid uid = ConvertDescIntoUid( attr->Value() );                          
                if ( aPluginUid == uid.iUid )
                    {
                    aInstanceCount++;
                    }
                }
            }
        
        prevNode = node;        
        node = iter->NextL();       
        }
    CleanupStack::PopAndDestroy( iter );    
    }
// -----------------------------------------------------------------------------
// hspsServerUtil::ConvertDescIntoUid()
//----------------------------------------------------------------------------
//
TUid hspsServerUtil::ConvertDescIntoUid(
        const TDesC8& aStr )
    {
    TLex8 lex(aStr);
    TUint pluginUid;
    lex.Mark();
    
    if(lex.Peek() == '0')
        {
        lex.Inc();
        }
    if(lex.Peek() == 'x')
        {
        lex.Inc();
        }
    lex.Mark();
    while (lex.Peek().IsHexDigit())
        {
        lex.Inc();
        }
    TPtrC8 uidToken = lex.MarkedToken();
    TLex8 uidLex(uidToken);
    TInt err = uidLex.Val(pluginUid,EHex);
    
    return TUid::Uid(pluginUid);
    }
//----------------------------------------------------------------------------
// hspsServerUtil::FindChildNodeByTagL()
// ----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::FindChildNodeByTagL(
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
// hspsServerUtil::FindFilesL()
// ----------------------------------------------------------------------------
//
void hspsServerUtil::FindFilesL(
    const TDesC& aDirName, 
    const TDesC& aFileName,
    RArray <TFileName>& aFiles )
    {
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    // Find files from root directory
    hspsServerUtil::FindFilesFromDirL( 
            aDirName, 
            aFileName, 
            aFiles );
    
    // Directory scanner to browse directory structure
    CDirScan* dirScan = CDirScan::NewL( fs );
    CleanupStack::PushL( dirScan );
    dirScan->SetScanDataL( 
        aDirName, 
        ( KEntryAttDir | KEntryAttMatchExclusive ), 
        ESortNone );
    
    // Directory path where installation files are searched
    TFileName dirName;

    // Get first directory list
    CDir* dirList( NULL );
    dirScan->NextL( dirList );
    // Find files from root directories
    while ( dirList )
        {
        CleanupStack::PushL( dirList );
        for ( TInt i = 0; i < dirList->Count(); i++ )
            {
            // Get directory path
            dirName = dirScan->FullPath();
            // Append directory entry
            const TEntry& dirEntry = ( *dirList )[ i ];
            dirName.Append( dirEntry.iName );
            dirName.Append( KDoubleBackSlash );
            hspsServerUtil::FindFilesFromDirL( 
                dirName, 
                aFileName, 
                aFiles );
            }
        // Get next directory list
        CleanupStack::PopAndDestroy( dirList );
        dirScan->NextL( dirList );
        }
    
    CleanupStack::PopAndDestroy( dirScan );

    CleanupStack::PopAndDestroy(); // fs
    
    }

//----------------------------------------------------------------------------
// hspsServerUtil::FindFilesFromDirL()
// ----------------------------------------------------------------------------
//
void hspsServerUtil::FindFilesFromDirL(
    const TDesC& aDirName, 
    const TDesC& aFileName,
    RArray <TFileName>& aFiles )
    {
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    // File finder to search files from a directory
    TFindFile fileFinder( fs );
    
    // Define drives where files are searched
    TInt driveNumber;
    TParse dirParser;
    dirParser.Set( aDirName, NULL, NULL );
    // Default drives C: and Z:
    TInt findMask( 
            KDriveAttExclude |
            KDriveAttRemovable |
            KDriveAttRemote );
    if( RFs::CharToDrive( dirParser.Drive()[0], driveNumber ) == KErrNone )
        {
        if ( driveNumber == EDriveC )
            {
            // Search from C: drive
            findMask = ( 
                KDriveAttExclude | 
                KDriveAttRemovable | 
                KDriveAttRemote | 
                KDriveAttRom );
            }
        else if ( driveNumber == EDriveZ )
            {
            // Search from Z: drive
            findMask = KDriveAttRom;
            }
        }            
    fileFinder.SetFindMask( findMask ); 
    
    // Find files from the directory entry
    CDir* fileList( NULL );
    fileFinder.FindWildByDir( aFileName, aDirName, fileList );
    CleanupStack::PushL( fileList );
    for ( TInt k = 0; fileList && k < fileList->Count(); k++ )
        {
        // Add found file with full path to file array
        const TEntry& fileEntry = (*fileList)[k];
        TFileName file;
        file.Append( aDirName );
        file.Append( fileEntry.iName );
        aFiles.Append( file );
        }
    CleanupStack::PopAndDestroy( fileList );

    CleanupStack::PopAndDestroy(); // fs
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::SetAttributeValueL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::SetAttributeValueL( 
    const ChspsODT& aOdt,
    const TDesC8& aNodeName,
    const TDesC8& aAttrName, 
    const TDesC8& aAttrValue,
    const TDesC8& aSetAttrName,
    const TDesC8& aSetAttrValue )
    {
    
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );   
    CleanupStack::PushL( iter );
                        
    TBool nodeFound( EFalse );
    ChspsDomNode* prevNode( NULL );
    ChspsDomNode* node = iter->First();
    while( node && 
           prevNode != node )
        {           
        const TDesC8& name = node->Name();        
        if ( name.CompareF( aNodeName ) == 0 )
            {
            // Node name match
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                attrList.FindByName( aAttrName ) );               
            if ( attr && attr->Value().CompareF( aAttrValue ) == 0 )
                {
                // Attribute name and value match - Defined node found
                attr = static_cast<ChspsDomAttribute*>( 
                    attrList.FindByName( aSetAttrName ) );
                if ( attr )
                    {
                    // Updated attribute found - Update attribute value
                    attr->SetValueL( aSetAttrValue );
                    }
                else
                    {
                    // Add new attribute
                    AddAttributeDescL( *node, aSetAttrName, aSetAttrValue );
                    }
                nodeFound = ETrue;
                }
            }       
        // Get next node
        prevNode = node;    
        node = iter->NextL();
        }

    if ( !nodeFound )
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( iter );
        
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetAttributeValueL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GetAttributeValueL( 
    const ChspsODT& aOdt,
    const TDesC8& aNodeName,
    const TDesC8& aAttrName, 
    const TDesC8& aAttrValue,
    const TDesC8& aGetAttrName,
    TPtrC8& aGetAttrValue )
    {
    __UHEAP_MARK;
    
    ChspsDomDocument& dom = aOdt.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );   
    CleanupStack::PushL( iter );
                        
    TBool nodeFound( EFalse );
    ChspsDomNode* prevNode( NULL );
    ChspsDomNode* node = iter->First();
    while( node && 
           prevNode != node &&
           !nodeFound )
        {           
        const TDesC8& name = node->Name();        
        if ( name.CompareF( aNodeName ) == 0 )
            {
            // Node name match
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                attrList.FindByName( aAttrName ) );               
            if ( attr && attr->Value().CompareF( aAttrValue ) == 0 )
                {
                // Attribute name and value match - Defined node found
                attr = static_cast<ChspsDomAttribute*>( 
                    attrList.FindByName( aGetAttrName ) );
                if ( attr )
                    {
                    // Updated 
                    aGetAttrValue.Set( attr->Value() );
                    nodeFound = ETrue;
                    }
                }
            }       
        // Get next node
        prevNode = node;    
        node = iter->NextL();
        }

    if ( !nodeFound )
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( iter );
    
    __UHEAP_MARKEND;
    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::CheckResourceFilesL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::CheckResourceFilesL( 
    const ChspsODT& aOdt,
    const TInt aConfUid )
    {
    __UHEAP_MARK;

    // Convert configuration UID to decimal string
    TBuf<10> confUid; 
    _LIT( KFormat, "%D" );
    confUid.AppendFormat( KFormat, aConfUid );

    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    
    TInt resCount = aOdt.ResourceCount();
    for ( TInt i = 0; i < resCount; i++ )
        {
        // Check if resource file belongs to defined configuration
        // (file path contains configuration UID string)
        ChspsResource& res = aOdt.ResourceL( i );
        TPtrC resFile = res.FileName();
        if ( resFile.FindC( confUid ) != KErrNotFound )
            {
            // Check that resource files exists
            if ( !BaflUtils::FileExists( fs, resFile ) )
                {
                User::Leave( KErrNotFound );
                }
            }
        }
    
    CleanupStack::PopAndDestroy(); // fs
    
    __UHEAP_MARKEND;
    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetConfigurationVersionL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::CheckConfigurationVersionL( 
    ChspsODT& aOdt,
    const TInt aConfUid,
    const TDesC& aVersion )
    {
    __UHEAP_MARK;

    // Create configuration UID string
    TBuf8<10> confUid;    
    _LIT8( KFormat8, "%X" );
    _LIT8( KHexPrefix, "0x" );
    confUid.Append( KHexPrefix );
    confUid.AppendFormat( KFormat8, aConfUid );

    // Find configuration node
    ChspsDomNode* confNode = hspsServerUtil::FindNodeByAttributeL(
        aOdt,
        KConfigurationElement,
        KConfigurationAttrUid,
        confUid );
    
    if ( confNode )
        {
        ChspsDomList& attrList = confNode->AttributeList();
        ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
            attrList.FindByName( KConfigurationAttrVersion ) );               
        if ( attr )
            {
            HBufC8* tv = HBufC8::NewLC( aVersion.Length() );
            TPtr8 tvPtr( tv->Des() );
            tvPtr.Copy( aVersion );
            // Check configuration version
            if ( attr->Value().Compare( tvPtr ) != 0 )
                {
                // Configuration not supported
                User::Leave( KErrNotSupported );
                }
            CleanupStack::PopAndDestroy( tv );
            }
        else
            {
            // Invalid configuration
            User::Leave( KErrGeneral );
            }
        }
    else
        {
        // Configuration not found
        User::Leave( KErrNotFound );
        }
    
    __UHEAP_MARKEND;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::EditPluginNodeActivityL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::EditPluginNodeActivityL( ChspsDomNode* aRootNode,
                                              const TNodeEditMode aEditMode,
                                              TInt aDepth  )
    {
    // This wrapper exists to prevent modification of original aDepth
    // (_EditPluginNodeActivityL uses TInt reference for depth count and
    // modifies it).
    hspsServerUtil::_EditPluginNodeActivityL( aRootNode, aEditMode, aDepth );
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::_EditPluginNodeActivityL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::_EditPluginNodeActivityL( ChspsDomNode* aRootNode,      
                                               const TNodeEditMode aEditMode,
                                               TInt& aDepth )
    {
    // Sanity check.
    if( !aRootNode )
        {
        return;
        }

    // Recursion depth logic.
    if( aDepth == RECURSION_DEPTH_INFINITE )
        {
        // Work as normal. Recurse as far as object tree exists.
        }
    else if( aDepth > 0 )
        {
        // Depth was defined. Decrement by one.
        aDepth--;
        }
    else if( aDepth == 0 )
        {
        // No more recursion.
        return;
        }
    
    // Iterate childs    
    ChspsDomList& childs = aRootNode->ChildNodes();    
    for( int i = 0; i < childs.Length(); i++ )
        {
        ChspsDomNode* child = static_cast<ChspsDomNode*>( childs.Item( i ) );         
        
        // Modify node activity based on selected edit mode.
        if( child->Name().CompareF( KPluginElement ) == 0 )
            {
            if( aEditMode == EActivateFirst && i == 0 )
                {
                hspsServerUtil::AddAttributeDescL( *child,
                                                   KPluginAttrActive,
                                                   KPluginActiveStateActive );                
                }
            else 
                {
                hspsServerUtil::AddAttributeDescL( *child,
                                                   KPluginAttrActive,
                                                   KPluginActiveStateNotActive );                
                }
            }

        // Recurse.
        _EditPluginNodeActivityL( child,
                                  aEditMode,
                                  aDepth );
        }    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetActivePluginNode
// -----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::GetActivePluginNode( ChspsDomNode* aParentNode )
    {
    // Sanity check.
    if( !aParentNode )
        {
        return NULL;
        }

    // Return structure.
    ChspsDomNode* activeNode = NULL;
    
    // Iterate childs    
    ChspsDomList& childs = aParentNode->ChildNodes();    
    for( int i = 0; i < childs.Length(); i++ )
        {
        ChspsDomNode* child = static_cast<ChspsDomNode*>( childs.Item( i ) );         
        
        // Only for plugin elements.
        if( child->Name().CompareF( KPluginElement ) == 0 )
            {
            // 1. Check if has activity attribute.            
            ChspsDomList& attrList = child->AttributeList();            
            ChspsDomAttribute* pluginActivityAttr = 
                static_cast<ChspsDomAttribute*>( attrList.FindByName( KPluginAttrActive ) );                
            if( pluginActivityAttr )
                {
                // 2. Check whether node is active.
                if( pluginActivityAttr->Value().CompareF( KPluginActiveStateActive ) == 0 )
                    {
                    // 3. Active node found. assign and break.
                    activeNode = child;
                    break;
                    }
                }                
            }
        }
    
    return activeNode;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetPluginId
// -----------------------------------------------------------------------------
//
TInt hspsServerUtil::GetPluginId( ChspsDomNode* aNode )
    {
    // Sanity check.
    if( !aNode )
        {
        return KErrArgument;
        }
    
    // Return value.
    TInt pluginId = KErrNotFound;

    // Find out plugin id.
    ChspsDomList& attrList = aNode->AttributeList();                                                                                                      
    ChspsDomAttribute* pluginIdAttr = 
        static_cast<ChspsDomAttribute*> ( attrList.FindByName( KPluginAttrId ) );        
    if( pluginIdAttr )
        {
        // Found.
        const TDesC8& pluginIdValue = pluginIdAttr->Value();
        pluginId = DecString2Int( pluginIdValue );  
        }
    
    return pluginId;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetPluginUid
// -----------------------------------------------------------------------------
//
TUid hspsServerUtil::GetPluginUid( ChspsDomNode* aNode )
    {
    // Sanity check.
    if( !aNode )
        {
        return KNullUid;
        }
    
    // Read Uid from attribute list.
    ChspsDomList& attrList = aNode->AttributeList();
    ChspsDomAttribute* pluginUidAttr = 
        static_cast<ChspsDomAttribute*> ( attrList.FindByName( KPluginAttrUid ) );                                                            

    // Sanity check.
    if( !pluginUidAttr )
        {
        return KNullUid;
        }
    
    // Convert uids from string to numeric format
    const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
    const TUid pluginUid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);    
    
    // Return result.
    return pluginUid;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetPluginIdsByUidL
// -----------------------------------------------------------------------------
//
void hspsServerUtil::GetPluginIdsByUidL(
        const ChspsODT& aAppODT,        
        const TUid aPluginUid,
        RArray<TInt>& aPluginIds )       
    {       
    ChspsDomDocument& dom = aAppODT.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );

    // Find a plugin node with the provided id attribute
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )
        {               
        const TDesC8& name = node->Name();
        
        // Plugin element was found 
        if ( name == KPluginElement )
            {           
            ChspsDomList& attrList = node->AttributeList();                 
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrUid) );           
            if ( !attr )
                {
                // Mandatory information is missing for some reason (should be set at installation handler)!
                // Exit with NULL
                jobDone = ETrue;                
                }
            else
                {
                // Convert from (hex?) string into TUid presentation
                const TUid uid = ConvertDescIntoUid( attr->Value() );                          
                if ( aPluginUid == uid )
                    {
                    const TDesC8& strPluginId = node->AttributeValue(KPluginAttrId);
                    TInt pluginId = DecString2Int( strPluginId );           
                    aPluginIds.AppendL( pluginId );                    
                    }
                }
            }
        
        prevNode = node;        
        node = iter->NextL();       
        }
    
    CleanupStack::PopAndDestroy( iter );    
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetParentNode
// -----------------------------------------------------------------------------
//
ChspsDomNode* hspsServerUtil::GetParentNode(
    const ChspsDomNode& aNode,
    const TDesC8& aNodeName,
    const TDesC8& aAttrName,
    const TDesC8& aAttrValue )       
    {
    ChspsDomNode* parent = aNode.Parent();
    TBool found( EFalse );
    while ( parent && !found )
        {
        if ( parent->Name().CompareF( aNodeName ) == 0 )
            {
            ChspsDomList& attrList = parent->AttributeList();                 
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName( aAttrName ) );
            if ( attr->Value().CompareF( aAttrValue ) == 0 )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            // Get next level parent node
            parent = parent->Parent();
            }
        }
    
    return parent;
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::IsFile
// -----------------------------------------------------------------------------
//
TBool hspsServerUtil::IsFile(
        const TDesC& aFileDeclaration,
        TFileName& aFilename )        
    {
    _LIT(KDeclarationSkin, "SKIN(");        // prefixes for filelogo/filepreview values 
    _LIT(KDeclarationMif, "MIF(");
    _LIT(KDeclarationUid, "UID(");
        
    aFilename = KNullDesC();
    if ( aFileDeclaration.FindF( KDeclarationSkin ) >= 0  
            || aFileDeclaration.FindF( KDeclarationMif ) >= 0             
            || aFileDeclaration.FindF( KDeclarationUid ) >= 0 )
        {                
        // pick filename from the mif declaration if it's available (e.g. "skin(<id> <id>):mif(<path> <id> <id>)")
        TInt mifOffset = aFileDeclaration.FindF( KDeclarationMif );
        if ( mifOffset >= 0 )            
            {              
            aFilename = aFileDeclaration.Mid( mifOffset + KDeclarationMif().Length() );
            aFilename.TrimAll();                
                        
            // drop everything before the first bitmap index
            TInt endPos = aFilename.Locate( ' ' );
            if ( endPos > 1 )
                {
                aFilename = aFilename.Left( endPos );                                
                }                                                      
            }                                       
        }
    else
        {
        // store possible icon path
        aFilename.Copy( aFileDeclaration );
        }
    
    return ( aFilename.Length() > 0 );
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::GetValidResourcesL
// -----------------------------------------------------------------------------
void hspsServerUtil::GetValidResourcesL(
        ChspsODT& aODT,        
        const TInt aConfUid,
        const TLanguage aActiveLanguage,
        RPointerArray<ChspsResource>& aWidgetResources )
    {    
    const TInt resourceCount = aODT.ResourceCount();
            
    // pick resources with the active language       
    for( TInt resourceIndex = 0; resourceIndex < resourceCount; resourceIndex++ )
        {
        ChspsResource& resource = aODT.ResourceL( resourceIndex );        
        if ( resource.ConfigurationUid() == aConfUid &&
             resource.FileName().FindF( KSourcesFolder ) > 0 &&
             resource.Language() == aActiveLanguage )                
            {                                
            aWidgetResources.Append( &resource );
            }
        }        
    
    // Resources for language none or language test.    
    for( TInt resourceIndex = 0; resourceIndex < resourceCount; resourceIndex++ )
        {
        ChspsResource& resource = aODT.ResourceL( resourceIndex );
        if ( resource.ConfigurationUid() == aConfUid &&
             resource.FileName().FindF( KSourcesFolder ) > 0 )
            {
            if( resource.Language() == ELangTest || resource.Language() == ELangNone )
                {
                // Checking also that not going to overwrite existing localized resource.                
                TBool localizedVersionAvailable = EFalse;
                for( TInt checkIndex = 0; checkIndex < aWidgetResources.Count(); checkIndex++ )
                    {
                    ChspsResource* checkResource = aWidgetResources[ checkIndex ];
                    if( !checkResource )
                        {
                        continue;
                        }
                    
                    TParsePtrC resource1FullPath( resource.FileName() );
                    TParsePtrC resource2FullPath( checkResource->FileName() );
                    
                    if( resource1FullPath.NameAndExt().CompareF( resource2FullPath.NameAndExt() ) == 0 )                 
                        {
                        localizedVersionAvailable = ETrue;
                        break;
                        }
                    }
            
                if( !localizedVersionAvailable )
                    {
                    aWidgetResources.Append( &resource );
                    }
                }
            }
        }           
    }

// -----------------------------------------------------------------------------
// hspsServerUtil::EnoughDiskSpaceAvailable
// -----------------------------------------------------------------------------
TInt hspsServerUtil::EnoughDiskSpaceAvailableL(
        ChspsODT& aODT,        
        const TLanguage aActiveLanguage,
        RFs& aFs,
        const TDriveNumber aDriveNumber,
        const TInt aAdditionalDiskSpace )
    {        
    __UHEAP_MARK;
    
    TInt err = KErrNone;   
    
    // Retrieve data for needed resource files.
    RPointerArray<ChspsResource> widgetResources; // Objects are not owned.
    CleanupClosePushL( widgetResources );
    
    GetValidResourcesL( aODT,
        aODT.ThemeUid(),
        aActiveLanguage,
        widgetResources );
    
    TInt requiredDiskSpace = aAdditionalDiskSpace;
    
    // Calculate disk space required for resources.
    for( TInt i = 0; i < widgetResources.Count() && !err; i++ )
        {      
        ChspsResource* resource = widgetResources[i];
        if( !resource )
            {
            continue;
            }
                         
        TEntry entryData;
        err = aFs.Entry( resource->FileName(), entryData );        
        requiredDiskSpace += entryData.iSize;  
        }
    
    widgetResources.Reset();
    CleanupStack::PopAndDestroy( 1, &widgetResources );    
    
    // Check if calculated space is available.
    if( !err &&
        SysUtil::DiskSpaceBelowCriticalLevelL( &aFs,
                requiredDiskSpace,
                aDriveNumber ) )
        {
        err = KErrDiskFull;
        }
        
    __UHEAP_MARKEND;
    
    return err;
    }
// -----------------------------------------------------------------------------
// Finds a node from a dom document.
// Looks for the next node tag.
// -----------------------------------------------------------------------------
ChspsDomNode* hspsServerUtil::FindNodeByTagL( 
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
    return targetNode;
    } 
// -----------------------------------------------------------------------------
// hspsServerUtil::hspsServerUtil
// -----------------------------------------------------------------------------
//
hspsServerUtil::hspsServerUtil() 
    {
    // Empty. Should never be called.
    }            
    
// end of file
