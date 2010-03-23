/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: causifscanner.h
 *
 */

#ifndef CAUSIFSCANNER_H
#define CAUSIFSCANNER_H

#include <e32base.h>
#include <e32std.h>
#include <usif/sif/sifcommon.h>
#include <usif/scr/scr.h>

#include "casrvplugin.h"

class CCaStorageProxy;
class CCaInnerEntry;

using namespace Usif;

/*
 An implementation of the CCaSrvPlugin interface for the Reference Installer.

 The CaSrvManager loads and uses this plug-in when the MIME type of a package is 'binary/sif-refpkg'.
 */
NONSHARABLE_CLASS(CCaUsifScanner): public CCaSrvPlugin
    {
public:
    static CCaUsifScanner* NewL( TPluginParams* aPluginParams );
    ~CCaUsifScanner();

    void InstallL( const TDesC& aFileName );

private:
    CCaUsifScanner( CCaStorageProxy& aStorageProxy );
    void ConstructL();
    CCaUsifScanner( const CCaUsifScanner & );
    CCaUsifScanner& operator =( const CCaUsifScanner & );

    void AddCollectionUsifL();
    TInt GetCollectionUsifIdL();

    void CreateCaEntryFromEntryL( const CComponentEntry* aEntry,
            CCaInnerEntry* aCaEntry );

    void AddNativeAppsL();

    void GetCaAppEntriesL( RPointerArray<CCaInnerEntry>& aArray );

    TInt AppExist( RPointerArray<CCaInnerEntry>& aArray,
            CCaInnerEntry* aEntry );

private:
    /*
     * not own
     */
    CCaStorageProxy& iStorageProxy;

    /*
     * Usif Collection id. Temporary solution.
     */
    TInt iCollectionUsifId;
    };

#endif // CAUSIFSCANNER_H
