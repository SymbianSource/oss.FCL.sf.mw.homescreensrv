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
* Description:  Publish and subscribe parameter observer implementation.
*
*/


#ifndef MAIPSPROPERTYOBSERVER_H
#define MAIPSPROPERTYOBSERVER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  It is the interface for handling property change events.
*
*  @since S60 3.2
*/
class MAiPSPropertyObserver
    {
    public:
	    
        /**
	 	 * Get the current value of the key.
	 	 * 
	 	 * @since S60 3.2
	 	 * @param aValue refrerence to an integer value of key
	 	 * @return integer value of the error.
	 	 */
        virtual TInt Get( TInt& aValue) = 0;
        
        /**
	 	 * Get the current value of the key.
	 	 * 
	 	 * @since S60 3.2
	     * @param aString reference to an 8-bit descriptor of the key
	 	 * @return integer value of the error.
	 	 */
        virtual TInt Get( TDes8& aString ) = 0;
        
        /**
	 	 * Get the current value of the key.
	 	 * 
	 	 * @since S60 3.2
	 	 * @param aString reference to an 16-bit descriptor of the key
	 	 * @return integer value of the error.
	 	 */
        virtual TInt Get( TDes16& aString ) = 0;
        
        /**
	     * Release the observer
	     */
        virtual void Release() = 0;
        
    protected:
        ~MAiPSPropertyObserver() { }
    };

/**
 * Helper for calling MAiPSPropertyObserver::Release() with NULL checking.
 */
inline void Release(MAiPSPropertyObserver* obj)
    {
    if (obj)
        {
        obj->Release();
        }
    }

#endif      // MAIPSPROPERTYOBSERVER_H

// End of File

