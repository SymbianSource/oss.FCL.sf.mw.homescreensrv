/*
* Copyright (c) 2002-2004,2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CenRepListener listens to given Central Repository setting id and notifies its
*                observer if there is any change. Observer is given as a parameter on 
*                initialising the listener. Observer must implement MhspsCenRepObserver-interface.
*                CenRepListener is asynchronous object that deploys a client-side handle
*                on Central Repository Server. CenRepListener is intended to be used by any object
*                that wants listen Central Repository changes.
*
*
*/


#ifndef __ChspsCENREPLISTENER_H__
#define __ChspsCENREPLISTENER_H__

#include <s32file.h>

class CRepository;

/**
* @ingroup group_hspsserver
*  MhspsCenRepObserver is used to listen to the changes in the Central Repository. 
*
*  @lib hspsThemeServer.exe 
*  @since S60 5.0
*/
class MhspsCenRepObserver
    {
    public:
        virtual void HandleCenRepChangeL( const TUint32 aId ) = 0;
    };

/**
*  @ingroup group_hspsserver
*  ChspsCenRepListener is used to listen to the changes in the Central Repository. 
*
*  @lib hspsThemeServer.exe 
*  @since S60 5.0
*/
class ChspsCenRepListener : public CActive
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsCenRepListener* NewL( 
            MhspsCenRepObserver& aObserver,
            const TUid& aRepositoryUid );
        /**
        * ~ChspsCenRepListener
        * Destructor.
        */
        virtual ~ChspsCenRepListener();

    public:

        /**
        * Setup
        * Sets up the Central Repository change listener for setting aId.
        * @since S60 3.1
        * @return void
        */
        void Setup( TUint32 aId );
        
        /**
        * SetupAll
        * Sets up the Central Repository change listener for all settings.
        * @since S60 3.1
        * @return void
        */
        void SetupAll();
        
    protected:  // Functions from base classes

        /**
        * From CActive Called when asynchronous request has completed
        * RunL
        * @since S60 3.1
        */
        void RunL();

        /**
        * DoCancel
        * From CActive Called when asynchronous request was cancelled
        * @since S60 3.1
        */
        void DoCancel();

    private:

        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * ChspsCenRepListener
        * C++ default constructor.
        */
        ChspsCenRepListener( MhspsCenRepObserver& aObserver,      
            const TUid& aRepositoryUid );

    private:    // Data

        // Observer which is called when RunL occurs
        MhspsCenRepObserver& iObserver;
        // Pointer to Central Repository
        CRepository* iRepository;
        // Uid of the accessed Central Repository
        TUid iRepositoryUid;
    };


#endif // __ChspsCENREPLISTENER_H__

// End of File