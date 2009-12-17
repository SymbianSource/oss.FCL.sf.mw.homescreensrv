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
* Description:  Backup/restore status observer for AI2
*
*/


#include <connect/sbdefs.h>
#include <aipspropertyobserver.h>
#include "aistatemanager.h"
#include "aibackuprestorestatusobserver.h"
#include "aifwpanic.h"
#include "debug.h"

const TUint KAI2BackupMask( conn::KBURPartTypeMask ^ conn::EBURNormal );

CAiBackupRestoreStatusObserver::CAiBackupRestoreStatusObserver()
    {
    }
    
CAiBackupRestoreStatusObserver::~CAiBackupRestoreStatusObserver()
    {
    }

CAiBackupRestoreStatusObserver* CAiBackupRestoreStatusObserver::NewL(
                                        MAiStateManager* aStateManager )
    {
    CAiBackupRestoreStatusObserver* self = new (ELeave) CAiBackupRestoreStatusObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiBackupRestoreStatusObserver::ConstructL( MAiStateManager* aStateManager )
    {
    BaseConstructL( TCallBack( HandleBackupOperationEvent, this ),
                    KUidSystemCategory,
                    conn::KUidBackupRestoreKey,
                    aStateManager );
    }
    
TAiStateChanges CAiBackupRestoreStatusObserver::Status()
    {
    TInt value = 0;
    TInt err = iObserver->Get( value );
    if( ( value & KAI2BackupMask ) &&
        ( err == KErrNone ) ) // any type of backup or restore operation
        {
        return ESMAIBackupOn;
        }
    else
        {
        return ESMAIBackupOff;
        } 
    }

TInt CAiBackupRestoreStatusObserver::HandleBackupOperationEvent( TAny* aPtr )
    {
    // see \epoc32\include\connect\sbdefs.h for enum descriptions
    CAiBackupRestoreStatusObserver* self =
                static_cast<CAiBackupRestoreStatusObserver*>( aPtr );
    
    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
    
    TInt value = 0;
    TInt err = self->iObserver->Get( value );

    if( ( value & KAI2BackupMask ) &&
        ( err == KErrNone ) ) // any type of backup or restore operation
        {
        __PRINTS("XAI: Backup = ON");
        self->iStateManager->ReportStateChange( ESMAIBackupOn );
        }
    else // aValue == conn::EBURUnset || aValue & conn::EBURNormal
        {
        // back operation finished -> return to previous state
        __PRINTS("XAI: Backup = OFF");
        self->iStateManager->ReportStateChange( ESMAIBackupOff );
        }
    return KErrNone;
    }

