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
#include <driveinfo.h>
#include <QObject>

#include "castorage_global.h"

// FORWARD DECLARATIONS
class CCaLocalizationEntry;
class QTranslator;
class CCaStorageProxy;


/**
 *  CCaLocalizerScannerProxy
 *
 */
class CCaLocalizerScannerProxy : public CBase
    {
    
//CA_STORAGE_TEST_FRIEND_CLASS    (TestCaLocalizerScanner)

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

    /**
     * Performs whole localization of database, can be called at any moment
     */
    void PerformL( );
    
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
     * Load translations to mTranslator
     * @param aFilename file name with translations
     * @return success or failure
     */   
    TBool LoadTranslator( QString aFilename );

    /**
     * Gets localization rows from database as table of CCaLocalizationEntries
     * @param aLocals pointers to localization entries
     */
    void GetLocalizationRowsL( RPointerArray<CCaLocalizationEntry>& aLocals );
    
    /**
     * Converts descriptor to const char*
     * @param aDescriptor descriptor with text to convert
     * @return pointer to const char containing text
     */
    const char* DescriptorToStringL( const TDesC& aDescriptor );

    /*
     * Not own
     */
    CCaStorageProxy* iStorageProxy;

    /*
     * Own
     */    
    QTranslator* iTranslator;

    //CA_STORAGE_TEST_FRIEND_CLASS    (TestCaLocalizerScanner)
    };

#endif // CALOCALSCANNERPROXY_H
