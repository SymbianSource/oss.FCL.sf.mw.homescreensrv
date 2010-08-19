/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CPSPROPERTYOBSERVER_H
#define C_CPSPROPERTYOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "aipspropertyobserver.h"

/**
 *  Publish and subscribe key observer
 * 
 *	@lib aiutils.lib
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS(CPSPropertyObserver) : 
    public CActive, public MAiPSPropertyObserver
    {
    
public:

	/**
     * Two-phased constructor.
     */
    static CPSPropertyObserver* NewL (TCallBack aCallBack, TUid aCategory, TInt aKey);
	
	/**
     * Destructor.
     */
    ~CPSPropertyObserver();

	/**
     * Release the observer
     */
	void Release();
	
// from base class CActive

    void RunL();
	
    void DoCancel();
    
// from base class MAiPSPropertyObserver    

    TInt Get( TInt& aValue );
    
    TInt Get( TDes8& aString );

    TInt Get( TDes16& aString );

private:

	/**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
	
	/**
     * C++ default constructor.
     */
    CPSPropertyObserver( TCallBack aCallBack,
                         TUid aCategory,
                         TInt aKey );
    
private:

    /**
     * Callback member
     */
    TCallBack   iCallBack;

    /**
     * PS category id
     */
    TUid        iCategory;

    /**
     * PS key
     */
    TInt        iKey;

    /**
     * Property member
     */
    RProperty   iProperty;
    
    };

#endif // C_CPSPROPERTYOBSERVER_H

// End of File.
