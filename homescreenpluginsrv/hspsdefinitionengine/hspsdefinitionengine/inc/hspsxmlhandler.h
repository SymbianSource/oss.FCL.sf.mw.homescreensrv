/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main class for XML parsing
*
*/



#ifndef C_hspsXMLHANDLER_H
#define C_hspsXMLHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <contenthandler.h>
#include <parser.h>

// FORWARD DECLARATIONS
class RFs;
class ChspsDomDocument;
class ChspsDomNode;

using namespace Xml; 
/**
*  Main class for XML parsing
*   @ingroup group_hspsdefinitionengine
*  @lib hspsdefinitionengine.lib
*  @since Series 60 3.1
*/
class ChspsXMLHandler : public CBase, public MContentHandler
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static ChspsXMLHandler* NewL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsXMLHandler();

    public: // New functions
        
        /**
        * Parses the given XML file, creates nodes and attributes and inserting
        * them to aDomDocument
        * @since Series 60 3.1
        * @param aFsSession: session to use for accessing the files
        * @param aFileName: name of XML file
        * @param aDomDocument: reference to an empty DOM
        */
        void ParseXmlDocL( RFs& aFsSession, const TDesC& aFileName,ChspsDomDocument& aDomDocument);

    public: // Functions from MXMLContentHandler

        /**
        * This method is a callback to indicate the start of the document.
        * @since Series 60 3.1
        * @param	aDocParam Specifies the various parameters of the document.
        *     	aDocParam.iCharacterSetName The character encoding of the document.
        * @param	aErrorCode is the error code. 
        * 		If this is not KErrNone then special action may be required.
        */
        void OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode);


        /**
        * This method is a callback to indicate the end of the document.
        * @since Series 60 3.1
        * @param  aErrorCode is the error code. 
        * 		If this is not KErrNone then special action may be required.
        */
        void OnEndDocumentL(TInt aErrorCode);


        /**
        * This method is a callback to indicate an element has been parsed.
        * @since Series 60 3.1
        * @param  aElement is a handle to the element's details.
        * @param	aAttributes contains the attributes for the element.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, TInt aErrorCode);

	
        /**
        * This method is a callback to indicate the end of the element has been reached.
        * @since Series 60 3.1
        * @param	aElement is a handle to the element's details.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnEndElementL(const RTagInfo& aElement, TInt aErrorCode);


        /**
        * This method is a callback that sends the content of the element.
        * Not all the content may be returned in one go. The data may be sent in chunks.
        * When an OnEndElementL is received this means there is no more content to be sent.
        * @since Series 60 3.1
        * @param  aBytes is the raw content data for the element. 
        * 		The client is responsible for converting the data to the 
        * 		required character set if necessary.
        * 		In some instances the content may be binary and must not be converted.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnContentL(const TDesC8& aBytes, TInt aErrorCode);

	
        /**
        * This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
        * This method is always called before the corresponding OnStartElementL method.
        * @since Series 60 3.1
        * @param  aPrefix is the Namespace prefix being declared.
        * @param	aUri is the Namespace URI the prefix is mapped to.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);


        /**
        * This method is a notification of the end of the scope of a prefix-URI mapping.
        * This method is called after the corresponding DoEndElementL method.
        * @since Series 60 3.1
        * @param  aPrefix is the Namespace prefix that was mapped.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);


        /**
        * This method is a notification of ignorable whitespace in element content.
        * @since Series 60 3.1
        * @param  aBytes are the ignored bytes from the document being parsed.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);


        /**
        * This method is a notification of a skipped entity. If the parser encounters an 
        * external entity it does not need to expand it - it can return the entity as aName 
        * for the client to deal with.
        * @since Series 60 3.1
        * @param  aName is the name of the skipped entity.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnSkippedEntityL(const RString& aName, TInt aErrorCode);


        /**
        * This method is a receive notification of a processing instruction.
        * @since Series 60 3.1
        * @param  aTarget is the processing instruction target.
        * @param	aData is the processing instruction data. If empty none was supplied.
        * @param	aErrorCode is the error code.
        * 		If this is not KErrNone then special action may be required.
        */
	    void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData,TInt aErrorCode);


        /**
        * This method indicates an error has occurred.
        * @since Series 60 3.1
        * @param  aError is the error code
        */
	    void OnError(TInt aErrorCode);


        /**
        * This method obtains the interface matching the specified uid.
        * @since Series 60 3.1
        * @param	aUid the uid identifying the required interface.
        * @return 0 if no interface matching the uid is found.
        * 		Otherwise, the this pointer cast to that interface.
        */
	    TAny* GetExtendedInterface(const TInt32 aUid);

    private:

        /**
        * C++ default constructor.
        */
        ChspsXMLHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
         
        // Pointer to Symbian parser, owned
        Xml::CParser*               iXmlParser;
 
        // Pointer to DOM, not owned
        ChspsDomDocument*             iDomDocument;

        // Pointer to the parent node
        ChspsDomNode*                 iParentNode;
        
        // Counter and ID at the same time
        TInt                        iCurrentNodeId;
 
        // Node stack with all the unfinished nodes
        RPointerArray<ChspsDomNode>*  iNodeStack;
        
        // Variable to store value got from OnError()
        TInt                        iError;
        
    };

#endif      // C_hspsXMLHANDLER_H
            
// End of File
