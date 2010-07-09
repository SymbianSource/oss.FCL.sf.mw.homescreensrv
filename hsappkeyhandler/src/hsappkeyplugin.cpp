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
#include "hsappkeyplugin.h"

const TUid KHSAppUid  = { 0x20022F35 };
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
    RWsSession& ws = iEikEnv->WsSession();
    TApaTaskList appList(ws);
    TApaTask task = appList.FindApp(KHSAppUid);

    if (task.Exists()) {
        task.BringToForeground();
    } else {        
        RApaLsSession apaLsSession;
        User::LeaveIfError(apaLsSession.Connect());
        CleanupClosePushL(apaLsSession);
        
        TApaAppInfo appInfo;
        apaLsSession.GetAppInfo(appInfo, KHSAppUid );
        
        CApaCommandLine *cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo.iFullName);
        User::LeaveIfError( apaLsSession.StartApp(*cmdLine));
    
        CleanupStack::PopAndDestroy(cmdLine);
        CleanupStack::PopAndDestroy(&apaLsSession);
    }
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


