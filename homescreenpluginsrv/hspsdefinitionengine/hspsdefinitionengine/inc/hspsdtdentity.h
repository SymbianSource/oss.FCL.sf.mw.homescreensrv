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
* Description:  Entity reference data class
*
*/



#ifndef C_hspsDTDENTITY_H
#define C_hspsDTDENTITY_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  Class represent dtd entity.
*   @ingroup group_hspsdefinitionengine
*  @lib hspsdefinitionengine.lib
*  @since Series 60 3.1
*/
class ChspsDtdEntity : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static ChspsDtdEntity* NewL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsDtdEntity();

    public: // New functions
        
        /**
        * Set entity reference.
        * @since Series 60 3.1
        * @param aEntity Entity reference name
        */
        void SetEntityL( const TDesC8& aEntity );
        
        /**
        * Get the name of the entity reference.
        * @since Series 60 3.1
        * @return Reference to entity.
        */
        const TDesC8& Entity();
        
        /**
        * Set entity's value.
        * @since Series 60 3.1
        * @param aValue Entity's value
        */
        void SetValueL( const TDesC8& aValue );
        
        /**
        * Get the value of the entity reference.
        * @since Series 60 3.1
        * @return Reference to value.
        */
        const TDesC8& Value();
    
    private:

        /**
        * C++ default constructor.
        */
        ChspsDtdEntity();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

       
    private:    // Data
        // Entity reference
        HBufC8*     iEntity;
        // Entity reference value
        HBufC8*     iValue;

    };

#endif      // C_hspsDTDENTITY_H
            
// End of File
