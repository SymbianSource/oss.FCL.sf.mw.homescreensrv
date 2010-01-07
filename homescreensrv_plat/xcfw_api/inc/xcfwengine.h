/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class definitions for XCFW Engine
*
*/



#ifndef XCFWENGINE_H
#define XCFWENGINE_H

//  INCLUDES
#include <s32std.h>
#include <GMXMLParser.h>
#include <gmxmlcomposer.h>
#include "gecodefaultobjectfactory.h"

// FORWARD DECLARATIONS
class CXCFWEntityConverter;
class CMDXMLNode;
class MXCFWTree;
class MXCFWNode;
class CXCFWLocalizer;

// DATA TYPES
typedef RPointerArray<CGECOObjectFactoryBase> RFactoryArray;

// CONSTANTS
// XCFW error range: -32350, -32379
const TInt KErrDTDLoadFailed = -32350;

// CLASS DECLARATION
/**
* Engine observer interface. Engine user must implement this interface
*
* @lib XCFW.lib
* @since Series 60 3.1
*/
class MXCFWEngineObserver
    {
    public: //event enumeration
    
    enum TXCFWEngineEvent //Engine events
        {
        EEvtNull = 0,           //for module testing purposes only
        EEvtParsingStarted,     //triggered when DOM to Tree parsing starts
        EEvtParsingComplete,    //triggered when whole DOM has been processed
        EEvtSavingStarted,      //triggered when Tree to DOM composing starts
        EEvtSavingComplete,     //triggered when composing is done
        EEvtParsingCanceled,    //triggered when parsing is canceled by client
        EEvtSavingCanceled,     //triggered when saving is canceled by client
        };

    public: //new functions
    /**
    * Called when Engine parsing / saving state changes
    * User can do desired actions on corresponding events.
    * @param aEvent Engine event
    */
    virtual void HandleEngineEventL( TXCFWEngineEvent aEvent ) = 0;
    
    /**
    * Called when there's an error during parsing / saving
    * @param aErrorCode Error code from engine
    */
    virtual void HandleEngineErrorL( TInt aErrorCode ) = 0;        
    };



/**
*  XCFW Engine class
*  Responsible for internalizing / externalizing XML files
*  to and from a XCFW tree 
*
*  @lib XCFW.lib
*  @since Series 60 3.1
*/
class CXCFWEngine: public CActive,
                   public MGECOAttributeProvider,
                   public MMDXMLParserObserver,
                   public MMDXMLComposerObserver
    {
    public:
    
    enum TXCFWEngineState // Engine states
        {
        EStateIdle = 0,             //when engine is not active
        EStateInitializingLoad,     //when initializing Load operation
        EStateLoadingFile,          //when GMXML parser is loading
        EStateInitializingSave,     //when initializing save op
        EStateSavingFile,           //when GMXML Composer is saving
        EStateParsing,              //when parsing DOM to content tree
        EStateConstructingDOM       //when constructing DOM from XCFWTree
        };


    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXCFWEngine* NewL( MXCFWEngineObserver* aObserver );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXCFWEngine();

    public: // New functions
        
        /**
        * Cancels currently ongoing load/save operation
        * @since Series 60 3.1
        */
        IMPORT_C void CancelOperation();

        /**
        * Loads given XML content definition to given tree.
        * Operation is asynchronous. Engine will notify client through
        * MXCFWEngineObserver interface when parsing completes.
        * XCFW Engine will look for localization DTD based on the
        *  doctype declaration of loaded XML file. It assumes that
        *  the localization files are stored in language variant
        *  specific subdirectories under the directory where the
        *  loaded content file exists. 
        * @since Series 60 3.1
        * @param aTree Target tree to construct the content into
        * @param aFile File to load
        */
        IMPORT_C void LoadL( MXCFWTree& aTree, const TDesC& aFile );
        
        /**
        * Loads given XML content definition to given tree with
        * localized strings from given DTD file
        * Operation is asynchronous.
        * @since Series 60 3.1
        * @param aTree Target tree to construct the content into
        * @param aFile File to load
        * @param aDTDFile DTD to use for localization. This parameter
        *   overrides the possible DOCTYPE definition in the XML file.
        */
        IMPORT_C void LoadL( MXCFWTree& aTree, const TDesC& aFile, 
            const TDesC& aDTDFile );

        /**
        * Saves given Tree to given XML content definition file.
        * Operation is asynchronous.
        * @since Series 60 3.1
        * @param aTree Target tree to construct XML from
        * @param aFile File to save the XML definition to
        */
        IMPORT_C void SaveL( MXCFWTree& aTree, const TDesC& aFile );

        /**
        * Saves given Tree to given XML content definition file with
        * entity references from given DTD file.
        * Operation is asynchronous.
        * @since Series 60 3.1
        * @param aTree Target tree to construct XML from
        * @param aFile File to save the XML definition to
        * @param aDTDFile DTD to use for entity localization. This
        *   parameter overrides the possible DOCTYPE definition that was
        *   internalized to content tree when it was loaded from XML.
        */
        IMPORT_C void SaveL( MXCFWTree& aTree, const TDesC& aFile,
            const TDesC& aDTDFile );
        
        /**
        * Adds new object factory to engine factory array.
        * Engine does NOT take ownership of the factory.
        * @since Series 60 3.1
        * @param aFactory Object factory to add. Engine will
        *  query factories for objects when constructing tree.
        */
        IMPORT_C void RegisterObjectFactoryL( 
            CGECOObjectFactoryBase* aFactory );

        /**
        * Removes a factory from Engine's object factory array.
        * @since Series 60 3.1
        * @param aFactory Object factory to be removed
        */
        IMPORT_C TInt UnRegisterObjectFactory( 
            CGECOObjectFactoryBase* aFactory );

        /**
        * Returns engine's current state
        * Client may want to check state at error events
        * @since Series 60 3.1
        * @return Engine state
        */
        IMPORT_C TXCFWEngineState CurrentState();


    public: // Functions from base classes

        /**
        * From MGECOAttributeProvider. 
        * Returns attribute details for given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, 
            TPtrC& aAttributeName, 
            TPtrC& aAttributeValue, 
            TBool& aIsLocalized);

        /**
        * From MGECOAttributeProvider. 
        * Returns attribute details for given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, 
            TPtrC& aAttributeName, 
            TPtrC& aAttributeValue);

        /** 
        * From MGECOAttributeProvider. 
        * Returns number of attributes for a xml tag.
        */
        IMPORT_C TInt NumAttributes();

        /**
        * From MGECOAttributeProvider. Returns node / data object text
        * and possible localization status for the text.
        */
        IMPORT_C void TextDetailsL( TPtrC& aText, TBool& aIsLocalized );
        
        /**
        * From MGECOAttributeProvider. Returns ETrue if xml node has text data
        */
        IMPORT_C TBool HasTextData();
        
        /**
        * From MMDXMLParserObserver. Called when XML to DOM 
        * parsing is done
        */
        void ParseFileCompleteL();

        /**
        * From MMDXMLComposerObserver. Called when DOM to XML 
        * composing is done.
        */
        void ComposeFileCompleteL();

    protected:  // Functions from base classes

        //From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );
        
    private:

        /**
        * C++ default constructor.
        * @param aObserver Engine observer
        */
        CXCFWEngine( MXCFWEngineObserver* aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        
        /**
        * Traverses XML DOM and constructs XCFW tree
        * Each call to this function processes one DOM element
        * @since Series 60 3.1
        */
        void DOM2TreeNextCycleL();
        
        /**
        * Traverses content tree and constructs XML DOM
        * Each call to this function processes one XCFW Tree element
        * @since Series 60 3.1
        */
        void Tree2DOMNextCycleL();
        
        /**
        * Adds a XCFW Tree node to DOM
        * Called repeatedly during parsing from Tree2DOMNextCycle()
        * @since Series 60 3.1
        */
        void AddCurrentTreeNodeToDOML();
        
        /**
        * Adds a XML element to content tree
        * Called repeatedly during parsing from Dom2TreeNextCycle()
        * @since Series 60 3.1
        */
        void AddCurrentXMLNodeToTreeL();

        /**
        * Frees resources used during parse / compose operation
        * @since Series 60 3.1
        */
        void FreeResources();

        /**
        * Loads DTD for the requested save / load op
        * and sets engine active. 
        * @since Series 60 3.1
        */
        void PrepareEntityConverterAndSetActiveL();
        
        /**
        * Prepares DTD file path for localization file loading
        * This function checks the DTD path from XML doctype declaration
        */
        void PrepareDTDPathL();
        

    private:    // Data
        // Engine observer. Not owned
        MXCFWEngineObserver*        iObserver;

        // Object factory array. Factory pointers are not owned.
        RFactoryArray               iFactoryList;

        // Default factory (used if there's no object factory from client)
        CGECODefaultObjectFactory*  iDefaultFactory;
        
        // file system handle for load / save ops
        RFs                         iFileSystem;
        
        // XML file name buffer
        HBufC*                      iFile;
        
        // DTD file name buffer
        HBufC*                      iDTD;
        
        // Current node's text (all text with cdatasections included)
        HBufC*                      iNodeText;
        
        // XML Parser
        CMDXMLParser*               iParser;
        
        // XML Composer
        CMDXMLComposer*             iComposer;
        
        //XML document to operate with.
        CMDXMLDocument*             iXMLDoc;

        //Current XML node
        CMDXMLNode*                 iCurrentXMLNode;
        
        // Localization utility class
        CXCFWLocalizer*             iLocalizer;

        //Tree to operate with. Not owned.
        MXCFWTree*                  iTree;

        //Current parent node used during parsing. Not owned.
        MXCFWNode*                  iCurrentTreeNode;
        
        // engine internal state
        TXCFWEngineState            iState;
        
        // engine internal state by last error
        TXCFWEngineState            iStateByLastError;
    
        // Entity converter
        CXCFWEntityConverter*       iConverter;

    };

#endif      // XCFWENGINE_H   
            
// End of File
