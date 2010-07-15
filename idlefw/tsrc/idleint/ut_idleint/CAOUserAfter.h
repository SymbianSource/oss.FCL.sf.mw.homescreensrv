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
#ifndef __CAOUSERAFTER_H__
#define __CAOUSERAFTER_H__

// INCLUDES
#include <E32base.h>
#include <E32svr.h>
#include <CoeMain.h>


/**
 * Active object version of User::After()
 */
class CAOUserAfter : public CTimer
    {
    private:
        CActiveSchedulerWait iWait;
        TInt                 iLength;


    public: //new functions

        /**
         * Constructs the waiting timer, executes
         * it and when timer expires, deletes self.
         * @param aLength The lenghth of wait in micro
         *        seconds.
         */
        static void AfterLD( TInt aLenght )
            {
            CAOUserAfter* wait= new ( ELeave ) CAOUserAfter( aLenght );
            CleanupStack::PushL( wait );
            wait->ConstructL();
            CleanupStack::Pop();

            RDebug::Print( _L("CAOUserAfter - Starting time consuming...") );
            wait->StartWaitD();
            RDebug::Print( _L("CAOUserAfter - completed.") );
            }

    private:
        CAOUserAfter( TInt aLenght )
           : CTimer( CActive::EPriorityStandard ),
             iLength( aLenght )
            {
            }

        void ConstructL()
            {
            CTimer::ConstructL();
            CActiveScheduler::Add( this );
            }

       virtual ~CAOUserAfter()
            {
            }

        void StartWaitD()
            {
            After( iLength );
            iWait.Start();
            }

        void RunL()
            {
            iWait.AsyncStop();
            Cancel();
            delete this;
            }
    };

#endif // __CAOUSERAFTER_H__
// End of File

