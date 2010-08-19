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
//DelayedFunctionCall.h
//DFC caller and templated generic function call objects.

#ifndef CDELAYEDFUNCTIONCALL_H__
#define CDELAYEDFUNCTIONCALL_H__


// INCLUDES
#include <E32base.h>


// CLASS DESCRIPTION

/**
 * Abstract interface for function call object.
 */
class MFunctionCall
    {
    public:

        /**
         * Function execution.
         *
         * Executes the function call.
         *
         * If function execution fails (e.g. leaves),
         * leave code is stored in CDelayedFunctionCall host.
         */
        virtual void InvokeL() = 0;

        virtual MFunctionCall* CloneLC() const = 0;


        /**
         * Public virtual destructor.
         *
         * Allows thus function call object deletion.
         */
        virtual ~MFunctionCall() {}
    };



// CLASS DESCRIPTION

/**
 * Delayed Function Call - caller.
 *
 * Executes DFC objects after given delay.
 */
class CDelayedFunctionCall : public CTimer
    {
    public: //Construction

        static inline CDelayedFunctionCall* NewLC();
        static inline CDelayedFunctionCall* NewL();
        inline ~CDelayedFunctionCall();

    public: //Delayed call

        /**
         * Issues DFC call. Panics if CDelayedFunctionCall object
         * is already in use.
         *
         * @param aFC The FC object to execute.
         * @param aCallDelay The delay when to execute the FC.
         * @param aFCOwnershipTransf If ETrue, the aFC object
         * ownership is transferred to DFC caller and DFC caller
         * deletes object. If EFalse, no ownership transfer is done.
         */
        inline void IssueDfc( MFunctionCall& aFC, TInt aCallDelay, TBool aFCOwnershipTransf = ETrue );


        /**
         * Similar like IssueDfc() but pushes
         * release to CleanupStack ==>
         * if there happens a leave before the DFC gets scheduled,
         * DFC is canceled.
         */
        inline void IssueDfcLC( MFunctionCall& aFC, TInt aCallDelay, TBool aFCOwnershipTransf = ETrue );


        /**
         * Releases the DFC.
         * Cancels the call and deletes possibly owned FC.
         */
        inline void Release();


        /**
         * Delayed FC error handling.
         */
        inline void LeaveIfFcFailedL();

        /**
         * Tests was the FC executed or not.
         */
        inline TBool FcExecuted();


    private:
        inline CDelayedFunctionCall();
        inline void RunL();
        inline TInt RunError( TInt aError );
        inline void InvokeL();


    private:    //data

        //REF/OWN: Function call object
        MFunctionCall*    iFC;

        //OWN: Is function call object owned
        TBool    iFCOwned;

        //OWN: Did function call leave
        TBool   iFCLeft;

        //OWN: Was function called
        TBool   iFCCalled;

        //OWN: Leave code from the function call
        TInt    iFCLeaveErr;
    };




// -----------------------------------------------------------------------------
// CDelayedFunctionCall public functions
// -----------------------------------------------------------------------------
//
inline CDelayedFunctionCall* CDelayedFunctionCall::NewLC()
    {
    CDelayedFunctionCall* self = new (ELeave) CDelayedFunctionCall();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

inline CDelayedFunctionCall* CDelayedFunctionCall::NewL()
    {
    CDelayedFunctionCall* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }


inline CDelayedFunctionCall::~CDelayedFunctionCall()
    {
    Cancel();
    if( iFCOwned )
        {
        delete iFC;
        }
    }


inline void CDelayedFunctionCall::IssueDfc( MFunctionCall& aFC,
                                            TInt aFCDelay,
                                            TBool aFCOwnershipTransf )
    {
    __ASSERT_ALWAYS( !IsActive(), User::Panic( _L("DFC caller"), KErrInUse ) );

    iFC = &aFC;
    iFCOwned = aFCOwnershipTransf;
    iFCLeft = EFalse;
    iFCLeaveErr = KErrNone;
    iFCCalled = EFalse;

    if( aFCDelay >= 0 )
        {
        After( aFCDelay );
        }
    else
        {
        InvokeL();
        }
    }


inline void CDelayedFunctionCall::IssueDfcLC( MFunctionCall& aFC,
                                              TInt aFCDelay,
                                              TBool aFCOwnershipTransf )
    {
    IssueDfc( aFC, aFCDelay, aFCOwnershipTransf );
    CleanupReleasePushL( *this );
    }


inline void CDelayedFunctionCall::Release()
    {
    Cancel();
    if( iFCOwned )
        {
        delete iFC;
        iFC = NULL;
        }
    }

inline void CDelayedFunctionCall::LeaveIfFcFailedL()
    {
    if( iFCLeft )
        {
        User::Leave( iFCLeaveErr );
        }
    }


inline TBool CDelayedFunctionCall::FcExecuted()
    {
    return iFCCalled;
    }



// -----------------------------------------------------------------------------
// CDelayedFunctionCall private functions
// -----------------------------------------------------------------------------
//
inline CDelayedFunctionCall::CDelayedFunctionCall()
    : CTimer( CActive::EPriorityHigh )
    {
    CActiveScheduler::Add( this );
    }

inline void CDelayedFunctionCall::RunL()
    {
    InvokeL();
    }

inline TInt CDelayedFunctionCall::RunError( TInt aError )
    {
    iFCLeft = ETrue;
    iFCCalled = ETrue;
    iFCLeaveErr = aError;
    if( iFCOwned )
        {
        delete iFC;
        iFC = NULL;
        }

    return KErrNone;
    }


inline void CDelayedFunctionCall::InvokeL()
    {
    iFC->InvokeL();
    iFCCalled = ETrue;
    if( iFCOwned )
        {
        delete iFC;
        iFC = NULL;
        }
    }







// =============================================================================
// Generic Function Call classes
// =============================================================================


// CLASS DESCRIPTION

/**
 * Base class for generic function call objects.
 * Initializes function call members to zero
 * during the object construction.
 */
class TMemFillBase
    {
    protected: //Protected constructor & virtual destructor
        TMemFillBase( TUint aSize ) { Mem::FillZ( this, aSize); }
        virtual ~TMemFillBase() {}
    };





// CLASS DESCRIPTION

/**
 * Generic function call object for function that returns void.
 *
 * This function call object doesn't own any of the
 * call parameters, nor the called object.
 */
template< class C,
          class P1=TInt,
          class P2=TInt,
          class P3=TInt,
          class P4=TInt >
class TGenericFuncCallVoid : public TMemFillBase, public MFunctionCall
    {
    private: //Function type shortcuts
        typedef void (C::*TFuncTypeVoid)();
        typedef void (C::*TFuncType1)( P1 aP1 );
        typedef void (C::*TFuncType2)( P1 aP1, P2 aP2 );
        typedef void (C::*TFuncType3)( P1 aP1, P2 aP2, P3 aP3 );
        typedef void (C::*TFuncType4)( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );


    public: //Construction / destruction
        TGenericFuncCallVoid( C& aObj, TFuncTypeVoid aFunc )
            : TMemFillBase( sizeof( TGenericFuncCallVoid ) ),
              iObj( aObj ),
              iFVoid( aFunc )
            {}

        TGenericFuncCallVoid( C& aObj, TFuncType1 aFunc, P1 aP1 )
            : TMemFillBase( sizeof( TGenericFuncCallVoid ) ),
              iObj( aObj ),
              iF1( aFunc ),
              iP1( aP1 )
            {}

        TGenericFuncCallVoid( C& aObj, TFuncType2 aFunc, P1 aP1, P2 aP2  )
            : TMemFillBase( sizeof( TGenericFuncCallVoid ) ),
              iObj( aObj ),
              iF2( aFunc ),
              iP1( aP1 ), iP2( aP2 )
            {}

        TGenericFuncCallVoid( C& aObj, TFuncType3 aFunc, P1 aP1 , P2 aP2, P3 aP3  )
            : TMemFillBase( sizeof( TGenericFuncCallVoid ) ),
              iObj( aObj ),
              iF3( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {}

        TGenericFuncCallVoid( C& aObj, TFuncType4 aFunc, P1 aP1 , P2 aP2, P3 aP3, P4 aP4 )
            : TMemFillBase( sizeof( TGenericFuncCallVoid ) ),
              iObj( aObj ),
              iF4( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {}

        ~TGenericFuncCallVoid()
            {}

    private: //From MFunctionCall

        void InvokeL()
            {
            if( iFVoid )
                {
                (iObj.*iFVoid)();
                }
            else if( iF1 )
                {
                (iObj.*iF1)( iP1 );
                }
            else if( iF2 )
                {
                (iObj.*iF2)( iP1, iP2 );
                }
            else if( iF3 )
                {
                (iObj.*iF3)( iP1, iP2, iP3 );
                }
            else if( iF4 )
                {
                (iObj.*iF4)( iP1, iP2, iP3, iP4 );
                }
            }

        MFunctionCall* TGenericFuncCallVoid::CloneLC() const
            {
			TAny* self = User::AllocL( sizeof( TGenericFuncCallVoid ) );
            Mem::Copy( self, this, sizeof( TGenericFuncCallVoid ) );
            TGenericFuncCallVoid* self2 = (TGenericFuncCallVoid*) self;
            CleanupDeletePushL( self2 );
            return (MFunctionCall*) self2;
            }


    private: //Call data

        C& iObj;
        P1 iP1;
        P2 iP2;
        P3 iP3;
        P4 iP4;
        TFuncTypeVoid iFVoid;
        TFuncType1 iF1;
        TFuncType2 iF2;
        TFuncType3 iF3;
        TFuncType4 iF4;
    };





// CLASS DESCRIPTION

/**
 * Generic function call object for function that returns a value
 * (with by-value convention).
 *
 * This function call object doesn't own any of the
 * call parameters, nor the called object or the result.
 */
template< class C,
          class R,
          class P1=TInt,
          class P2=TInt,
          class P3=TInt,
          class P4=TInt >
class TGenericFuncCallRetByValue : public TMemFillBase, public MFunctionCall
    {
    private: //Function type shortcuts
        typedef R (C::*TFuncTypeVoid)();
        typedef R (C::*TFuncType1)( P1 aP1 );
        typedef R (C::*TFuncType2)( P1 aP1, P2 aP2 );
        typedef R (C::*TFuncType3)( P1 aP1, P2 aP2, P3 aP3 );
        typedef R (C::*TFuncType4)( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );


    public: //Construction / destruction
        TGenericFuncCallRetByValue( C& aObj, TFuncTypeVoid aFunc )
            : TMemFillBase( sizeof( TGenericFuncCallRetByValue ) ),
              iObj( aObj ),
              iFVoid( aFunc )
            {}

        TGenericFuncCallRetByValue( C& aObj, TFuncType1 aFunc, P1 aP1 )
            : TMemFillBase( sizeof( TGenericFuncCallRetByValue ) ),
              iObj( aObj ),
              iF1( aFunc ),
              iP1( aP1 )
            {}

        TGenericFuncCallRetByValue( C& aObj, TFuncType2 aFunc, P1 aP1, P2 aP2  )
            : TMemFillBase( sizeof( TGenericFuncCallRetByValue ) ),
              iObj( aObj ),
              iF2( aFunc ),
              iP1( aP1 ), iP2( aP2 )
            {}

        TGenericFuncCallRetByValue( C& aObj, TFuncType3 aFunc, P1 aP1 , P2 aP2, P3 aP3  )
            : TMemFillBase( sizeof( TGenericFuncCallRetByValue ) ),
              iObj( aObj ),
              iF3( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {}

        TGenericFuncCallRetByValue( C& aObj, TFuncType4 aFunc, P1 aP1 , P2 aP2, P3 aP3, P4 aP4 )
            : TMemFillBase( sizeof( TGenericFuncCallRetByValue ) ),
              iObj( aObj ),
              iF4( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {}

        ~TGenericFuncCallRetByValue()
            {}

    private: //From MFunctionCall

        void InvokeL()
            {
            if( iFVoid )
                {
                iReturnValue = (iObj.*iFVoid)();
                }
            else if( iF1 )
                {
                iReturnValue = (iObj.*iF1)( iP1 );
                }
            else if( iF2 )
                {
                iReturnValue = (iObj.*iF2)( iP1, iP2 );
                }
            else if( iF3 )
                {
                iReturnValue = (iObj.*iF3)( iP1, iP2, iP3 );
                }
            else if( iF4 )
                {
                iReturnValue = (iObj.*iF4)( iP1, iP2, iP3, iP4 );
                }
            }

        MFunctionCall* TGenericFuncCallRetByValue::CloneLC() const
            {
			TAny* self = User::AllocL( sizeof( TGenericFuncCallRetByValue ) );
            Mem::Copy( self, this, sizeof( TGenericFuncCallRetByValue ) );
            TGenericFuncCallRetByValue* self2 = (TGenericFuncCallRetByValue*) self;
            CleanupDeletePushL( self2 );
            return (MFunctionCall*) self2;
            }

    private: //Call data

        C& iObj;
        P1 iP1;
        P2 iP2;
        P3 iP3;
        P4 iP4;
        TFuncTypeVoid iFVoid;
        TFuncType1 iF1;
        TFuncType2 iF2;
        TFuncType3 iF3;
        TFuncType4 iF4;

    public: //Return value

        R iReturnValue;
    };




template< class P1=TInt,
          class P2=TInt,
          class P3=TInt,
          class P4=TInt >
class TGenericGlobalFuncCallVoid : public TMemFillBase, public MFunctionCall
    {
    private: //Function type shortcuts
        typedef void (*TFuncTypeVoid)();
        typedef void (*TFuncType1)( P1 aP1 );
        typedef void (*TFuncType2)( P1 aP1, P2 aP2 );
        typedef void (*TFuncType3)( P1 aP1, P2 aP2, P3 aP3 );
        typedef void (*TFuncType4)( P1 aP1, P2 aP2, P3 aP3, P4 aP4 );


    public: //Construction / destruction
        TGenericGlobalFuncCallVoid( TFuncTypeVoid aFunc )
            : TMemFillBase( sizeof( TGenericGlobalFuncCallVoid ) ),
              iFVoid( aFunc )
            {}

        TGenericGlobalFuncCallVoid( TFuncType1 aFunc, P1 aP1 )
            : TMemFillBase( sizeof( TGenericGlobalFuncCallVoid ) ),
              iF1( aFunc ),
              iP1( aP1 )
            {}

        TGenericGlobalFuncCallVoid( TFuncType2 aFunc, P1 aP1, P2 aP2  )
            : TMemFillBase( sizeof( TGenericGlobalFuncCallVoid ) ),
              iF2( aFunc ),
              iP1( aP1 ), iP2( aP2 )
            {}

        TGenericGlobalFuncCallVoid( TFuncType3 aFunc, P1 aP1 , P2 aP2, P3 aP3  )
            : TMemFillBase( sizeof( TGenericGlobalFuncCallVoid ) ),
              iF3( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 )
            {}

        TGenericGlobalFuncCallVoid( TFuncType4 aFunc, P1 aP1 , P2 aP2, P3 aP3, P4 aP4 )
            : TMemFillBase( sizeof( TGenericGlobalFuncCallVoid ) ),
              iF4( aFunc ),
              iP1( aP1 ), iP2( aP2 ), iP3( aP3 ), iP4( aP4 )
            {}

        ~TGenericGlobalFuncCallVoid()
            {}

    private: //From MFunctionCall

        void InvokeL()
            {
            if( iFVoid )
                {
                (*iFVoid)();
                }
            else if( iF1 )
                {
                (*iF1)( iP1 );
                }
            else if( iF2 )
                {
                (*iF2)( iP1, iP2 );
                }
            else if( iF3 )
                {
                (*iF3)( iP1, iP2, iP3 );
                }
            else if( iF4 )
                {
                (*iF4)( iP1, iP2, iP3, iP4 );
                }
            }

        MFunctionCall* TGenericGlobalFuncCallVoid::CloneLC() const
            {
			TAny* self = User::AllocL( sizeof( TGenericGlobalFuncCallVoid ) );
            Mem::Copy( self, this, sizeof( TGenericGlobalFuncCallVoid ) );
            TGenericGlobalFuncCallVoid* self2 = (TGenericGlobalFuncCallVoid*) self;
            CleanupDeletePushL( self2 );
            return (MFunctionCall*) self2;
            }

    private: //Call data

        P1 iP1;
        P2 iP2;
        P3 iP3;
        P4 iP4;
        TFuncTypeVoid iFVoid;
        TFuncType1 iF1;
        TFuncType2 iF2;
        TFuncType3 iF3;
        TFuncType4 iF4;
    };




// CLASS DESCRIPTION


/**
 * Generic function call object to delete objects.
 *
 * This function call object doesn't own any of the
 * parameters ==> the objects are deleted only when
 * the DFC is executed.
 *
 * If deleted objects are provided as pointer reference ( Class*& ),
 * the original pointer to object (e.g. class member)
 * is set to NULL during the deletion.
 */
template< class C1,
          class C2 = TAny*,
          class C3 = TAny* >
class TGenericObjDelete : public TMemFillBase, public MFunctionCall
    {
    public: //Construction / destruction
        explicit TGenericObjDelete( C1 aObj1 )
            : TMemFillBase( sizeof( TGenericObjDelete ) ),
              iObj1( aObj1 )
            {}

        TGenericObjDelete( C1 aObj1, C2 aObj2 )
            : TMemFillBase( sizeof( TGenericObjDelete ) ),
              iObj1( aObj1 ),
              iObj2( aObj2 )
            {}

        TGenericObjDelete( C1 aObj1, C2 aObj2, C3 aObj3 )
            : TMemFillBase( sizeof( TGenericObjDelete ) ),
              iObj1( aObj1 ),
              iObj2( aObj2 ),
              iObj3( aObj3 )
            {}

        ~TGenericObjDelete()
            {}

    private: //From MFunctionCall

        void InvokeL()
            {
            delete iObj1;
            iObj1 = NULL;

            delete iObj2;
            iObj2 = NULL;

            delete iObj3;
            iObj3 = NULL;
            }

        MFunctionCall* TGenericObjDelete::CloneLC() const
            {
			TAny* self = User::AllocL( sizeof( TGenericObjDelete ) );
            Mem::Copy( self, this, sizeof( TGenericObjDelete ) );
            TGenericObjDelete* self2 = (TGenericObjDelete*) self;
            CleanupDeletePushL( self2 );
            return (MFunctionCall*) self2;
            }

    private: //Call data

        C1 iObj1;
        C2 iObj2;
        C3 iObj3;
    };


#endif // CDELAYEDFUNCTIONCALL_H__


// End of File


