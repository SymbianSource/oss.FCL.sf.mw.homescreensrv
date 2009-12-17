/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ChspsFileChangeListenerEmu listens to given file folder and notifies it observer
*                if there is any file change. Observer is given as a parameter on 
*                initialising the listener. Observer must implement MhspsEmuFileChangeObserver-interface.
*                FileChangeListener is intended to be used by any object that wants listen Windows file system events
*                like file drag/drop events.
*                This listener works only in emulator environment.        
*
*
*/


#ifndef __CFILECHANGEEMU_H__
#define __CFILECHANGEEMU_H__

#include "hsps_builds_cfg.hrh"

#ifdef _hsps_EMULATOR_SUPPORT_

#include <e32base.h>
#include <s32file.h>
#include <badesca.h>

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

const TInt KFileSystemPollingTime = 5000000;
const TInt KFilelistGranularity = 10; 

enum TEmuNotifyType
	{
	/**
	Any change
	*/
	EEmuNotifyAll=0x01,
	};
	
enum TStateType
	{
	EEmuStateIdle,
	EEmuStateRunning
	};


/** 
* MhspsFileChangeObserver is interface definition to be implemented by change events observers.
*/
class MhspsEmuFileChangeObserver
    {
    public:
        virtual void HandleEmuFileSystemChange() = 0;
    };

/**
*
*
*  @lib hspsThemeServer.exe 
*  @since S60 3.1
*/
class ChspsFileChangeListenerEmu 
: public CTimer
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsFileChangeListenerEmu* NewL( RFs& aFs, 
            MhspsEmuFileChangeObserver& aObserver,
            TEmuNotifyType aNotifyType, 
            const TDesC& aPath);
        /**
        * ~ChspsFileChangeListenerEmu
        * Destructor.
        */
        virtual ~ChspsFileChangeListenerEmu();

    public:

        /**
        * Setup
        * Starts file change listener.
        * @since S60 3.1
        * @return void
        */
        void StartL();
        /**
        * Stop
        * Stops file change listener.
        * @since S60 3.1
        * @return void
        */
        void Stop();

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
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
        void DoCancel();

    private:

        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aFullPath );

        /**
        * ChspsFileChangeListenerEmu
        * C++ default constructor.
        */
        ChspsFileChangeListenerEmu( RFs& aFs, 
            MhspsEmuFileChangeObserver& aObserver,
            TEmuNotifyType aNotifyType);
        /**
        * 
        * InformObserver
        * Informs observer of file change
        * @since S60 3.1
        */
        void InformObserver();
        /**
        * 
        * RetrieveFileList
        * Retrieves current filelist from filesystem
        * @since S60 3.1
        * @param aFileList  Destination array for fetched filelist
        * @param aPath      Source path to read files from
        */
        void RetrieveFileListL(CDesCArraySeg& aFileList,const TDesC& aPath);
        /**
        * 
        * CompareFiles
        * Compares given filelist to filelist in memory
        * @since S60 3.1
        * @param    aFileList   Filelist to use comparing against filelist in memory
        */
        TBool CompareFilesL(CDesCArraySeg& aFileList);
        /**
        * 
        * StoreFileList
        * Stores filelist to memory
        * @since S60 3.1
        * @param    aFileList   filelist to store to memeory
        */
        void StoreFileListL(CDesCArraySeg& aFileList);


    private:    // Data
        // Reference to opened File System session
        RFs&                    iFs;
    
        // This object is called when RunL occurs
        MhspsEmuFileChangeObserver& iObserver;

        // Type of the notify
        TEmuNotifyType         iNotifyType;

        // Full path of folder from where notification is needed
        TFileName              iFullPath;
        
        // Last updated list of files in filesyste
        CDesCArraySeg          *iFileList;
        
        // stores the current state of the listener
        TStateType             iState;

#ifdef HSPS_LOG_ACTIVE        
        // Log bus. Not owned.
        ChspsLogBus* iLogBus;
#endif        
    };


#endif //_hsps_EMULATOR_SUPPORT_

#endif // __CFILECHANGEEMU_H__

// End of File
