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
* Description:  Represents the entire xmluiml specific xml and css data. 
*
*/



#ifndef hsps_DOM_DOCUMENT_H
#define hsps_DOM_DOCUMENT_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>


//FORWARD DECLARATIONS
class ChspsDomStringPool;
class ChspsDomNode;
// CLASS DECLARATION

/**
 * Provides the primary access to the document's data.
 *  
 * @lib hspsdomdocument.lib
 * @since S60 5.0
 * @ingroup group_hspsdom
*/
class ChspsDomDocument: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsDomDocument* NewL();
        
        /**
        * Two-phased stream constructor.
        * 
        * @since S60 5.0
        * @param aStream Source stream.
        */
        IMPORT_C static ChspsDomDocument* NewL( RReadStream& aStream );
        
        /**
        * Construct ChspsDomDocument from streamed HBufC8.
        * 
        * @since S60 5.0
        * @param aBufStream Source buffer stream.
        */
        IMPORT_C static ChspsDomDocument* NewL( const HBufC8* aBufStream );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~ChspsDomDocument();

    public: // New functions
        /**
        * Makes a clone of this document.
        * 
        * @since S60 5.0
        * @return Pointer to a document. Caller has the ownership.
        */
        IMPORT_C ChspsDomDocument* CloneL();
        
        /**
        * Factory method to create node elements, because elements cannot exist outside 
        * the context of a document.
        * 
        * @since S60 5.0
        * @param aName UTF-8 encoded Element name.
        * @param aNamespace UTF-8 encoded Element namespace.
        * @return A new ChspsDomNode type instance. Ownership is transferred to caller. 
        */
        IMPORT_C ChspsDomNode* CreateElementNSL( const TDesC8& aName, const TDesC8& aNamespace );
        
        /**
        * Set the root of the document tree. Root node provides primary access to 
        * document data. This class owns root node.
        * 
        * @since S60 5.0
        * @param aRootNode Documents's root node. Document takes ownership.
        */
        IMPORT_C void SetRootNode( ChspsDomNode* aRootNode );

        /**
        * Get the root node of the document.
        * 
        * @since S60 5.0
        * @return Pointer to ChspsDomNode type node.
        */
        IMPORT_C ChspsDomNode* RootNode() const;

        /**
        * Get the last node of the document.
        * 
        * @since S60 5.0
        * @return Pointer to ChspsDomNode type node.
        */
        IMPORT_C ChspsDomNode* LastNode() const;

        /**
        * Get the size of the document's data. 
        * 
        * @since S60 5.0
        * @return Document's data size.
        */
        IMPORT_C TInt Size() const;
        
        /**
        * Get document node count.
        * 
        * @since S60 5.0
        * @return Count of all nodes in a document.
        */
        IMPORT_C TInt DomNodeCount() const;
        
        /**
        * Externalize document's data.
        * 
        * @since S60 5.0
        * @param aStream Output stream.
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalize document's data.
        * 
        * @since S60 5.0
        * @param aStream Input stream.
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );

        /**
        * Returns document's data as a streamed buffer.
        * 
        * @since S60 5.0
        * @return Pointer to a new HBufC8 buffer which holds streamed document. 
        * Ownership is transferred to a caller.
        */
        IMPORT_C HBufC8* MarshallL();
        
        /**
        * Get reference to string pool.
        * 
        * @since S60 5.0
        * @return Reference to string pool implementation.
        */
        IMPORT_C ChspsDomStringPool& StringPool() const;
    private:

        /**
        * C++ default constructor.
        */
        ChspsDomDocument();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        //Document owns the root node.
        ChspsDomNode*         iRootNode;
        
        //Namespace map stores namespaces used in dom
        ChspsDomStringPool*   iDomStringPool;
        
        
    };

#endif      // hsps_DOM_DOCUMENT_H  
            
// End of File
