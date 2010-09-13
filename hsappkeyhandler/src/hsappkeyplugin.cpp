/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main plugin class
*
*/

#include <w32std.h>
#include <eikenv.h>
#include <apgtask.h>
#include <apgcli.h>
#include <e32property.h>
#include <hb/hbcore/hbdevicedialogsymbian.h>
#include <hb/hbcore/hbsymbianvariant.h>
#include <tspropertydefs.h>
#include <afactivitylauncher.h>
#include <homescreendomainpskeys.h>
#include "hsappkeyplugin.h"

_LIT( KHsActivactionUri, "appto://20022F35?activityname=HsIdleView" );
_LIT( KAppLibActivactionUri, "appto://20022F35?activityname=AppLibMainView" );
_LIT( KTsPluginName, "com.nokia.taskswitcher.tsdevicedialogplugin/1.0" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin::CHsAppKeyPlugin()
    : iEikEnv( CEikonEnv::Static() ) // codescanner::eikonenvstatic
    {
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin* CHsAppKeyPlugin::NewL()
    {
    CHsAppKeyPlugin* self = new ( ELeave ) CHsAppKeyPlugin;
    return self;
    }

// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin::~CHsAppKeyPlugin()
    {
    delete iDialog;
    }

// ---------------------------------------------------------------------------
// From MHbDeviceDialogObserver
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::DataReceived( CHbSymbianVariantMap& /*aData*/ )
    {
    //no implementation required
    }

// ---------------------------------------------------------------------------
// From MHbDeviceDialogObserver
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::DeviceDialogClosed( TInt /*aCompletionCode*/ )
    {
    delete iDialog;
    iDialog = 0;
    }

// ---------------------------------------------------------------------------
// From CSysapKeyHandlerPlugin
//
// ---------------------------------------------------------------------------
//
TKeyResponse CHsAppKeyPlugin::HandleKeyEventL( const TKeyEvent& aKeyEvent,  
                                               TEventCode /*aType*/ )
    {
    TKeyResponse retVal( EKeyWasNotConsumed );
    if ( aKeyEvent.iCode == EKeyApplication0 )
        {
        if ( aKeyEvent.iRepeats > 0 )
            {
            HandleLongPressL();
            }
        else
            {
            HandleShortPressL();
            }
        retVal = EKeyWasConsumed;
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// From CSysapKeyHandlerPlugin
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::ProvideKeyEventsL( RArray<TKeyEvent>& aKeyEventArray ) // codescanner::LFunctionCantLeave
    {
    TKeyEvent key;
    key.iCode = EKeyApplication0;
    key.iScanCode = EStdKeyApplication0;
    key.iModifiers = 0;
    key.iRepeats = 0;
    aKeyEventArray.Append( key );

    TKeyEvent longkey;
    longkey.iCode = EKeyApplication0;
    longkey.iScanCode = EStdKeyApplication0;
    longkey.iModifiers = 0;
    longkey.iRepeats = 1;
    aKeyEventArray.Append( longkey );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::HandleShortPressL()
    {
    TInt value( 0 );
    RProperty::Get( TsProperty::KCategory, TsProperty::KVisibilityKey, value );
    if ( value )
        {
        // when Task Switcher dialog is visible, Appkey is used to dismiss it
        DismissTaskSwitcher();
        }
    else
        {
        // when there is no Task Switcher dialog, Appkey is used to switch between Applib and HomeScreen
        RApaLsSession apaLsSession;
        CleanupClosePushL( apaLsSession );
        User::LeaveIfError( apaLsSession.Connect() );
        CAfActivityLauncher* activityEnabler =
            CAfActivityLauncher::NewLC( apaLsSession,
                                        iEikEnv->WsSession() );
        TInt state( 0 );
        RProperty::Get( KHsCategoryUid, KHsCategoryStateKey, state );
        if ( state == EHomeScreenWidgetViewForeground )
            {
            activityEnabler->launchActivityL( KAppLibActivactionUri );
            }
        else
            {
            activityEnabler->launchActivityL( KHsActivactionUri );
            }
        CleanupStack::PopAndDestroy( activityEnabler );
        CleanupStack::PopAndDestroy( &apaLsSession );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::HandleLongPressL()
    {
    TInt tsVisible( 0 );
    RProperty::Get( TsProperty::KCategory,
                    TsProperty::KVisibilityKey,
                    tsVisible );
    if ( tsVisible ) 
        {
        DismissTaskSwitcher();
        } 
    else 
        {
        if ( !iDialog )
            {
            iDialog = CHbDeviceDialogSymbian::NewL();
            CHbSymbianVariantMap* params = CHbSymbianVariantMap::NewL();
            if ( KErrNone != iDialog->Show( KTsPluginName, *params, this ) )
                {
                delete iDialog;
                iDialog = 0;
                }
            delete params;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::DismissTaskSwitcher()
    {
    // @todo: remove notification through property when SharedDialog flag
    // is implemented in Orbit.
    if ( !iDialog )
        {
        // dismiss dialog launched from HomeScreen
        User::LeaveIfError( RProperty::Set( TsProperty::KCategory, TsProperty::KDismissRequestKey, 1 ) );
        }
    else
        {
        // dismiss dialog launched by this plugin
        delete iDialog;
        iDialog = 0;
        }
    }
