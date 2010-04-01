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
* Description:  Class for application shortcut target
*
*/


#include <coemain.h>            // For CCoeEnv
#include <w32std.h>             // For RWsSession
#include <akntranseffect.h>
#include <akntransitionutils.h>
#include <apacmdln.h>           // For CApaCommandLine
#include <apgtask.h>            // For TApaTaskList
#include <apgwgnam.h>           // For CApaWindowGroupName
#include <gulicon.h>            // For CGulIcon
#include <AknsUtils.h>          // For AknsUtils
#include <AknTaskList.h>        // For CAknTaskList
#include <gfxtranseffect/gfxtranseffect.h>		// For Transition effect
#include <gslauncher.h>
#include <viewcli.h>            // For CVwsSessionWrapper
#include <escapeutils.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif

#include <featmgr.h>
#include <centralrepository.h>
#include <e32property.h>
#include <settingsinternalcrkeys.h>
#include <UikonInternalPSKeys.h>

#include <aiscuttexts.rsg>
//#include <pslnactiveidleplugin.mbg>
#include <data_caging_path_literals.hrh>

#include <activeidle2domainpskeys.h>

#include "aiscutcontentmodel.h"
#include "hs_app_caiscuttargetapp.h"
#include "hs_app_caiscutengine.h"

#include <aiscutplugin.mbg>
#include "debug.h"

#include <AknSgcc.h>

const TInt KIconSizeArray = 4;

// ======== MEMBER FUNCTIONS =================================================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetApp::CAiScutTargetApp(
    CAiScutEngine& aEngine, TShortcutType aType, const TUid aUid)
    : CAiScutTarget(aEngine, aType)
    , iAppUid(aUid)
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetApp::ConstructL(const TAiScutParser& aParser)
    {
    iDefinition = aParser.Get(EScutDefComplete).AllocL();

    iViewUid.iUid = -1;

    switch (Type())
        {
    case EScutApplicationView:
    case EScutChangeTheme:
        if (iAppUid == KScutAppShellUid)
            {
            iViewUid.iUid = 1; // activate always appshell main view.
            }
        else
            {
            iViewUid = TAiScutParser::ParseUid(aParser.Get(EScutDefParamValue));
            }
        break;

    case EScutLogsMissedCallsView:
        iMsg = TPtrC8(KLogsMissedCallsView).AllocL();
        break;

    case EScutLogsDialledCallsView:
        iMsg = TPtrC8(KLogsDialledCallsView).AllocL();
        break;

    case EScutLogsReceivedCallsView:
        iMsg = TPtrC8(KLogsReceivedCallsView).AllocL();
        break;

    case EScutLogsMainView:
        iMsg = TPtrC8(KLogsMainView).AllocL();
        break;

    case EScutApplicationWithParams:
        {
        TPtrC ptr = aParser.Get(EScutDefParamNameAndValue);
        iMsg = EscapeUtils::ConvertFromUnicodeToUtf8L(ptr);
        break;
        }

    default:
        break;
        }

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetApp* CAiScutTargetApp::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser)
    {
    CAiScutTargetApp* self =
        new (ELeave) CAiScutTargetApp(aEngine, aType, aParser.Uid());

    CleanupStack::PushL(self);
    self->ConstructL(aParser);

    CleanupStack::Pop(self);

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetApp::~CAiScutTargetApp()
    {
    delete iCaption;
    delete iShortCaption;
	delete iMsg;
    delete iDefinition;
    }


// ---------------------------------------------------------------------------
// Returns the shortcut definition string.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetApp::Definition() const
    {
    return iDefinition ? TPtrC(*iDefinition) : TPtrC();
    }


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetApp::GetCaption(TPtrC& aDes, TAiScutAppTitleType aTitleType) const
    {
    TRAP_IGNORE(GetCaptionL(aTitleType));

    if (aTitleType == EAiScutSkeyTitle)
        {
        aDes.Set(iShortCaption ? *iShortCaption : KNullDesC());
        }
    else
        {
        aDes.Set(iCaption ? *iCaption : KNullDesC());
        }

    return 0;
    }

// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
void CAiScutTargetApp::GetCaptionL(TAiScutAppTitleType aTitleType) const
    {
    TApaAppInfo appInfo;

    // Use lazy evaluation, create the caption only when it is first needed.
    if (aTitleType == EAiScutSkeyTitle)
        {
        if( !iShortCaption )
            {
            iEngine.ApaSession().GetAppInfo(appInfo, iAppUid);
            iShortCaption = appInfo.iShortCaption.AllocL();            
            }
        }
    else
        {
        if (!iCaption)
            {
            iEngine.ApaSession().GetAppInfo(appInfo, iAppUid);
            iCaption = appInfo.iCaption.AllocL();
            }
        }
    }

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetApp::GetIcon(CGulIcon*& aIcon) const
    {
    // First try to make the override icon
    // if not successful then do it ourself
    if ( CAiScutTarget::GetIcon(aIcon) != KErrNone )
        {
        TRAP_IGNORE(CreateAppIconL(aIcon));
        }


    return 0;
    }

// ---------------------------------------------------------------------------
// Returns the shortcut target icon for toolbar.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetApp::GetToolbarIcon(CGulIcon*& aIcon) const
    {
    if ( CAiScutTarget::GetToolbarIcon(aIcon) != KErrNone)
        {
        TRAP_IGNORE(CreateAppIconL(aIcon));
        }

    return 0;
    }

// ---------------------------------------------------------------------------
// Creates the application icon.
// ---------------------------------------------------------------------------
//

void CAiScutTargetApp::CreateAppIconL(CGulIcon*& aIcon) const
    {
    // To make sure we won't end up with a partially created icon, we first create
    // a temporary icon and take it into use only when it is propertly initialized.
    CGulIcon* tempIcon = NULL;
    TInt err = KErrNone;

    // If no scalable icon support is available then the icon is constructed the "old way"
    // java icon separately from the native icon. If the support is available then the
    // java icon is constructed the same way the native icon is constructed.

    const TBool isNonNative = iEngine.IsNonNative(iAppUid);

    // First try to create a normal non-native icon
    if (isNonNative)
        {
        TRAP(err,
            tempIcon = CreateNonNativeIconL()
            );
        // Just to be sure
        if (err != KErrNone)
            {
            tempIcon = NULL;
            }
        }
    // No icon yet so either native icon or non-native SVG icon.
    if (!tempIcon)
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        if ((iAppUid == KScutPersonalisationUid && iViewUid == KScutChangeThemeViewId)
         || (iAppUid == KScutGeneralSettingsUid && (iViewUid == KScutInstallationViewId
                               ||  iViewUid == KScutConnectivityViewId))
          )
          {
          tempIcon = CreateSubstituteIconL(skin, iAppUid, iViewUid);
          }
    else
      {
          CFbsBitmap* bitmap = NULL;
          CFbsBitmap* mask   = NULL;

        AknsUtils::CreateAppIconLC(skin, iAppUid, EAknsAppIconTypeList, bitmap, mask);
        if (bitmap)
          {
        tempIcon = CGulIcon::NewL(bitmap, mask);
        CleanupStack::Pop(2);   // Bitmap and mask. They have to be popped out by number
                                    // because the order in which they are pushed in is undefined.
        }

          }

    }

    aIcon = tempIcon;
  }


CGulIcon* CAiScutTargetApp::CreateSubstituteIconL( MAknsSkinInstance* aSkin, TUid aAppUid, TUid iViewUid ) const
  {
  CGulIcon* tempIcon = NULL;
  TParse* parse = new (ELeave) TParse;
    CleanupStack::PushL(parse);
    parse->Set(KBitmapFile, &KDC_APP_BITMAP_DIR, NULL);
    HBufC* fileName = parse->FullName().AllocLC();
  TPtr fileNamePtr = fileName->Des();

  //change idle theme
  if (aAppUid == KScutPersonalisationUid && iViewUid == KScutChangeThemeViewId)
      {
      tempIcon = AknsUtils::CreateGulIconL(
          aSkin,
          KAknsIIDQgnPropPslnAiSub,
          fileNamePtr,
            EMbmAiscutpluginQgn_prop_psln_ai_sub,
          EMbmAiscutpluginQgn_prop_psln_ai_sub_mask);
      }
    //appmngr
    else if(aAppUid == KScutGeneralSettingsUid && iViewUid == KScutInstallationViewId)
      {
      CFbsBitmap* bitmap = NULL;
      CFbsBitmap* mask   = NULL;

    AknsUtils::CreateAppIconLC(aSkin, KScutAppMngrUid, EAknsAppIconTypeList, bitmap, mask);
    
    // The icon may reside in cache so we need to exclude in order
    // for updated icons to be loaded properly.
    AknIconUtils::ExcludeFromCache( bitmap );
    if (bitmap)
      {
      tempIcon = CGulIcon::NewL(bitmap, mask);
      CleanupStack::Pop(2);   // Bitmap and mask. They have to be popped out by number
                  // because the order in which they are pushed in is undefined.
      }
      }
    //connectivity view
    else if(aAppUid == KScutGeneralSettingsUid && iViewUid == KScutConnectivityViewId)
      {
      tempIcon = AknsUtils::CreateGulIconL(
          aSkin,
          KAknsIIDQgnPropAiShortcut,
          fileNamePtr,
            EMbmAiscutpluginQgn_prop_cp_conn_shortcut,
          EMbmAiscutpluginQgn_prop_cp_conn_shortcut_mask);
      }
    else
      {
      //never should go here!
      }

      CleanupStack::PopAndDestroy(2, parse); // fileName, parse
      return tempIcon;
  }

// ---------------------------------------------------------------------------
// Creates the NonNative application icon.
// ---------------------------------------------------------------------------
//

CGulIcon* CAiScutTargetApp::CreateNonNativeIconL() const
    {
    CApaMaskedBitmap* maskedbitmap = CApaMaskedBitmap::NewLC();

    CArrayFixFlat<TSize>* sizesArray = new (ELeave) CArrayFixFlat<TSize>(KIconSizeArray);
    CleanupStack::PushL(sizesArray);

    TInt err = iEngine.ApaSession().GetAppIconSizes(iAppUid, *sizesArray);

    // If there is no error and there is something in array
    // use first icon size and get an icon.
    if (!err && sizesArray->Count())
        {
        err = iEngine.ApaSession().GetAppIcon(iAppUid, sizesArray->At(0), *maskedbitmap);
        }

    // If there was an error, delete every allocated object and leave.
    if (err)
        {
        CleanupStack::PopAndDestroy(sizesArray);
        CleanupStack::PopAndDestroy(maskedbitmap);
        User::Leave(err);
        }

    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    CFbsBitmap* mask   = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(mask);

    User::LeaveIfError(bitmap->Duplicate(maskedbitmap->Handle()));
    User::LeaveIfError(mask->Duplicate(maskedbitmap->Mask()->Handle()));

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    //icon->SetBitmapsOwnedExternally(ETrue);

    CleanupStack::Pop(mask);
    CleanupStack::Pop(bitmap);

    CleanupStack::PopAndDestroy(sizesArray);
    CleanupStack::PopAndDestroy(maskedbitmap);

    return icon;
    }

// ---------------------------------------------------------------------------
// Checks if the target application is accessible.
// ---------------------------------------------------------------------------
//
TBool CAiScutTargetApp::IsAccessibleL(TInt /*aCheckType*/)
    {
    if (iAppUid == KNullUid || iEngine.IsHidden(iAppUid))
        {
        return EFalse;
        }

    TApaAppInfo appInfo;
    TInt ret = iEngine.ApaSession().GetAppInfo(appInfo, iAppUid);

    if (ret == RApaLsSession::EAppListInvalid)
        {
        // Application list not fully populated yet. Leave with KErrNotReady so
        // the engine can trap it and start the timer to check accessiblity later.
        User::Leave(KErrNotReady);
        }

    return (ret == KErrNone);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetApp::LaunchL()
    {
    LaunchL( KNullDesC8 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetApp::LaunchL( const TDesC8& aCustomMessage )
    {
    __PRINTS( "XAI: CAiScutShortcut::LaunchL");
    __PRINT( __DBG_FORMAT("XAI:   type = %d"), Type());

    if (iAppUid != KNullUid)
        {

        // Some special cases
        // ---------------------------------
        if( Type() == EScutApplicationView &&
            iAppUid == KScutGeneralSettingsUid )
            {
            CGSLauncher* l = CGSLauncher::NewLC();
            l->LaunchGSViewL ( iViewUid,
                               KScutActiveIdleUid,
                               aCustomMessage );            
            CleanupStack::PopAndDestroy( l );
            
            return;    
            }
        else if ( ( ( Type() == EScutChangeTheme) && ( iAppUid == KScutPersonalisationUid ) ) ||  // For AI2 Themes
                  ( iAppUid == KScutDiallerUid ) )                                                // Dialer
            {
            const TVwsViewId viewId(iAppUid, iViewUid);
            TUid msgId = KScutGeneralSettingsUid;
            
            if( iAppUid == KScutDiallerUid ) 
                {
                msgId = KScutDiallerViewCommand;
                RProperty::Set(KPSUidAiInformation, KActiveIdleState, EPSAiNumberEntry );
                }

            TInt err = iEngine.VwsSession()->CreateActivateViewEvent( viewId, msgId, aCustomMessage );

            return;
            }

        TBool         exists = EFalse;

        CAknTaskList* taskList = CAknTaskList::NewL(iEngine.Env()->WsSession());
        TApaTask      task(taskList->FindRootApp(iAppUid));
        delete        taskList;

        exists = task.Exists();

        // If not found, try again little harder
        // ----------------------------------------
        if (!exists)
            {
            RWsSession wsSession = iEngine.Env()->WsSession();
            task.SetWgId(0);                // Set task to non-existant task
            TInt wgId=0;
            CApaWindowGroupName::FindByAppUid(iAppUid, wsSession, wgId);

            if (wgId != KErrNotFound)
                {
                exists = ETrue;
            task.SetWgId(wgId);
                }
            }

        // Actual reactivatio / starting
        // -----------------------------
        if (exists)       // Found, reactivate
            {
            if (iMsg && iMsg->Length())
                {
                task.SendMessage(KNullUid, *iMsg);
                }
            if ( iAppUid == KScutAppShellUid ) // AppShell effect is an exception
            	{
            	//start different fullscreen effect when launching appshell
            	GfxTransEffect::BeginFullScreen( AknTransEffect::EApplicationActivate ,
            			TRect(0,0,0,0),
            	        AknTransEffect::EParameterType,
            	        AknTransEffect::GfxTransParam( iAppUid,
            	        AknTransEffect::TParameter::EActivateExplicitContinue ) );        
            	}
            
            if( iAppUid == KScutAppShellUid ) // Appshell starting is an exception
                {
                task.SendMessage( KUidApaMessageSwitchOpenFile , KNullDesC8 );
                }
            else
                {
                // If message was sent, don't try to bring task to foreground as task will do it itself
                if ( !( (iAppUid == KScutLogsUid) && (iMsg && iMsg->Length() > 0)) )
                    {
                    CAknSgcClient::MoveApp(task.WgId(), ESgcMoveAppToForeground);
                    }
                }
            }
        else             // Not exists, starting
            {
            TApaAppInfo appInfo;

            if (iEngine.ApaSession().GetAppInfo(appInfo, iAppUid) == KErrNone)
                {
                if (FeatureManager::FeatureSupported(KFeatureIdCommonVoip) &&
                    (iAppUid == KScutEasyVoIPApplicationUid) )
                    {
                    SetEasyVoIPShortcutStartL();
                    }

                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL(appInfo.iFullName);
                if (iMsg && iMsg->Length())
                    {
                    cmdLine->SetCommandL(EApaCommandRun);
                    cmdLine->SetTailEndL(*iMsg);
                    }
                if (iBackground)
                    {
                    cmdLine->SetCommandL(EApaCommandBackground);
                    }
                if ( iAppUid == KScutVoiceDialUid )
                	{
                	cmdLine ->SetCommandL( EApaCommandRunWithoutViews );
                	}

                TInt err = iEngine.ApaSession().StartApp(*cmdLine);


                __PRINT( __DBG_FORMAT("XAI:   StartApp err = %d"), err);

                User::LeaveIfError(err);

                CleanupStack::PopAndDestroy(cmdLine);
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetApp::AppUid() const
    {

#ifdef __WEB_WIDGETS
    // for widgets return widgetapp uid.
    if (iEngine.IsWidget(iAppUid))
        {
        return KUidWidgetUi;
        }
#endif

    return iAppUid;
    }

// ---------------------------------------------------------------------------
// Return the view id
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetApp::AdditionalUid() const
    {
    return iViewUid;
    }

// ---------------------------------------------------------------------------
// CScShortcutNativeApp::SetEasyVoIPShortcutStartL()
//
// Performs Central Repository shortcut set for EasyVoIP application.
// ---------------------------------------------------------------------------
//
void CAiScutTargetApp::SetEasyVoIPShortcutStartL() const
    {
    CRepository* repository = CRepository::NewL(KUidEasyVoIPRepository);
    CleanupStack::PushL(repository);

    // Set shortcut start for EasyVoIP application.
    TInt error = repository->Set(KEasyVoIPShortcutStartup, 1);
    User::LeaveIfError(error);

    CleanupStack::PopAndDestroy(repository);
    }

// End of File.
