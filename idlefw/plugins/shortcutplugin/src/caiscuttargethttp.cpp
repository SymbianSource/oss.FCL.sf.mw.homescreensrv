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
* Description:  Class for http shortcut target
*
*/


#include <coemain.h>            // For CCoeEnv
#include <w32std.h>             // For RWsSession
#include <apgtask.h>            // For TApaTaskList
#include <gulicon.h>            // For CGulIcon
#include <AknsUtils.h>          // For AknsUtils
#include <data_caging_path_literals.hrh>

#include "caiscuttargethttp.h"
#include "caiscutengine.h"
#include <aiscutplugin.mbg>

#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetHttp::CAiScutTargetHttp(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetHttp::ConstructL(const TAiScutParser& aParser)
{
    iDefinition = aParser.Get( EScutDefComplete ).AllocL();
    iChecksum = TUid::Uid( aParser.ChecksumForString( *iDefinition) );
    
    // Get the possible custom title string from the parser
    iCaption = HBufC::NewL( iDefinition->Length() ); 
    TPtr captionPtr = iCaption->Des();
    TInt err = aParser.CustomTitle( captionPtr );    
    
    if ( err != KErrNone || iCaption->Length() <= 0 )
        {
        delete iCaption;
        iCaption = NULL;
        }
    TPtr defPtr = iDefinition->Des();    
    // we need to strip the possible icon definitions away from 
    // the URL as they are not part of it.
    aParser.RemoveExtraDefinitionsL( defPtr );
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetHttp* CAiScutTargetHttp::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser)
{
    CAiScutTargetHttp* self = new (ELeave) CAiScutTargetHttp(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aParser);
    CleanupStack::Pop(self);

    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetHttp::~CAiScutTargetHttp()
{
    delete iDefinition;
    delete iCaption;
}


// ---------------------------------------------------------------------------
// Returns the shortcut definition string.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetHttp::Definition() const
{
    return iDefinition ? TPtrC(*iDefinition) : TPtrC();
}


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetHttp::GetCaption(TPtrC& aDes, TAiScutAppTitleType /*aTitleType*/) const
{
    // Custom title is fetched when constructing. If no custom
    // title then use part of the URL as title
    if ( !iCaption )
    {        
        TUriParser parser;
        TInt err = parser.Parse(*iDefinition);

        if (err == KErrNone)
            {
            // Remove scheme from the url.
            iCaption = parser.Extract(EUriHost).Alloc();
            }
        else
            {
            iCaption = iDefinition->Alloc();
            }
    }

    aDes.Set(*iCaption);

    return 0;
}


// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetHttp::GetIcon(CGulIcon*& aIcon) const
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


// ---------------------------------------------------------------------------
// Checks if the shortcut target is accessible.
// ---------------------------------------------------------------------------
//
TBool CAiScutTargetHttp::IsAccessibleL(TInt /*aCheckType*/)
{
    return (iDefinition->Length() > 0);
}


// -----------------------------------------------------------------------------
// Launches the browser
// -----------------------------------------------------------------------------
//
void CAiScutTargetHttp::LaunchL()
{
    // Store the http address directly in the browser parameter format.
    // For example "4 http://www.nokia.com". 4 = open an url.
    HBufC* param = HBufC::NewLC(iDefinition->Length() + KOpenUrlParam().Length());
    param->Des().Copy(KOpenUrlParam());
    param->Des().Append(*iDefinition);

    __PRINT( __DBG_FORMAT("XAI: CAiScutTargetHttp::LaunchL '%S' "), param);

    TApaTaskList taskList(iEngine.Env()->WsSession());
    TApaTask task = taskList.FindApp(KScutBrowserUid);

    if (task.Exists())
    {
        HBufC8* param8 = HBufC8::NewLC(param->Length());
        param8->Des().Copy(*param);
        task.SendMessage(KNullUid, *param8); // Uid is not used.
        CleanupStack::PopAndDestroy(param8);
    }
    else
    {
        TThreadId id;
        User::LeaveIfError(iEngine.ApaSession().StartDocument(
            *param, KScutBrowserUid, id));
    }

    CleanupStack::PopAndDestroy(param);
}

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetHttp::AppUid() const
{
    return KScutBrowserUid;
}

TUid CAiScutTargetHttp::AdditionalUid() const
{
    return iChecksum;
}

// End of File.
