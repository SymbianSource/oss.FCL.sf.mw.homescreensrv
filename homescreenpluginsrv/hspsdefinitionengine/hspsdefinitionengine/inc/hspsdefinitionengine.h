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
* Description:  Main class for this component
*
*/



#ifndef C_hspsDEFINITIONENGINE_H
#define C_hspsDEFINITIONENGINE_H

//  INCLUDES
#include <e32base.h>
#include "hspsdefinitionengineinterface.h"


// CONSTANTS
const TInt KErrXmlParsing       = -32270;
const TInt KErrRefBuilding      = -32272;

// FORWARD DECLARATIONS
class RFs;
class ChspsXMLHandler;
class ChspsODT;

/**
*  Main class for this component
*  @ingroup group_hspsdefinitionengine
*  @lib hspsdefinitionengine.lib
*  @since Series 60 5.0
*/
class ChspsDefinitionEngine : public ChspsDefinitionEngineInterface
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static ChspsDefinitionEngine* NewL();
        
        /**
        * Destructor.
        */
        ~ChspsDefinitionEngine();

    public: // New functions
        
        /**
        * Parses the given XML file. Inserts all nodes to DOM tree,
        * assignes all properties to the nodes
        * @since Series 60 3.1
        * @param aFsSession: file server session
        * @param aXMLFileName: XML file to parse
        * @param ahspsOdt: ODT structure, which contains the DomDocument
        * @return: Error code 
        */
        virtual ChspsDefinitionEngineInterface::TError CreateDOM( 
            RFs& aFsSession,
            const TDesC& aXMLFileName,
            ChspsODT& ahspsOdt );
                                    
        /**
        * Parses the given DTD file. Replaces all entity referencies 
        * in the skeleton DOM tree. Returns Localized DOM if it was successfull 
        * @since Series 60 3.1
        * @param aFsSession: file server session
        * @param aDTDFileName: Input DTD file for a certain language
        * @param ahspsOdt: Input/Output ODT structure, which contains the DomDocument.
        */
        virtual void ApplyLanguageL(
            RFs& aFsSession,
            const TDesC& aDTDFileName,
            ChspsODT& ahspsOdt );
                                    
    private:

        /**
        * C++ default constructor.
        */
        ChspsDefinitionEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
         
        // Owned
        ChspsXMLHandler*      ihspsXMLHandler;
        
    };

#endif      // C_hspsDEFINITIONENGINE_H   
            
// End of File
