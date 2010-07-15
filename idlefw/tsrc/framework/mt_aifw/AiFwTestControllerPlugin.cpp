/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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


// ========== INCLUDE FILES ===================================================

#include "AiFwTestControllerPlugin.h"

#include <e32std.h>

#include "AiFwTestDefines.h"
#include "AiFwTestContentModel.h"

#include <aipluginsettings.h>
#include "aifweventhandler.h"
#include <aicontentmodel.h>
#include <aiutility.h>
#include "aiuiframeworkobserver.h"
using namespace AiTestUiController;


// ========== MEMBER FUNCTIONS ================================================

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestControllerPlugin::CAiFwTestControllerPlugin()
    {
    }

EXPORT_C CAiFwTestControllerPlugin* CAiFwTestControllerPlugin::Instance()
    {
    TAny* ptr = Dll::Tls();
    
    if ( ptr )
        {
        TAiTestTLSStorage* tlsSet = static_cast<TAiTestTLSStorage*>( ptr );
        return tlsSet->iControllerPlugin;
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::ConstructL()
    {
       TAiTestTLSStorage* tlsSet = TAiTestTLSStorage::CreateInstanceL();
    tlsSet->iControllerPlugin = this;

    iContent   = AiUtility::CreateContentItemArrayIteratorL(KTestPluginContent);
    iResources = AiUtility::CreateContentItemArrayIteratorL(KTestPluginResources);
    iEvents    = AiUtility::CreateContentItemArrayIteratorL(KTestPluginEvents);
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestControllerPlugin* CAiFwTestControllerPlugin::NewL()
    {
    CAiFwTestControllerPlugin* self = new (ELeave) CAiFwTestControllerPlugin();
    
    CleanupStack::PushL( self );
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
CAiFwTestControllerPlugin::~CAiFwTestControllerPlugin()
    {
       TAiTestTLSStorage::DestroyInstance();
    Release( iContent );
    Release( iResources );
    Release( iEvents );
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::StartTransaction(TInt /*aTxId*/)
    {
    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Commit(TInt /*aTxId*/)
    {
    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::CancelTransaction(TInt /*aTxId*/)
    {
    return 0;
    }

TBool CAiFwTestControllerPlugin::CanPublish( MAiPropertyExtension& /*aPlugin*/,
                                             TInt /*aContent*/,
                                             TInt /*aIndex*/ )
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Publish( MAiPropertyExtension& /*aPlugin*/,
                              TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
    {
    /*  // Read ref value.
        MAiContentItemIterator* i =
            (MAiContentItemIterator*) aPlugin->GetPropertyL( EAiPublisherResources );

        const TAiContentItem& ref = i->ItemL( aResource );
        
        const TDesC8& refType = ContentType( ref );
        const TDesC& refCid = ContentCid( ref );
        
        if ( refType.Match( KDataTypeText ) != KErrNotFound )
            {
            const TDesC* text = GetText( refCid );
            
            if ( text )
                {
                HBufC8* buf8 = HBufC8::NewLC( text->Length() );
                buf8->Des().Copy( *text );
                TPtrC8 ptr = buf8->Des();
                Publish( aPlugin, aContent, ptr );
                CleanupStack::PopAndDestroy(); // buf8
                }
            }
        
        // TODO: Support another types.
        */        

    
    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Publish( MAiPropertyExtension& /*aPlugin*/,
                              TInt /*aContent*/, const TDesC16& /*aBuf*/, TInt /*aIndex*/ )
    {
    /*
        MAiContentItemIterator* i =
            (MAiContentItemIterator*) aPlugin->GetPropertyL( EAiPublisherContent );

        const TAiContentItem& item = i->ItemL( aContent );
        
        const TDesC8& type = ContentType( item );
        const TDesC& cid = ContentCid( item );
        
        if ( type.Match( KDataTypeText ) != KErrNotFound )
            {
            SetText( cid, aBuf );
            }
        
        // TODO: Support another types.
        
        */
    
    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Publish( MAiPropertyExtension& /*aPlugin*/,
                              TInt /*aContent*/, const TDesC8& /*aBuf*/, TInt /*aIndex */)
    {
    /*
        MAiContentItemIterator* i =
            (MAiContentItemIterator*) aPlugin->GetPropertyL( EAiPublisherContent );

        const TAiContentItem& item = i->ItemL( aContent );
        
        const TDesC8& type = ContentType( item );
        const TDesC& cid = ContentCid( item );
        
        if ( type.Match( KDataTypeText ) != KErrNotFound )
            {
            SetText( cid, aBuf );
            }
        
        // TODO: Support another types.
        
        */
    
    return 0;
    }
    
// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Publish( MAiPropertyExtension& /*aPlugin*/,
                              TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/ )
    {
    // TODO: get content from aFile.

    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin::Clean( MAiPropertyExtension& /*aPlugin*/,
                                       TInt /*aContent*/, TInt /*aIndex*/ )
    {
        // TODO: clean content.
    
    return 0;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestControllerPlugin::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TBool CAiFwTestControllerPlugin::RequiresSubscription( 
                                 const TAiPublisherInfo& aPublisherInfo ) const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::LoadUIDefinitionL()
    {
    iDefinitionLoaded = ETrue;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::GetPluginsL( RAiPublisherInfoArray& aPlugins )
    {
    TAiPublisherInfo info;
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin);
    info.iName = _L( "AiFwTestPlugin" );
    info.iNamespace = _L8( "AiFw" );
    User::LeaveIfError( aPlugins.Append( info ) );
    /*
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin2);
    info.iName = _L( "AiFwTestPlugin2" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin3);
    info.iName = _L( "AiFwTestPlugin3" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin6);
    info.iName = _L( "Plurgin" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin4);
    info.iName = _L( "AiFwTestPlugin4" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin5);
    info.iName = _L( "AiFwTestPlugin5" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin7);
    info.iName = _L( "AiFwTestPlugin7" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin8);
    info.iName = _L( "AiFwTestPlugin8" );
    User::LeaveIfError( aPlugins.Append( info ) );
    info.iUid = TUid::Uid(KImplementationUidAiTestContentPlugin9);
    info.iName = _L( "AiFwTestPlugin9" );
    User::LeaveIfError( aPlugins.Append( info ) );
    */
    }
    
void CAiFwTestControllerPlugin::GetSettingsL(
                                          const TAiPublisherInfo& /*aPubInfo*/,
                                          RAiSettingsItemArray& aSettings)
    {
    MAiPluginSettings* settings = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL(settings);
    MAiPluginSettingsItem& item = settings->AiPluginSettingsItem();
    item.SetPublisherId( TUid::Uid( 0x10170FFF ) );
    item.SetValueL( _L("DesValue") );
    item.SetKey( 1 );
    
    // This will fail as no cenrep is setup, -> TRAP it.
    //TRAP_IGNORE( item.UpdateFromCenRepL() );
    
    aSettings.AppendL( settings );
    CleanupStack::Pop( settings );
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::RunApplicationL()
    {
    iRunApplicationCalled = ETrue;
    SendReadyEventL();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::ActivateUI()
    {
    iUiActivated = ETrue;
    }
    
// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
MAiContentObserver& CAiFwTestControllerPlugin::GetContentObserver()
    {
    return *this;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin::SetText( const TDesC8& /*aId*/, TPtrC8& /*aBuf*/ )
    {
    /*TBuf<KMaxLenContentId> id;
    id.Copy( aId );
    HBufC* value = HBufC::NewLC( aBuf.Length() );
    value->Des().Copy( aBuf );
    
    CXnNode* node = UiEngine()->FindNodeByIdL( id );
    
    if ( node )
        {
        node = node->Parent();
        MXnTextInterface* textIntr = CXnText::MakeInterfaceL( *node );
        
        if ( textIntr )
            {
            const TDesC* text = textIntr->Text();
            textIntr->SetTextL( value->Des() );
            CXnComponentNodeImpl* nodeImpl = node->ComponentNodeImpl();
            CXnComponent* component = nodeImpl->Component();
            CXnControlAdapter* adapter = component->ControlAdapter();
            adapter->DrawNow();
            }
        }
    
    CleanupStack::PopAndDestroy(); // value*/
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
const TDesC* CAiFwTestControllerPlugin::GetText( const TDesC8& /*aId*/ )
    {
    /*TBuf<KMaxLenContentId> id;
    id.Copy( aId );
    
    CXnNode* node = UiEngine()->FindNodeByIdL( id );
    
    if ( node )
        {
        node = node->Parent();
        MXnTextInterface* textIntr = CXnText::MakeInterfaceL( *node );
        
        if ( textIntr )
            {
            return textIntr->Text();
            }
        }
    */
    return NULL;
    }
    
void CAiFwTestControllerPlugin::HandleLoadedPlugins( 
                                      const RAiPublisherInfoArray& aPlugins ) 
    {
    for ( int i = 0; i < aPlugins.Count(); ++i )
        {
        iFwEventHandler->HandleLoadPluginL( aPlugins[i] );
        }
    }

void CAiFwTestControllerPlugin::SetEventHandler(MAiFwEventHandler& aEventHandler)
    {
    iFwEventHandler = &aEventHandler;
    }

EXPORT_C TBool CAiFwTestControllerPlugin::UiActivated()
    {
    return iUiActivated;  
    }

EXPORT_C TBool CAiFwTestControllerPlugin::RunApplicationCalled()
    {
    return iRunApplicationCalled;
    }

EXPORT_C TBool CAiFwTestControllerPlugin::DefinitionLoaded()
    {
    return iDefinitionLoaded;
    }
    
EXPORT_C void CAiFwTestControllerPlugin::SendReadyEventL()
    {
    if( iFwEventHandler )
        {
        iFwEventHandler->HandleUiReadyEventL(*this);
        }
    }

EXPORT_C void CAiFwTestControllerPlugin::SendTestEventL()
    {
    if( iFwEventHandler )
        {
        iFwEventHandler->HandlePluginEvent(  KTestEvent );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/not_supp") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/not_supp()") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/not_supp(34342fvdsf)") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/test_event(34342fvdsf)") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/test_event)") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/test_event(") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin/test_event()") );
        iFwEventHandler->HandlePluginEvent(  _L("someerrorevent/error") );
        iFwEventHandler->HandlePluginEvent(  KTestEvent );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin4/error") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin3/error") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin7/error") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin7/error") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin7") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin7/test(ehe)") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlugin7/test()") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlu/test(grrraaah)") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlu/test(grrraaah") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlu/test(") );
        iFwEventHandler->HandlePluginEvent(  _L("AiFwTestPlu/test)") );
        iFwEventHandler->HandlePluginEvent(  _L("theme_change_noticed") );
        iFwEventHandler->HandlePluginEvent(  KTestEvent );
        iFwEventHandler->HandlePluginEvent(  KTestEvent );
        }
    }

EXPORT_C void CAiFwTestControllerPlugin::SetResourceChangedL( TInt aRes )
    {
    iFwObserver->HandleResourceChange( aRes );
    }
    
EXPORT_C void CAiFwTestControllerPlugin::SetForegroundL( TBool aFore )
    {
    iFwObserver->HandleForegroundEvent( aFore );
    }
    
EXPORT_C void CAiFwTestControllerPlugin::SetAppEnvReadyL()
    {
    TRAP_IGNORE( iFwEventHandler->AppEnvReadyL() );
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
