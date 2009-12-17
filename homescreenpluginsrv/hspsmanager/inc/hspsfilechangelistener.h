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
* Description:  FileChangeListener listens to given file folder and notifies it observer
*                if there is any file change. Observer is given as a parameter on 
*                initialising the listener. Observer must implement MhspsFileChangeObserver-interface.
*                FileChangeListener is asynchronous object that deploys Symbian RFs client-side
*                handle on File Manager. FileChangeListener is intended to be used by any object
*                that wants listen Symbian file system events.
*
*
*/


#ifndef __ChspsFILECHANGELISTENER_H__
#define __ChspsFILECHANGELISTENER_H__

#include <e32base.h>
#include <s32file.h>

#include "hsps_builds_cfg.hrh"
#ifdef _hsps_EMULATOR_SUPPORT_
#include "hspsfilechangelisteneremu.h"
#endif

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif
class RFs;

enum TListenerType
{
  TListenerNone,
  TImportDirListener,
  TRepositoryListener,
};

/** 
* @ingroup group_hspsserver
* MhspsFileChangeObserver is interface definition to be implemented by change events observers.
*/
class MhspsFileChangeObserver
    {
    public:
        virtual void HandleFileSystemChange() = 0;
        virtual void HandleRepositoryChange() = 0;
    };


/**
*
*  @ingroup group_hspsserver
*  FileChangeListener listens to given file folder and notifies it observer
*  if there is any file change. Observer is given as a parameter on 
*  initialising the listener. Observer must implement MhspsFileChangeObserver-interface.
*  FileChangeListener is asynchronous object that deploys Symbian RFs client-side
*  handle on File Manager. FileChangeListener is intended to be used by any object
*  that wants listen Symbian file system events
*  @lib hspsThemeServer.exe 
*  @since S60 5.0
*/
class ChspsFileChangeListener 
: public CActive
#ifdef _hsps_EMULATOR_SUPPORT_
,public MhspsEmuFileChangeObserver
#endif
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsFileChangeListener* NewL( RFs& aFs, 
            MhspsFileChangeObserver& aObserver,
            TNotifyType aNotifyType, 
            const TDesC& aPath,
            TListenerType aListenerType );
        /**
        * ~ChspsFileChangeListener
        * Destructor.
        */
        virtual ~ChspsFileChangeListener();

    public:

        /**
        * Setup
        * Sets up the file change listener.
        * @since S60 3.1
        * @return void
        */
        void Setup();
#ifdef _hsps_EMULATOR_SUPPORT_
        /**
        * Setup
        * Sets up the file change listener.
        * @since S60 3.1
        * @return void
        */
        void HandleEmuFileSystemChange();
#endif

        /**
        * Set log bus to be used.
        * @param aLogBus Log bus to be used.
        * @return void
        */
#ifdef HSPS_LOG_ACTIVE
        void SetLogBus( ChspsLogBus* aLogBus );
#endif
        
    protected:  // Functions from base classes

        /**
        * From CActive Called when asynchronous request has completed
        * RunL
        * @since S60 3.1
        */
        void RunL();

        /**
        * From CActive Called when asynchronous request has failed
        * RunError
        * @since S60 3.1
        */
        TInt RunError( TInt aError );

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
        void ConstructL( const TDesC& aFullPath );

        /**
        * ChspsFileChangeListener
        * C++ default constructor.
        */
        ChspsFileChangeListener( RFs& aFs, 
            MhspsFileChangeObserver& aObserver,
            TNotifyType aNotifyType,
            TListenerType aListenerType );
        /**
        * Informs observer
        * InformObserver
        * @since S60 3.1
        */
        void InformObserver();

    private:    // Data

        // Ref: Reference to opened File System session
        RFs&                iFs;

        // Ref: This object is called when RunL occurs
        MhspsFileChangeObserver& iObserver;

        // Type of the notify, entry or disk.
        TNotifyType         iNotifyType;

        // Full path of folder from where notification is needed
        TFileName           iFullPath;
        TListenerType iListenerType;

#ifdef HSPS_LOG_ACTIVE        
        // Log bus. Not owned.
        ChspsLogBus* iLogBus;
#endif        

#ifdef _hsps_EMULATOR_SUPPORT_
        ChspsFileChangeListenerEmu* iFileChange;
#endif
    };


#endif // __ChspsFILECHANGELISTENER_H__

// End of File
