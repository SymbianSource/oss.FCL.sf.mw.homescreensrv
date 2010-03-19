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


#ifndef C_CCHFACTORYSETTINGS_H
#define C_CCHFACTORYSETTINGS_H

#include <contentharvesterplugin.h>
#include <f32file.h>
#include <badesca.h>

class MLiwInterface;
class CCHFactorySettingsInstallNotifier;
class CFileMan;
class CCHDbStateHandler;
class CCHFactorySettingsDOMParser; // mw

class CCHFactorySettings : public CContentHarvesterPlugin

    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aStorage instance of AS storage DB
     */
    static CCHFactorySettings* NewL( MLiwInterface* aInterface );
    static CCHFactorySettings* NewLC( MLiwInterface* aInterface );
    /**
     * C++ destructor
     */
    ~CCHFactorySettings();
private:

    /**
     * C++ default constructor.
     */
    CCHFactorySettings();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( MLiwInterface* aInterface );
    /**
     * CreateStorageL.
     */
    void CreateStorageL();
    /**
     * AddNewL.
     */
    void AddNewL();
    /**
     * UnInstallL.
     */
    void UnInstallL();
    /**
     * CheckDrivesL.
     */
    void CheckDrivesL();

public:
    // from CADatServerPlugin
    /**
     * This function updates all information in CPS related to FactorySettings
     */
    void UpdateL();

private:
    /**
     * Data base state handler
     */
	CCHDbStateHandler* iDBStateHandler;

    /**
     * Sis installation notifier
     */
    CCHFactorySettingsInstallNotifier* iInstallNotifier;

    /**
     *List containing xml files that should be installed from all drives
     */
    CDesCArrayFlat* iFileNewList;

    /**
     * File server session
     */
    RFs iFs;

    /**
     * File manager used for files copying
     */
    CFileMan* iFileMan;

    /**
     * An interface to Content Publisher Service
     */
    MLiwInterface* iCPSInterface;

    /**
     * Pointer to a parser object
     */
    CCHFactorySettingsDOMParser* iParser; //

    /**
     * Descriptor containing a path to a directory with files to parse
     */
    RBuf iFilePath;

    };

#endif      // C_CCHFACTORYSETTINGS_H
