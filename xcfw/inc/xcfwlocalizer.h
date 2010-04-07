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
* Description:  Class definition of XCFW DTD handler responsible for 
*                entity conversions (localization)
*
*/



#ifndef CXCFWLOCALIZER_H
#define CXCFWLOCALIZER_H

//  INCLUDES
#include <s32std.h>
#include <badesca.h>
//#include <gmxmlentityconverter.h>
//#include <gmxmlcomposer.h>

// FORWARD DECLARATIONS
class RFileReadStream;
class RFs;

/**
* Localization handling class. Responsible for holding entity references 
* and their corresponding text values loaded from a DTD file. Used for content 
* localization purposes in XCFW.
*
*  @lib XCFW_hs_hs.lib
*  @since Series 60 3.1
*/
class CXCFWLocalizer :  public CBase //CMDXMLEntityConverter
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aData Node data object
        */
        static CXCFWLocalizer* NewL();

        /**
        * Destructor.
        */
        virtual ~CXCFWLocalizer();

    public: // New functions
        
        /**
        * Loads Entity references from DTD file
        * @since Series 60 3.1
        * @param aDTD File to load
        * @param aFs File system handle to use
        * @param aStatus Engine's request status, which Localizer will
        *   complete when DTD parsing is ready
        */
        void LoadDTDL(const TDesC& aDTD, 
            RFs& aFileSystem, 
            TRequestStatus* aStatus );
        
        /**
        * Converts a XML entity reference to localized text
        * @since Series 60 3.1
        * @param aRef Entity reference to convert
        * @param aText Localized text returned
        * @return Error code, e.g. KErrNotFound if given entity reference
        *  is not found.
        */
        TInt EntityRefToText(TDesC& aRef, TPtrC& aText);
        
        /** 
        * Converts given localized text into an entity reference
        * @s
        ince Series 60 3.1
        * @param aText text to get entity reference for
        * @param aRef Entity reference returned
        * @return Error code, e.g. KErrNotFound if no entity reference
        *  is found for the given text.
        */
        TInt TextToEntityRef(TDesC& aText, TPtrC& aRef);
        
        /**
        * Returns error code from the last LoadDTDL operation
        * Engine uses this to check that operation was successful when
        * Localizer completes the pending request.
        * @since Series 60 3.1
        * @return Error code from latest DTD load operation
        */
        TInt LastError();

    private:

        /**
        * C++ default constructor.
        */
        CXCFWLocalizer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * DTD file parsing facility
        * @param aStream file stream to parse
        * @since Series 60 3.1
        */
        void ParseDTDL( RFileReadStream& aStream );

    private:    // Data
        
        //Descriptor arrays for entity references and texts
        CDesCArraySeg*  iReferences; //Own: Entity references without & and ;
        CDesCArraySeg*  iLocStrings; //Own: Localized texts
        TRequestStatus* iStatus; //Not owned: 
                
    };

#endif      // CXCFWLOCALIZER_H   
            
// End of File
