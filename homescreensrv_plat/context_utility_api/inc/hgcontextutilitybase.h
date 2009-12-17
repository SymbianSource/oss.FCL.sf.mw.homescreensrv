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
* Description:  Context publishing helper dll
 *
*/


#ifndef HGCONTEXTUTILITYBASE_H
#define HGCONTEXTUTILITYBASE_H

#include <e32base.h>
#include <bamdesca.h>

class CHgContextUtilityImpl;

/**
 *  Utility base class to publish and access context through the Context Framework.
 *  @lib hgcontextutility.lib
 */
class CHgContextUtilityBase : public CBase
    {
public:    
    /**
     * Publishes context.
     * Also defines the context if it has not been defined.
     * Publishing empty value is not allowed, however such errors are ignored
     * here so the function will not leave when CFW responds with KErrArgument.
     * The security policy for the context will be set to require
     * LocalServices capability.
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context
     */
    IMPORT_C void PublishContextL( const TDesC& aContextType,
        const TDesC& aContextData );
    
    /**
     * Publishes context, the value will contain all the strings
     * from the given array, typically by using some separator character.
     * @see PublishContextL
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context will be a combined
     * version of all the strings from this array
     */
    IMPORT_C void PublishContextL( const TDesC& aContextType,
        const MDesCArray& aContextData );
    
    /**
     * Publishes context but only after a short interval, using a timer.
     * If it is called again before the timer expires then the timer
     * is restarted (and so the previous pending value is never published).
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context
     * @param   aDelay          delay for the timer, in microseconds
     */
    IMPORT_C void PublishContextDelayedL( const TDesC& aContextType,
        const TDesC& aContextData, const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Overload for delayed publishing of a value combined from multiple strings.
     * @param   aContextType    context type
     * @param   aContextData    string array
     * @param   aDelay          delay for the timer, in microseconds
     */
    IMPORT_C void PublishContextDelayedL( const TDesC& aContextType,
        const MDesCArray& aContextData, const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Requests the given context and returns the value for the
     * first result. Returns NULL if not found.
     * @param   aContextType    context type, the source is always KHgCFSource
     */        
    IMPORT_C HBufC* GetContextL( const TDesC& aContextType );
    
    /**
     * Requests the given context and returns the value for the
     * first result. Returns NULL if not found.
     * @param   aContextSource  context source
     * @param   aContextType    context type
     */
    IMPORT_C HBufC* GetContextL( const TDesC& aContextSource,
        const TDesC& aContextType );

protected:
    CHgContextUtilityBase();
    ~CHgContextUtilityBase();
    void BaseConstructL();
    CHgContextUtilityImpl* iImpl;
    };
    
#endif /* HGCONTEXTUTILITYBASE_H */
