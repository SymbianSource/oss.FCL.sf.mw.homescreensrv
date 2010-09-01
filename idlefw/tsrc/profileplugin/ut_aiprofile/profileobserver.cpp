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
* Description:  
*
*/


#include <coemain.h>
#include <f32file.h>
#include <gulicon.h>
#include <aicontentmodel.h>
// #include <aipropertyextension.h>

#include "profileobserver.h"
//#include "caiplugintesterview.h"


const char KAiProfileCaption[]  = "ProfileCaption";
const char KAiProfileIcon[]     = "ProfileIcon";


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
ProfileObserver::ProfileObserver()
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void ProfileObserver::ConstructL()
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
ProfileObserver* ProfileObserver::NewL()
{
    ProfileObserver* self = new(ELeave) ProfileObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
ProfileObserver::~ProfileObserver()
{
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that it initiates content publishing transaction
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::StartTransaction(TInt aTxId)
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
TInt ProfileObserver::Commit(TInt aTxId)
{
    TRAPD(err, DoCommitL(aTxId));
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by plug-in to indicate that content publishing transaction is cancelled
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Rollback(TInt aTxId)
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
TBool ProfileObserver::CanPublish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/)
{
    return EFalse; //ETrue;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Publish(CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex)
{
    TRAPD(err, DoPublishL(aPlugin, aContent, aResource, aIndex));
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex)
{
    TRAPD(err, DoPublishL(aPlugin, aContent, aText, aIndex));
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Publish(CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex)
{
    TRAPD(err, DoPublishL(aPlugin, aContent, aBuf, aIndex));
    return err;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be published
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Publish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/)
{
    return KErrNotSupported;
}


// ---------------------------------------------------------------------------
// Invoked by the plug-in to inform that content must be cleaned in UI
// ---------------------------------------------------------------------------
//
TInt ProfileObserver::Clean(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/)
{
    // TODO: clean content.
    iTransactionId = 0;
    return 0;
}


// ---------------------------------------------------------------------------
// Returns interface extension. Not used in Series 60 v3.2 release.
// ---------------------------------------------------------------------------
//
TAny* ProfileObserver::Extension(TUid /*aUid*/)
{
    return NULL;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool ProfileObserver::RequiresSubscription( const THsPublisherInfo& /*aPublisherInfo*/ ) const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// Leaving version of the publish operation
// ---------------------------------------------------------------------------
//
void ProfileObserver::DoPublishL(CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& /*aBuf*/, TInt /*aIndex*/)
{
    MAiContentItemIterator* iter = static_cast<MAiContentItemIterator*>(
    	aPlugin.GetProperty( CHsContentPublisher::EPublisherContent ) );
    TAiContentItem item = iter->ItemL(aContent);
    /*if (*item.cid == *KAiShortcutIcon && *item.type == *KAiContentTypeBitmap)
    {
        CGulIcon* icon = UnpackPtr<CGulIcon>(aBuf);
        if (icon != NULL)
        {
        	delete icon;
        //    iView->SetShortcutIconL(aIndex, icon);
        }
        else
        {
            //TODO report error
        }
    }*/
}


// ---------------------------------------------------------------------------
// Leaving version of the publish operation
// ---------------------------------------------------------------------------
//
void ProfileObserver::DoPublishL(CHsContentPublisher& aPlugin, TInt aContent, TInt /*aResource*/, TInt /*aIndex*/)
{
    MAiContentItemIterator* iter = static_cast<MAiContentItemIterator*>(
    	aPlugin.GetProperty( CHsContentPublisher::EPublisherContent ) );
    TAiContentItem item = iter->ItemL(aContent);

/*     Does not work for some reason. substitute with ugly magic numbers
     if (*item.cid == *KAiShortcutIcon)
     {
     iView->SetShortcutIconL(aIndex, aResource);
     }
     else if (*item.cid == *KAiShortcutCaption)
     {
     iView->SetShortcutCaptionL(aIndex, aResource);
     }*/
     
    if (aContent == 0)
    {
    //    iView->SetShortcutIconL(aIndex, aResource);
    }
    else if (aContent == 1)
    {
    //    iView->SetShortcutCaptionL(aIndex, aResource);
    }

}


// ---------------------------------------------------------------------------
// Leaving version of the publish operation
// ---------------------------------------------------------------------------
//
void ProfileObserver::DoPublishL(CHsContentPublisher& aPlugin, TInt aContent, const TDesC& /*aText*/, TInt /*aIndex*/)
{
    MAiContentItemIterator* iter = static_cast<MAiContentItemIterator*>(
    	aPlugin.GetProperty( CHsContentPublisher::EPublisherContent ) );
    TAiContentItem item = iter->ItemL(aContent);
    //if (*item.cid == *KAiShortcutCaption)
    //{
    //   iView->SetShortcutCaptionL(aIndex, aText);
    //}
}


// ---------------------------------------------------------------------------
// Leaving version of the commit operation
// ---------------------------------------------------------------------------
//
void ProfileObserver::DoCommitL(TInt aTxId)
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
TInt ProfileObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
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
TInt ProfileObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/,
									          MAiContentObserver::TValueType /*aValueType*/ )
{
	return KErrNone;
}		

// End of File.
