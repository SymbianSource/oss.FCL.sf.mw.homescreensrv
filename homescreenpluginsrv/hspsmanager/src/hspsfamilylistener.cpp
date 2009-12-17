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
* Description:  FamilyListener listens to resolution&orientation
*               changes in emulator environment.
*
*/

#include "hspsfamilylistener.h"
#include "hsps_builds_cfg.hrh"
#include "hspsmanifest.h"
#include <featmgr.h>


_LIT8(KTch, "_tch");
const TInt KMaxFamilyLength( 20 );


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// ChspsFamilyListener::NewL
// -----------------------------------------------------------------------------
//
ChspsFamilyListener* ChspsFamilyListener::NewL( MhspsFamilyObserver& aObserver )
    {
    ChspsFamilyListener* self = new(ELeave) ChspsFamilyListener( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsFamilyListener::ChspsFamilyListener
// -----------------------------------------------------------------------------
//
ChspsFamilyListener::ChspsFamilyListener( MhspsFamilyObserver& aObserver ) 
    : CActive( EPriorityStandard ), iObserver( aObserver), iActiveFamily( EhspsFamilyUnknown )    
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// ChspsFamilyListener::ConstructL
// -----------------------------------------------------------------------------
//
void ChspsFamilyListener::ConstructL()
    {    
    User::LeaveIfError( iWsSession.Connect() );
    
    // A group needs to be instansiated so that we're able to receive events 
    iWindowGroup = RWindowGroup( iWsSession );
    User::LeaveIfError( iWindowGroup.Construct(2,ETrue) ); // '2' is a meaningless handle
    
    // Enables for EEventScreenDeviceChanged events
    iWindowGroup.EnableScreenChangeEvents(); 
            
    iScreenDevice = new (ELeave) CWsScreenDevice( iWsSession );
    User::LeaveIfError( iScreenDevice->Construct() );  
    
    // Start the listener
    Queue();    
    }

// ------------------------------------------------------------------------------
// ChspsFamilyListener::Queue
// ------------------------------------------------------------------------------
void ChspsFamilyListener::Queue()
    {
    ASSERT ( !IsActive() );
    iWsSession.EventReady( &iStatus );
    SetActive();
    }

// ------------------------------------------------------------------------------
// ChspsFamilyListener::~ChspsFamilyListener
// ------------------------------------------------------------------------------
ChspsFamilyListener::~ChspsFamilyListener()
    {
    Cancel();
    if ( iFeatureManagerLoaded )
        {
        FeatureManager::UnInitializeLib();
        }
    delete iScreenDevice;
    iWindowGroup.Close();
    iWsSession.Close();    
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::GetFamilyString 
// -----------------------------------------------------------------------------
void ChspsFamilyListener::GetFamilyString(        
        TDes8& aFamily )
    {                     
    // Append input with a prefix based on the active screen resolution    
    TPixelsTwipsAndRotation sizeAndRotation;
    iScreenDevice->GetDefaultScreenSizeAndRotation( sizeAndRotation );
    TSize resolution( sizeAndRotation.iPixelSize );  
    if( resolution.iWidth > resolution.iHeight )
        {
        TInt temp = resolution.iHeight;
        resolution.iHeight = resolution.iWidth;
        resolution.iWidth = temp;        
        }          
    switch( resolution.iHeight )
        {
        case 320:
            {
            if ( resolution.iWidth == 240 )
                {
                aFamily.Append( KFamilyQvga );
                }
            }
            break;
        case 640:
            {
            if( resolution.iWidth == 360 )
                {
                aFamily.Append( KFamilyQhd );
                }
            else if( resolution.iWidth == 480 )
                {
                aFamily.Append( KFamilyVga );
                }            
            }
            break;
        
        default:
            break;
        }    
    if( aFamily.Length() > 0 )
        {                     
//        // Append input with a suffix based on the touch support
//        if ( !iFeatureManagerLoaded )
//            {
//            FeatureManager::InitializeLibL();
//            iFeatureManagerLoaded = ETrue;
//            }
//        if ( FeatureManager::FeatureSupported( KFeatureIdPenSupport ) )
//            {
            aFamily.Append( KTch );
//            }                
        }
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::GetFamilyType 
// -----------------------------------------------------------------------------
ThspsFamily ChspsFamilyListener::GetFamilyType( 
        const TDesC8& aFamilyString )
    {
    ThspsFamily family( EhspsFamilyUnknown );
    
    if( aFamilyString == KFamilyQvga )
       {
       family = EhspsFamilyQvga;
       }
    else if( aFamilyString == KFamilyQvga2 )
       {
       family = EhspsFamilyQvga2;
       }
    else if( aFamilyString == KFamilyVga )
       {
       family = EhspsFamilyVga;            
       }
    else if( aFamilyString == KFamilyVga3 )
       {
       family = EhspsFamilyVga3;
       }
    else if( aFamilyString == KFamilyQhd )
       {
       family = EhspsFamilyQhd;
       }
    else if( aFamilyString == KFamilyQhd_tch )
       {
       family = EhspsFamilyQhd_tch;
       }
   else if( aFamilyString == KFamilyVga_tch )
       {
       family = EhspsFamilyVga_tch;
       }    
   
    return family;     
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::GetFamilyType
// -----------------------------------------------------------------------------
ThspsFamily ChspsFamilyListener::GetFamilyType()
    {
    TBuf8<KMaxFamilyLength> familyString;
    GetFamilyString( familyString );   
    return GetFamilyType( familyString );
    }

// -----------------------------------------------------------------------------
// ChspsFileChangeListener::RunL 
// -----------------------------------------------------------------------------
void ChspsFamilyListener::RunL()
    {
    TWsEvent wsEvent;
    iWsSession.GetEvent(wsEvent);                                   
    switch( wsEvent.Type() )
        {
        case EEventScreenDeviceChanged:
            {                    
            ThspsFamily newFamily = GetFamilyType();
            if ( newFamily > EhspsFamilyUnknown )
                {                
                iObserver.HandleFamilyChangeL( newFamily );
                iActiveFamily = newFamily;
                }
            break;
            }
        default:
            break;
    }
       
    Queue();    
    }
 
// -----------------------------------------------------------------------------
// ChspsFileChangeListener::DoCancel
// -----------------------------------------------------------------------------
//
void ChspsFamilyListener::DoCancel()
    {
    iWsSession.EventReadyCancel();
    }
 
// -----------------------------------------------------------------------------
// ChspsFileChangeListener::RunError
// -----------------------------------------------------------------------------
TInt ChspsFamilyListener::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// End of File

