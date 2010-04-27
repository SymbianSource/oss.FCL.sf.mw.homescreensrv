/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cps command buffer interface
*
*/


#ifndef M_CAICPSCOMMANDBUFFER_H
#define M_CAICPSCOMMANDBUFFER_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class MLiwInterface;
class CLiwDefaultMap;
class CLiwServiceHandler;

/**
 * AI Cps command buffer interface
 *
 * @lib aifw  
 * @since S60 v5.2
 */
class MAiCpsCommandBuffer
    {
public:
    // new functions
    
	/**
    * Adds command to command buffer queue
    *
    * @since S60 5.2
    * @param aPluginId plugin id.
    * @param aType type of the cps registry.
    * @param aFilter filter values.
    * @param aAction action trigger.
    */
    virtual void AddCommand( const TDesC& aPluginId,
            const TDesC& aType, CLiwDefaultMap* aFilter,
            const TDesC8& aAction ) = 0;
            
    /**
     * Gets Service Handler 
     *
     * @since S60 5.2
     * @return Service handler
     */                 
    virtual CLiwServiceHandler* ServiceHandler() const = 0;            

    /**
     * Gets IContentPublishing Interface
     *
     * @since S60 5.2
     * @return Interface
     */                     
    virtual MLiwInterface* CpsInterface() const = 0;
       
protected:
    // destructor

    ~MAiCpsCommandBuffer() { }
    };

#endif // M_CAICPSCOMMANDBUFFER_H

// End of file

