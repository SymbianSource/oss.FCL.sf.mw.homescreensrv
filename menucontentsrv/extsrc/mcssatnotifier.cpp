/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


// INCLUDES
#include "mcssatnotifier.h"

// ============================ MEMBER FUNCTIONS =============================
// -----------------------------------------------------------------------------
// CMcsSatNotifier::CMcsSatNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMcsSatNotifier::CMcsSatNotifier( MMcsSATNotifierCallback* aCallback, 
                                    TUid aCategory, 
                                    TUint aKey ) : CActive( EPriorityNormal )
    {    
    CActiveScheduler::Add( this );
    
    iCallback = aCallback;
    iCategory = aCategory;
    iKey = aKey;
    }

// -----------------------------------------------------------------------------
// CMcsSatNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMcsSatNotifier::ConstructL()
    {
    // Prepare automatically
    iProperty.Attach( iCategory, iKey );
    SetActive();
    iStatus = KRequestPending;
    iProperty.Subscribe( iStatus );
    }

// ---------------------------------------------------------------------------
// CMcsSatNotifier::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMcsSatNotifier* CMcsSatNotifier::NewL( MMcsSATNotifierCallback* aCallback,
                                          TUid aCategory, 
                                          TUint aKey  )
    {
    CMcsSatNotifier* self = new (ELeave) CMcsSatNotifier( aCallback,
                                                            aCategory,
                                                            aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// CMcsSatNotifier::~CMcsSatNotifier
// ---------------------------------------------------------------------------
//

CMcsSatNotifier::~CMcsSatNotifier()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CMcsSatNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CMcsSatNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CMcsSatNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CMcsSatNotifier::RunError( TInt /*aError*/ )
    {
    // Prime the active object
    SetActive();
    iStatus = KRequestPending;
    iProperty.Subscribe( iStatus );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMcsSatNotifier::RunL
// ---------------------------------------------------------------------------
//
void CMcsSatNotifier::RunL()
    {
    // Filter out cancel status
    if( iStatus != KErrCancel )
        {
        // Re-issue request before notifying
        SetActive();
        iStatus = KRequestPending;
        iProperty.Subscribe( iStatus );
        iCallback->SATChangeL();
        }
    else
        {
        // For PC-Lint
        }
    }
// End of File
