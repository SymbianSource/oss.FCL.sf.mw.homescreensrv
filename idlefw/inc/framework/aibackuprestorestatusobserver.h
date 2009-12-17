/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Backup / Restore status observer
*
*/


#ifndef C_AIBACKUPRESTORESTATUSOBSERVER_H
#define C_AIBACKUPRESTORESTATUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "aipsstatusobserver.h"

class MAiPSPropertyObserver;

/**
 * @ingroup group_aifw
 * 
 *  Publish and subscribe key observer
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiBackupRestoreStatusObserver ) : public CAiPSStatusObserver
    {
public:

// Construction

    static CAiBackupRestoreStatusObserver* NewL( MAiStateManager* aStateManager );

    virtual ~CAiBackupRestoreStatusObserver();

// functions from base class CAiPSStatusObserver

    TAiStateChanges Status();

private:

// Construction
    
    CAiBackupRestoreStatusObserver();
    
    void ConstructL( MAiStateManager* aStateManager );
    
// new functions

    static TInt HandleBackupOperationEvent( TAny* aPtr );
    
    };

#endif // C_AIBACKUPRESTORESTATUSOBSERVER_H

// End of File.
