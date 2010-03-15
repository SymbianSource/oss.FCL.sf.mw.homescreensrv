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
* Description:  App for Action handler
 *
*/


#include <apgcli.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <vwsdef.h>
#include <eikenv.h>
#include <eikappui.h>
#include <liwvariant.h>

#include "ahproperties.hrh"
#include "ahpapplauncher.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CAHAppLauncher::ConstructL()
    {
    iEnv = CEikonEnv::Static( );
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHAppLauncher* CAHAppLauncher::NewL()
    {
    CAHAppLauncher* self = CAHAppLauncher::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHAppLauncher* CAHAppLauncher::NewLC()
    {
    CAHAppLauncher* self = new( ELeave ) CAHAppLauncher;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CAHAppLauncher::~CAHAppLauncher()
    {
    }

// ---------------------------------------------------------------------------
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExecuteTypeLaunchL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    RBuf launchMethod;
    CleanupClosePushL( launchMethod );
    
    if ( !ExtractDesL( aMap, launchMethod, KLaunchMethod ) )
        {
        if ( !launchMethod.CompareF( KLaunchMethodValueCmdLine ) )
            {
            errCode = ExecuteCommmandLineL( aMap );
            }
        else if ( !launchMethod.CompareF( KLaunchMethodValueMessageWithDoc )
                || !launchMethod.CompareF( KLaunchMethodValueMessageWithTail ) )
            {
            errCode = ExecuteApaMessageL( aMap );
            }
        }
        
    CleanupStack::PopAndDestroy( &launchMethod );       
    return errCode;
    }

// ---------------------------------------------------------------------------
// Executes provided command line action
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExecuteCommmandLineL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TUid appUid= TUid::Null( );

    if ( !ExtractUidL( aMap, appUid, KApplicationUid ) )
        {
        TApaTaskList taskList( iEnv->WsSession() );       
        TApaTask task = taskList.FindApp( appUid );
        if ( task.Exists( ) )
            {
            errCode = KErrNone;
            task.BringToForeground();
            }
        else
            {
            // app not yet running
            RApaLsSession appSession;
            CleanupClosePushL( appSession );
            User::LeaveIfError( appSession.Connect( ) );
    
            CApaCommandLine* cmd = CApaCommandLine::NewLC( );
            cmd->SetCommandL( GetCommandL( aMap ) );
            RBuf documentNameValue;
            CleanupClosePushL( documentNameValue );
            if( !ExtractDesL( aMap, documentNameValue, KDocumentName ) )
                {
                cmd->SetDocumentNameL( documentNameValue );
                }
            TApaAppInfo appInfo;
            appSession.GetAppInfo( appInfo, appUid );
            cmd->SetExecutableNameL( appInfo.iFullName );
            errCode = appSession.StartApp( *cmd );
    
            CleanupStack::PopAndDestroy( &documentNameValue );
            CleanupStack::PopAndDestroy( cmd );
            CleanupStack::PopAndDestroy( &appSession );
            }
        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// Executes provided apa message action
// ---------------------------------------------------------------------------
// 
TInt CAHAppLauncher::ExecuteApaMessageL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TUid appUid= TUid::Null( );
    if ( !ExtractUidL( aMap, appUid, KApplicationUid ) )
        {
        TApaTaskList taskList( iEnv->WsSession() );       
        TApaTask task = taskList.FindApp( appUid );
        if ( task.Exists( ) )
            {
            TUid messageUid= TUid::Null( );
            
            RBuf8 additionalData;
            CleanupClosePushL( additionalData );
            if ( !ExtractUidL( aMap, messageUid, KMessageUid )
                    && !ExtractDes8L( aMap, additionalData, KAdditionalData ) )
                {
                errCode = task.SendMessage( messageUid, additionalData );
                }
            CleanupStack::PopAndDestroy( &additionalData );    
            }
        else
            { // app not yet running
            RBuf launchMethod;
            CleanupClosePushL( launchMethod );
            if ( !ExtractDesL( aMap, launchMethod, KLaunchMethod ) )
                {
                if ( !launchMethod.CompareF( KLaunchMethodValueMessageWithDoc ) )
                    {
                    errCode = StartDocumentL( aMap ) ;
                    }
                else if ( !launchMethod.CompareF( KLaunchMethodValueMessageWithTail ) )
                    {
                    errCode = StartAppL( aMap );
                    }
                }
            CleanupStack::PopAndDestroy( &launchMethod );     
            }
        }
    return errCode;

    }
// ---------------------------------------------------------------------------
// Start document
// ---------------------------------------------------------------------------
// 
TInt CAHAppLauncher::StartDocumentL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TUid appUid= TUid::Null( );
    RBuf documentNameValue;
    CleanupClosePushL( documentNameValue );
    if ( !ExtractUidL( aMap, appUid, KApplicationUid ) 
        && !ExtractDesL( aMap, documentNameValue, KDocumentName ) )
        {
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        User::LeaveIfError( appArcSession.Connect( ) );
        TThreadId id;
        errCode = appArcSession.StartDocument( documentNameValue, appUid, id );
        CleanupStack::PopAndDestroy( &appArcSession );
        }
    CleanupStack::PopAndDestroy( &documentNameValue );
    return errCode;
    }
// ---------------------------------------------------------------------------
// Starts application
// ---------------------------------------------------------------------------
// 
TInt CAHAppLauncher::StartAppL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TUid appUid= TUid::Null( );
    RBuf8 additionalData;
    CleanupClosePushL( additionalData );
    if ( !ExtractUidL( aMap, appUid, KApplicationUid )
        && !ExtractDes8L( aMap, additionalData, KAdditionalData ) )
        {
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        User::LeaveIfError( appArcSession.Connect( ) );
        TApaAppInfo appInfo;
        errCode = appArcSession.GetAppInfo( appInfo, appUid );
        if ( errCode == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC( );
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            cmdLine->SetTailEndL( additionalData );

            errCode = appArcSession.StartApp( *cmdLine );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        CleanupStack::PopAndDestroy( &appArcSession );
        }
    CleanupStack::PopAndDestroy( &additionalData );
    return errCode;
    }
// ----------------------------------------------------------------------------
// Executes provided action - activate view
// ----------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExecuteActivateViewL( const CLiwMap* aMap )
    {
    TInt errCode(KErrGeneral);
    if ( iEnv )
        {
        CCoeAppUi* appui = iEnv-> EikAppUi( );
        if ( appui )
            {
            TUid messageUid= TUid::Null( );
            TVwsViewId viewId;
            if ( !ExtractViewIdL( aMap, viewId ) )
                {
                RBuf8 additionalData;
                CleanupClosePushL( additionalData );
                if ( !ExtractUidL( aMap, messageUid, KMessageUid )
                    && !ExtractDes8L( aMap, additionalData, KAdditionalData ) )
                    {
                    appui->ActivateViewL( viewId, messageUid, additionalData );
                    }
                else
                    {
                    appui->ActivateViewL( viewId );
                    }
                errCode = KErrNone;
                CleanupStack::PopAndDestroy( &additionalData );
                }
            }
        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt errCode(KErrNotFound);
    RBuf type;
    CleanupClosePushL( type );
    errCode = ExtractDesL( aMap, type, KType );

    if ( errCode == KErrNone )
        {
        if ( !type.CompareF( KActionValueLaunchApplication ) )
            {
            errCode = ExecuteTypeLaunchL( aMap );
            }
        else if ( !type.CompareF( KActionValueViewActivation ) )
            {
            errCode = ExecuteActivateViewL( aMap );
            }
        }
    CleanupStack::PopAndDestroy( &type );
    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExtractDesL( const CLiwMap* aMap,
    RBuf& aString, const TDesC8& aMapName )
    {
    TInt errCode(KErrNotFound);
    TLiwVariant variant;
    variant.PushL( );
    TPtrC tempString( KNullDesC );
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( tempString );
        aString.ReAllocL( tempString.Length( ) );
        aString.Append( tempString );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );    
    return errCode;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExtractDes8L( const CLiwMap* aMap,
    RBuf8& aString, const TDesC8& aMapName )
    {
    TInt errCode(KErrNotFound);
    TLiwVariant variant;
    variant.PushL( );
    TPtrC8 tempString( KNullDesC8 );
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( tempString );
        aString.ReAllocL( tempString.Length( ) );
        aString.Append( tempString );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );    
    return errCode;
    }    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExtractUidL( const CLiwMap* aMap, TUid& aUid,
    const TDesC8& aMapName )

    {
    TInt errCode(KErrNotFound);
    TInt32 temp;
    TLiwVariant variant;
    variant.PushL( );
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( temp );
        aUid = TUid::Uid( temp );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );
    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHAppLauncher::ExtractViewIdL( const CLiwMap* aMap, TVwsViewId& aViewId )

    {
    TInt errCode(KErrNotFound);
    TLiwVariant variant;
    if ( aMap->FindL( KViewId, variant ) )
        {
        variant.Get( aViewId.iViewUid.iUid );
        variant.Reset( );
        if ( aMap->FindL( KViewAppUid, variant ) )
            {
            variant.Get( aViewId.iAppUid.iUid );
            variant.Reset( );
            errCode = KErrNone;
            }
        }

    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TApaCommand CAHAppLauncher::GetCommandL( const CLiwMap* aMap )
    {
    TApaCommand command(EApaCommandOpen);
    RBuf appCommand;
    CleanupClosePushL( appCommand );
    if ( ExtractDesL( aMap, appCommand, KApaCommand ) == KErrNone )
        {
        if( appCommand.Compare( KApaCommandBackground ) == KErrNone )
            {
            command = EApaCommandBackground;
            }
        }
    CleanupStack::PopAndDestroy( &appCommand );
    return command;
    }

// End of file   
