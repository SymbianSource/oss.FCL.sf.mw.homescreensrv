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
*  Utility for common code.
*
*
*/


#ifndef __hspsSERVERUTIL_H__
#define __hspsSERVERUTIL_H__

// Includes
#include <e32def.h> 
#include <e32base.h>

// Forward declarations.
class ChspsODT;
class ChspsDomNode;
class ChspsResource;

// Constants.
#define RECURSION_DEPTH_INFINITE (-1)

/**
*  Class hspsAttribUtil.
*
*  Static utility for maintainance and installation handlers.
*  @ingroup group_hspsserver
*  @lib hspsThemeServer.exe
*/
class hspsServerUtil
    {
    public: // New methods.
        /**
         * Generates attributes for the Configuration element from manifest data
         * @since S60 5.0
         * @param aOdt is an ODT instance to be updated
         */
        static void GenerateConfigurationAttributesL( ChspsODT& aOdt );
        
        /**
         * Generates attributes for the Object element from manifest data
         * @since S60 5.0
         * @param aOdt is an ODT instance to be updated
         */
        static void  GenerateObjectAttributesL( ChspsODT& aOdt );       
        
        /**
         * Retrieves locale independent path which used by HSPS clients to access resources
         * in their private application folders.
         * @since S60 5.0
         * @param aResourceLanguage is the language of the resource
         * @param aPath is the path to be unlocalized                 
         */
        static void GetLocaleIndependentResourcePath(
        		const TLanguage& aResourceLanguage,
        		TFileName& aPath );        
        
        /**
         * Retrieves relative path of a resource under the themes folder.         
         * @since S60 5.0
         * @param aSourceFile is the input file 
         * @param aRelativePath is the relative path                 
         */
        static void GetRelativeResourcePath(
                const TFileName& aSourceFile,
                TPath& aRelativePath );
        
        /**
         * Creates a new attribute for the provided plugin node. 
         * @since S60 5.0
         * @param aPluginsNode is the plugins node that should be modified
         * @param aAttrName is name of the attribute
         * @param aValue is value of the attribute
         * @param aFormat is requested value string format
         * @return KErrNone
         */
        static TInt AddAttributeNumericL(
            ChspsDomNode& aNode,
            const TDesC8& aAttrName, 
            const TInt aValue,
            const TRadix aFormat = EDecimal );

        /**
         * Creates a new attribute for the provided plugin node. 
         * @since S60 5.0
         * @param aPluginsNode is the plugins node that should be modified
         * @param aAttrName is name of the attribute
         * @param aValue is value of the attribute
         */
        static void AddAttributeDescL(
            ChspsDomNode& aNode,
            const TDesC8& aAttrName, 
            const TDesC8& aValue );        
  
        /**
         * Finds a configuration node with the provided id attribute
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class  
         * @param aConfigurationId is an unique identifier of the configuration node
         * @return plugins node or NULL if the node couldn't be found 
         */
        static ChspsDomNode* FindConfigurationNodeL(
    		    ChspsODT& aOdt,		
    		    const TInt aConfigurationId );
                
        /**
         * Finds a plugin node with the provided id
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class      
         * @param aPluginId is an ID of the plugin instance to be found
         * @return Pointer to the plugin node or NULL
         */
        static ChspsDomNode* FindPluginNodeL(
    		    ChspsODT& aOdt,		
    		    const TInt aPluginId );
        
        /**
         * Gets an attribute value of a configuration element.
         * @since S60 5.0
         * @param aOdt is an ODT instance form which the name is fetched
         * @param aAttr is the attribute to be found
         * @return value of the name attribute in a configuration node         
         */
        static TPtrC8 FindConfigurationAttrL( 
            const ChspsODT& aOdt,
            const TDesC8& aAttr );
        
        /**
         * Copies a file to a target path when neccessary.
         * Target path is created if it's missing.
         * File is copied if the target's size or date are different or 
         * if the file is missing.
         * @since S60 5.0 
         * @param aFs           Reference to OPEN file server session handle.
         * @param aFilemanager  Referemce to file manager instance
         * @param aTargetPath   Path where the file should be copied to
         * @param aFilename     Path and name of the file to be copied
         * @return error code
         */
        static TInt CopyResourceFileL(
                RFs& aFs,
                CFileMan& aFilemanager,
                const TPath& aTargetPath,
                const TFileName& aSourceFile );
        
        /**
         * Checks:
         * - Is there enough space for target file?
         * - Do source and target files have same timestamp and size
         *   (if target file exists)?
         * 
         *  @param  aSource     Full path to source file.
         *  @param  aTarget     Full path to target file.
         *  @param  aFs         Reference to OPEN file server session handle.
         *  @return TBool       ETrue if copy can continue, EFalse if copy not needed.
         */
        static TBool ResourceCopyIsRelevantL( 
        		const TDesC& aSource,
                const TDesC& aTarget,
                RFs& aFs );

        /**
         * Check configuration states
         * @since S60 5.0
         * @param aOdt        ODT instance to be checked
         * @param aConfState     Old state to be replaced
         * @param aNextConfState New state         
         * @param aOdtUpdated ODT update status
         */
        
        static void UpdateConfigurationStateL( 
            ChspsODT& aOdt,
            TDesC8& aConfState,
            TDesC8& aNextConfState,
            TBool& aOdtUpdated );

        /**
         * Check state of an application configuration
         * @since S60 5.0
         * @param aOdt        ODT instance to be checked
         * @param aConfState     Old state to be replaced
         * @param aNextConfState New state         
         */
        static void UpdateAppConfigurationStateL( 
            ChspsODT& aAppOdt,
            const TDesC8& aConfState,
            const TDesC8& aNextConfState );
        
        /**
         * Finds a defined node with defined attribute value
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class
         * @param aAttrName is name of the attribute to be found
         * @param aAttrValue is the value which the attribute should hold 
         */
        static ChspsDomNode* FindNodeByAttributeL( 
            ChspsODT& aOdt,
            const TDesC8& aNodeName,
            const TDesC8& aAttrName, 
            const TDesC8& aAttrValue );
        
        /**         
         * Retrieves all unique plugin uids from a configuration.
         * @since S60 5.0
         * @param aOdt holds the configuration from which the plugins are searched from
         * @param aPluginArray is an array of found plugin uids         
         */
        static void FindUniquePluginsL( 
                ChspsODT& aOdt, 
                RArray<TInt>& aPluginArray );
        

        /**
         * Converts a hex value string (0xXXXX) to TUint.
         * 
         * @param aStr  String containing hex value.
         * @param aTrg  Reference to target TUint. Will be modified only
         *              if conversion is successful.
         * @return      KErrNone if successfull. Otherwise related symbian error code.
         */
        static TInt HexString2Uint(
                    const TDesC8& aStr,
                    TUint& aTrg );

        /**
         * Converts a decimal value string to TInt
         * @param aStr String containing hex value
         * @return corresponding TInt
         */
        static TInt DecString2Int(
                      const TDesC8& aStr );
        
        /**
         * Returns a count of plugin instances.
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginUid is an UID of the plugin instances to be found
         * @param aInstanceCount is the return value         
         */
        static void PluginInstanceCountL(
                        const ChspsODT& aAppODT,        
                        const TInt aPluginUid,
                        TInt& aInstanceCount );
        /**
         * Converts UID in DesC8 format into TUid format.
         * @since S60 5.0
         * @param aStr is the descriptor value to be converted
         * @return the converted uid value
         */
        static TUid ConvertDescIntoUid(
                const TDesC8& aStr );
        /**
         * Finds a child node by tag 
         * @since S60 5.0
         * @param aNodeTag is the tag of the child node to be found
         * @param aParentNode is the node whose child is searched
         * @param aIndex is index that sets position where to start search
         * @return pointer to child node
         */
       
        static ChspsDomNode* FindChildNodeByTagL(
                const TDesC8& aNodeTag, 
                ChspsDomNode& aParentNode,
                TInt& aIndex );
        
        /**
         * Returns fixed name of ODT resources (e.g. clock.o0005 name converts into clock.o0000)
         * @since S60 5.0
         * @param aNameAndExtension is resource name to be fixed
         * @return fixed resource name  
         */
        static TFileName GetFixedOdtName( 
                const TDesC& aNameAndExtension );

        /**
         * Finds files from defined directory and its sub directories from
         * C: drive, Z: drive or both. If the directory path contains a drive 
         * files are searched from that drive only.          
         * @since S60 5.0
         * @param aDirName is directory path where the files are searched
         * @param aFileName is name of file to be searched
         * @param aFiles is a list of found files (full path)
         */
        static void FindFilesL( 
                const TDesC& aDirName,
                const TDesC& aFileName,
                RArray <TFileName>& aFiles );

        /**
         * Finds files from defined directory from C: drive, Z: drive or both. 
         * If the directory path contains a drive files are searched from that 
         * drive only.          
         * @since S60 5.0
         * @param aDirName is directory path where the files are searched
         * @param aFileName is name of file to be searched
         * @param aFiles is a list of found files (full path)
         */
        static void FindFilesFromDirL( 
                const TDesC& aDirName,
                const TDesC& aFileName,
                RArray <TFileName>& aFiles );

        /**
         * Set node attribute value. Updated node is defined with 
         * node name, attribute name and attribute value
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class
         * @param aNodeName is name of updated node
         * @param aAttrName is name of the attribute to be found
         * @param aAttrValue is the value which the attribute should hold
         * @param aSetAttrName is name of updated attribute
         * @param aSetAttrValue is new value of updated attribute 
         */
        static void SetAttributeValueL( 
            const ChspsODT& aOdt,
            const TDesC8& aNodeName,
            const TDesC8& aAttrName, 
            const TDesC8& aAttrValue,
            const TDesC8& aSetAttrName,
            const TDesC8& aSetAttrValue );

        /**
         * Get node attribute value. Node which attribute value is requested 
         * is defined with node name, attribute name and attribute value
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class
         * @param aNodeName is the name of node which attribute value is requested
         * @param aAttrName is the name of the attribute which should be contained by the node
         * @param aAttrValue is the requested value of the attribute
         * @param aGetAttrName is the name of attribute which value is requested
         * @param aGetAttrValue is the requested attribute value
         */
        static void GetAttributeValueL( 
            const ChspsODT& aOdt,
            const TDesC8& aNodeName,
            const TDesC8& aAttrName, 
            const TDesC8& aAttrValue,
            const TDesC8& aGetAttrName,
            TPtrC8& aGetAttrValue );

        /**
         * Checks resource files of defined configuration
         * Function leaves if all resource files cannot be found
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class
         * @param aConfUid is configuration UID which resources are checked
         */
        static void CheckResourceFilesL( 
            const ChspsODT& aOdt,
            const TInt aConfUid );

        /**
         * Returns configuration's version string
         * Function leaves if configuration cannot be found
         * @since S60 5.0
         * @param aOdt is an instance of the ODT class
         * @param aConfUid is configuration UID which version is requested
         * @param aVersion is configuration version string
         */
        static  void CheckConfigurationVersionL( 
            ChspsODT& aOdt,
            const TInt aConfUid,
            const TDesC& aVersion );

        /**
         * Edit mode for EditNodeActivity method.
         */
        enum TNodeEditMode
            {
            EActivateFirst = 0, // Activate first one on specific level only.
                                // Others will be deactivated.
            
            EDeactivateAll      // Deactivate all.            
            };                
        
        /**
         * Modify node's childs according to given edit mode.
         * Recursive method. Modifies only nodes that are type plugin.
         * 
         * @since S60 5.0
         * @param aRootNode     Root node to start tree recursion.
         * @param aEditMode     Edit mode. Look for TNodeEditMode.
         * @param aDepth        Optional edit depth. e.g. If 1 then
         *                      only one level of child nodes is processed.
         *                      If not defined (or RECURSION_DEPTH_INFINITE
         *                      used) then whole node tree from aRootNode
         *                      forward will be processed.
         */
        static void EditPluginNodeActivityL( ChspsDomNode* aRootNode,
                                             const TNodeEditMode aEditMode,
                                             TInt aDepth = RECURSION_DEPTH_INFINITE );         

        /**
         * Get active plugin node.
         * Not recursive method. Only one level from parent will be checked.
         * 
         * @since S60 5.0
         * @param aParentNode   Parent node.
         * @return ChspsDomNode* Pointer to active plugin node. NULL if none found.
         */
        static ChspsDomNode* GetActivePluginNode( ChspsDomNode* aParentNode );        

        /**
         * Get plugin id of a node.
         * 
         * @since S60 5.0
         * @param aNode     Node.
         * @return TInt     Plugin id.
         */
        static TInt GetPluginId( ChspsDomNode* aNode );          

        /**
         * Get plugin uid of a node.
         * 
         * @since S60 5.0
         * @param   aNode   Node.
         * @return  TUid    Plugin uid.       
         */
        static TUid GetPluginUid( ChspsDomNode* aNode );       
        
        /**
         * Gather plugin ids for specific plugin uid from given ODT.
         * @since S60 5.0
         * @param aAppODT       ODT to search plugin ids from.
         * @param aPluginUid    Plugin uid to be used as search keyword.
         * @param aPluginIds    Target array for plugin ids.
         */
        static void GetPluginIdsByUidL(
            const ChspsODT& aAppODT,
            const TUid aPluginUid,
            RArray<TInt>& aPluginIds );        
        
        /**
         * Finds a parent node with defined name and attribute 
         * for defined child node
         * @since S60 5.0
         * @param aNode        Child node
         * @param aNodeName    Name of parent node
         * @param aAttrName    Name of parent node attribute
         * @param aAttrValue   Value of parent node attribute
         * @return pointer to parent node or 
         *         NULL if defined parent node is not found
         */
        static ChspsDomNode* GetParentNode(
            const ChspsDomNode& aNode,
            const TDesC8& aNodeName,
            const TDesC8& aAttrName,
            const TDesC8& aAttrValue );        
        
        /**
         * Retrieves filename from a logo declaration.
         * @since S60 5.2
         * @param aFileDeclaration "SKIN():MIF()", "MIF()", "UID()" or "file.ext" string
         * @param aFilename Referred file name or NULL
 		 * @return True if a file was referred from the declaration
         */
        static TBool hspsServerUtil::IsLogoFile(
                const TDesC& aFileDeclaration,
                TFileName& aFilename );

        /**
         * Resolve resource files that need to be copied.
         * 
         * @since S60 5.1
         * 
         * @param aODT              Configuration.
         * @param aConfUid          Plugin which is to be processed.
         * @param aActiveLanguage   Active language.
         * @param aWidgetResources  Target array. Will receive list
         *                          of resources if any applicable.
         *                          Ownership of given resources is not transferred!
         */
        static void GetValidResourcesL(
                ChspsODT& aODT,        
                const TInt aConfUid,
                const TLanguage aActiveLanguage,
                RPointerArray<ChspsResource>& aWidgetResources );        

        /**
         * Check if there is enough space on given drive for resources included in ODT.
         * 
         * @since   S60 5.1
         * 
         * @param   aODT                    Configuration.
         * @param   aActiveLanguage         Active language.
         * @param   aFs                     Reference to open file server handle.
         * @param   aDriveNumber            Drive number. Default EDriveC.
         * @param   aAdditionalDiskSpace    Additional disk space added to disk (bytes)
         *                                  space requirement. Default 0.
         * 
         * @return  TInt                    System wide error code. KErrDiskFull
         *                                  if not enough space on C drive.  
         */
        static TInt EnoughDiskSpaceAvailableL(
                ChspsODT& aODT,        
                const TLanguage aActiveLanguage,
                RFs& aFs,
                const TDriveNumber aDriveNumber = EDriveC,
                const TInt aAdditionalDiskSpace = 0 );
				
		/**
         * Finds a node by tag 
         * @since S60 5.0
         * @param aNodeTag is the tag of the node to be found
         * @param aDomNode is the root node where to search specific node 
         * @return pointer to node that is found
         */		
	    static ChspsDomNode* FindNodeByTagL( 
                        const TDesC8& aNodeTag,
                        ChspsDomNode& aDomNode );	
        
	    /**	     
         * Eclipsing enabler for customization where the input is 
         * searched from all secure unremovable drives. 
         * Drives are searched in descending alphabetical order, 
         * from Y: to A:, and ending with the Z: drive. 
         * All drives which end-user can freely modify/crack 
         * are skipped from the search.         
         * @since S60 5.2
	     * @param aFs is a reference to open file server session handle
	     * @param aPath is path of the file
	     * @param aFilename is name and extension of the file
	     * @param aDrivePathName Full path with a drive letter to the 
         *                       resource file (output)
	     */
	    static TInt FindFile(
	             RFs& aFs,
	             const TDesC& aPath,
	             const TDesC& aFilename,         
	             TFileName& aDrivePathName );

      /**
       * Resolves icon path information from the provided logo declaration.
       * Example:
       *    Decl.   = "mif(536999050\270513751\268475903\1.0\sources\icon.mif)"
       *    Source  = "c\private\200159c0\themes\536999050\270513751\268475903\1.0\sources\icon.mif"
       *    Target  = "c\private\102750f0\536999050\270513751\268475903\1.0\sources\icon.mif"
       *    Up.decl = "mif(c\private\102750f0\536999050\270513751\268475903\1.0\sources\icon.mif)"	        
       * @since S60 5.2
       * @param aLogoDeclaration Skin():mif(), mif(), uid(), icon.mif declaration
       * @param aAppUid      Identifies the private directory where the logo file is copied to
       * @param aTargetPath  Empty or location of the target file
       * @param aSourcePath  Empty or location of the source file 
       * @param aUpdatedDeclaration Empty or declaration which points to the target location
       */
	    static void PopulateLogoPathsL(
	            const TDesC& aLogoDeclaration,
                const TUint aAppUid,
                RBuf& aTargetPath,
                RBuf& aSourcePath,
                RBuf& aUpdatedDeclaration );

	    /**
	     * Finds ecplised files from the provided path in given drive order.
	     * @since S60 5.2
	     * @param aFs is a reference to open file server session handle
	     * @param aDriveArray An array of drives in search order
	     * @param aPath Relative path to be found
	     * @param aFolders Search results or empty
	     * @param aRecursive True if files should be found from any sudirectories 
	     */	    	    
	    static void FindFilesRecursivelyL(
                RFs& aFs,
                const RArray<TInt>& aDriveArray, 
                const TDesC& aPath,        
                RPointerArray<HBufC>& aFolders,
                TBool aRecursive = EFalse );
	    
	private:
        /**
         * Internal method. Do not call directly!
         * 
         * Modify node's childs according to given edit mode.
         * Recursive method. Modifies only nodes that are type plugin.
         * 
         * @since S60 5.0
         * @param aRootNode     Root node to start tree recursion.
         * @param aEditMode     Edit mode. Look for TNodeEditMode.
         * @param aDepth        Edit depth. e.g. If 1 then
         *                      only one level of child nodes is processed.
         *                      If RECURSION_DEPTH_INFINITE used
         *                      then whole node tree from aRootNode
         *                      forward will be processed.
         */
        static void _EditPluginNodeActivityL( ChspsDomNode* aRootNode,
                                              const TNodeEditMode aEditMode,
                                              TInt& aDepth  );     
	    	    
        /**  	
        * Not for instanciation. Only static functionality provided.
        */
	    hspsServerUtil();
	};

#endif //__hspsSERVERUTIL_H__
