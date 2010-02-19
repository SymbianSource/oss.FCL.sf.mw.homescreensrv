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
* Description: HS Publisher info
*
*/


#ifndef _HSPUBLISHERINFO_H
#define _HSPUBLISHERINFO_H

// System includes
#include <e32std.h>

// User includes

// Forward declarations

// Constants

/**
 * Maximum length for Content publisher name.
 * 
 * @since S60 5.2
 */
const TInt KHsPublisherNameMaxLength( 128 );

/**
 * Maximum length for Content publisher namespace.
 * 
 * @since S60 5.2
 */
const TInt KHsPublisherNamespaceMaxLength( 32 );

// Types

/**
 * Content publisher name buffer.
 * 
 * @since S60 5.2 
 */
typedef TBuf< KHsPublisherNameMaxLength > THsPublisherName;

/**
 * Content publisher namespace buffer.
 *
 * @since S60 5.2 
 */
typedef TBuf8< KHsPublisherNamespaceMaxLength > THsPublisherNamespace;

/**
 * Content publisher information
 *
 * @since S60 5.2
 */

/**
 * Holds content publisher plug-in information.
 * 
 * @since S60 5.2
 */
class THsPublisherInfo
    {
public:
    // Constructors
    
    /**
     * C++ default contrutor
     */    
    inline THsPublisherInfo(); 

    /**
     * C++ contrutor
     */        
    inline THsPublisherInfo( const TUid& aUid, 
        const TDesC& aName, const TDesC8& aNamespace );
    
public:
    // new functions
    
    /**
     * Assigment operator
     * 
     * @since S60 5.2
     * @param aInfo Publisher info to assign.
     */
    inline THsPublisherInfo& operator= ( const THsPublisherInfo& aInfo );
    
    /**
     * Equals operator
     * 
     * @since S60 5.2
     * @param aInfo Publisher info  to compare
     * @return ETrue if this and aInfo equals, EFalse otherwise.
     */
    inline TBool operator== ( const THsPublisherInfo& aInfo ) const;
            
    /**
     * Gets Uid
     * 
     * @since S60 5.2
     * @return Uid 
     */
    inline TUid Uid() const;

    /**
     * Gets Name
     * 
     * @since S60 5.2
     * @return Name 
     */    
    inline const TDesC& Name() const;

    /**
     * Gets Namespace
     * 
     * @since S60 5.2
     * @return Namespace
     */        
    inline const TDesC8& Namespace() const;
        
private:
    // data

    /** Publisher implementation UID */
    TUid iUid;
    /** Publisher name */
    THsPublisherName iName;
    /** Publisher namespace */
    THsPublisherNamespace iNamespace;
    
private:
    // friend classes
    
    friend class CAiPluginFactory;

#ifdef _AIFW_UNIT_TEST
    friend class UT_HsPublisherInfo;
#endif
    };

#include <hspublisherinfo.inl>

#endif  // _HSPUBLISHERINFO_H

// End of file
