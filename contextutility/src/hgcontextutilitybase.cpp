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
* Description:  Context publishing helper dll
*
*/


#include <hg/hgcontextutilitybase.h>
#include "hgcontextutilityimpl.h"

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::CHgContextUtilityBase
// -----------------------------------------------------------------------------
//
CHgContextUtilityBase::CHgContextUtilityBase()
    {
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityBase::BaseConstructL()
    {
    iImpl = CHgContextUtilityImpl::NewL();
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::~CHgContextUtilityBase
// -----------------------------------------------------------------------------
//
CHgContextUtilityBase::~CHgContextUtilityBase()
    {
    delete iImpl;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::PublishContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtilityBase::PublishContextL( const TDesC& aContextType,
        const TDesC& aContextData )
    {
    iImpl->PublishContextL( aContextType, aContextData );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::PublishContextL
// Array version
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtilityBase::PublishContextL( const TDesC& aContextType,
        const MDesCArray& aContextData )
    {
    iImpl->PublishContextL( aContextType, aContextData );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::GetContextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CHgContextUtilityBase::GetContextL( const TDesC& aContextSource,
        const TDesC& aContextType )
    {
    return iImpl->GetContextL( aContextSource, aContextType );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::GetContextL
// Version using a fixed context source
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CHgContextUtilityBase::GetContextL( const TDesC& aContextType )
    {
    return iImpl->GetContextL( aContextType );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::PublishContextDelayedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtilityBase::PublishContextDelayedL(
        const TDesC& aContextType,
        const TDesC& aContextData,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContextDelayedL( aContextType, aContextData, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityBase::PublishContextDelayedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtilityBase::PublishContextDelayedL(
        const TDesC& aContextType,
        const MDesCArray& aContextData,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContextDelayedL( aContextType, aContextData, aDelay );
    }


// end of file
