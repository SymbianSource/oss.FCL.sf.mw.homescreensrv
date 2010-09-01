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
* Description:  AiFw stub
*
*/


#ifndef _AIFW_STUB_H
#define _AIFW_STUB_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CRepository;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 * AiFw stub.
 */
NONSHARABLE_CLASS( CAiFw ) : public CBase      
    {
public: 
    // constructors and destructor    

    /**
     * Leaving constructor
     */    
    static CAiFw* NewL();
    
    /**
     * Leaving constructor
     */
    static CAiFw* NewLC();
  
    /**
     * Destructor
     */
    ~CAiFw();
          
private: 
    // constructors
    
    /**
     * Default C++ constructor
     */
    CAiFw();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
 
public:
    // new functions
    
    /**
     * Get repository
     * 
     * @since S60 5.2
     * @return Repositury
     */
    CRepository& Repository() const;
    
private: 
    // data     
    
    /** Idle repository, Owned. */
    CRepository* iRepository;    
    };

#endif // _AIFW_STUB_H

