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
 * Description:  ?Description
 *
 */

#ifndef CAPROGRESSCANER_H
#define CAPROGRESSCANER_H

#include <e32base.h>
#include <usif/sif/sifnotification.h>
#include <usif/usifcommon.h>
#include <usif/sif/sifcommon.h>
#include <usif/scr/scr.h>

#include "casrvplugin.h"
#include "castorage_global.h"

using namespace Usif; 

class CCaStorageProxy;
class CCaInnerEntry;

/**
 *  Uninstall Progress notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaProgresScanner ) : public CCaSrvPlugin, 
        public MSifOperationsHandler
    {

public:
    /**
     * Creates an instance of CCaProgresScanner.
     * @param aPLuginParam plugin param consist of storage, storage utils and scr.
     */
IMPORT_C static CCaProgresScanner* NewL( TPluginParams* aPLuginParam );

    /**
     * Destructor.
     */
    virtual ~CCaProgresScanner();

private:

    /**
     * Constructor.
     * @param aPLuginParam plugin param consist of storage, storage utils and scr.
     */
    CCaProgresScanner( TPluginParams* aPLuginParam );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL( );
    
    void UpdateProgressL( const CSifOperationProgressData& aProgressData );
    void EndOperationL(TInt aError);
    void MarkEntrysForUnistallL(TUint aKey, TComponentId aComponentId);

public:
    /**
     * From MSifOperationsHandler
     */
    void StartOperationHandler(TUint aKey, const CSifOperationStartData& aStartData);

    /**
     * From MSifOperationsHandler
     */
    void EndOperationHandler(const CSifOperationEndData& aEndData);

    /**
     * From MSifOperationsHandler
     */
    void ProgressOperationHandler(const CSifOperationProgressData& aProgressData);
    
private:

    /**
     * USIF notifier
     * Own.
     * 
     * Commented out since USIF notifications do not
     * work on MCL wk20
     */
    CSifOperationsNotifier* iNotifier;
    
    /**
     * Not Own.
     * Reference to the Storage Proxy.
     */
    CCaStorageProxy& iStorageProxy;
    
    /**
     * Reference to Software component registry. Not own.
     */
    RSoftwareComponentRegistry& iSoftwareRegistry;

    /**
     * All entries with componentID of uninstalling application
     */
    RPointerArray<CCaInnerEntry> iResultArrayItems;
    
    TUint iKey;
    
    CA_STORAGE_TEST_FRIEND_CLASS    (T_casrvProgresScaner)    
    };


#endif /* CAPROGRESSCANER_H */

// End of File

