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

#ifndef HSPSFAMILYLISTENER_H_
#define HSPSFAMILYLISTENER_H_

#include "hspsfamily.h"

class ChspsFamilyListener;
/**
 * @ingroup group_hspsserver  
 * HandleFamilyChangeL.
 * Handles notifications related to resolution/orientation changes.
 * Clients are requested to reload UIs with a resolution specific widget set.      
 * Executable on emulator environment only!
 * @lib hspsThemeServer.exe 
 * @since S60 5.0
 * @param aNewFamily An id for retrieving the new application configuration
 * @return True if the event was handled properly
 */
class MhspsFamilyObserver
    {
    public:
        virtual TBool HandleFamilyChangeL( const ThspsFamily aNewFamily ) = 0;
    };


class ChspsFamilyListenerActive : public CActive
	{
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsFamilyListenerActive* NewL(
                ChspsFamilyListener& aListener,
                MhspsFamilyObserver& aObserver );
        
        /**
        * ~ChspsFamilyListener
        * Destructor.
        */
        virtual ~ChspsFamilyListenerActive();
        

    private:

        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        ChspsFamilyListenerActive(
                        ChspsFamilyListener& aListener,
                        MhspsFamilyObserver& iObserver);

    public:
         /**
         * Start listener.
         */
        void Queue();

    protected:  // Functions from base classes

        /**
        * From CActive Called when asynchronous request has completed
        * RunL
        * @since S60 5.0
        */
        void RunL();

        /**
        * DoCancel
        * From CActive Called when asynchronous request was cancelled
        * @since S60 5.0
        */
        void DoCancel();
        
        /**
         * RunError
         * Handles a leave occurring in the request completion event handler RunL().
         * @since S60 5.0 
         */
        TInt RunError(TInt aError);
        
    private: // data
    
        // Listener reference, not owned
        ChspsFamilyListener& iListener;

        // Observer which is called when RunL occurs
        MhspsFamilyObserver& iObserver;

	};

/**
*  @ingroup group_hspsserver
*  ChspsFamilyListener is used to listen to the changes in UI's resolution and orientation.
*  Used on emulator environment only. 
*
*  @lib hspsThemeServer.exe 
*  @since S60 5.0
*/
class ChspsFamilyListener : public ChspsFamily
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsFamilyListener* NewL(
                MhspsFamilyObserver& aObserver );
        
        /**
        * ~ChspsFamilyListener
        * Destructor.
        */
        virtual ~ChspsFamilyListener();
        

    private:

        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( MhspsFamilyObserver& aObserver );

        /**
        * ChspsCenRepListener
        * C++ default constructor.
        */
        ChspsFamilyListener();
        
        
        /**
         * Retrieves used display code.
         */
        void GetFamilyString(        
                TDes8& aFamily );                

    private:    // Data

        // Client-side handle to a server-side window group.
        RWindowGroup iWindowGroup;
        
        ChspsFamilyListenerActive* iFamilyListenerActive;
        
    };

#endif /* HSPSFAMILYLISTENER_H_ */

// End of File
