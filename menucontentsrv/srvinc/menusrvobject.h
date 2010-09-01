/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MENUSRVOBJECT_H__
#define __MENUSRVOBJECT_H__

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class RMessage2;

// CLASS DECLARATION

/**
* Async request server side base object. Takes ownership of the completion
* of one asynchronously completing message.
* Derived classes should implement a request method which takes a message
* and completes it.
*/
NONSHARABLE_CLASS( CMenuSrvObject ): public CObject
    {

public:     // construction

    /**
    * Denstructor. Outstanding message, if any, is completed with cancel.
    */
    virtual ~CMenuSrvObject() { Cancel(); }

    /**
    * Constructor.
    */
    CMenuSrvObject() {}

public:     // new methods

    /**
    * Cancel request, if any. Base implementation completes the message with
    * KErrCancel. Override and add real cancellation of outstanding requests.
    */
    virtual void Cancel() { Complete( KErrCancel ); }

protected:  // new methods

    /**
    * Set this message as pending (take ownership of completion).
    * Call this as LAST thing in the request method,
    * and DO NOT LEAVE AFTER this is called.
    * Leaving after this method is called causes double completion of the
    * message (once by this object, once by the sessions ServiceError()).
    * @param aMessage Message to complete. Will be completed by this object.
    */
    void SetPending( const RMessage2& aMessage );

    /**
    * Panic aMessage and leave if this object is pending. Call this as first
    * thing in request method. (Sanity checking.)
    * @param aMessage Message.
    */
    void PanicIfPendingL( const RMessage2& aMessage );

    /**
    * Completes the mesage, if pending. Does nothing if not pending.
    * @param aReason Completion code.
    */
    void Complete( TInt aReason );

protected:  // data

    TBool iPending; ///< Pending. (Owner of an incompleted message).
    RMessage2 iMessage; ///< The message (valid only if iPending).

    };

#endif // __MENUSRVOBJECT_H__
            
// End of File
