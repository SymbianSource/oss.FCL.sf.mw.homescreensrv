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
* Description:  Xuikon hspshspsFileChangeListenerEmu listens to given file folder and notifies it observer (Only in Emulator environment)
*                if there is any file change.
*
*
*/


#include "hsps_builds_cfg.hrh"

#ifdef _hsps_EMULATOR_SUPPORT_

#include "hspsfilechangelisteneremu.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::ChspsFileChangeListenerEmu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsFileChangeListenerEmu::ChspsFileChangeListenerEmu( RFs& aFs, 
                              MhspsEmuFileChangeObserver& aObserver,
                              TEmuNotifyType aNotifyType) :
    CTimer(EPriorityLow),
    iFs( aFs ), 
    iObserver( aObserver), 
    iNotifyType( aNotifyType )
    {
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::ConstructL( const TDesC& aFullPath )
    {
    
    iFileList = new( ELeave ) CDesCArraySeg(KFilelistGranularity );
    iState = EEmuStateIdle;
   
    if ( aFullPath.Length() > 0 )
        {
        iFullPath.Append( aFullPath );
        }
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsFileChangeListenerEmu* ChspsFileChangeListenerEmu::NewL(   RFs& aFs, 
                                        MhspsEmuFileChangeObserver& aObserver, 
                                        TEmuNotifyType aNotifyType,
                                        const TDesC& aFullPath)
    {
    ChspsFileChangeListenerEmu* self = new(ELeave) ChspsFileChangeListenerEmu( aFs, 
        aObserver, 
        aNotifyType);
    CleanupStack::PushL( self );
    self->ConstructL( aFullPath );
    CleanupStack::Pop( self );

    return self;
    }


// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::~ChspsFileChangeListenerEmu
// Destructor
// ------------------------------------------------------------------------------
ChspsFileChangeListenerEmu::~ChspsFileChangeListenerEmu()
    {
    Cancel();
    
    if (iFileList)
        {
        delete iFileList;
        iFileList = NULL;
        }
        
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::RunL
// From CActive. Called when asynchronous request is completed. 
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::RunL()
    {
    TBool Diff(EFalse);
    TInt errCode = KErrNone;
    if (iStatus.Int() == KErrNone)
        {
        CDesCArraySeg* CurFileList = new( ELeave ) CDesCArraySeg(KFilelistGranularity );
        CleanupStack::PushL(CurFileList);

        // Retrieve current list of files in filesystem
        TRAP(errCode, RetrieveFileListL(*CurFileList, iFullPath);)
        if (errCode)
            {
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsFileChangeListenerEmu::RunL():RetrieveFileListL, Error=%d" ),
                        errCode );
                }
#endif    
            }

        // Compare files  
        TRAP(errCode, Diff = CompareFilesL(*CurFileList);)
        if (errCode)
            {
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsFileChangeListenerEmu::RunL():CompareFilesL, Error=%d" ),
                        errCode );
                }
#endif
            }
        
        // If changes, call observer handle function
        if (Diff) 
            {
            InformObserver(); 
            }

        // Store current file situation
        TRAP(errCode, StoreFileListL(*CurFileList);)
        if (errCode)
            {
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsFileChangeListenerEmu::RunL():StoreFileListL, Error=%d" ),
                        errCode );
                }
#endif
            }
            
        CleanupStack::Pop( CurFileList );
        if ( CurFileList )
            {
        	delete CurFileList;
        	CurFileList = NULL;
            }
                        
        // Restart timer
        After(KFileSystemPollingTime);  
        }
 
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::RunError
// From CActive. Called when error occurred in asynchronous request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsFileChangeListenerEmu::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::DoCancel()
// Not implemented yet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void ChspsFileChangeListenerEmu::DoCancel()
    {
    iState = EEmuStateIdle;
    CTimer::DoCancel();
    } 
    
// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::StartL
// Starts polling directory for changes
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::StartL()
    {
    TInt errCode = KErrNone;
    
    if (iState == EEmuStateIdle)
        {
        CDesCArraySeg* CurFileList = new( ELeave ) CDesCArraySeg(KFilelistGranularity );
        
        CleanupStack::PushL(CurFileList);
        
        // Retrieve current list of files in filesystem
        TRAP(errCode, RetrieveFileListL(*CurFileList, iFullPath);)
        if (errCode)
            {
            User::Leave(errCode);
            }
        
        // Store current file situation
        TRAP(errCode, StoreFileListL(*CurFileList);)
        if (errCode)
            {
            User::Leave(errCode);
            }
        
        CleanupStack::Pop( CurFileList );
        if ( CurFileList )
            {
        	delete CurFileList;
        	CurFileList = NULL;
            }

        // Start timer
        After(KFileSystemPollingTime);
        
        iState = EEmuStateRunning;
        }
    
    }
   
// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::Stop
// Stops polling of directory
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::Stop()
    {
    Cancel();
    }

#ifdef HSPS_LOG_ACTIVE
// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::SetLogBus
// Set log bus to be used.
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#endif

    
// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::InformObserver
// Informs observer of file change
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::InformObserver()
    {
    iObserver.HandleEmuFileSystemChange();        
    }

// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::RetrieveFileListL
// Retrieves current list of files in filesystem
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::RetrieveFileListL( CDesCArraySeg& aFileList, const TDesC& aPath )
    {  
   CDirScan* dir_list = CDirScan::NewL( iFs );
   CleanupStack::PushL( dir_list );
   
   dir_list->SetScanDataL( aPath,
           KEntryAttNormal,
           ESortByName | EAscending,
           CDirScan::EScanDownTree );
   
   CDir* dirEntries = NULL;   
   dir_list->NextL( dirEntries );   
   while( dirEntries != NULL )
       {
       CleanupStack::PushL( dirEntries );
       
       TInt iDirCount = dirEntries->Count();
       for( TInt i = 0; i < iDirCount; i++ )
           {
           const TEntry a = dirEntries->operator[]( i );
           aFileList.AppendL( a.iName );
           }
       
       CleanupStack::PopAndDestroy( dirEntries );
       dirEntries = NULL;
       dir_list->NextL( dirEntries );       
       }

   CleanupStack::PopAndDestroy(dir_list);

   }
    
// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::StoreFileListL
// Stores list of files to memory
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListenerEmu::StoreFileListL(CDesCArraySeg& aFileList)
    {
    
    if (iFileList)
        {
        iFileList->Reset();
        
        for ( TInt i = 0; i < aFileList.Count(); i++)
            {
            TPtrC t = aFileList.MdcaPoint(i);
            iFileList->AppendL(t);	
            }
        }
    }    
    

// ------------------------------------------------------------------------------
// ChspsFileChangeListenerEmu::CompareFilesL
// Compares given filelist to filelist on memory
// ------------------------------------------------------------------------------
//
TBool ChspsFileChangeListenerEmu::CompareFilesL(CDesCArraySeg& aFileList)
    {
    TInt p = 0;
    TBool ret(EFalse);
    
    if (iFileList->Count() == aFileList.Count() )
        {
        for (p = 0;p < iFileList->Count(); p++)
            {
               TBool found(EFalse);
               TInt i=0;
               while (!found && i<aFileList.Count())
                  {
                     TPtrC t = iFileList->MdcaPoint(i);
                     if (!(t.CompareF(aFileList.MdcaPoint(p)) ) )
                     {
                        found=ETrue;
                        break;
                     }
                     
                    i++;
                  }
                  
               if (!found)
               {
                  ret = ETrue;
                  break;
               }
            }
        }
    else
        {
        ret = ETrue;        
        }
          
   return ret;
  
   }

#endif // _hsps_EMULATOR_SUPPORT_
    
// End of File
