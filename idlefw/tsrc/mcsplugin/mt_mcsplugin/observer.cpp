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


#include <coemain.h>
#include <f32file.h>
#include <gulicon.h>
#include <aicontentmodel.h>

#include "observer.h"
//#include "caiplugintesterview.h"


const char KAiShortcutCaption[]  = "ShortcutCaption";
const char KAiShortcutIcon[]     = "ShortcutIcon";


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
Observer::Observer()
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void Observer::ConstructL()
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
Observer* Observer::NewL()
{
    Observer* self = new(ELeave) Observer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
Observer::~Observer()
{
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that it initiates content publishing transaction
// ---------------------------------------------------------------------------
//
TInt Observer::StartTransaction(TInt aTxId)
{
    if (iTransactionId != 0)
    {
        //TODO: report error
    }

    iTransactionId = aTxId;
    return KErrNone;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to request framework that content publishing is committed
// ---------------------------------------------------------------------------
//
TInt Observer::Commit(TInt aTxId)
{
    TRAPD(err, DoCommitL(aTxId));
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by plug-in to indicate that content publishing transaction is cancelled
// ---------------------------------------------------------------------------
//
TInt Observer::Rollback(TInt aTxId)
{
    if (iTransactionId == 0)
    {
        //TODO: report error
    }
    else if (iTransactionId != aTxId)
    {
        //TODO: report error
    }

    return KErrNone;
}

// ---------------------------------------------------------------------------
// Invoked by plug-in to test if content can be published.
// ---------------------------------------------------------------------------
//
TBool Observer::CanPublish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/)
{
    return ETrue;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt Observer::Publish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/)
{
    //TRAPD(err, DoPublishL(aPlugin, aContent, aResource, aIndex));
    TInt err( KErrNone );
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt Observer::Publish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, const TDesC16& /*aText*/, TInt /*aIndex*/)
{
    //TRAPD(err, DoPublishL(aPlugin, aContent, aText, aIndex));
    TInt err( KErrNone );
    return err;
}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt Observer::Publish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, const TDesC8& aBuf, TInt /*aIndex*/)
    {
    CGulIcon* icon = UnpackPtr<CGulIcon>(aBuf);
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

// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt Observer::Publish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/)
{
    return KErrNotSupported;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be cleaned in UI
// ---------------------------------------------------------------------------
//
TInt Observer::Clean(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/)
{
    // TODO: clean content.
    iTransactionId = 0;
    return 0;
}


// ---------------------------------------------------------------------------
// Returns interface extension. Not used in Series 60 v3.2 release.
// ---------------------------------------------------------------------------
//
TAny* Observer::Extension(TUid /*aUid*/)
{
    return NULL;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool Observer::RequiresSubscription( const THsPublisherInfo& /*aPublisherInfo*/ ) const
{
    return EFalse;
}


// ---------------------------------------------------------------------------
// Leaving version of the commit operation
// ---------------------------------------------------------------------------
//
void Observer::DoCommitL(TInt aTxId)
{
    if (iTransactionId == 0)
    {
        //TODO: report error
    }
    else if (iTransactionId != aTxId)
    {
        //TODO: report error
    }

    iTransactionId = 0;
}

// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt Observer::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/ )
{
	return KErrNone;
}				

// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt Observer::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/,
									          MAiContentObserver::TValueType /*aValueType*/ )
{
	return KErrNone;
}							                                   

// End of File.
