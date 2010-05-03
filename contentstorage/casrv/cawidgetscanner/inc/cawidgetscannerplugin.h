/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Ecom plugin
 *
*/


#ifndef C_CAWIDGETSCANNERPLUGIN_H
#define C_CAWIDGETSCANNERPLUGIN_H

#include <casrvplugin.h>
#include <f32file.h>
#include "cawidgetdescription.h"
#include "cammcwatcher.h"

class TPluginParams;
class CCaWidgetScannerParser;
class CCaWidgetScannerInstallNotifier;
class CCaWidgetStorageHandler;

class CCaWidgetScannerPlugin : public CCaSrvPlugin, MMmcWatcherCallback
    {

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aStorage instance of AS storage DB
     */
    static CCaWidgetScannerPlugin* NewL( TPluginParams* aPluginParams );
    static CCaWidgetScannerPlugin* NewLC( TPluginParams* aPluginParams );
    /**
     * C++ destructor
     */
    ~CCaWidgetScannerPlugin();

    /**
     * SynchronizeL
     */
    void SynchronizeL();

private:
//from MWidgetMmcWatcherCallback

    /**
     * MmcChangeL is called when the MMC is removed or inserted.
     */
    void MmcChangeL();

private:

    /**
     * C++ default constructor.
     */
    CCaWidgetScannerPlugin( );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TPluginParams* aPluginParams );

private:

    /**
     * File session. Own
     */
    RFs iFs;

    /**
     * Sis installation notifier. Own
     */
    CCaWidgetScannerInstallNotifier* iInstallNotifier;

    /**
     * Pointer to a parser object. Own
     */
    CCaWidgetScannerParser* iParser;

    /**
     * Content Storage Handler. Own.
     */
    CCaWidgetStorageHandler* iStorageHandler;

    /**
     * Mmc watcher. Own.
     */
    CCaMmcWatcher* iMmcWatcher;

    };

#endif      // C_CAWIDGETSCANNERPLUGIN_H
