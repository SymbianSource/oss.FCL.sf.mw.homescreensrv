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
 * Description: CCaLocalizerScannerProxy.h
 *
 */

#ifndef CALOCALSCANNERPROXY_H
#define CALOCALSCANNERPROXY_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "hbtextresolversymbian.h"

#include "castorage_global.h"

// FORWARD DECLARATIONS
class CCaLocalizationEntry;
class CCaStorageProxy;
class CCaInnerEntry;

/**
 *  CCaLocalizerScannerProxy
 *
 */
class CCaLocalizerScannerProxy : public CBase
    {
    

public:
    

    /**
     * Two-phased constructor.
     */
    static CCaLocalizerScannerProxy* NewL( CCaStorageProxy* storageProxy );
    
    /**
     * Two-phased constructor.
     */
    static CCaLocalizerScannerProxy* NewLC( CCaStorageProxy* aStorageProxy );
    
    /**
     * Destructor.
     */
    ~CCaLocalizerScannerProxy( );

   
private:

    /**
     * Constructor.
     */
    CCaLocalizerScannerProxy( CCaStorageProxy* aStorageProxy );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );

private:
    /**
     * Get localization rows and updates database 
     */
    void UpdateLocalNamesL( );

    /**
     * Gets localization rows from database as table of CCaLocalizationEntries
     * @param aLocals pointers to localization entries
     */
    void GetLocalizationRowsL( RPointerArray<CCaLocalizationEntry>& aLocals );
    
    const TDesC& GetEntryText(
            RPointerArray<CCaInnerEntry> aEntries, TInt aId );
    /*
     * Not own
     */
    CCaStorageProxy* iStorageProxy;

    /*
     * Owned
     */
    HbTextResolverSymbian* iResolver;

    };

#endif // CALOCALSCANNERPROXY_H
