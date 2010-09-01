/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


// ========== INCLUDE FILES ===================================================

#include "AiFwTestContentPlugin.h"
#include "AiFwTestDefines.h"
#include "AiFwTestContentModel.h"

#include <aicontentobserver.h>
#include <aipluginsettings.h>
#include <aiutility.h>

// ========== EXTERNAL DATA STRUCTURES ========================================

// ========== EXTERNAL FUNCTION PROTOTYPES ====================================

// ========== CONSTANTS =======================================================

// ========== MACROS ==========================================================

// ========== LOCAL CONSTANTS AND MACROS ======================================

// ========== MODULE DATA STRUCTURES ==========================================

// ========== LOCAL FUNCTION PROTOTYPES =======================================

// ========== FORWARD DECLARATIONS ============================================

// ========== LOCAL FUNCTIONS =================================================

// ========== MEMBER FUNCTIONS ================================================

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin::CAiFwTestContentPlugin()
{}

EXPORT_C CAiFwTestContentPlugin* CAiFwTestContentPlugin::Instance()
    {
    TAny* ptr = Dll::Tls();
	
    if ( ptr )
		{
    	TAiTestTLSStorage* tlsSet = static_cast<TAiTestTLSStorage*>( ptr );
    	return tlsSet->iContentPlugin;
		}
	return NULL;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::ConstructL()
{
    iText = HBufC8::NewL(128);

    iInfo = new (ELeave) TAiPublisherInfo;

    iContent   = AiUtility::CreateContentItemArrayIteratorL(KTestPluginContent);
    iResources = AiUtility::CreateContentItemArrayIteratorL(KTestPluginResources);
    iEvents    = AiUtility::CreateContentItemArrayIteratorL(KTestPluginEvents);

   	TAiTestTLSStorage* tlsSet = TAiTestTLSStorage::CreateInstanceL();
	tlsSet->iContentPlugin = this;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin* CAiFwTestContentPlugin::NewL()
{
    CAiFwTestContentPlugin* self = new (ELeave) CAiFwTestContentPlugin;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin::~CAiFwTestContentPlugin()
{
   	TAiTestTLSStorage::DestroyInstance();
	    
    delete iText;
    iObservers.Close();

    delete iInfo;
    Release( iContent );
    Release( iResources );
    Release( iEvents );

    if( iTicker )
        iTicker->Cancel();
    delete iTicker;
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin::Callback(TAny* aPtr)
{
    CAiFwTestContentPlugin* plugin = reinterpret_cast<CAiFwTestContentPlugin*> (aPtr);
    plugin->iCounter++;
    plugin->iText->Des().Copy(_L8("Count:"));
    plugin->iText->Des().AppendNum(plugin->iCounter);

    // Notify
    for (TInt i = 0; i < plugin->iObservers.Count(); i++)
    {
        MAiContentObserver* observer = plugin->iObservers[i];
        observer->StartTransaction(0);

        HBufC* value = HBufC::NewLC(plugin->iText->Length());
        value->Des().Copy(*plugin->iText);

        observer->Publish((MAiPropertyExtension&) *plugin, KTestPluginContent[0].id, *value, 0);

        if (plugin->iCounter % 2 == 0)
        {
            observer->Publish((MAiPropertyExtension&) *plugin, KTestPluginContent[1].id, KTestPluginResources[0].id, 0);
        }
        else
        {
            observer->Publish((MAiPropertyExtension&) *plugin, KTestPluginContent[1].id, KTestPluginResources[1].id, 0);
        }

        observer->Commit(0);

        CleanupStack::PopAndDestroy(); // value
    }

    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::LoadEngineL(TAiTransitionReason /*aReason*/)
{
    iPluginLoaded = ETrue;
    if( !iTicker )
        {
        iTicker = CPeriodic::NewL(CActive::EPriorityStandard);
        }
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::Resume(TAiTransitionReason aReason)
{
    TRAP_IGNORE( LoadEngineL( aReason ) );
    iPluginResumed = ETrue;
    if( iTicker && !iTicker->IsActive() )
        {
        TCallBack cb(Callback, this);
        iTicker->Start(1000000, 1000000, cb);
        }
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::Suspend(TAiTransitionReason /*aReason*/)
{
    if( iTicker && iTicker->IsActive() )
    {
        iTicker->Cancel();
    }
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::Stop(TAiTransitionReason /*aReason*/)
{
    if( iTicker )
        {
        iTicker->Cancel();
        delete iTicker;
        iTicker = NULL;
        }
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::SubscribeL(MAiContentObserver& aObserver)
{
    return iObservers.AppendL(&aObserver);
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    iSettingFound = ETrue;
    for( TInt i = 0; i < aSettings.Count(); ++i )
        {
        MAiPluginSettingsItem& item = aSettings[i]->AiPluginSettingsItem();
        if( item.Value().Compare( _L("DesValue") ) == 0 )
            {
            // pass
            if( item.Key() == 1 )
                {
                }
            item.PublisherId();
            }
        /*TRAP_IGNORE(
        item.UpdateFromCenRepL();
        item.SetKey( -1 );
        item.UpdateFromCenRepL();
        item.SetKey( 0 );
        item.SetPublisherId( TUid::Uid( 0 ) );
        item.UpdateFromCenRepL();
        );*/
        }
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty)
    {
        return static_cast<MAiPropertyExtension*>(this);
    }
    else if (aUid == KExtensionUidEventHandler)
        {
        return static_cast<MAiEventHandlerExtension*>(this);
        }

    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin::GetPropertyL(TInt aProperty)
{
    switch (aProperty)
    {
    case EAiPublisherInfo:
        return iInfo;

    case EAiPublisherContent:
        return static_cast<MAiContentItemIterator*>( iContent );

    case EAiPublisherResources:
        return static_cast<MAiContentItemIterator*>( iResources );

    case EAiPublisherEvents:
        return static_cast<MAiContentItemIterator*>( iEvents );

    case EAiContentRequest:
        return static_cast<MAiContentRequest*>(this);

    case EAiResourceRequest:
        return static_cast<MAiContentRequest*>(this);
    }

    User::Leave(KErrNotSupported);
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::SetPropertyL(TInt aProperty, TAny* aValue)
    {
    if( aValue )
        {
        switch( aProperty )
            {
            case EAiPublisherInfo:
                {
                const TAiPublisherInfo* info =
                    static_cast<const TAiPublisherInfo*>(aValue);
                iInfo->iUid.iUid = info->iUid.iUid;
                iInfo->iName.Copy( info->iName );
                iInfo->iNamespace.Copy( info->iNamespace );
                break;
                }
            }
        }
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin::HandleEvent(TInt aEvent, const TDesC& aParam)
    {
    if( i2ndEvent )
        {
        return;
        }
    if( aEvent == 3 && aParam.Length() == 0 )
        {
        iEventReceived = ETrue;
        }
    i2ndEvent = ETrue;
    }

TBool CAiFwTestContentPlugin::RefreshContent( TInt aContentId )
    {
    iContentRefreshed = ETrue;
    return ETrue;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//

EXPORT_C TBool CAiFwTestContentPlugin::ContentRefreshed()
    {
    return iContentRefreshed;
    }

EXPORT_C RPointerArray<MAiContentObserver>& CAiFwTestContentPlugin::Observers()
    {
    return iObservers;
    }
    
EXPORT_C TBool CAiFwTestContentPlugin::WasLoaded()
    {
    return iPluginLoaded;
    }

EXPORT_C TBool CAiFwTestContentPlugin::WasResumed()
    {
    return iPluginResumed;
    }
    
EXPORT_C TBool CAiFwTestContentPlugin::WasConfigured()
    {
    return iSettingFound;
    }

EXPORT_C TBool CAiFwTestContentPlugin::EventReceived()
    {
    return iEventReceived;
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
