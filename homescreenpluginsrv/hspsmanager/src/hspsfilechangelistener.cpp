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
* Description:  hsps FileChangeListener listens to given file folder and notifies it observer
*                if there is any file change.
*
*
*/

#include "hsps_builds_cfg.hrh"
#include "hspsfilechangelistener.h"

#ifdef _hsps_EMULATOR_SUPPORT_
#include "hspsfilechangelisteneremu.h"
#endif

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::ChspsFileChangeListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

ChspsFileChangeListener::ChspsFileChangeListener( RFs& aFs, 
                                              MhspsFileChangeObserver& aObserver,
                                              TNotifyType aNotifyType,
                                              TListenerType aListenerType) :
    CActive( EPriorityStandard ), 
    iFs( aFs ), 
    iObserver( aObserver), 
    iNotifyType( aNotifyType ),
    iListenerType( aListenerType )
    {
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsFileChangeListener::ConstructL( const TDesC& aFullPath )
    {
    if ( aFullPath.Length() > 0 )
        {
        iFullPath.Append( aFullPath );
        }
    CActiveScheduler::Add( this );
    // should not make file change listener active yet because server must let 
    // start fully first. See ChspsThemeServer constructor.
#ifdef _hsps_EMULATOR_SUPPORT_
   iFileChange = ChspsFileChangeListenerEmu::NewL( iFs, *this, EEmuNotifyAll, aFullPath);
#endif
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsFileChangeListener* ChspsFileChangeListener::NewL( RFs& aFs, 
                                                    MhspsFileChangeObserver& aObserver, 
                                                    TNotifyType aNotifyType,
                                                    const TDesC& aFullPath,
                                                    TListenerType aListenerType )
    {
    ChspsFileChangeListener* self = new(ELeave) ChspsFileChangeListener( aFs, 
        aObserver, 
        aNotifyType,
        aListenerType );
    CleanupStack::PushL( self );
    self->ConstructL( aFullPath );
    CleanupStack::Pop( self );

    return self;
    }


// ------------------------------------------------------------------------------
// ChspsFileChangeListener::~ChspsFileChangeListener
//
// ------------------------------------------------------------------------------
ChspsFileChangeListener::~ChspsFileChangeListener()
    {
    iFs.NotifyChangeCancel(iStatus);
    Cancel();
#ifdef _hsps_EMULATOR_SUPPORT_
    if (iFileChange)
        {
        delete iFileChange;
        iFileChange = NULL;
        }
#endif
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::RunL
// From CActive. Called when asynchronous request is completed. 
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsFileChangeListener::RunL()
    {
    if ( iStatus.Int() == KErrNone ) 
        {
        InformObserver();
        }  
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::RunError
// From CActive. Called when error occurred in asynchronous request
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsFileChangeListener::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::DoCancel
// From CActive. Called when asynchronous request is canceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsFileChangeListener::DoCancel()
    {
    iFs.NotifyChangeCancel(iStatus);
    }

// ------------------------------------------------------------------------------
// ChspsFileChangeListener::Setup
//
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListener::Setup()
    {
    if( !IsActive() )
        {
        iFs.NotifyChangeCancel(iStatus);
        if( iFullPath.Length() > 0 && !IsActive() )
            {
            iStatus = KRequestPending;
            SetActive();
            iFs.NotifyChange( ENotifyAll, iStatus, iFullPath );
            }
        }
#ifdef _hsps_EMULATOR_SUPPORT_
    TInt errCode = KErrNone;
    TRAP(errCode,iFileChange->StartL();)
    if (errCode)
        {
#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsFileChangeListener::Setup() errCode=%d" ),
                    errCode );
            }
#endif        
        } 
#endif
    }
    
// ------------------------------------------------------------------------------
// ChspsFileChangeListener::InformObserver
//
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListener::InformObserver()
    {
    
    switch (iListenerType)
        {
        case TImportDirListener:
            {
            iObserver.HandleFileSystemChange();        
            }
        break;
        
        case TRepositoryListener:
            {
            iObserver.HandleRepositoryChange();    
            }
        break;
        
        default:
            {
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsFileChangeListener::RunL(): - Invalid listener type(%d)" ),
                        iListenerType );
                }
#endif       
            }
        break;
        }
    }

#ifdef _hsps_EMULATOR_SUPPORT_

// ------------------------------------------------------------------------------
// ChspsFileChangeListener::HandleEmuFileSystemChange
//
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListener::HandleEmuFileSystemChange()
    {
    InformObserver();
    }    
#endif

#ifdef HSPS_LOG_ACTIVE
// ------------------------------------------------------------------------------
// ChspsFileChangeListener::SetLogBus
// Set log bus to be used.
// ------------------------------------------------------------------------------
//
void ChspsFileChangeListener::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    
#ifdef _hsps_EMULATOR_SUPPORT_
    if( iFileChange )
        {
        iFileChange->SetLogBus( aLogBus );
        }
#endif

    }
#endif

// End of File
