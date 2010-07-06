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

#ifndef CAPROGRESSNOTIFIER_H
#define CAPROGRESSNOTIFIER_H

#include <e32base.h>
#include <usif/sif/sifnotification.h>
#include <usif/usifcommon.h> 

#include "castorage_global.h"
using namespace Usif; 

class CCaStorageProxy;
class CCaInnerEntry;

/**
 *  Uninstall Progress notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaProgressNotifier ) : public CBase, 
        public MSifOperationsHandler
    {

public:
    /**
     * Creates an instance of CCaProgressNotifier.
     * @param aCaStorageProxy Reference to storage.
     */
IMPORT_C static CCaProgressNotifier* NewL( CCaStorageProxy& aCaStorageProxy);

    /**
     * Destructor.
     */
    virtual ~CCaProgressNotifier();

private:

    /**
     * Constructor.
     */
    CCaProgressNotifier( CCaStorageProxy& aCaStorageProxy );

    /**
     * Symbian 2nd phase constructor.
     * @param aCaStorageProxy Reference to storage.
     */
    void ConstructL( );
    
    void UpdateProgressL( const CSifOperationProgressData& aProgressData );
    void EndOperationL(TInt aError);
    TComponentId ComponentIdL(const TDesC& aGlobalComponentId, const TDesC& aSwType);
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
    
    RPointerArray<CCaInnerEntry> iResultArrayItems;
    
    TUint iKey;
    
    };


#endif /* CAPROGRESSNOTIFIER_H */

// End of File

