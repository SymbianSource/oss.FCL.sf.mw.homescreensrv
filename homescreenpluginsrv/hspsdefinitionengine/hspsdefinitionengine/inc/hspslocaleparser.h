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
* Description:  Locale parsing and applying languages
*
*/



#ifndef C_hspsLOCALEPARSER_H
#define C_hspsLOCALEPARSER_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class RFs;
class ChspsDomDocument;
class ChspsDtdEntity;
// CLASS DECLARATION

/**
*  Locale parsing and applying languages
*  @ingroup group_hspsdefinitionengine
*  @lib hspsdefinitionengine.lib
*  @since Series 60 3.1
*/
class ChspsLocaleParser : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static ChspsLocaleParser* NewL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsLocaleParser();

    public: // New functions
        
        /**
        * Applies locale references to a skeleton dom
        * @since Series 60 3.1
        * @param aFs: An open file server session
        * @param aFile: Dtd file to be parsed
        * @param aLocalizedDom: On return Dom containing replaced entity references
        */
        void ApplyDtdToDomL( 
            RFs& aFsSession, 
            const TDesC& aFile,
            ChspsDomDocument& aLocalizedDom );
        
        
    private:

        /**
        * C++ default constructor.
        */
        ChspsLocaleParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        //Array holds parsed entity references
        RPointerArray<ChspsDtdEntity> iEntityReferences;    
    };

#endif      // C_hspsLOCALEPARSER_H   
            
// End of File
