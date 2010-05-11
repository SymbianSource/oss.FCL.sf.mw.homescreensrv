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
* Description:  AiFw internal publisher info
*
*/


#ifndef _AIFWPUBLISHERINFO_H
#define _AIFWPUBLISHERINFO_H

// System includes
#include <e32std.h>

// User includes
#include <hspublisherinfo.h>
#include <aifwdefs.h>

// Type definitions

// Class declaration
/**
 * Callback
 *
 * @since S60 5.2
 */              
class TAiFwCallback
	{
public:
    // constructors

    /**
     * C++ default contrutor
     */        
	inline TAiFwCallback();
	
    /**
     * C++ contrutor
     */    	
	inline TAiFwCallback( void (*aFunction)( TAny* aPtr, TInt aResult ), TAny* aPtr );
        
public:
    // new functions        
    
    /**
     * Assigment operator
     * 
     * @since S60 5.2
     * @param aCb Callback to assign.
     */    
	inline TAiFwCallback& operator= ( const TAiFwCallback& aCb );
    
    /**
     * Callback 
     * 
     * @since S60 5.2
     * @param aResult Result which is pased to callback
     */        
	inline void Callback( TInt aResult ) const;
	    
private:
	// data
	
	/** Callback function */
	void ( *iFunction )( TAny* aPtr, TInt aResult );		
	/** A pointer to pass in callback function */
	TAny* iPtr;
    };
    
class TAiFwPublisherInfo
    {
public:        
    // constructors
    
    /**
     * C++ default contrutor
     */            
    inline TAiFwPublisherInfo();
            
    /**
     * C++ contrutor
     */                    
    inline TAiFwPublisherInfo( const THsPublisherInfo& aInfo, 
        const TAiFwCallback& aCb, TInt aReason );
             
public:
    // new functions

    /**
     * Assigment operator
     * 
     * @since S60 5.2
     * @param aInfo Info to assign.
     */    
	inline TAiFwPublisherInfo& operator= ( const TAiFwPublisherInfo& aInfo );

    /**
     * Equals operator
     *
     * @since S60 5.2
     * @param aInfo Info to use in comparision
     * @return Publisher info
     */
    inline TBool operator==( const THsPublisherInfo& aInfo ) const;
    
    /**
     * Gets publisher info
     *
     * @since S60 5.2
     * @return Publisher info
     */            
    inline const THsPublisherInfo& Info() const;

    /**
     * Gets reason
     *
     * @since S60 5.2
     * @return Reason
     */                    
    inline TInt Reason() const;
    
    /**
     * Calls callback functions
     *
     * @since S60 5.2
     * @param aResult Result which is passed to callback function
     */                         
    inline void Callback( TInt aResult ) const;
     
private:
    // data
    
    /** Publisher info */
    THsPublisherInfo iInfo;
    /** Callback */
    TAiFwCallback iCb;
    /** Reason */
    TInt iReason;
    };    

#include <aifwpublisherinfo.inl>
    
#endif // _AIFWPUBLISHERINFO_H

// End of file
