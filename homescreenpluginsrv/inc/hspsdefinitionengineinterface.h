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
* Description:  DefinitionEngine class interface 
*
*/


#ifndef C_hspsDEFINITIONENGINEINTERFACE_H
#define C_hspsDEFINITIONENGINEINTERFACE_H

#include <e32base.h>

// CLASS FORWARD
class RFs;
class ChspsODT;


// CLASS DECLARATION
/**
 *  Definition Enginen plugin interface class.
 *
 * @lib hspsdefinitionengineinterface.lib
 * @since S60 5.0
 * @ingroup group_hspsdefinitionengine
 */
class ChspsDefinitionEngineInterface : public CBase
    {
    public:
        struct TError
            {
            TInt iDefEngError;
            TInt iSubComponentError;
            };

    public:
	
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        * @param aPluginId  Name of implementation.
        */
     	IMPORT_C static ChspsDefinitionEngineInterface* NewL(const TDesC8& aPluginId );
        
        /**
        * Destructor
        */
        IMPORT_C virtual ~ChspsDefinitionEngineInterface();


    public: // New functions
        /**
        * Parses the given XML file. Inserts all nodes to DOM tree,
        * parses the CSS file and assignes all properties to the nodes
        * @since Series 60 3.1
        * @param aFsSession: file server session
        * @param aXMLFileName: XML file to parse
        * @param ahspsOdt: ODT structure, which contains the DomDocument
        * @return: Error code 
        */
        virtual TError CreateDOM( 
            RFs& aFsSession,
            const TDesC& aXMLFileName,
            ChspsODT& ahspsOdt )=0;

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
            ChspsODT& ahspsOdt )=0;

	private:
		TUid iDtorKey;
    
    };

#endif      
            
// End of File
