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
* Description:
 *
*/

// INCLUDE FILES

#include <ecom/implementationproxy.h>
#include <sacls.h>

#include "cawidgetscannerplugin.h"
#include "cawidgetscannerparser.h"
#include "cawidgetstoragehandler.h"
#include "cawidgetscannerinstallnotifier.h"


// Function used to return an instance of the proxy table.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x20028707, CCaWidgetScannerPlugin::NewL)
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy*
ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) /
    sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCaWidgetScannerPlugin ::CCaWidgetScannerPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerPlugin::CCaWidgetScannerPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerPlugin ::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaWidgetScannerPlugin::ConstructL( TPluginParams* aPluginParams )
    {
    User::LeaveIfError( iFs.Connect() );
    iMmcWatcher = CCaMmcWatcher::NewL( iFs, this );
    iInstallNotifier = CCaWidgetScannerInstallNotifier::NewL( this,
        KUidSystemCategory, KSAUidSoftwareInstallKeyValue );
    iParser = CCaWidgetScannerParser::NewL( iFs );
    iStorageHandler = CCaWidgetStorageHandler::NewL(
            aPluginParams->storageProxy, iFs );
    SynchronizeL();
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerPlugin ::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerPlugin * CCaWidgetScannerPlugin::NewL(
        TPluginParams* aPluginParams )
    {
    CCaWidgetScannerPlugin * self = NewLC( aPluginParams );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerPlugin ::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetScannerPlugin * CCaWidgetScannerPlugin::NewLC(
        TPluginParams* aPluginParams )
    {
    CCaWidgetScannerPlugin * self = new( ELeave ) CCaWidgetScannerPlugin ( );
    CleanupStack::PushL( self );
    self->ConstructL( aPluginParams );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCaWidgetScannerPlugin::~CCaWidgetScannerPlugin ()
    {
    delete iMmcWatcher;
    delete iInstallNotifier;
    delete iParser;
    delete iStorageHandler;
    iFs.Close();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerPlugin::SynchronizeL()
    {
    iStorageHandler->SynchronizeL( iParser->WidgetsScanL() );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetScannerPlugin::MmcChangeL()
    {
    SynchronizeL();
    }

// end of file
