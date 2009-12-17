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
* Description:  Plug-in main class
*
*/


#include <ecom/implementationproxy.h>
#include <centralrepository.h>

#include <aicontentobserver.h>
#include <aipluginsettings.h>

#include <platform/mw/aiscutuids.hrh>
#include "aiscutpluginprivatecrkeys.h"
#include "aiscutdefs.h"
#include "aiscutcontentmodel.h"
#include "caiscutplugin.h"
#include "caiscutengine.h"
#include "aiscutfactory.h"
#include "caiscutshortcut.h"

#include "debug.h"

const TImplementationProxy KImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KImplUidScutPlugin, CAiScutPlugin::NewL)
};

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutPlugin::CAiScutPlugin()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::ConstructL()
{
    // Initialize the info which the fw uses to identify the plug-in.
    // Fill in only the uid at this point, the fw will send this back with the
    // name filled in later through SetPropertyL().
    iInfo.iUid.iUid = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_SCUTPLUGIN;

    iContent = AiUtility::CreateContentItemArrayIteratorL(KAiScutContent);
    iResources = AiUtility::CreateContentItemArrayIteratorL(KAiScutResources);
    iEvents = AiUtility::CreateContentItemArrayIteratorL(KAiScutEvents);

    CRepository* repository = NULL;
    TRAP_IGNORE(repository = CRepository::NewL(KCRUidShortcutItems));
    // No leaving code here since 'repository' is not in cleanup stack.
    if (repository)
    {
        // A theme might not contain any publishable shortcuts at all, only
        // locked ones. To take this into account we must always delete old
        // theme-default settings to make sure the engine won't create any
        // unwanted shortcut objects. Any errors that might happen during
        // deletion are ignored to make sure the plug-in is kept alive.
        TUint32 errorKey;

        repository->Delete(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, errorKey);
    }
    delete repository;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutPlugin* CAiScutPlugin::NewL()
{
    CAiScutPlugin* self = new (ELeave) CAiScutPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutPlugin::~CAiScutPlugin()
{
    TRAP_IGNORE( DeleteDefaultShortcutsL() );
    Release(iContent);
    Release(iResources);
    Release(iEvents);
    delete iEngine;
    iObservers.Close();
}

// ---------------------------------------------------------------------------
// Publishes the given shortcut.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::PublishShortcutsL(RAiShortcutArray& aShortcuts)
{
    TInt err = KErrNone;
    TInt observers = iObservers.Count();
    TInt shortcuts = aShortcuts.Count();
    
    for (TInt i = 0; i < observers; ++i)
    {
        MAiContentObserver* observer = iObservers[i];
        TBool transactionStarted = EFalse;
        TInt transactionId = reinterpret_cast<TInt>(this);
        err = observer->StartTransaction(transactionId);

        if (err != KErrNotSupported)
        {
            // The observer does not support transactions, check for real errors.
            User::LeaveIfError(err);
            transactionStarted = ETrue;
        }
        err = KErrAlreadyExists;
        for (TInt j = 0; j < shortcuts; j++)
        {
            CAiScutShortcut* shortcut = aShortcuts[j];

            if (shortcut->NeedsToBePublished())
            {
                shortcut->Publish(*this, *observer);
                err = KErrNone;
            }
        }

        if ( transactionStarted && err == KErrNone )
            {
            User::LeaveIfError(observer->Commit(transactionId));
            }
        else if ( transactionStarted )
            {
            User::LeaveIfError(observer->CancelTransaction(transactionId));
            }
    }
}

// ---------------------------------------------------------------------------
// Is plug-in suspended or not.
// ---------------------------------------------------------------------------
//
TBool CAiScutPlugin::IsAlive() const
{
    return iAlive;
}

// ---------------------------------------------------------------------------
// Resume reason
// ---------------------------------------------------------------------------
//
TAiTransitionReason CAiScutPlugin::ResumeReason() const
    {
    return iResumeReason;
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Resumes the plug-in.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::Resume(TAiTransitionReason aReason)
{
    TRAP_IGNORE(DoResumeL(aReason));    
}

// ---------------------------------------------------------------------------
// Resumes the plug-in.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::DoResumeL(TAiTransitionReason aReason)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutPlugin::Resume reason %d alive = %d"), aReason, iAlive);
    iResumeReason = aReason;
	// Reload the engine in case general theme changed or
	// the engine has been suspended. 
    if (aReason == EAiGeneralThemeChanged || !iAlive)
    {
        // if general theme changed, free engine so that is will be
        // loaded again because shortcut icons must be re-created.
        FreeEngine();
    }

    if (!iEngine)
    {
		iEngine = AiScutFactory::CreateAiScutEngineL(*this);
    }

    iEngine->ResumeL( (iAlive == EFalse) ||
        (
        aReason != EAiBacklightOn    &&
        aReason != EAiBacklightOff   &&
        aReason != EAiIdleBackground &&
        aReason != EAiIdleForeground
        ),
        aReason
    );
    
    iAlive = ETrue;
}

// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Suspends the plug-in.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::Suspend(TAiTransitionReason aReason)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutPlugin::Suspend reason %d"), aReason);

    if (iEngine)
    {
        iEngine->Suspend();
    }

    iAlive = EFalse;
}

// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Frees the plug-in engine.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::Stop(TAiTransitionReason aReason)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutPlugin::Stop reason %d"), aReason);

	if ( aReason == EAiBackupRestoreStarted )
	    {
	    Suspend( aReason );
	    }
	else
	    {
	    FreeEngine();
	    }
}
// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Adds the content observer / subscriber to plug-in.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::SubscribeL(MAiContentObserver& aObserver)
{
    iObservers.AppendL(&aObserver);
}

void CAiScutPlugin::DeleteDefaultShortcutsL()
    {
    // Ignore any errors that might occur when deleting
    // the default keys
    TUint32 errorKey;
    CRepository* cr = CRepository::NewL(KCRUidShortcutItems);
    cr->Delete(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, errorKey);
    delete cr;
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Configures the plug-in.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::ConfigureL(RAiSettingsItemArray& aSettings)
{
    CRepository* repository = NULL;

    TRAPD(err, repository = CRepository::NewL(KCRUidShortcutItems));

    if (err == KErrNotFound)
    {
        // CenRep file is missing from the image, this is a serious error.
        User::Leave(err);
    }

    // Write settings if repository was successfully opened. All other errors
    // are ignored to ensure that the plug-in is up and running, even if
    // crippled.

    // No leaving code here since 'repository' is not in cleanup stack.
    if (repository)
    {
        repository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);

        // A theme might not contain any publishable shortcuts at all, only
        // locked ones. To take this into account we must always delete old
        // theme-default settings to make sure the engine won't create any
        // unwanted shortcut objects. Any errors that might happen during
        // deletion are ignored to make sure the plug-in is kept alive.
        TUint32 errorKey;

        repository->Delete(KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, errorKey);
        
		__PRINTS( "XAI: CAiScutPlugin::ConfigureL");

        TInt count = aSettings.Count();
        if (count > 0)
        {
            // Write new shortcut definitions.
            for (TInt i = 0; i < count; ++i)
            {
                MAiPluginSettings* settings = aSettings[ i ];
                
                if( settings->AiPluginItemType() == EAiPluginSettingsItem )
                    {
                    MAiPluginSettingsItem& item = settings->AiPluginSettingsItem();
                    TUint32 key = item.Key(); // implicit cast from TInt32 to TUint32.
    
    				__PRINT( __DBG_FORMAT( "XAI:   %d. key = 0x%x"), i+1, key);
    
                    // Add the bit that indicates this is a default shortcut setting.
                    key |= KScutFlagBitThemeDefault;
    
                    // Ignore possible error and keep going.
                    repository->Create(key, item.Value());
                    }
            }
        }

        TUint32 info = 0;
        repository->CommitTransaction(info);

        delete repository;
        repository = NULL;
        
        if (iEngine)
        {
            iEngine->CreateShortcutsL();
            iEngine->ResumeL( ETrue, EAiGeneralThemeChanged );
        }
    }

    // We don't need to store the settings so clear the array.
    aSettings.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// From class CAiContentPublisher.
// Returns the extension interface.
// ---------------------------------------------------------------------------
//
TAny* CAiScutPlugin::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty)
    {
        return static_cast<MAiPropertyExtension*>(this);
    }
    else if (aUid == KExtensionUidEventHandler)
    {
        return static_cast<MAiEventHandlerExtension*>(this);
    }
    else
    {
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::HandleEvent(TInt aEvent, const TDesC& aParam)
{
    if (iEngine)
    {
        // We have no way of reporting errors to framework so just ignore them.
        TRAP_IGNORE(iEngine->HandleAiEventL(aEvent, aParam));
    }
}

// ---------------------------------------------------------------------------
// CAiScutPlugin::HasMenuItem
//
// ---------------------------------------------------------------------------
//
TBool CAiScutPlugin::HasMenuItem(const TDesC16& /*aMenuItem*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Returns a plug-in property.
// ---------------------------------------------------------------------------
//
TAny* CAiScutPlugin::GetPropertyL(TInt aProperty)
{
    switch (aProperty)
    {
    case EAiPublisherInfo:
        return static_cast<TAiPublisherInfo*>(&iInfo);

    case EAiPublisherContent:
        return static_cast<MAiContentItemIterator*>(iContent);

    case EAiPublisherResources:
        return static_cast<MAiContentItemIterator*>(iResources);

    case EAiPublisherEvents:
        return static_cast<MAiContentItemIterator*>(iEvents);

    default:
        break;
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Sets a plug-in property to optimize the content model.
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::SetPropertyL(TInt aProperty, TAny* aValue)
    {
    if( aProperty == EAiPublisherInfo )
        {
        ASSERT( aValue );
        
        const TAiPublisherInfo* info( 
                static_cast<const TAiPublisherInfo*>( aValue ) );
        
        iInfo = *info;               
        }    
    }

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CAiScutPlugin::FreeEngine()
{
    delete iEngine;
    iEngine = NULL;
    iAlive = EFalse;    
}

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
}

// End of File.
