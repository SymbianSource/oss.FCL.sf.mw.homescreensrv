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
* Description:  Shortcut plug-in engine class
*
*/


#include <coemain.h>                // For CCoeEnv
#include <centralrepository.h>      // For CRepository
#include <w32std.h>                 // For RWsSession
#include <apgtask.h>                // For TApaTaskList
#include <viewcli.h>                // For CVwsSessionWrapper
#include <vwsdef.h>                 // For TVwsViewId
#include <e32property.h>            // For RProperty
#include <utf.h>                    // For CnvUtfConverter
#include <bautils.h>
#include <featmgr.h>
#include <ActiveFavouritesDbNotifier.h> // For CActiveFavouritesDbNotifier
#include <pathinfo.h>
#include <data_caging_path_literals.hrh>
#include <AknSgcc.h>                    // for fast swap
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif


#include <ctsydomainpskeys.h>
#include <UikonInternalPSKeys.h>
#include <menu2internalcrkeys.h>
#include <activeidle2internalpskeys.h>
#include <activeidle2domaincrkeys.h>
#include <activeidle2domainpskeys.h>
#include <aipspropertyobserver.h>

#include <aiscuttexts.rsg>
#include "caiscutengine.h"
#include "aiscutpluginprivatecrkeys.h"
#include "aiscutcontentmodel.h"
#include "caiscutplugin.h"
#include "caiscutshortcut.h"
#include "caiscutshortcutinfo.h"
#include "aidefaultshortcut.h"
#include "aiscutappuidparser.h"
#include "aiscutrepositorywatcher.h"
#include "aiscuttargetshutter.h"
#include "aiscutdefs.h"
#include "aiscutfactory.h"

#include <keylockpolicyapi.h>
#include "debug.h"
/**
 * Timer delay for access check retry. Two seconds.
 */
const TInt KScutAccessCheckRetryDelay = 2000000;
const TUid KVoiceCallUidViewId = { 0x10282D81 };
const TUid KVideoCallUid = { 0x101F8681 };

_LIT(KScutTextsResourceFileName,    "aiscuttexts.rsc");
_LIT8(KScutDirectOpen,    "?open");

// ======== LOCAL FUNCTIONS ========

LOCAL_C TInt CompareKey(const TUint32& aLeft, const TUint32& aRight)
{
    TUint32 left  = aLeft  & (KScutBitMaskThemeDefault & KScutBitMaskLocked);
    TUint32 right = aRight & (KScutBitMaskThemeDefault & KScutBitMaskLocked);

    if (left < right)
    {
        return -1;
    }
    else if (left > right)
    {
        return 1;
    }

    return 0;
}

static TInt IntFromDesc( const TDesC &aParam )
    {
    TInt err = KErrArgument;
    if (aParam.Length() > 0)
       {
           _LIT(KHexPrefix, "0x");
           const TInt prefixLen = 2;

           TRadix radix(EDecimal);
           TPtrC ptr(aParam);

           if (aParam.Left(prefixLen).CompareC(KHexPrefix) == 0)
           {
               // Strip the '0x' prefix.
               ptr.Set(aParam.Right(aParam.Length() - prefixLen));

               radix = EHex;
           }

           TLex lexer(ptr);
           TUint32 id;
           
           err = lexer.Val(id, radix);
           if ( err == KErrNone )
               {
               return id;
               }
           else
               {
               return err;
               }
       }
    return err;
    }
// ---------------------------------------------------------------------------
// Timer callback.
// ---------------------------------------------------------------------------
//
TInt DelayedCheckCallBack(TAny* aEngine)
{
    if (aEngine)
    {
    	static_cast<CAiScutEngine*>(aEngine)->RetryAccessCheck();	
    }
    
    return KErrNone;
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngine::CAiScutEngine(CAiScutPlugin& aPlugin)
    :
    iPlugin(aPlugin),
    iResourceLoaderTexts(*CCoeEnv::Static()),
    iResourceLoaderSendUi(*CCoeEnv::Static())
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngine::ConstructL()
{
    FeatureManager::InitializeLibL();

    User::LeaveIfError(iApaSession.Connect());
    User::LeaveIfError(iBookmarkSess.Connect());
    User::LeaveIfError(iBookmarkDb.Open(iBookmarkSess, KBrowserBookmarks));

    iVwsSession = CVwsSessionWrapper::NewL();
    iRepository = CRepository::NewL(KCRUidShortcutItems);
   
    iHiddenAppsRepository = CRepository::NewL(KCRUidMenu);

    // Create message server session because it may be needed during shortcut
    // creation for checking mailbox access.
    iMsvSession = CMsvSession::OpenAsObserverL(*this);

    iEnv = CCoeEnv::Static();

    TParsePtrC driveParse(PathInfo::RomRootPath());
    TFileName resourceName(driveParse.Drive());
    TParse parse;
    parse.Set(KScutTextsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    resourceName.Append(parse.FullName());
    iResourceLoaderTexts.OpenL(resourceName);

    TFileName resourceName2(driveParse.Drive());
    TParse parse2;
    parse2.Set(KSendNormResource, &KDC_RESOURCE_FILES_DIR, NULL);
    resourceName2.Append(parse2.FullName());
    iResourceLoaderSendUi.OpenL(resourceName2);

    LoadAppTitleListL();

    // Engine should still remain alive even though shortcut construction failed
    // because LaunchByValue must still be possible.
    TRAP_IGNORE(CreateShortcutsL());

    // Close message server session for now. It's opened again in Resume if needed.
    delete iMsvSession;
    iMsvSession = NULL;

    iKeyEventObserver = AiUtility::CreatePSPropertyObserverL(
        TCallBack(HandlePSCommand, this),
        KUidSystemCategory, KPSUidShortcutCmd);

    iCallStateObserver = AiUtility::CreatePSPropertyObserverL(
        TCallBack(CallStateChangeCallback, this),
        KPSUidCtsyCallInformation, KCTsyCallState);

    iKeylockApi = CKeyLockPolicyApi::NewL( EPolicyActivateKeyguard );
    if ( !iKeylockApi->HasConfiguration() )
        {
        delete iKeylockApi;
        iKeylockApi = NULL;
        }
    CRepository* repository = CRepository::NewLC(TUid::Uid(KCRUidActiveIdleLV));
    
    repository->Get(KAIFirstKeyLockKey,  iFirstLockKey);
    repository->Get(KAISecondKeyLockKey, iSecondLockKey);
    repository->Get(KAIKeyLockTimeout,  iDelayTimerDelay);    
    CleanupStack::PopAndDestroy(repository);
    // convert micro to milliseconds
    const TInt KUsInMs = 1000;
    iDelayTimerDelay = KUsInMs * iDelayTimerDelay;       
    iDelayTimer = CPeriodic::NewL (CActive::EPriorityStandard );
}

TInt CAiScutEngine::DelayTimerCallBack (TAny *aSelf )
    {
    CAiScutEngine* self = static_cast<CAiScutEngine*>(aSelf );
    TInt err = KErrNone;
    if (self )
        {
        self->iDelayTimer->Cancel();
        TRAP(err,
                self->HandleLaunchByIndexL( *(self->iDelayedLaunchCmd) );
                );
        
        }
    return err;
    }

void CAiScutEngine::DelayedLaunchByIndexL( const TDesC &aParam )
    {
    delete iDelayedLaunchCmd;        
    iDelayedLaunchCmd = NULL;
    iDelayedLaunchCmd = aParam.AllocL();
    iDelayTimer->Cancel();
    iDelayTimer->Start( iDelayTimerDelay,
            iDelayTimerDelay,
            TCallBack( DelayTimerCallBack, this ));
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngine* CAiScutEngine::NewL(CAiScutPlugin& aPlugin)
{
    CAiScutEngine* self = new (ELeave) CAiScutEngine(aPlugin);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngine::~CAiScutEngine()
{
#ifdef __WEB_WIDGETS
    if( iWidgetRegistryConnected )
        {
        iWidgetRegistry.Disconnect();
        }
#endif
    delete iKeylockApi;
    delete iSettingsNotifier;
    delete iRepository;

    delete iHiddenAppsNotifier;
    delete iHiddenAppsRepository;

    iHiddenApps.Close();

    delete iAppNotifier;
    delete iTimer;
    delete iMsvSession;
    delete iVwsSession;
    delete iScutShutter;

    Release(iKeyEventObserver);
    Release(iCallStateObserver);

    iShortcuts.ResetAndDestroy();
    iThemeShortcuts.ResetAndDestroy();
    iDefaultUsed.Close();

    delete iBookmarkDbObserver;
    iBookmarkDb.Close();
    iBookmarkSess.Close();

    iApaSession.Close();

    while(iAppTitleList.Count())
    {
        delete iAppTitleList[0].iLongTitle;
        delete iAppTitleList[0].iShortTitle;
        delete iAppTitleList[0].iSkeyTitle;
        delete iAppTitleList[0].iMskTitle;
        iAppTitleList.Remove(0);
    }
    iAppTitleList.Close();

    iResourceLoaderTexts.Close();
    iResourceLoaderSendUi.Close();

    delete iDelayTimer;
    delete iDelayedLaunchCmd;
    iIcons.Close();
    FeatureManager::UnInitializeLib();
}

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
    switch (aEvent)
    {
    case EMsvEntriesCreated:
    case EMsvEntriesDeleted:
    case EMsvEntriesChanged:
        if (iShortcuts.Count() > 0)
        {
            MergeShortcuts(EScutMailbox, ETrue);
            CheckAccessAndPublish(EScutCheckMailbox, EFalse);
        }
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// From class MApaAppListServObserver.
// Handles a change in the application list.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleAppListEvent(TInt /*aEvent*/)
{
    TRAP_IGNORE( CheckForThemeDefaultReinstalledL() );
    MergeShortcuts(EScutAnyType, ETrue);
    CheckAccessAndPublish(EScutCheckApp, ETrue);
}

// ---------------------------------------------------------------------------
// Resumes the engine.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::ResumeL(TBool aPublishAll, TAiTransitionReason /*aReason*/)
{
    // Merge shortcuts and start notifiers only if we have publishable shortcuts.
    if (iShortcuts.Count() > 0)
    {
        if (!iSettingsNotifier)
        {
            iSettingsNotifier = CAiScutRepositoryWatcher::NewL(
                KCRUidShortcutItems,
                TCallBack(HandleShortcutsChanged, this),
                iRepository);
        }

        if (!iHiddenAppsNotifier)
        {
            iHiddenAppsNotifier = CAiScutRepositoryWatcher::NewL(
                KCRUidMenu,
                KMenuHideApplication,
                CCenRepNotifyHandler::EStringKey,
                TCallBack(HandleHiddenAppsChanged, this),
                iHiddenAppsRepository);
        }
    }
#ifdef __WEB_WIDGETS
    if( !iWidgetRegistryConnected )
        {
        TInt cError = iWidgetRegistry.Connect();
        if( cError == KErrNone )
            {
            iWidgetRegistryConnected = ETrue;
            }
        }
#endif

    GetHiddenAppsL();

    MergeShortcutsL(EScutAnyType, EFalse);

    // Call state check must be done always because RSK "Back"
    // must be published when theme is changed during phone call, too.
    HandleCallStateChange();

    CheckAccessAndPublish(EScutCheckAll, aPublishAll);

    if (iShortcuts.Count() > 0)
    {
        iSettingsNotifier->StartListeningL();

        iHiddenAppsNotifier->StartListeningL();

        if (!iAppNotifier)
        {
            iAppNotifier = CApaAppListNotifier::NewL(this, CActive::EPriorityStandard);
        }
    }
}

// ---------------------------------------------------------------------------
// Suspends the engine.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::Suspend()
{
    delete iMsvSession; // This is to stop receiving message server events.
    iMsvSession = NULL;

    delete iSettingsNotifier;
    iSettingsNotifier = NULL;

    delete iHiddenAppsNotifier;
    iHiddenAppsNotifier = NULL;

    delete iAppNotifier;
    iAppNotifier = NULL;

    delete iTimer;
    iTimer = NULL;

    delete iScutShutter;
    iScutShutter = NULL;

    if (iBookmarkDbObserver)
    {
        iBookmarkDbObserver->Cancel();
    }
    delete iBookmarkDbObserver;
    iBookmarkDbObserver = NULL;
#ifdef __WEB_WIDGETS
    if( iWidgetRegistryConnected )
        {
        TInt cError = iWidgetRegistry.Disconnect();
        if( cError == KErrNone )
            {
            iWidgetRegistryConnected = EFalse;
            }
        }
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::HandleShortcutsChanged(TAny* aPtr)
{
    __PRINTS("XAI: CAiScutEngine::HandleShortcutsChanged");
    CAiScutEngine* self = static_cast<CAiScutEngine*>(aPtr);
	
	if (self)
	{
    	self->MergeShortcuts(EScutAnyType, ETrue);
    	self->CheckAccessAndPublish(EScutCheckAll, EFalse);		
	}

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::HandleHiddenAppsChanged(TAny* aPtr)
{
    CAiScutEngine* self = static_cast<CAiScutEngine*>(aPtr);

	if (self)
	{
    	TRAP_IGNORE(self->GetHiddenAppsL());
    	self->MergeShortcuts(EScutAnyType, ETrue);
    	self->CheckAccessAndPublish(EScutCheckAll, EFalse);		
	}

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsLockKey( TInt aScanCode, TScutLockKey aLockKey ) const
    {
    TBool returnValue = EFalse;
    if ( iKeylockApi )
        {      
        TInt index = 0;
        TUint32 priKey = 0;
        TUint32 secKey = 0;
        TInt err = KErrNone;  
        // Loop through all the combinations in order
        // to find wether this key is part of the locking
        // process or not (1st or 2nd key)      
        while( !returnValue )
            {
            err = iKeylockApi->GetKeyCombination(index, priKey, secKey);
            if ( err == KErrNone )
                {
                switch( aLockKey )
                    {
                    case EScutFirstLockKey:
                        returnValue = (priKey == aScanCode);
                        break;
                    case EScutSecondLockKey:
                        returnValue = (secKey == aScanCode);
                        break;
                    default:
                        returnValue = EFalse;
                        break;                        
                    }
                }               
            else // no more combinations
                {
                break;
                }
            ++index;
            }
        return returnValue;
        }
    switch( aLockKey )
        {
        case EScutFirstLockKey:
            returnValue = iFirstLockKey == aScanCode;
            break;
        case EScutSecondLockKey:
            returnValue = iSecondLockKey == aScanCode;
            break;
        default:
            returnValue = EFalse;
            break;
        }
    return returnValue;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsHidden(const TUid& aAppUid) const
{
    if (iHiddenApps.Find(aAppUid) == KErrNotFound)
    {
        return EFalse;
    }

	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::IsHidden (0x%x) ETrue"), aAppUid);
    return ETrue;
}

// ---------------------------------------------------------------------------
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleAiEventL(TInt aEvent, const TDesC& aParam)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleAiEventL( %d, '%S' ) alive = %d"), aEvent, &aParam, iPlugin.IsAlive());

    TUid appUid = KNullUid;
    iFirstLockKeyPressed = EFalse;
    iSoftkeyAppRunning = EFalse;

    if (aParam.Compare(KLeftSoftkey) == 0)
    {
        if ( IsLockKey( EStdKeyDevice0, EScutFirstLockKey) || IsLockKey( EStdKeyDevice0 , EScutSecondLockKey))
        {
            // Raise the first lock key pressed flag
            // only when this is raised, keylock skip causes scut launch
            iFirstLockKeyPressed = ETrue;
            appUid = SoftkeyUid(KLeftSoftkeyId);
        }
    }

    if (aParam.Compare(KRightSoftkey) == 0)
    {
        if ( IsLockKey( EStdKeyDevice1 , EScutFirstLockKey) || IsLockKey( EStdKeyDevice1 , EScutSecondLockKey))
        {
            // Raise the first lock key pressed flag
            // only when this is raised, keylock skip causes scut launch
            iFirstLockKeyPressed = ETrue;
            appUid = SoftkeyUid(KRightSoftkeyId);
        }
    }

    if (iFirstLockKeyPressed && appUid != KNullUid)
    {
        TApaTaskList taskList(iEnv->WsSession());
        TApaTask task(taskList.FindApp(appUid));

        if (task.Exists())
        {
        	if (IsHiddenFromFSW(appUid))
            	iSoftkeyAppRunning = EFalse;
        	else
        		iSoftkeyAppRunning = ETrue;	
        }
    }

    if (IsDelayRequired(appUid))
    {
        DelayedLaunchByIndexL( aParam );
        return;
    }

    if( iActiveCall && aParam.Compare( KRightSoftkey ) == 0 )
        {   
        ActivateTopMostApp();
        return;
        }

    switch( aEvent )
        {
        case EAiScutEventLaunchByIndex:
            HandleLaunchByIndexL(aParam);
            break;
    
        case EAiScutEventLaunchByValue:
            HandleLaunchByValueL(aParam);
            break;
    
        case EAiScutEventShowSettings:
            ShowSettingsL(aParam);
            break;
            
        case EAiScutEventShowSetting:
            ShowSettingL(aParam);
            break;
            
        case EAiScutEventLaunchFastswap:
            OpenFastSwap();
            break;
    
        case EAiScutEventLaunchByIndexAlternate:
            HandleLaunchByIndexAlternateL(aParam);
            break;
            
        default:
            break;
        }

}

// ---------------------------------------------------------------------------
// Called by the timer. Retries the access check for shortcuts.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::RetryAccessCheck()
{
    CheckAccessAndPublish(EScutCheckAll, iPublishAll);

    // One retry is enough.
    iTimer->Cancel();
    delete iTimer;
    iTimer = NULL;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsNonNative(const TUid& aUid) 
{
    TBool ret = EFalse;

    const TUid KMidletAppType = { 0x10210E26 };
    TUid typeuid = KNullUid;

    if (KErrNone == iApaSession.GetAppType(typeuid, aUid))
    {
        if (typeuid == KMidletAppType)
        {
            ret = ETrue;
        }
    }

#ifdef __WEB_WIDGETS
    if (IsWidget(aUid))
    {
        ret = ETrue;
    }
#endif

    return ret;
}

#ifdef __WEB_WIDGETS
TBool CAiScutEngine::IsWidget(const TUid& aUid)
{
    if( iWidgetRegistryConnected )
        {
        return iWidgetRegistry.IsWidget(aUid);
        }
    else
        {
        return EFalse;
        }
}
#endif

// ---------------------------------------------------------------------------
// Creates the default shortcuts.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::CreateShortcutsL()
{
    TInt scIndex = 0;

    iShortcuts.ResetAndDestroy();

    RArray<TUint32> keys;
    CleanupClosePushL(keys);
    // Find the keys that define theme-default shortcut settings.
    iRepository->FindL(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, keys);
    
    
    TLinearOrder<TUint32> order(CompareKey);
    keys.Sort(order);

    HBufC* buffer = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    if (iSettingsNotifier)
    {
        iSettingsNotifier->StopListening();
    }
    if (iHiddenAppsNotifier)
    {
        iHiddenAppsNotifier->StopListening();
    }

    for (TInt i = 0; i < keys.Count(); ++i)
    {
        TUint32 key = keys[i];
        if (iRepository->Get(key, bufferPtr) == KErrNone)
        {
            // Strip off the default-setting-bit, it's not part of the shortcut id.
            key &= KScutBitMaskThemeDefault;

            // Parse the icon from the URL and fetch the icon to our icon array
            // The actual icon => shortcut matching and overriding is done in 
            // CheckAccessAndPublishL() just before publishing                
            iParser.Parse(bufferPtr);
            TAiScutIcon icon = iParser.Icon();
            if ( icon.iType != EScutIconNone  )
                {
                AddOverrideIcon( icon );                
                }
            // Keys over 0x20000000 means that they are just icon overrides                      
            if ( !(key & KScutFlagBitIconOverride ) )
                {
                key &= KScutMaskAdditionalSetting;
                // Ignore possible errors during shortcut construction
                // and simply try to move on to the next shortcut.
                CAiScutShortcut* shortcut = NULL;
                TRAPD(err, shortcut = AiScutFactory::CreateAiScutShortcutL(key, bufferPtr, *this));
                if (err == KErrNone)
                {
                    // If shortcut is not accessible, replace it with platform default.
                    TBool access = EFalse;
                    if (shortcut)
                    {
                        // Append theme default to our array, incase of uninstall/reinstall/mem card removal
                        // restoration is required.
                        CAiScutShortcutInfo* shortcutInfo = NULL;
                        TRAP_IGNORE(shortcutInfo = CAiScutShortcutInfo::NewL(key | KScutFlagBitThemeDefault,
                                                                             bufferPtr));
                        if ( shortcutInfo )
                        {
                            iThemeShortcuts.Append(shortcutInfo);
                        }
                    
                        TScutDefault usedDefault = EScutDefaultToPlatform;
                        
                    	TRAP_IGNORE(access = shortcut->CheckAccessL(EScutCheckAll));
                    	if (!access)
                    	{
                            HBufC* target = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
                            TPtr targetPtr = target->Des();
                        	TUid   dummy;
    	
        	                // Get the default shortcut definition for index 'scIndex'.
            	            // uid is ignored.
                	        TAiDefaultShortcut::GetDefaultShortcut(key, dummy, targetPtr);
    	
        	                delete shortcut;
            	            shortcut = NULL;
                            TRAP(err, shortcut = AiScutFactory::CreateAiScutShortcutL(key, targetPtr, *this));

                	        if (err == KErrNone)
                        	{
                            	// Add the bit that indicates this is a default shortcut setting.
    	                        key |= KScutFlagBitThemeDefault;
        	                    iRepository->Set(key, targetPtr);
        	                    
        	                    // Try to locate a icon override from the default
        	                    // definition
        	                    iParser.Parse( targetPtr );
        	                    TAiScutIcon tIcon = iParser.Icon();
        	                    if ( tIcon.iType != EScutIconNone  )
        	                        {
        	                        AddOverrideIcon( tIcon );                
        	                        }
            	            }
                    	    CleanupStack::PopAndDestroy( target );
                	    }
                	    else
                	    {
                    	    usedDefault = EScutDefaultToTheme;
                	    }

                    	scIndex++;
    	
    	                if (shortcut) // This test ensures that the creation right above went well.
    	                {
    	                    iDefaultUsed.Append(usedDefault);
            	            err = iShortcuts.Append(shortcut);
            	            
                	        if (err != KErrNone)
                    	    {
                        	    delete shortcut;
                            	shortcut = NULL;
                        	}	
    	                }
                    }
                }
            }
        }
    }

    if (iSettingsNotifier)
    {
        iSettingsNotifier->StartListeningL();
    }
    if (iHiddenAppsNotifier)
    {
        iHiddenAppsNotifier->StartListeningL();
    }

    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PopAndDestroy(&keys);
}

// ---------------------------------------------------------------------------
// Check whether theme default sc was reinstalled.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::CheckForThemeDefaultReinstalledL()
{
    HBufC* buffer = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    if (iSettingsNotifier)
    {
        iSettingsNotifier->StopListening();
    }
    if (iHiddenAppsNotifier)
    {
        iHiddenAppsNotifier->StopListening();
    }

    TInt count = iThemeShortcuts.Count();
    for (TInt i = 0; (i < count) &&
                     (i < iDefaultUsed.Count()) &&
                     (i < iShortcuts.Count());
         ++i)
    {
        if (iDefaultUsed[i] == EScutDefaultToPlatform ||
            iDefaultUsed[i] == EScutUserDefined)
        {
            bufferPtr.Zero();
            CAiScutShortcut*& shortcut = iShortcuts[i];
            // Strip off the default-setting-bit, it's not part of the shortcut id.
            TUint32 scutKey = iThemeShortcuts[i]->Id() & KScutBitMaskThemeDefault;
            bufferPtr.Append(iThemeShortcuts[i]->Target());
            CAiScutShortcut* newScut = NULL;
            TRAPD(err, newScut = AiScutFactory::CreateAiScutShortcutL(scutKey, bufferPtr, *this));
    	    if (err == KErrNone)
          	{
                delete shortcut;
                shortcut = NULL;
                shortcut = newScut;
                // Add the bit that indicates this is a default shortcut setting.
                scutKey |= KScutFlagBitThemeDefault;
                iRepository->Set(scutKey, bufferPtr);
            }
            iDefaultUsed[i] = EScutDefaultToTheme;
            // Recheck access
            shortcut->CheckAccessL(EScutCheckAll);
        }
    }
    if (iSettingsNotifier)
    {
        iSettingsNotifier->StartListeningL();
    }
    if (iHiddenAppsNotifier)
    {
        iHiddenAppsNotifier->StartListeningL();
    }

    CleanupStack::PopAndDestroy(buffer);
}

// ---------------------------------------------------------------------------
// Merges the user defined shortcuts with the defaults and checks shortcut access.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::MergeShortcuts(TShortcutType aType, TBool aRecreateAll)
{
    TRAP_IGNORE(MergeShortcutsL(aType, aRecreateAll));
}

// ---------------------------------------------------------------------------
// Merges the user defined shortcuts with the defaults.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::MergeShortcutsL(TShortcutType aType, TBool aRecreateAll)
{
    RArray<TUint32> keys;
    CleanupClosePushL(keys);

    // Find the keys that define user defined shortcut settings.
    iRepository->FindL(KScutCenRepKeyUserDefined, KScutCenRepKeyMask, keys);

    HBufC* buffer = HBufC::NewLC(
        NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    for (TInt i = 0; i < iShortcuts.Count(); ++i)
    {
        CAiScutShortcut* shortcut = iShortcuts[i];
        if (aType == EScutAnyType || shortcut->Type() == aType)
        {

            TInt keyIndex = keys.Find(shortcut->Id());
            if (keyIndex != KErrNotFound)
            {
                // Set the new user target.
                if (keys.Count() > keyIndex && keyIndex >= 0 &&
                    iRepository->Get(keys[keyIndex], bufferPtr) == KErrNone)
                {
                    iParser.Parse(bufferPtr);
                    TAiScutIcon icon = iParser.Icon();
                    if ( icon.iType != EScutIconNone  )
                        {
                        AddOverrideIcon( icon );
                        }
                    if ( aRecreateAll ||
                            buffer->CompareC( shortcut->ActiveDefinition() ) != 0 )
                        {
                        shortcut->DeleteUserTarget();                    
                        shortcut->SetUserTarget(bufferPtr);                       
                        }
                    if (i < iDefaultUsed.Count())
                    {
                        iDefaultUsed[i] = EScutUserDefined;
                    }
                }
            }
        }
    }

    CleanupStack::PopAndDestroy(buffer);
    CleanupStack::PopAndDestroy(&keys);
}

// ---------------------------------------------------------------------------
// Checks shortcut accessibility.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::CheckAccessAndPublish(TInt aCheckType, TBool aPublishAll)
{
    TRAPD(err, CheckAccessAndPublishL(aCheckType, aPublishAll));

    if (err == KErrNotReady)
    {
        // Access check failed, create a timer to try again later.
        iTimer = CPeriodic::New(CActive::EPriorityStandard);
        iTimer->Start(KScutAccessCheckRetryDelay, KScutAccessCheckRetryDelay,
            TCallBack(DelayedCheckCallBack));
    }
}

// ---------------------------------------------------------------------------
// Tells each shortcut to check whether or not its target is accessible.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::CheckAccessAndPublishL(TInt aCheckType, TBool aPublishAll)
{
	__PRINTS("XAI: CheckAccessAndPublishL");
    if (!iMsvSession && IsMsvSessionNeeded())
    {
        iMsvSession = CMsvSession::OpenAsObserverL(*this);
    }

    if (!iBookmarkDbObserver && IsBookmarkObserverNeeded())
    {
        iBookmarkDbObserver =
            new (ELeave) CActiveFavouritesDbNotifier(iBookmarkDb, *this);
        if (! iBookmarkDbObserver->IsActive())
        {
        	iBookmarkDbObserver->Start();	
        }        
    }

    iPublishAll = aPublishAll;

    TInt count = iShortcuts.Count();
    for (TInt i = 0; i < count; ++i)
    {
        CAiScutShortcut*& shortcut = iShortcuts[i];

        // Check shortcut access and publish it if its target has changed.
        // When the plug-in is being resumed, all shortcuts are published.
        // Non-visible and locked shortcuts are never published.
        if ( !shortcut->CheckAccessL(aCheckType) )
        {
            HBufC* target = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
            TPtr targetPtr = target->Des();
        	TUid dummy;
            // Get the default shortcut definition from cenrep using key 'key'.
            // uid is ignored.
        	TUint32 key = shortcut->Id();
	        TAiDefaultShortcut::GetDefaultShortcut(key, dummy, targetPtr);
            delete shortcut;
            shortcut = NULL;
            TRAPD(err, shortcut = AiScutFactory::CreateAiScutShortcutL(key, targetPtr, *this));
    	    if (err == KErrNone)
        	{
            	// Add the bit that indicates this is a default shortcut setting.
                key |= KScutFlagBitThemeDefault;
                iRepository->Set(key, targetPtr);
                
                // Try to locate a icon override from the default
                // definition
                iParser.Parse( targetPtr );
                TAiScutIcon tIcon = iParser.Icon();
                if ( tIcon.iType != EScutIconNone  )
                    {
                    AddOverrideIcon( tIcon );                
                    }
        	}
    	    CleanupStack::PopAndDestroy( target );
            if (i < iDefaultUsed.Count())
                {
                iDefaultUsed[i] = EScutDefaultToPlatform;
                }
            // Recheck access
            shortcut->CheckAccessL(aCheckType);
        }

        TBool targetChanged = shortcut->IsTargetChanged();
        TInt32 id = shortcut->Id();
        TBool nonVisible = (0 != (id & KScutFlagBitNonVisible));
        
        if (aCheckType == EScutCheckBkm && shortcut->Type() == EScutBookmark)
        {
            targetChanged = ETrue;
        }

        shortcut->SetToBePublished(
            (targetChanged || iPublishAll) &&
            !nonVisible 
            );

         // Assign overridden icons to shortcuts if needed
         for ( TInt j = 0; j < iIcons.Count(); j++)
             {         
             // Check that the appuid and type matches
             if ( shortcut->AppUid() == iIcons[j].iAppUid &&
                  shortcut->Type() == iIcons[j].iShortcutType )
                {   
                // We need to check also the view id / bkm id if present or otherwise all bookmarks
                // app views would be overridden           
                if ( iIcons[j].iViewId.iUid <= 0 || 
                     iIcons[j].iViewId == shortcut->AdditionalUid() )
                    {
                    shortcut->SetIcon(iIcons[j]);
                    }          
                }
            }

		__PRINT( __DBG_FORMAT( "XAI:   id = 0x%x, type = %d, publish = %d"), 
			shortcut->Id(), shortcut->Type(), shortcut->NeedsToBePublished());
    }

    iPlugin.PublishShortcutsL(iShortcuts);
}

// ---------------------------------------------------------------------------
// Finds the shortcut object with the given id.
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::FindShortcutIndex(TInt32 aId)
{
    for (TInt i = 0; i < iShortcuts.Count(); ++i)
    {
        if (iShortcuts[i]->Id() == aId)
        {
            return i;
        }
    }

    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// Handles the shortcut launch by index.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleLaunchByIndexL(const TDesC& aParam)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleLaunchByIndexL ('%S')"), &aParam);
    if (aParam.Length() > 0)
    {
        _LIT(KHexPrefix, "0x");
        const TInt prefixLen = 2;

        TRadix radix(EDecimal);
        TPtrC ptr(aParam);

        if (aParam.Left(prefixLen).CompareC(KHexPrefix) == 0)
        {
            // Strip the '0x' prefix.
            ptr.Set(aParam.Right(aParam.Length() - prefixLen));

            radix = EHex;
        }

        TLex lexer(ptr);
        TUint32 id;
        if (lexer.Val(id, radix) == KErrNone)
        {
            TInt idx = FindShortcutIndex(id);
            if (idx != KErrNotFound && idx >= 0 && iShortcuts.Count() > idx)
            {
                iShortcuts[idx]->LaunchL();
            }
        }
    }
}
// ---------------------------------------------------------------------------
// Handles the shortcut launch by index.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleLaunchByIndexAlternateL(const TDesC& aParam)
/**
 * These are the cases at the moment that are handled here.
 * 1) Appshell targeted => Fastswap opened
 */
{
    __PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleLaunchByIndexAlternateL ('%S')"), &aParam);
    TInt index = IntFromDesc( aParam );
    TInt idx = FindShortcutIndex( index );   
    if (idx != KErrNotFound && idx >= 0 && iShortcuts.Count() > idx)
        {
        CAiScutShortcut *scut = iShortcuts[idx];
        // Open fastswap in case appshell was targeted with alternate launch
        if ( scut->AppUid() == KScutAppShellUid )
            {
            OpenFastSwap();
            return;
            }
        // add other special cases here
        /*if ( index == EAiScutSoftKeyLeft )
            {
            // do something
            }*/
    }
}

// ---------------------------------------------------------------------------
// Handles the shortcut launch by value.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleLaunchByValueL(const TDesC& aParam)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleLaunchByIndexL ('%S')"), &aParam);

    if (aParam.Length() > 0)
    {
        CAiScutShortcut* shortcut = AiScutFactory::CreateAiScutShortcutLC(0x0, aParam, *this);
        shortcut->CheckAccessL(EScutCheckAll);
        shortcut->LaunchL();
        CleanupStack::PopAndDestroy(shortcut);
    }
}

// ---------------------------------------------------------------------------
// Shows the plug-in settings dialog.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::ShowSettingsL(const TDesC& aParam)
{
    __PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::ShowSettingsL ('%S')"), &aParam);
    
    _LIT(KGeneralSettings, "localapp:0x100058EC?view=0x102750FC");
    	        
    CAiScutShortcut* shortcut = CAiScutShortcut::NewLC(0x0, KGeneralSettings, *this);
    shortcut->CheckAccessL(EScutCheckAll);

    if (aParam.Length() > 0)
        {
        HBufC8* param = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aParam);
        CleanupStack::PushL(param);
        
        const TDesC8& msg(*param);
        
        shortcut->LaunchL(msg);
        
        CleanupStack::PopAndDestroy(param);
        }
    else
        {
        shortcut->LaunchL(KNullDesC8);
        }        
        
                    
    CleanupStack::PopAndDestroy(shortcut);        
}

// ---------------------------------------------------------------------------
// Shows the plug-in setting.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::ShowSettingL(const TDesC& aParam)
{
    __PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::ShowSettingL ('%S')"), &aParam);
    
    _LIT(KGeneralSettings, "localapp:0x100058EC?view=0x102750FC");
                
    CAiScutShortcut* shortcut = CAiScutShortcut::NewLC(0x0, KGeneralSettings, *this);
    shortcut->CheckAccessL(EScutCheckAll);

    if (aParam.Length() > 0)
        {
        HBufC8* param = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aParam);
        CleanupStack::PushL(param);
        
        HBufC8* param2 = HBufC8::NewLC(param->Des().Length()+KScutDirectOpen.iTypeLength);
        param2->Des().Copy(param->Des());
        param2->Des().Append(KScutDirectOpen);
        const TDesC8& msg(*param2);
        
        shortcut->LaunchL(msg);
        
        CleanupStack::PopAndDestroy(param2);
        CleanupStack::PopAndDestroy(param);
        }
    else
        {
        shortcut->LaunchL(KNullDesC8);
        }        
        
                    
    CleanupStack::PopAndDestroy(shortcut);       
}

// ---------------------------------------------------------------------------
// Opens the fastswap window
// ---------------------------------------------------------------------------
//
void CAiScutEngine::OpenFastSwap()
{
    RAknUiServer* uiServer = CAknSgcClient::AknSrv();
    if ( uiServer )
        {
        uiServer->MakeTaskListVisible( ETrue );
        }    
}



// ---------------------------------------------------------------------------
// Handles PS commands from WSPlugin.
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::HandlePSCommand(TAny* aAny)
{
    CAiScutEngine* self = reinterpret_cast< CAiScutEngine* >(aAny);
    TInt err = KErrNone;
    if (self)
    {
    	TRAP(err, self->DoHandlePSCommandL());	
    }
    
    return err;
}

// ---------------------------------------------------------------------------
// Handles call state changes.
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::CallStateChangeCallback(TAny* /*aAny*/)
{
    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUid CAiScutEngine::SoftkeyUid(TUint32 aSoftkeyId)
{
    TUid uid = KNullUid;

    TInt idx = FindShortcutIndex(aSoftkeyId);
    if (idx != KErrNotFound && idx >= 0 && iShortcuts.Count() > idx)
    {
        uid = iShortcuts[idx]->AppUid();

	__PRINT( __DBG_FORMAT( "XAI: SoftkeyUid %d app uid = 0x%x"), idx, uid.iUid);
    }

    return uid;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsDelayRequired(TUid aAppUid)
{
    // softkey delay is reuired for these applications because of problems
    // in killing them immediatly after keylock activation.
    if (aAppUid.iUid == KScutBrowserUidValue      ||
        aAppUid.iUid == KScutOperatorMenuUidValue ||
        aAppUid.iUid == KScutVideoServicesUidValue ||
        IsNonNative( aAppUid )
        )
    {
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// Handles PS commands from WSPlugin.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::DoHandlePSCommandL()
{
    TBuf<RProperty::KMaxPropertySize> command;
    TUint32 lockKeyId = 0;
    iKeyEventObserver->Get(command);

	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::DoHandlePSCommandL command = '%S'"), &command);

    TUid appUid = KNullUid;

    if (iFirstLockKey == EStdKeyDevice0)
    {
        appUid = SoftkeyUid(KLeftSoftkeyId);
        lockKeyId = KLeftSoftkeyId;
    }

    if (iFirstLockKey == EStdKeyDevice1)
    {
        appUid = SoftkeyUid(KRightSoftkeyId);
        lockKeyId = KRightSoftkeyId;
    }

    if (iFirstLockKeyPressed && command.Compare(KAiPSEnableKeyLock) == 0)
    {
        if (appUid != KNullUid)
        {
            TBool isDialog = EFalse;
            TInt idx = 0;
            delete iScutShutter;
            iScutShutter = NULL;
            
            // In case new message shortcut in lockkey
            // we need to dismiss the dialog 
            if ( lockKeyId > 0 )
                {
                idx = FindShortcutIndex( lockKeyId );
                if (idx != KErrNotFound)
                    {
                    switch (iShortcuts[idx]->Type())
                        {
                        case EScutNewMsgType:
                            isDialog = ETrue;
                            break;
                        default:
                            isDialog = EFalse;
                            break;                          
                        }                                                
                    }

                }                    
            iDelayTimer->Cancel();
            iScutShutter = CAiScutTargetShutter::NewL(iEnv, appUid, iSoftkeyAppRunning, isDialog);
            iScutShutter->StartL();
        }
    }

    if (command.Compare(KAiPSSkipKeyLock) == 0)
    {
        iDelayTimer->Cancel();
        return ;
    }
    else if (command.Compare(KAiPSKeyLockTimeout) == 0)
    {
        if (iFirstLockKeyPressed)
        {
            iFirstLockKeyPressed = EFalse;

            if (iFirstLockKey == EStdKeyDevice0)
            {
                if (IsDelayRequired(appUid))
                {
                    HandleLaunchByIndexL(KLeftSoftkey);
                }
                return ;
            }

            if (iFirstLockKey == EStdKeyDevice1)
            {
                if (IsDelayRequired(appUid))
                {
                    HandleLaunchByIndexL(KRightSoftkey);
                }
                return ;
            }
        }
        return ;
    }

    if (FeatureManager::FeatureSupported(KFeatureIdKeypadNoVoiceKey) &&
        command.Compare(KAiPSSkipNameDialer) == 0)
    {
        HandleLaunchByIndexL(KRightSoftkey);
        return ;
    }

    HandleLaunchByValueL(command);
}

// ---------------------------------------------------------------------------
// Handles call state changes.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleCallStateChange()
{
    TInt value = 0;
    TInt err = iCallStateObserver->Get(value);
    iActiveCall = (value > EPSCTsyCallStateNone) && err == KErrNone;

	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleCallStateChange = %d"), value);
	__PRINT( __DBG_FORMAT( "XAI:   iActiveCall = %d"), iActiveCall);

    for (TInt i = 0; i < iShortcuts.Count(); ++i)
    {
        iShortcuts[i]->SetCallState(iActiveCall);
    }
}

// ---------------------------------------------------------------------------
// Activates TopMost Application
// ---------------------------------------------------------------------------
//
void CAiScutEngine::ActivateTopMostApp()
{
    // Phone Topmost app as default
    TInt idleAppUid   = 0;
    TInt topMostAppId = 0;

    RProperty::Get(KPSUidAiInformation, KActiveIdleUid, idleAppUid);

    RProperty property;
    TInt err = property.Attach(KPSUidUikon, KUikVideoCallTopApp);

    if (err == KErrNone)
    {
        property.Get(topMostAppId);
    }

    property.Close();

	__PRINT( __DBG_FORMAT( "XAI: ActivateTopMostApp idle uid = 0x%x topmost = 0x%x"), idleAppUid, topMostAppId);

    TUid appId(TUid::Uid(topMostAppId));
    
    if(appId == KVideoCallUid)
    {
	    const TVwsViewId viewId(appId, appId);
	    err = iVwsSession->CreateActivateViewEvent(viewId, KNullUid, KNullDesC8());

		__PRINT( __DBG_FORMAT( "XAI:   CreateActivateViewEvent = %d"), err);	
    }
    else
    {
    	const TVwsViewId viewId(appId, KVoiceCallUidViewId);
	    err = iVwsSession->CreateActivateViewEvent(viewId, KVoiceCallUidViewId, KNullDesC8());

		__PRINT( __DBG_FORMAT( "XAI:   CreateActivateViewEvent = %d"), err);	
    }

}

// ---------------------------------------------------------------------------
// From class MFavouritesDbObserver.
// Handles database event.
// ---------------------------------------------------------------------------
//
void CAiScutEngine::HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutEngine::HandleFavouritesDbEventL aEvent = %d"), aEvent);

    if (aEvent == RDbNotifier::ECommit)
    {
        MergeShortcuts(EScutBookmark, ETrue);
        CheckAccessAndPublish(EScutCheckBkm, EFalse);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsMsvSessionNeeded()
{
    TInt count = iShortcuts.Count();

    for (TInt i = 0; i < count; ++i)
    {
        CAiScutShortcut* shortcut = iShortcuts[i];
        TShortcutType type = shortcut->Type();

        if (type == EScutNewMessage    ||
            type == EScutNewEmail      ||
            type == EScutNewSyncMLMail ||
            type == EScutNewPostcard   ||
            type == EScutNewAudioMsg   ||
            type == EScutNewMsgType    ||
            type == EScutMailbox)
        {
            return ETrue;
        }
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsBookmarkObserverNeeded()
{
    TInt count = iShortcuts.Count();

    for (TInt i = 0; i < count; ++i)
    {
        CAiScutShortcut* shortcut = iShortcuts[i];
        if (shortcut->Type() == EScutBookmark)
        {
            return ETrue;
        }
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::GetAppTitle(
    const TUid& aAppUid, const TUid& aViewId, TDes& aTitle, TAiScutAppTitleType aType)
{
    TBool ret = EFalse;
    TInt count = iAppTitleList.Count();

    for (TInt i = 0; i < count; i++)
    {
        TAiScutAppTitleEntry entry = iAppTitleList[i];

        if (entry.iAppUid == aAppUid && entry.iViewId == aViewId)
        {
            switch (aType)
            {
            case EAiScutLongTitle:
                if (entry.iLongTitle)
                {
                    aTitle = *entry.iLongTitle;
                    ret = ETrue;
                }
                break;

            case EAiScutShortTitle:
                if (entry.iShortTitle)
                {
                    aTitle = *entry.iShortTitle;
                    ret = ETrue;
                }
                break;

            case EAiScutSkeyTitle:
                if (entry.iSkeyTitle)
                {
                    aTitle = *entry.iSkeyTitle;
                    ret = ETrue;
                }
                break;

            case EAiScutMskTitle:
                if (entry.iMskTitle)
                {
                    aTitle = *entry.iMskTitle;
                    ret = ETrue;
                }
                break;
            default :
            	break;
            }

            break; // break the for loop
        }
    }

	if (! ret) __PRINT( __DBG_FORMAT( "XAI: GetAppTitle NOT found for uid = 0x%x"), aAppUid);

    return ret;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngine::LoadAppTitleListL()
{
    TResourceReader reader;
    iEnv->CreateResourceReaderLC(reader, R_SCUT_APP_TITLE_LIST);

    TInt items = reader.ReadInt16();

	__PRINT( __DBG_FORMAT( "XAI: LoadAppTitleListL items = %d"), items);

    TUid appuid;
    TUid viewid;

    for (TInt i = 0; i < items; i++)
    {
        appuid.iUid = reader.ReadInt32();
        viewid.iUid = reader.ReadInt32();

        HBufC16* longTitle = reader.ReadHBufC16L();
        CleanupStack::PushL(longTitle);

        HBufC16* shortTitle = reader.ReadHBufC16L();
        CleanupStack::PushL(shortTitle);

        HBufC16* skeyTitle = reader.ReadHBufC16L();
        CleanupStack::PushL(skeyTitle);

        HBufC16* mskTitle = reader.ReadHBufC16L();
        CleanupStack::PushL(mskTitle);

        TAiScutAppTitleEntry entry;

        entry.iAppUid     = appuid;
        entry.iViewId     = viewid;
        entry.iLongTitle  = longTitle;
        entry.iShortTitle = shortTitle;
        entry.iSkeyTitle  = skeyTitle;
        entry.iMskTitle   = mskTitle;

        User::LeaveIfError(iAppTitleList.Append(entry));

        // Title names are deleted in destructor.
        CleanupStack::Pop(4, longTitle);
    }

    CleanupStack::PopAndDestroy(); // reader
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngine::GetHiddenAppsL()
{
    HBufC* buffer = HBufC::NewLC(
        NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    iHiddenAppsRepository->Get(KMenuHideApplication, bufferPtr);

	__PRINT( __DBG_FORMAT( "XAI: CAiScutSettingsModel::GetHiddenAppsL '%S' "), buffer);

    iHiddenApps.Reset();
    TAiScutAppUidParser uidParser(bufferPtr, iHiddenApps);
    uidParser.ParseL();

    CleanupStack::PopAndDestroy(buffer);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngine::IsHiddenFromFSW( const TUid& aAppUid )
{
	const TUid KPSUidUikon = { 0x101F8773 };
 	const TUint32 KUikAppHiddenList  = 0x00000010;
 	const TInt KMaxHiddenApps = 25;

	TBuf16 <2*KMaxHiddenApps> retrievedList;
    TInt err = RProperty::Get( KPSUidUikon, KUikAppHiddenList,
retrievedList );
    TInt i = 0;

	TUint32 listValue;
	while( i < KMaxHiddenApps && KErrNone == err )
    {
    	// 32-bit uid values are retrieved in two 16-bit parts
    	listValue = retrievedList[2*i] << 16;
  		listValue += retrievedList[2*i+1];
   
    	if ( listValue )
    	{
        	TUid t ( KNullUid );
        	t.iUid = listValue;
        	if( t == aAppUid )
        	{
        		return ETrue;	
        	}
   		}
    	else
        {
        	err = KErrEof;
        	return EFalse;                
        }
    	i++;
    }
    return EFalse;
} 
// ---------------------------------------------------------------------------
// Adds an override icon to iIcons array in case the same icon 
// isn't there already
// ---------------------------------------------------------------------------
//
TInt CAiScutEngine::AddOverrideIcon( TAiScutIcon &aIcon )
    {
    TBool foundExisting = EFalse;
    
    for ( TInt i = 0; i < iIcons.Count(); i++)
        {
        if ( aIcon.iAppUid == iIcons[i].iAppUid &&
             aIcon.iShortcutType == iIcons[i].iShortcutType && 
             aIcon.iViewId == iIcons[i].iViewId &&
             aIcon.iDestination == iIcons[i].iDestination )             
            {
            foundExisting = ETrue;
            break;
            }
        }
    if ( !foundExisting )
        {
        return iIcons.Append( aIcon );
        }
    return KErrNone;
    }
// End of File.
