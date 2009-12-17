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

#ifndef __MENUSRVSESSION_H__
#define __MENUSRVSESSION_H__

//  INCLUDES

#include <e32base.h>
#include "menusrv.h"

// FORWARD DECLARATION

class MStreamBuf;
class CMenuSrvStream;
class CMenuSrvOperation;
class CMenuSrvNotifier;
class CMenuSrvEng;

// CLASS DECLARATION

/**
* Menu Server Session.
*/
NONSHARABLE_CLASS( CMenuSrvSession ) : public CSession2
    {

public:     // construct / destruct

    /**
    * Destructor.
    */
    virtual ~CMenuSrvSession();

    /**
    * Two phase constructor. Leaves on failure.
    * @param aMenuSrv Menu Server.
    */
    static CMenuSrvSession* NewL( CMenuSrv& aMenuSrv );

protected:  // construct

    /**
    * Constructor. Leaves on failure.
    * @param aMenuSrv Menu Server.
    */
    CMenuSrvSession( CMenuSrv& aMenuSrv );

    /**
    * Second phase constructor. Leaves on failure.
    */
    void ConstructL();

public:     // From CSession2

    /**
    * Count resources.
    * @return Resource count.
    */
    virtual TInt CountResources();

    /**
    * Service message.
    * @param aMessage Message.
    */
    virtual void ServiceL( const RMessage2& aMessage );

    /**
    * Error in message processing.
    * @param aMessage Message.
    * @param aError Error.
    */
    virtual void ServiceError( const RMessage2& aMessage, TInt aError );

public:     // new methods

    /**
    * Engine events -> complete matching notifiers.
    * @param aFolder Events occurred in this folder. (0==unspecified.)
    * @param aEvents Events. A combination of TEvent bits.
    */
    void EngineEvents( TInt aFolder, TInt aEvents );

    /**
    * Unrecoverable Engine error. The engine has stopped working and is
    * useless -> so are we. Disconnect this session from the Engine, this
    * allow it to delete itself. From now on, (almost) all messages are
    * completed with error.
    * @param aErr Error code.
    */
    void EngineError( TInt aErr );

private:    // Message handling

    /**
    * Create new stream object, write its subsession handle back in slot 3.
    * @param aHost Host. Ownership taken by the stream.
    * @param aMessage Message.
    */
    void NewStreamL( MStreamBuf& aHost, const RMessage2& aMessage );

    /**
    * Get stream by handle. Leaves with KErrBadHandle (==panics client) if
    * not found.
    * @param aHandle Handle.
    * @return Stream.
    */
    CMenuSrvStream& StreamL( TInt aHandle );

    /**
    * Store operation, write its subsession handle back in slot 3.
    * @param aOperation Operation. Note: ownership taken BEFORE adding (at
    * the beginning of this method)!
    * @param aMessage Message.
    */
    void AddOperationL( CMenuSrvOperation* aOperation, const RMessage2& aMessage );

    /**
    * Get operation by handle. Leaves with KErrBadHandle (==panics client) if
    * not found.
    * @param aHandle Handle.
    * @return Operation.
    */
    CMenuSrvOperation& OperationL( TInt aHandle );

    /**
    * Create new notifier object, write its subsession handle back in slot 3.
    * @param aMessage Message.
    */
    void NewNotifierL( const RMessage2& aMessage );

    /**
    * Get notifier by handle. Leaves with KErrBadHandle (==panics client) if
    * not found.
    * @param aHandle Handle.
    * @return Notifier.
    */
    CMenuSrvNotifier& NotifierL( TInt aHandle );

    /**
    * Client error, panic client and leave.
    * @param aMessage Message.
    */
    void PanicClientL( const RMessage2& aMessage );
    
    /**
    * Service message.
    * @param aMessage Message.
    * @return ETrue if function was serviced 
    */
    TBool ServiceCapabilityTestL( const RMessage2& aMessage );
    
    /**
    * Service message.
    * @param aMessage Message.
    * @return ETrue if function was serviced
    */
    TBool ServiceCapReadL( const RMessage2& aMessage );
    
    /**
    * Service message.
    * @param aMessage Message.
    * @return ETrue if function was serviced
    */
    TBool ServiceCapWriteL( const RMessage2& aMessage );

private:    // Data

    TBool iEngineDead; ///< If the engine dies, the session stops working.
    CMenuSrv& iMenuSrv; ///< Menu server. Not owned.
    CMenuSrvEng* iSharedEng; ///< Engine instance. Shared object, Close() needed.
    CObjectCon* iObjectCon; ///< Object container. Owned.
    CObjectIx* iNotifierIx; ///< Notifier index. Owned.
    CObjectIx* iStreamIx; ///< Stream index. Owned.
    CObjectIx* iOperationIx; ///< Operation index. Owned.

};

#endif // __MENUSRVSESSION_H__

// End of File
