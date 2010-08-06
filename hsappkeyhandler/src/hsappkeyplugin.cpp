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

_LIT(KHsActivactionUri, "appto://20022F35?activityname=HsIdleView");
_LIT(KAppLibActivactionUri, "appto://20022F35?activityname=AppLibMainView");
_LIT(KTsPluginName, "com.nokia.taskswitcher.tsdevicedialogplugin/1.0");


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin::CHsAppKeyPlugin()
: iEikEnv( CEikonEnv::Static() )
{
}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin* CHsAppKeyPlugin::NewL()
{
    CHsAppKeyPlugin* self = new (ELeave) CHsAppKeyPlugin;
    return self;
}
    
// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CHsAppKeyPlugin::~CHsAppKeyPlugin()
{
    delete mDialog;
}

// ---------------------------------------------------------------------------
// From MHbDeviceDialogObserver
// 
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::DataReceived(CHbSymbianVariantMap& /*aData*/)
{
    //no implementation required
}

// ---------------------------------------------------------------------------
// From MHbDeviceDialogObserver
// 
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::DeviceDialogClosed(TInt /*aCompletionCode*/)
{
    delete mDialog;
    mDialog = 0;
}

// ---------------------------------------------------------------------------
// From CSysapKeyHandlerPlugin
// 
// ---------------------------------------------------------------------------
//
TKeyResponse CHsAppKeyPlugin::HandleKeyEventL( const TKeyEvent&  aKeyEvent , 
                                              TEventCode  /*aType*/ 
                                            )
{
    TKeyResponse retVal(EKeyWasNotConsumed);
    if (aKeyEvent.iCode == EKeyApplication0) {
        if (iSecondEvent) {
            if (aKeyEvent.iRepeats > 0)
                HandleLongPressL();
            else 
                HandleShortPressL();
            retVal = EKeyWasConsumed;
        }
        iSecondEvent =!iSecondEvent; 
    }
    return retVal;
}

// ---------------------------------------------------------------------------
// From CSysapKeyHandlerPlugin
// 
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::ProvideKeyEventsL(RArray<TKeyEvent>& aKeyEventArray) 
{
    TKeyEvent key;
    key.iCode = EKeyApplication0;
    key.iScanCode = EStdKeyApplication0;
    key.iModifiers = 0;
    key.iRepeats = 0;
    aKeyEventArray.Append(key);
    
    TKeyEvent longkey;
    longkey.iCode = EKeyApplication0;
    longkey.iScanCode = EStdKeyApplication0;
    longkey.iModifiers = 0;
    longkey.iRepeats = 1;
    aKeyEventArray.Append(longkey);

}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::HandleShortPressL()
{   
    RApaLsSession apaLsSession;
    CleanupClosePushL(apaLsSession);
    User::LeaveIfError(apaLsSession.Connect());
    CAfActivityLauncher *activityEnabler = 
        CAfActivityLauncher::NewLC(apaLsSession, 
                                   iEikEnv->WsSession());
    TInt state(0);
    RProperty::Get(KHsCategoryUid, KHsCategoryStateKey, state);
    if (state == EHomeScreenIdleState) {
        activityEnabler->launchActivityL(KAppLibActivactionUri);
    } else {
        activityEnabler->launchActivityL(KHsActivactionUri);
    }    
    CleanupStack::PopAndDestroy(activityEnabler);
    CleanupStack::PopAndDestroy(&apaLsSession);
    delete mDialog;
    mDialog =0;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHsAppKeyPlugin::HandleLongPressL()
{
    if( !mDialog ) {
        TInt value( 0 );
        RProperty::Get( TsProperty::KCategory, 
                    TsProperty::KVisibilityKey,
                    value );
        if(!value){
            mDialog = CHbDeviceDialogSymbian::NewL();
    
            CHbSymbianVariantMap* params = CHbSymbianVariantMap::NewL();
        if(KErrNone != mDialog->Show(KTsPluginName,*params,this)) {
            delete mDialog;
            mDialog = 0;
            }
        delete params;
        }
    }
}
//  End of File


