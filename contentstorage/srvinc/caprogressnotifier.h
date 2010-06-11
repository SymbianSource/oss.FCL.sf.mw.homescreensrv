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
#include "castorage_global.h"
using namespace Usif; 

class CCaStorageProxy;

// needed for fake notifier
#include "cainstallnotifier.h"

/**
 * Interface for updating after installer events.
 */
class MCaFakeProgressListener
    {
public:
    /**
     * Pure virtual method.
     * @param aEvent event type.
     */
    virtual void HandleFakeProgressNotifyL(TInt aCurrentProgress) = 0;
    };

/**
 *  Fake Install notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaFakeProgressNotifier ) : public CActive
    {
public:

/**
     * Creates an instance of CCaFakeProgressNotifier.
     * @param aListener Reference to notifier interface.
     */
IMPORT_C static CCaFakeProgressNotifier* NewL( MCaFakeProgressListener& aListener);

    /**
     * Destructor.
     */
    virtual ~CCaFakeProgressNotifier();

    /**
     * Starts fake progress notifications.
     */
    void StartNotifying();
    
private:

    /**
     * Constructor.
     * @param aListener Reference to notifier interface.
     */
    CCaFakeProgressNotifier( MCaFakeProgressListener& aListener );

    /**
     * Symbian 2nd phase constructor.
    */
    void ConstructL( );

    /**
     * From CActive.
     */ 
    void DoCancel();

    /**
     * From CActive.
     */ 
    void RunL();

    /**
     * From CActive.
     */ 
    TInt RunError( TInt aError );

private:

    /**
     * Interface for fake notifications
     * Not Own.
     */
    MCaFakeProgressListener& iListener;
    
    /**
     * Timer
     */
    RTimer iTimer;
    
    /**
     * Counter for fake notifications
     */
    TInt iCount;

    };


/**
 *  Uninstall Progress notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaProgressNotifier ) : public CBase, 
        //public MSifOperationsHandler, // USIF not working yet
        public MCaInstallListener,      //needed for fake
        public MCaFakeProgressListener  //needed for fake
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
    CCaProgressNotifier( );

    /**
     * Symbian 2nd phase constructor.
     * @param aCaStorageProxy Reference to storage.
     */
    void ConstructL( CCaStorageProxy& aCaStorageProxy );

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
    
public:

    /**
     * From MCaInstallListener, needed for fake
     */
    void HandleInstallNotifyL( TInt aUid );
     
    /**
     * From MCaFakeProgressListener, needed for fake
     */
    void HandleFakeProgressNotifyL(TInt aCurrentProgress);
    
private:

    /**
     * USIF notifier
     * Own.
     * 
     * Commented out since USIF notifications do not
     * work on MCL wk20
     */
    //CSifOperationsNotifier* iNotifier;
    
    /**
     * Not Own.
     * Reference to the Storage Proxy.
     */
    CCaStorageProxy* iStorageProxy;

    /**
     * Uninstall notifier. Own. Needed for fake
     */
    CCaInstallNotifier *iUsifUninstallNotifier;

    /**
     * Install and uninstall notifier. Own. Needed for fake
     */
    CCaInstallNotifier *iJavaInstallNotifier;
    
    /**
     * Fake uninstall progress notifier. Own. Needed for fake
     */
    CCaFakeProgressNotifier *iFakeProgressNotifier;
    
    };


#endif /* CAPROGRESSNOTIFIER_H */

// End of File

