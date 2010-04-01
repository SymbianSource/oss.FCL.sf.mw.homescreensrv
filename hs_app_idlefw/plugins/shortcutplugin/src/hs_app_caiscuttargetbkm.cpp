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
* Description:  Class for bookmark shortcut target
*
*/


#include <coemain.h>            // For CCoeEnv
#include <w32std.h>             // For RWsSession
#include <apgtask.h>            // For TApaTaskList
#include <gulicon.h>            // For CGulIcon
#include <AknsUtils.h>          // For AknsUtils
#include <data_caging_path_literals.hrh>

#include "aiscutcontentmodel.h"
#include "hs_app_caiscuttargetbkm.h"
#include "hs_app_caiscutengine.h"
#include <aiscutplugin.mbg>

#include "debug.h"



_LIT16(KParam, "1 ");
const TInt KTBUF16 = 16;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetBkm::CAiScutTargetBkm(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetBkm::ConstructL(const TAiScutParser& aParser)
{
    iDefinition = aParser.Get(EScutDefComplete).AllocL();
    iBkmUid = aParser.ParseUid(aParser.Get(EScutDefParamValue));
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetBkm* CAiScutTargetBkm::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser)
{
    CAiScutTargetBkm* self = new (ELeave) CAiScutTargetBkm(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aParser);
    CleanupStack::Pop(self);

    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetBkm::~CAiScutTargetBkm()
{
    delete iCaption;
    delete iDefinition;
}


// ---------------------------------------------------------------------------
// Returns the shortcut definition string.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetBkm::Definition() const
{
    return iDefinition ? TPtrC(*iDefinition) : TPtrC();
}


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetBkm::GetCaption(TPtrC& aDes, TAiScutAppTitleType /*aTitleType*/) const
{
    // Use lazy evaluation, create the caption only when it is first needed.
    if (!iCaption)
    {
        // Get the caption.
        TRAP_IGNORE(GetCaptionL());
    }

    aDes.Set(iCaption ? *iCaption : KNullDesC());

    return 0;
}

// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
void CAiScutTargetBkm::GetCaptionL() const
{
    RFavouritesDb& db = iEngine.FavouritesDb();

    CFavouritesItem* favItem = CFavouritesItem::NewLC();
    TInt err = db.Get(iBkmUid.iUid, *favItem);
    iCaption = favItem->Name().AllocL();

    CleanupStack::PopAndDestroy(favItem);
}


// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetBkm::GetIcon(CGulIcon*& aIcon) const
{
     if ( CAiScutTarget::GetIcon(aIcon) != KErrNone )
        {
        
        CGulIcon* tempIcon = NULL;

        TFileName pluginIconFile(KDC_APP_BITMAP_DIR);
        pluginIconFile.Append(KBitmapFile);

        TRAP_IGNORE(
            tempIcon = AknsUtils::CreateGulIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDQgnPropAiShortcut,
                pluginIconFile,
                EMbmAiscutpluginQgn_menu_url,
                EMbmAiscutpluginQgn_menu_url_mask
                )
            );

        aIcon = tempIcon;
        }

    return 0;
}

// -----------------------------------------------------------------------------
// Checks if the target bookmark is accessible.
// -----------------------------------------------------------------------------
//
TBool CAiScutTargetBkm::IsAccessibleL(TInt /*aCheckType*/)
{
    if (iBkmUid == KNullUid)
    {
        return EFalse;
    }

    // Search bookmark from favourites.
    RFavouritesDb& db = iEngine.FavouritesDb();

    TBool exists = EFalse;
    TInt err = db.ItemExists(iBkmUid.iUid, exists);
    if (err != KErrNone)
    {
        exists = EFalse;
    }

    return exists;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAiScutTargetBkm::LaunchL()
{
/*
1. Start/Continue the browser and nothing is specified (the default case)   Parameter = <Any text>
2. Start/Continue the browser specifying a Bookmark                         Parameter = "1"+"<Space>"+"<Uid of the Bookmark>"
3. Start/Continue the browser specifying a Saved deck                       Parameter = "2"+"<Space>"+"<Uid of the Saved deck>"
4. Start/Continue the browser specifying a URL                              Parameter = "4"+"<Space>"+"<Url>"
5. Start/Continue the browser specifying a URL and an Access Point          Parameter = "4"+"<Space>"+"<Url>"+"<Space>"+"<Uid of Ap>"
6. Start/Continue the browser with the start page.
   (Used when starting the browser with a long press of "0" in the
   Idle state of the phone.                                                 Parameter = "5"
7. Start/Continue the browser specifying a Bookmark folder                  Parameter = "6"+"<Space>"+"<Uid of the Folder>"
*/

    TApaTaskList taskList(iEngine.Env()->WsSession());
    TApaTask task = taskList.FindApp(KScutBrowserUid);

    
    TBuf<KTBUF16> param(KParam);
    param.AppendNum(iBkmUid.iUid);

	__PRINT( __DBG_FORMAT("XAI: CAiScutTargetBkm::LaunchL '%S' "), &param);
    
    if (task.Exists())
    {
        HBufC8* param8 = HBufC8::NewLC(param.Length());
        param8->Des().Copy(param);
        task.SendMessage(KNullUid, *param8); // Uid is not used.
        CleanupStack::PopAndDestroy(param8);
    }
    else
    {
        TThreadId id;
        User::LeaveIfError(iEngine.ApaSession().StartDocument(
            param, KScutBrowserUid, id));
    }
}

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetBkm::AppUid() const
{
    return KScutBrowserUid;
}

// ---------------------------------------------------------------------------
// Return the bkm id
// ---------------------------------------------------------------------------
TUid CAiScutTargetBkm::AdditionalUid() const
    {
    return iBkmUid;
    }

// End of File.
