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
* Description:
*
*/
#include "tsbacksteppingfilter.h"

const TInt KBlockedApps [] = {0x100058b3/* <-- phoneui.exe */,
                              0x2001843A/* <-- javainstaller */};

// -----------------------------------------------------------------------------
/** 
 * CTsBacksteppingFilter::NewL
 */
CTsBacksteppingFilter* CTsBacksteppingFilter::NewL()
    {
    CTsBacksteppingFilter *self = new(ELeave)CTsBacksteppingFilter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBacksteppingFilter::~CTsBacksteppingFilter
 */
CTsBacksteppingFilter::~CTsBacksteppingFilter()
    {
    iBlockedApps.Close();
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBacksteppingFilter::isBlocked
 */
TBool CTsBacksteppingFilter::isBlocked( TUid aApplicationUid ) const
    {
    return KErrNotFound != iBlockedApps.Find( aApplicationUid );
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBacksteppingFilter::CTsBacksteppingFilter
 */
CTsBacksteppingFilter::CTsBacksteppingFilter()
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBacksteppingFilter::ConstructL
 */
void CTsBacksteppingFilter::ConstructL()
    {
    const TInt count(sizeof( KBlockedApps ) / sizeof(TInt)); 
    for( TInt offset(0);offset < count; ++offset )
        {
        iBlockedApps.AppendL(TUid::Uid(KBlockedApps[offset]));
        }
    }
