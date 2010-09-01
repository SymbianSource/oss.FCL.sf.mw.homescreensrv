/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Class header
#include "observer.h"

// External includes
#include <coemain.h>
#include <f32file.h>
#include <gulicon.h>
#include <aicontentmodel.h>

const char KAiShortcutCaption[] = "ShortcutCaption";
const char KAiShortcutIcon[] = "ShortcutIcon";

// ======== MEMBER FUNCTIONS ========

//---------------------------------------------------------------------------
// Observer::Observer()
//---------------------------------------------------------------------------
//
Observer::Observer()
    {
    }

//---------------------------------------------------------------------------
// void Observer::ConstructL()
//---------------------------------------------------------------------------
//
void Observer::ConstructL()
    {
    }

//---------------------------------------------------------------------------
// Observer* Observer::NewL()
//---------------------------------------------------------------------------
//
Observer* Observer::NewL()
    {
    Observer* self = new( ELeave ) Observer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//---------------------------------------------------------------------------
// Observer::~Observer()
//---------------------------------------------------------------------------
//
Observer::~Observer()
    {
    }

//---------------------------------------------------------------------------
// TInt Observer::StartTransaction()
//---------------------------------------------------------------------------
//
TInt Observer::StartTransaction( TInt aTxId )
    {
    if ( iTransactionId != 0 )
        {
        //TODO: report error
        }
    iTransactionId = aTxId;
    return KErrNone;
    }


//---------------------------------------------------------------------------
// TInt Observer::Commit()
//---------------------------------------------------------------------------
//
TInt Observer::Commit( TInt aTxId )
    {
    TRAPD( err, DoCommitL( aTxId ) );
    return err;
    }

//---------------------------------------------------------------------------
// TInt Observer::Rollback()
//---------------------------------------------------------------------------
//
TInt Observer::Rollback( TInt aTxId )
    {
    if ( iTransactionId == 0 )
        {
        //TODO: report error
        }
    else if ( iTransactionId != aTxId )
        {
        //TODO: report error
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------
// TBool Observer::CanPublish()
//---------------------------------------------------------------------------
//
TBool Observer::CanPublish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, TInt /*aIndex*/ )
    {
    return ETrue;
    }

//---------------------------------------------------------------------------
// TInt Observer::Publish()
//---------------------------------------------------------------------------
//
TInt Observer::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
    {
    //TRAPD( err, DoPublishL( aPlugin, aContent, aResource, aIndex ) );
    TInt err( KErrNone );
    return err;
    }


//---------------------------------------------------------------------------
// TInt Observer::Publish()
//---------------------------------------------------------------------------
//
TInt Observer::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, 
    const TDesC16& /*aText*/, TInt /*aIndex*/ )
    {
    //TRAPD( err, DoPublishL( aPlugin, aContent, aText, aIndex ) );
    TInt err( KErrNone );
    return err;
    }


//---------------------------------------------------------------------------
// TInt Observer::Publish()
//---------------------------------------------------------------------------
//
TInt Observer::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, 
    const TDesC8& aBuf, TInt /*aIndex*/ )
    {
    CGulIcon* icon = UnpackPtr<CGulIcon>( aBuf );
    if (icon != NULL)
        {
        delete icon;
        }
    else
        {
        //TODO report error
        }
    return KErrNone;
    }

//---------------------------------------------------------------------------
// TInt Observer::Publish()
//---------------------------------------------------------------------------
//
TInt Observer::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

//---------------------------------------------------------------------------
// TInt Observer::Clean()
//---------------------------------------------------------------------------
//
TInt Observer::Clean( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, 
    TInt /*aIndex*/ )
    {
    // TODO: clean content.
    iTransactionId = 0;
    return 0;
    }

//---------------------------------------------------------------------------
// TAny* Observer::Extension()
//---------------------------------------------------------------------------
//
TAny* Observer::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

//---------------------------------------------------------------------------
// TBool Observer::RequiresSubscription()
//---------------------------------------------------------------------------
//
TBool Observer::RequiresSubscription( 
    const THsPublisherInfo& /*aPublisherInfo*/ ) const
    {
    return EFalse;
    }


//---------------------------------------------------------------------------
// void Observer::DoCommitL()
//---------------------------------------------------------------------------
//
void Observer::DoCommitL( TInt aTxId )
    {
    if ( iTransactionId == 0 )
        {
        //TODO: report error
        }
    else if ( iTransactionId != aTxId )
        {
        //TODO: report error
        }
    iTransactionId = 0;
    }

//---------------------------------------------------------------------------
// TInt Observer::SetProperty()
//---------------------------------------------------------------------------
//
TInt Observer::SetProperty( CHsContentPublisher& /*aPlugin*/,
    const TDesC8& /*aElementId*/, const TDesC8& /*aPropertyName*/,
    const TDesC8& /*aPropertyValue*/ )
    {
	return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt Observer::SetProperty()
// ---------------------------------------------------------------------------
//
TInt Observer::SetProperty( CHsContentPublisher& /*aPlugin*/,
    const TDesC8& /*aElementId*/, const TDesC8& /*aPropertyName*/,
    const TDesC8& /*aPropertyValue*/, 
    MAiContentObserver::TValueType /*aValueType*/ )
    {
	return KErrNone;
    }							                                   
// End of File.
