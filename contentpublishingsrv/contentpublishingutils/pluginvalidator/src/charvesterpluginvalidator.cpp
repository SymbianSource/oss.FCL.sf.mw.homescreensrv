/*
 * Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#include <e32property.h>
#include <ecom/ecom.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <contentharvesterplugin.h> // harvesting plugin
#include "charvesterpluginvalidator.h"
#include "cblacklisthandler.h"
#include "chplugindebug.h"

const TUint32 KInProgressPropertyKey =
    {
    0x1028000E
    };
const TUint32 KHarvesterUid =
    {
    0x10282E5A
    };

const TInt KADatFactorySettingsServerPluginUid(0x102830EF);

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CHarvesterPluginValidator* CHarvesterPluginValidator::NewL(
        TUid aUid, TAny *aParameter)
    {
    CHarvesterPluginValidator* self = CHarvesterPluginValidator::NewLC(aUid,
            aParameter);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CHarvesterPluginValidator* CHarvesterPluginValidator::NewLC(
        TUid aUid, TAny *aParameter)
    {
    CHarvesterPluginValidator* self = new (ELeave) CHarvesterPluginValidator(
            aUid, aParameter);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CHarvesterPluginValidator::~CHarvesterPluginValidator()
    {
    iInProgressProperty.Close();
    delete iBlacklist;
    delete iUpdateIdle;
    iUpdatePluginArray.Close();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CHarvesterPluginValidator::CHarvesterPluginValidator(TUid aUid,
        TAny* aParameter) :
    CPluginValidator(aUid, aParameter)
    {

    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::ConstructL()
    {
    iBlacklist = CBlacklistHandler::NewL();
    iUpdateIdle = CIdle::NewL( CActive::EPriorityIdle );
    CPluginValidator::ConstructL();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::ManagePluginsL()
    {
    TInt errorCode = iInProgressProperty.Define(TUid::Uid(KHarvesterUid),
            KInProgressPropertyKey, RProperty::EInt);

    if (KErrAlreadyExists == errorCode)
        {
        TInt value(-1);
        iInProgressProperty.Get(TUid::Uid(KHarvesterUid),
                KInProgressPropertyKey, value);
        if (value == 1)
            {
            // property value == inprogress
            // there was a panic in the previous startup
            // so we make unofficial blacklist official
            iBlacklist->CopyBlacklistL(EFalse);
            }
        }
    else
        {
        User::LeaveIfError(errorCode);
        }
    // copy blacklisted plugins to unoffical blacklist at startup
    iBlacklist->CopyBlacklistL(ETrue);

    // set property value to 1 (which means "in progress")
    iInProgressProperty.Set( TUid::Uid( KHarvesterUid ),
            KInProgressPropertyKey, 1 );
    CPluginValidator::ManagePluginsL();
    if( !iUpdateIdle->IsActive() )
        {
        iUpdateIdle->Start( TCallBack( UpdateCallback, this ) );
        }
    // set property value to 0 (which means "finished")
    iInProgressProperty.Set( TUid::Uid( KHarvesterUid ),
        KInProgressPropertyKey, 0 );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::LoadPluginL(TPluginInfo& aPluginInfo)
    {
    if (!iBlacklist->IsPresentL(aPluginInfo.iImplementationUid))
        {
        //first we append UID to the blacklist
        iBlacklist->AppendL(aPluginInfo.iImplementationUid);
        TAny* plug(NULL);
        TInt err(KErrNone);
        CPSPERF( ("CHarvesterPluginValidator::LoadPluginL Uid: 0x%X - START",
                aPluginInfo.iImplementationUid.iUid) );
            TRAP( err, plug = REComSession::CreateImplementationL(
                            aPluginInfo.iImplementationUid,
                            aPluginInfo.iDtor_ID_Key, iParameter ) );
        CPSPERF( ("CHarvesterPluginValidator::LoadPluginL - DONE") );
        if (err == KErrNone && plug)
            {
                TRAP_IGNORE(
                        CleanupStack::PushL( plug );
                        aPluginInfo.iPlugin = plug;
                        iPluginArray.AppendL( aPluginInfo );
                        if( aPluginInfo.iImplementationUid.iUid
                                == KADatFactorySettingsServerPluginUid)
                            iUpdatePluginArray.Insert( aPluginInfo, 0);
                        else
                            iUpdatePluginArray.AppendL( aPluginInfo );
                        CleanupStack::Pop( plug );
                );
            }
        //no panic during load so we can remove UID from blacklist
        iBlacklist->RemoveL(aPluginInfo.iImplementationUid);
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::UpdatePluginsL()
    {
    // set property value to 1 (which means "in progress")
    iInProgressProperty.Set( TUid::Uid( KHarvesterUid ),
            KInProgressPropertyKey, 1 );
    CContentHarvesterPlugin* plugin = NULL;

    while( iUpdatePluginArray.Count() )
        {
        plugin = static_cast<CContentHarvesterPlugin*> ( iUpdatePluginArray[0].iPlugin );
        //first we append UID to the blacklist
        iBlacklist->AppendL( iUpdatePluginArray[0].iImplementationUid );
        CPSPERF( ("CHarvesterPluginValidator::UpdatePluginsL Uid: 0x%X - START",
                            iUpdatePluginArray[0].iImplementationUid) );
        TRAP_IGNORE( plugin->UpdateL() );
        CPSPERF( ("CHarvesterPluginValidator::UpdatePluginsL - DONE") );
        //no panic during update so we can remove UID from blacklist
        iBlacklist->RemoveL( iUpdatePluginArray[0].iImplementationUid );
        // plugin was updated correctly so we remove it from update plugin array
        iUpdatePluginArray.Remove( 0 );
        }
    // set property value to 0 (which means "finished")
    iInProgressProperty.Set( TUid::Uid( KHarvesterUid ),
            KInProgressPropertyKey, 0 );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CHarvesterPluginValidator::UpdateCallback( TAny* aValidator )
    {
    if ( aValidator )
        {
        TRAP_IGNORE( static_cast<CHarvesterPluginValidator*>(aValidator)->
                UpdatePluginsL( ));
        }
    return KErrNone;
    }
