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
* Description:  Class for messaging view shortcut target
*
*/


#include <msvuids.h>            // For KMsvRootIndexEntryIdValue
#include <gulicon.h>            // For CGulIcon
#include <SenduiMtmUids.h>
#include <viewcli.h>            // For CVwsSessionWrapper
#include <AknsUtils.h>          // For AknsUtils
#include <data_caging_path_literals.hrh>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif

#include "caiscuttargetmessagingview.h"
#include "caiscutengine.h"
#include <aiscutplugin.mbg>

#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetMessagingView::CAiScutTargetMessagingView(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetMessagingView::ConstructL(const TAiScutParser& aParser)
{
    iDefinition = aParser.Get(EScutDefComplete).AllocL();
    // Updates the view name also
    FindViewIdL();
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetMessagingView* CAiScutTargetMessagingView::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser)
{
    CAiScutTargetMessagingView* self = new (ELeave) CAiScutTargetMessagingView(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aParser);
    CleanupStack::Pop(self);

    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetMessagingView::~CAiScutTargetMessagingView()
{
    delete iDefinition;
    delete iViewName;
}


// ---------------------------------------------------------------------------
// Returns the shortcut definition string.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetMessagingView::Definition() const
{
    return TPtrC(*iDefinition);
}


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetMessagingView::GetCaption(TPtrC& aDes, TAiScutAppTitleType /*aTitleType*/) const
{
    aDes.Set(*iViewName);
    return 0;
}


// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetMessagingView::GetIcon(CGulIcon*& aIcon) const
{
    if ( CAiScutTarget::GetIcon(aIcon) != KErrNone )
        {
        TRAP_IGNORE(GetIconL(aIcon));    
        }

    return 0;
}

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
void CAiScutTargetMessagingView::GetIconL(CGulIcon*& aIcon) const
{
    CGulIcon* tempIcon = NULL;

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask   = NULL;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateAppIconLC(skin, KScutMessagingUid, EAknsAppIconTypeList, bitmap, mask);

    tempIcon = CGulIcon::NewL(bitmap, mask);

    CleanupStack::Pop(2); // Bitmap and mask. They have to be popped out by number
                          // because the order in which they are pushed in is undefined.

    //Do not need to sets the bitmap and mask to be owned externally
    
    aIcon = tempIcon;
}


// ---------------------------------------------------------------------------
// Checks if the shortcut target is accessible.
// ---------------------------------------------------------------------------
//
TBool CAiScutTargetMessagingView::IsAccessibleL(TInt /*aCheckType*/)
{
	return FindViewIdL() != KErrNotFound;
}


// ---------------------------------------------------------------------------
// Launches a remote mailbox.
// ---------------------------------------------------------------------------
//
void CAiScutTargetMessagingView::LaunchL()
{
    TMsvId id(FindViewIdL());
    if (id != KErrNotFound)
    {
        const TVwsViewId viewId(KScutMessagingUid, KScutRemoteMailboxViewId);
        iEngine.VwsSession()->CreateActivateViewEvent(viewId, TUid::Uid(id), KNullDesC8());
    }
}

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetMessagingView::AppUid() const
{
    return KScutMessagingUid;
}


// ---------------------------------------------------------------------------
// Tries to find a view id.
// ---------------------------------------------------------------------------
//
TMsvId CAiScutTargetMessagingView::FindViewIdL()
    {
    TMsvId id(KErrNotFound);

    TInt mailboxId = KErrNone;
    TAiScutParser parser;
    parser.Parse(*iDefinition);          		
    TLex lex(parser.Get(EScutDefParamValue));
    lex.Val(mailboxId);

    if (iEngine.MsvSession())
    {
        // KErrNotReady is the only allowed leave code. Engine will trap it and start a timer
        // to check access later. Other possible leaves emitted by the message server are
        // substituted with KErrNotReady.
        CMsvEntry* rootEntry = NULL;
        TRAPD(err, rootEntry = iEngine.MsvSession()->GetEntryL(KMsvRootIndexEntryIdValue));
        if (err != KErrNone)
        {
            User::Leave(KErrNotReady);
        }

		if(rootEntry)
		{
	        // No leaving code here since rootEntry is not in cleanup stack.
    	    for (TInt i = rootEntry->Count(); --i >= 0;)
        	{
            	const TMsvEntry& tentry = (*rootEntry)[i];
	
				__PRINT( __DBG_FORMAT("XAI: CAiScutTargetMessagingView::FindViewIdL id = 0x%x '%S'"),
					 tentry.Id(), &tentry.iDetails);
            	if ((tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid) &&
                	tentry.Id() == mailboxId)
	            {
    	            id = tentry.Id();
    	            delete iViewName;
    	            iViewName = NULL;
    	            iViewName = tentry.iDetails.AllocL();
        	        break;
            	}
	        }
	
    	    delete rootEntry;
        	rootEntry = NULL;
			
		}
    }

    return id;
}

// End of File.
