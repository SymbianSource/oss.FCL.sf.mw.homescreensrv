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
*/

#ifndef SUITEOBJECT_H
#define SUITEOBJECT_H

#include <e32base.h>
#include "mcssuiteobjectattr.h"

/**
* Menu Engine Object attribute.
*/
NONSHARABLE_CLASS( CSuiteObject ): public CBase
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CSuiteObject();

    /**
    * Two-phased constructor.
    * @param aFileName File Name.
    * @return The created object.
    */
    static CSuiteObject* NewL( const TDesC& aFileName );

    /**
    * Two-phased constructor.
    * @param aFileName File Name.
    * @return The created object.
    */
    static CSuiteObject* NewLC( const TDesC& aFileName );

protected:  // construction

	/**
	 * Constructor
	 */
	CSuiteObject();
	
	/**
	 * 2nd phase constructor.
	 * @param aFileName File Name.
	 */
	void ConstructL( const TDesC& aFileName );

public:     // new methods

    /**
    * Get file name.
    * @return file name.
    */
    TPtrC FileName() const { return iFileName; }
    
    /**
    * Get suite name.
    * @return suite name.
    */
    TPtrC SuiteName() const { return iName; }

    /**
    * Set suite name.
    * @param aName Name.
    */
    void SetSuiteNameL( const TDesC& aName );

    /**
    * Set attribute.
    * @param aAttrName atrribute name.
    * @param aAttrValue atrribute value.
    * @param aLocalized localisation of attribute.
    */
    void SetInitialAttributeL( const TDesC& aAttrName, 
    		const TDesC& aAttrValue,
    		TBool aLocalized );

    void GetAttribute( const TDesC& aAttrName, 
    		TBool& aAttrExists, TDes& aAttrVal );
    
    /**
    * Reset, destroy and close iAttributes array.
    */
    void CloseSuiteArray();
    
    
private:    // data
    
    RBuf iFileName; ///<File name. Own.
    RBuf iName; ///< Name. Own.
    RSuiteAttrArray iAttributes; ///< Attributes. Own.

    };


#endif // SUITEOBJECT_H
