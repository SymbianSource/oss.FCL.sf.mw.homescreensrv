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
* Description:  Application list for settings listbox
*
*/


#include <msvuids.h>        // For KMsvRootIndexEntryIdValue
#include <SenduiMtmUids.h>
#include <StringLoader.h>

#include <aiscutsettingsres.rsg>
#include <aiscuttexts.rsg>

#include <featmgr.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <javaregistry.h>
#include <javaregistryentry.h>
#include <pathinfo.h>

#include "caiscutsettingsapplist.h"
#include "taiscutparser.h"
#include "caiscutsettingsitem.h"
#include "aiscutdefs.h"
#include <activeidle2domaincrkeys.h>

#include "debug.h"

using namespace Java;


const TInt KDriveAndColon = 2; //drive char and colon string length, e.g. "c:"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList::CAiScutSettingsAppList(CCoeEnv* aEnv, MAiScutListObserver& aObserver)
    : CActive(CActive::EPriorityLow)    // Background task priority.
                                        // Higher priority would block the wait dialog.
    , iEnv(aEnv)
    , iObserver(aObserver)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::ConstructL()
    {
    User::LeaveIfError(iApaSession.Connect());
    iMsvSession = CMsvSession::OpenAsObserverL(*this);

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList* CAiScutSettingsAppList::NewL(CCoeEnv* aEnv,
    MAiScutListObserver& aObserver)
    {
    CAiScutSettingsAppList* self = new (ELeave) CAiScutSettingsAppList(aEnv, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList::~CAiScutSettingsAppList()
    {
    if (IsActive())
        {
        Cancel();
        }

    delete iAppNotifier;
    delete iMsvSession;
    iApaSession.Close();
    iListItems.ResetAndDestroy();
    iMailboxes.Close(); // Do not call ResetAndDestroy() to this.
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::MdcaCount() const
    {
    return iListItems.Count();
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsAppList::MdcaPoint(TInt aIndex) const
    {
    if (aIndex < 0 || aIndex >= iListItems.Count())
        {
        TPtrC ret(KNullDesC);
        return ret;
        }
    return iListItems[aIndex]->Caption();
    }

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    TBool updated = ETrue;
    TBool added   = EFalse;

    switch (aEvent)
        {
    case EMsvEntriesCreated:
    case EMsvEntriesDeleted:
    case EMsvEntriesChanged:
        if (iObserving)
            {
            CMsvEntry* rootEntry = GetRootEntryL();
            
            CleanupStack::PushL(rootEntry);
            TBuf<255> mailboxId;
            
            // Reset iMailboxes
            for (TInt j = iMailboxes.Count() - 1; j >= 0; --j)
            {            
              RemoveMailboxL(iMailboxes[j]->Params());
            }

      // Add all mailboxes
      for (TInt i = rootEntry->Count() - 1; i >= 0; --i)
                {
                    const TMsvEntry& tentry = (*rootEntry)[i];

              if (tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid)
                {   
                  mailboxId.Num(tentry.Id());       
                        AddMailboxL(tentry.iDetails, mailboxId);
                    }                      
              }      
              
            CleanupStack::PopAndDestroy(rootEntry);
            }

        if (updated)
            {
            iObserver.HandleScutListEventL(MAiScutListObserver::EAppListUpdated, added);
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
void CAiScutSettingsAppList::HandleAppListEvent(TInt /*aEvent*/)
    {
    TRAP_IGNORE(UpdateAppListL());
    }

// ---------------------------------------------------------------------------
// Starts the asynchronous application list initialization.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::StartL()
    {
    iListItems.ResetAndDestroy();

    AddStaticItemsL();
    AddMailboxesL();
    AddExtraItemsFromCRL();

    if (iApaSession.GetAllApps() == KErrNone &&
        iApaSession.AppCount(iTotalAppCount) == KErrNone)
        {
        CompleteSelf();
        }
    }

// ---------------------------------------------------------------------------
// Checks if the application list is ready to be shown.
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsAppList::IsReady() const
    {
    return iReady;
    }

// ---------------------------------------------------------------------------
// Tells the application list to start or stop observing for changes.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::SetObservingL(TBool aObserving)
    {
    delete iAppNotifier;
    iAppNotifier = NULL;

    if (aObserving)
        {
        UpdateAppListL();

        iAppNotifier = CApaAppListNotifier::NewL(this, CActive::EPriorityStandard);
        }

    iObserving = aObserving;
    }

// ---------------------------------------------------------------------------
// Finds the index of the given settings item in the application list.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::FindIndex(CAiScutSettingsItem& aItem)
    {
    TInt index = KErrNotFound;

    TAiScutParser parser;
    parser.Parse(aItem.Value());
    TPtrC params(parser.Get(EScutDefParamNameAndValue));

    TUid uid = aItem.Uid();
    for (TInt i = iListItems.Count() - 1; i >= 0; --i)
        {
        CAppListItem* item = iListItems[i];
        if ( (item->Uid() == uid) && params.Compare(item->Params()) == 0)
        {
            index = i;
            break;
        }
        // Url => compare the whole url not just the parameters
        else if( item->Type() == EAiScutSettingTypeUrl )
            {
            if ( parser.Get(EScutDefComplete).Compare(item->Params()) == 0 )
                {
                index = i;
                break;
                }
            }
        }

    return index;
    }

// ---------------------------------------------------------------------------
// Returns target application data from the given index.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::GetDataByIndex(TInt aIndex, TUid& aUid,
    TPtrC& aParams, TPtrC& aCaption, TAiScutSettingType& aType ) const
    {
    if (aIndex >= 0 && aIndex < iListItems.Count())
        {
        CAppListItem* item = iListItems[aIndex];
        aUid = item->Uid();
        aType = item->Type();
        aParams.Set(item->Params());
        aCaption.Set(item->Caption());
        return KErrNone;
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From CActive
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// From CActive
// Performs one step of the app list initialization.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::RunL()
    {
    TApaAppInfo appInfo;
    TInt err = GetNextValidApp(appInfo);

    switch (err)
        {
    case KErrNone:
        AddApplicationL(appInfo);
        CompleteSelf();
        break;

    case RApaLsSession::ENoMoreAppsInList:
        iReady = ETrue;
        iObserver.HandleScutListEventL(MAiScutListObserver::EAppListReady, EFalse);
        break;

    //Indicates that an RApaLsSession member function was called before the session's 
    //  application list is fully populated. 
    case RApaLsSession::EAppListInvalid:
        StartL(); // Just try again.
        break;

    default:
        User::LeaveIfError(err);
        break;
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles an error situation.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::RunError(TInt aError)
    {
    return aError;
    }

// ---------------------------------------------------------------------------
// Completes own request status to make sure active scheduler calls RunL again.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::CompleteSelf()
    {
    if (!IsActive())
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// Adds an application to the list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddApplicationL(TApaAppInfo& aAppInfo)
    {
    TBool allowed = ETrue;

    // EasyVoIP is only added when VoIP is supported
    if (aAppInfo.iUid == KScutEasyVoIPApplicationUid)
        {
        // Check VoIP variation status
        TInt voipStatus = 0;
        // For checking VoIP variation status
        CRepository* repository = CRepository::NewL(KCRUidTelephonySettings);
        repository->Get(KDynamicVoIP, voipStatus);
        delete repository;
        allowed = (voipStatus && FeatureManager::FeatureSupported(KFeatureIdCommonVoip));

        __PRINTS( "XAI: CAiScutSettingsAppList::AddApplicationL");
        __PRINT( __DBG_FORMAT( "XAI:   voipStatus = %d, allowed = %d"), voipStatus, allowed);
        }

    // VoIP launcher is always invisible in shortcuts.
    if (aAppInfo.iUid == KScutVoIPLauncherUid)
        {
        allowed = EFalse;
        }

    if (iObserver.IsHidden(aAppInfo.iUid))
        {
        allowed = EFalse;
        }

    if (allowed)
        {
        CAppListItem* listItem = CAppListItem::NewLC(aAppInfo.iUid, aAppInfo.iCaption);
        listItem->SetType( EAiScutSettingTypeApplication );
        TLinearOrder<CAppListItem> sortMethod(CAppListItem::CompareCaption);
        User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
        CleanupStack::Pop(listItem);
        }
    }

// ---------------------------------------------------------------------------
// Adds the static list items to the application list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddStaticItemsL()
    {
    HBufC* caption = NULL;
    CAppListItem* listItem = NULL;
    TLinearOrder<CAppListItem> sortMethod(CAppListItem::CompareCaption);

    // Connectivity Status item
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_CONNECTIVITY_STATUS, iEnv);
    listItem = CAppListItem::NewLC(KScutGeneralSettingsUid, *caption);
    listItem->SetParamsL(KScutParamConnectivityView);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;

    // New Message item.
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NEW_MSG, iEnv);
    listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
    listItem->SetParamsL(KScutParamNewMsg);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;

    // New Email item.
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NEW_EMAIL, iEnv);
    listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
    listItem->SetParamsL(KScutParamNewEmail);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;

#ifdef __SYNCML_DS_EMAIL
    // New SyncML mail item.
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NEW_SYNCML_MAIL, iEnv);
    listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
    listItem->SetParamsL(KScutParamNewSyncMLMail);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;
#endif

    if (FeatureManager::FeatureSupported(KFeatureIdMmsPostcard)
        && IsInRom( KScutPostcardEditorUid ) )
        {
        // New Postcard item.
        caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NEW_POSTCARD, iEnv);
        listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
        listItem->SetParamsL(KScutParamNewPostcard);
        listItem->SetType( EAiScutSettingTypeApplication );
        User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
        CleanupStack::Pop(listItem);
        CleanupStack::PopAndDestroy(caption);
        caption = NULL;
        listItem = NULL;
        }

    if (FeatureManager::FeatureSupported(KFeatureIdAudioMessaging))
        {
        // New Audio Message item.
        caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NEW_AUDIO_MSG, iEnv);
        listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
        listItem->SetParamsL(KScutParamNewAudioMsg);
        listItem->SetType( EAiScutSettingTypeApplication );
        User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
        CleanupStack::Pop(listItem);
        CleanupStack::PopAndDestroy(caption);
        caption = NULL;
        listItem = NULL;
        }

    // Select Message type item.
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_SELECT_MSG_TYPE, iEnv);
    listItem = CAppListItem::NewLC(KScutMessagingUid, *caption);
    listItem->SetParamsL(KScutParamSelectMsgType);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;
    // Installation view.
    caption = StringLoader::LoadLC(R_SCUT_SETTINGS_APPMNGR, iEnv); 
    listItem = CAppListItem::NewLC(KScutGeneralSettingsUid, *caption);
    listItem->SetParamsL(KScutParamInstallationsView);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    caption = NULL;
    listItem = NULL;
    }

// ---------------------------------------------------------------------------
// Adds extra list items to the application list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddExtraItemsL()
    {
    // No Effect item.
    HBufC* caption = StringLoader::LoadLC(R_SCUT_SETTINGS_NO_EFFECT, iEnv);
    CAppListItem* listItem = CAppListItem::NewLC(KScutSettingsDllUid, *caption);
    listItem->SetParamsL(KScutParamNoEffect);
    listItem->SetType( EAiScutSettingTypeApplication );
    User::LeaveIfError(iListItems.Insert(listItem, 0)); // insert as first item.
    CleanupStack::Pop(listItem);
    CleanupStack::PopAndDestroy(caption);
    }

void CAiScutSettingsAppList::AddExtraItemsFromCRL()
    {
    CRepository *cr = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );        
    User::LeaveIfNull( cr );
    
    CleanupStack::PushL( cr );
    
    TUint32 crKey = KAIExtraShortcutsKeyRangeStart;
    TBool moreSettings = ETrue;
    TInt err = KErrNone;
    TAiScutParser parser;
    
    HBufC* settingValue = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr settingValuePtr = settingValue->Des();
    
    while( moreSettings )
        {
        err = cr->Get( crKey++, settingValuePtr ); 
        if ( err == KErrNone && settingValue->Length() > 0 )
            {
            CAppListItem* listItem = NULL;
            parser.Parse( *settingValue );
            
            // First try to find the custom title
            HBufC* caption = HBufC::NewLC( settingValue->Length() );
            TPtr captionPtr = caption->Des();
            TInt err = parser.CustomTitle( captionPtr );    
            TUid appUid = parser.Uid();
                
            if ( err != KErrNone || caption->Length() <= 0 )
                {
                CleanupStack::PopAndDestroy( caption );
                caption = NULL;
                }
            
            if ( !caption )
                {
                // Web address...
                 if ( parser.Type() == EScutWebAddress || parser.Type() == EScutUnknown )
                     {
                     TUriParser parser;
                     TInt err = parser.Parse(*settingValue);

                     if (err == KErrNone)
                         {
                         // Remove scheme from the url.
                         caption = parser.Extract(EUriHost).AllocLC();
                         }
                     else
                         {
                         caption = settingValue->AllocLC();
                         }
                     }
                 //  ...application. Nothing else supported at the moment
                 else
                     {
                     TApaAppInfo appInfo;
                     err = iApaSession.GetAppInfo(appInfo, appUid);
                     if (err == KErrNone)
                         {
                         caption = appInfo.iCaption.AllocLC();
                         }
                     }
                }
            if ( caption )
                {
                listItem = CAppListItem::NewLC( KNullUid , *caption  );
                listItem->SetParamsL( *settingValue );
                listItem->SetType( EAiScutSettingTypeUrl );
                TLinearOrder<CAppListItem> sortMethod(CAppListItem::CompareCaption);
                User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
                CleanupStack::Pop( listItem );
                CleanupStack::PopAndDestroy( caption );
                }
            }
        else
            {
            moreSettings = EFalse;
            }
        }
    

    
    CleanupStack::PopAndDestroy( settingValue );
    CleanupStack::PopAndDestroy( cr );
    }
// ---------------------------------------------------------------------------
// Removes extra list items from the application list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::RemoveExtraItemsL()
    {
    // No Effect item.
    CAppListItem* listItem = iListItems[0];
    iListItems.Remove(0);
    if (listItem)
        {
        delete listItem;
        listItem = NULL;      
        }
    }

// ---------------------------------------------------------------------------
// Returns the root entry containing mailboxes.
// ---------------------------------------------------------------------------
//
CMsvEntry* CAiScutSettingsAppList::GetRootEntryL()
    {
    return iMsvSession->GetEntryL(KMsvRootIndexEntryIdValue);
    }

// ---------------------------------------------------------------------------
// Adds remote mailboxes to the application list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddMailboxesL()
    {
    CMsvEntry* rootEntry = GetRootEntryL();

    CleanupStack::PushL(rootEntry);
    TBuf<255> mailboxId;

    for (TInt i = rootEntry->Count() - 1; i >= 0; --i)
        {
        const TMsvEntry& tentry = (*rootEntry)[i];

        if (tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid)
            {
            mailboxId.Num(tentry.Id());
            AddMailboxL(tentry.iDetails, mailboxId);
            }
        }

    CleanupStack::PopAndDestroy(rootEntry);
    }

// ---------------------------------------------------------------------------
// Adds a mailbox to the list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddMailboxL(const TDesC& aMailbox, const TDesC& aMailboxId)
{
    CAppListItem* listItem = CAppListItem::NewLC(KScutMessagingUid, aMailbox);

  // Insert params containing mailbox id
    HBufC* params = HBufC::NewLC(KScutParamMailbox().Length() + aMailboxId.Length());
    params->Des().Copy(KScutParamMailbox);
    params->Des().Append(aMailboxId);
    listItem->SetParamsL(*params);
    listItem->SetType( EAiScutSettingTypeApplication );
    CleanupStack::PopAndDestroy(params);

    TLinearOrder<CAppListItem> sortMethod(CAppListItem::CompareCaption);
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    User::LeaveIfError(iMailboxes.Append(listItem));
    listItem = NULL;
}

// ---------------------------------------------------------------------------
// Removes a mailbox from the list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::RemoveMailboxL(const TDesC& aMailboxParam)
    {
    for (TInt i = iListItems.Count() - 1; i >= 0; --i)
        {
        CAppListItem* listItem = iListItems[i];
        if (aMailboxParam.Compare(listItem->Params()) == 0)
            {
            // Also remove the mailbox from the mailbox array
            for (TInt j = iMailboxes.Count() - 1; j >= 0; --j)
                {
                if (aMailboxParam.Compare(iMailboxes[j]->Params()) == 0)
                    {
                    iMailboxes.Remove(j);
                    }
                }

            iListItems.Remove(i);
            delete listItem;
            listItem = NULL;

            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Gets the next valid application from application architecture server.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::GetNextValidApp(TApaAppInfo& aAppInfo)
    {
    TInt err = KErrNone;
    TBool validAppFound = EFalse;

    while (!validAppFound)
        {
        TApaAppInfo appInfo;
        err = iApaSession.GetNextApp(appInfo);
        if (err == KErrNone)
            {
            TApaAppCapabilityBuf capBuf;
            TApaAppCapability cap;

            if ((iApaSession.GetAppCapability(capBuf, appInfo.iUid) == KErrNone))
                {
                cap = capBuf();

                TInt screenNumber(0);
                err = iApaSession.GetDefaultScreenNumber( screenNumber, appInfo.iUid );

                if ((err == KErrNone) && (cap.iAppIsHidden == EFalse) && (screenNumber == 0)) 
                    {
                    aAppInfo = appInfo;
                    validAppFound = ETrue;
                    }
                }
            }
        else
            {
            return err;
            }
        }

    return err;
    }

// ---------------------------------------------------------------------------
// Updates the application list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::UpdateAppListL()
    {
    TBool updated = EFalse;
    TBool added   = EFalse;

    TInt totalAppCount = 0;

    if (iApaSession.AppCount(totalAppCount) == KErrNone)
        {
        if (totalAppCount < iTotalAppCount)
            {
            RemoveUninstalledAppFromListL(iTotalAppCount - totalAppCount);
            updated = ETrue;
            added = EFalse;
            }
        else if (totalAppCount > iTotalAppCount)
            {
            AddInstalledAppToListL(totalAppCount - iTotalAppCount);
            updated = ETrue;
            added = ETrue;
            }

        iTotalAppCount = totalAppCount;
        }

    if (updated)
        {
        iObserver.HandleScutListEventL(MAiScutListObserver::EAppListUpdated, added);
        }
    }

// ---------------------------------------------------------------------------
// Figures out which application is missing from the list and adds it.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::AddInstalledAppToListL(TInt aCount)
    {
    TInt added = 0;
    TApaAppInfo appInfo;
    TInt err = GetNextValidApp(appInfo);

    while (err == KErrNone)
        {
        TBool found = EFalse;

        for (TInt i = iListItems.Count() - 1; i >= 0; --i)
            {
            if (iListItems[i]->Uid() == appInfo.iUid)
                {
                found = ETrue;
                break;
                }
            }

        if (!found)
            {
            AddApplicationL(appInfo);
            if (++added == aCount)
                {
                // Found all of them, no need to go further
                return ;
                }
            }

        err = GetNextValidApp(appInfo);
        }

    // create registry 
    CJavaRegistry* registry = CJavaRegistry::NewLC();

    // get all uids stored in registry
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL( uids );

    CleanupClosePushL(uids);

    for (TInt javaIndex = 0; javaIndex < uids.Count(); ++javaIndex)
        {
        TInt found = EFalse;
        for (TInt listIndex = 0; listIndex < iListItems.Count(); ++listIndex )
            {
            if (iListItems[listIndex]->Uid() == uids[javaIndex])
                {
                found = ETrue;
                }
            }
        if (!found)
            {
            // get registry entry by uid (i.e. the first entry)
            CJavaRegistryEntry* entry =
                registry->RegistryEntryL( uids[javaIndex] );
            if( entry )
                {
                CleanupStack::PushL(entry);

                // get entry properties (i.e. name)
                const TDesC& name = entry->Name();
                TUid uid = entry->Uid();

                    CAppListItem* listItem = CAppListItem::NewLC(uid, name);
                  listItem->SetType( EAiScutSettingTypeApplication );
                TLinearOrder<CAppListItem> sortMethod(CAppListItem::CompareCaption);
                if (entry->Type() >= EGeneralApplication)
                    {
                    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
                    ++added;
                    }
                else
                    {
                    delete listItem;
                    }
                CleanupStack::Pop(listItem);

                CleanupStack::PopAndDestroy(entry);
                }
            if (added == aCount)
                {
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy(); // uids
    CleanupStack::PopAndDestroy(registry);
    }

// ---------------------------------------------------------------------------
// Figures out which application should not be in the list and removes it.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::RemoveUninstalledAppFromListL(TInt aCount)
    {
    TInt removed = 0;

    for (TInt i = iListItems.Count() - 1; i >= 0; --i)
        {
        CAppListItem* listItem = iListItems[i];
        TApaAppInfo ignored;
        if (iApaSession.GetAppInfo(ignored, listItem->Uid()) == KErrNotFound)
            {
            iListItems.Remove(i);
            delete listItem;
            listItem = NULL;

            if (++removed == aCount)
                {
                return ;
                }
            }
        }
    // create registry 
    CJavaRegistry* registry = CJavaRegistry::NewLC();
    // get all uids stored in registry
    RArray<TUid> uids;
    registry->GetRegistryEntryUidsL( uids );

    CleanupClosePushL(uids);

    for (TInt listIndex = iListItems.Count(); listIndex >= 0 ; --listIndex )
        {
        TInt found = EFalse;
        for (TInt javaIndex = 0; javaIndex < uids.Count(); ++javaIndex)
            {
            if (iListItems[listIndex]->Uid() == uids[javaIndex])
                {
                found = ETrue;
                }
            }
        if (!found)
            {
            // Item removed, remove from item list
            CAppListItem* listItem = iListItems[listIndex];
            iListItems.Remove(listIndex);
            delete listItem;
            if (++removed == aCount)
                {
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy(); // uids
    CleanupStack::PopAndDestroy(registry);
    }

TBool CAiScutSettingsAppList::IsNonNative(const TUid& aUid)
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

    return ret;
    }


TBool CAiScutSettingsAppList::IsInRom( const TUid& aUid )
    {
    TBool inROM = EFalse;

    TApaAppInfo appInfo;    
    if( iApaSession.GetAppInfo( appInfo, aUid ) == KErrNone )
        {
        // Does the full path start with z: 
        if( appInfo.iFullName.FindC( 
                PathInfo::RomRootPath().Left( KDriveAndColon ) ) >= 0 )            
            {          
            inROM = ETrue;                     
            }
        }
    return inROM;
    }


// ---------------------------------------------------------------------------
// Nested class to store individual application list items
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList::CAppListItem::CAppListItem(TUid aUid) : iUid(aUid)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::CAppListItem::ConstructL(const TDesC& aCaption)
    {
    iCaption = aCaption.AllocL();
    // Defaults to application
    iType = EAiScutSettingTypeApplication;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList::CAppListItem* CAiScutSettingsAppList::CAppListItem::NewLC(
    TUid aUid, const TDesC& aCaption)
    {
    CAppListItem* self = new (ELeave) CAppListItem(aUid);
    CleanupStack::PushL(self);
    self->ConstructL(aCaption);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList::CAppListItem::~CAppListItem()
    {
    delete iCaption;
    delete iParams;
    }

// ---------------------------------------------------------------------------
// Compare method used to add the items to the list in sorted order.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsAppList::CAppListItem::CompareCaption(const CAppListItem& aFirst,
    const CAppListItem& aSecond)
    {
    return aFirst.iCaption->Des().CompareC(*aSecond.iCaption);
    }

// ---------------------------------------------------------------------------
// Returns the item target application uid.
// ---------------------------------------------------------------------------
//
TUid CAiScutSettingsAppList::CAppListItem::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// Returns the item target application caption.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsAppList::CAppListItem::Caption() const
    {
    return TPtrC(*iCaption);
    }

// ---------------------------------------------------------------------------
// Returns the possible parameters for item target.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsAppList::CAppListItem::Params() const
    {
    TPtrC params;
    if (iParams)
        {
        params.Set(*iParams);
        }
    return params;
    }

// ---------------------------------------------------------------------------
// Sets the parameters for the item target.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsAppList::CAppListItem::SetParamsL(const TDesC& aParams)
    {
    HBufC* newParams = aParams.AllocL();
    delete iParams;
    iParams = newParams;
    }

TAiScutSettingType CAiScutSettingsAppList::CAppListItem::Type() const
{
    return iType;
}

void CAiScutSettingsAppList::CAppListItem::SetType( TAiScutSettingType aType )
{
    iType = aType;
}
// End of File.
