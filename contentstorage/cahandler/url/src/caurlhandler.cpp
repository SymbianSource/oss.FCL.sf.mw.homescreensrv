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
 * Description:  ?Description
 *
 */

#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <AknTaskList.h>

#include "caurlhandler.h"
#include "cadef.h"
#include "cainnerentry.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CCaUrlHandler::~CCaUrlHandler
// --------------------------------------------------------------------------
//
CCaUrlHandler::~CCaUrlHandler()
    {
    }

// --------------------------------------------------------------------------
// CCaUrlHandler::NewL
// --------------------------------------------------------------------------
//
CCaUrlHandler* CCaUrlHandler::NewL()
    {
    CCaUrlHandler* handler = new ( ELeave ) CCaUrlHandler();
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// --------------------------------------------------------------------------
// CCaUrlHandler::CCaUrlHandler
// --------------------------------------------------------------------------
//
CCaUrlHandler::CCaUrlHandler()
    {
    }

// --------------------------------------------------------------------------
// CCaUrlHandler::ConstructL
// --------------------------------------------------------------------------
//
void CCaUrlHandler::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CCaUrlHandler::HandleCommandL
// --------------------------------------------------------------------------
//
void CCaUrlHandler::HandleCommandL( CCaInnerEntry& aEntry,
        const TDesC8& aCommand )
    {

    if( !aCommand.Compare( KCaCmdOpen() ) )
        {
        const RCaEntryAttrArray& attributes = aEntry.GetAttributes();
        TInt attributesArrayCount = attributes.Count();
        for( int i = 0; i < attributesArrayCount; ++i )
            {
            const CCaEntryAttribute * const attribute = attributes[i];
            if( attribute->Name().Compare( KCaAttrUrl ) == 0 )
                {
                LaunchUrlL( attribute->Value() );
                break;
                }
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

// --------------------------------------------------------------------------
// CCaUrlHandler::LaunchUrlL
// --------------------------------------------------------------------------
//

void CCaUrlHandler::LaunchUrlL( const TDesC& aUrl )
    {
    // Launches browser stand-alone.
    // Browser is launched with a parameter 4 ("4 http://...."), which 
    // Start/Continue the browser specifying a URL. 
    // Other available parameters are described in the Browser API Specification
    // Document.
    TInt length = aUrl.Length() + KBrowserPrefix.iTypeLength;

    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL<RWsSession> ( wsSession );

    CAknTaskList* taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( KUidBrowser );
    delete taskList;

    if( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( length );
        TPtr8 ptr8 = param8->Des();
        ptr8.Append( KBrowserPrefix );
        ptr8.Append( aUrl );

        // Sends message to existing Browser task.
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy( param8 );
        }
    else
        {
        HBufC* buf = HBufC::NewLC( length );
        TPtr ptr = buf->Des();
        ptr.Append( KBrowserPrefix );
        ptr.Append( aUrl );

        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() );
        CleanupClosePushL<RApaLsSession> ( appArcSession );
        TThreadId id;
        appArcSession.StartDocument( *buf, KUidBrowser, id );
        CleanupStack::PopAndDestroy( &appArcSession );
        CleanupStack::PopAndDestroy( buf );
        }

    CleanupStack::PopAndDestroy( &wsSession );
    }
