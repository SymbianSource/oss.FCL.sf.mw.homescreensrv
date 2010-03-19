/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "chswiplugin.h"

#include <e32std.h>

#include <ecom/implementationproxy.h>
#include <contentharvesterpluginuids.hrh>
#include <liwservicehandler.h>
#include "cpglobals.h"

_LIT( KHsWidgetPublisher, "hswidgetpublisher");
_LIT( KHsPublisher, "ai3templatedwidget" );
_LIT8( KWidgetUid, "widget_uid");

// Map the interface UIDs to implementation factory functions
// ============================= LOCAL FUNCTIONS ===============================

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
    {
      {{KCHSwiPluginUidImpl}, (TProxyNewLPtr)CCHSwiPlugin::NewL},
    };

// ----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CCHSwiPlugin::CCHSwiPlugin
// Constructor
// ----------------------------------------------------------------------------
//
CCHSwiPlugin::CCHSwiPlugin( MLiwInterface* aInterface )
	{
	iCPSInterface = aInterface;
    }

// ----------------------------------------------------------------------------
// CADatSwi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CCHSwiPlugin::ConstructL()
    {
    iNotifier = CApaAppListNotifier::NewL( this, CActive::EPriorityStandard );
    User::LeaveIfError( iApaLsSession.Connect() );
    }

// ----------------------------------------------------------------------------
// CADatSwi::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CCHSwiPlugin* CCHSwiPlugin::NewL(  MLiwInterface* aInterface)
    {
    CCHSwiPlugin* self = new (ELeave) CCHSwiPlugin( aInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::~CCHSwiPlugin
// Destructor.
// ----------------------------------------------------------------------------
//
CCHSwiPlugin::~CCHSwiPlugin()
    {
    iApaLsSession.Close();
    delete iNotifier;
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::UpdateL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
void CCHSwiPlugin::UpdateL()
    {
    UpdateWidgetsL();
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::HandleAppListEvent
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
void CCHSwiPlugin::HandleAppListEvent( TInt /*aEvent*/ )
    {
    TRAP_IGNORE( UpdateWidgetsL() );
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::UpdateWidgetsL
// MMC watcher callback.
// ----------------------------------------------------------------------------
//
void CCHSwiPlugin::UpdateWidgetsL()
    {
    CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC( );
    CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC( );
    inparam->AppendL( TLiwGenericParam( KType, TLiwVariant( KPublisher ) ) );
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KHsWidgetPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KHsPublisher ));
    inparam->AppendL( TLiwGenericParam( KFilter, TLiwVariant( filter ) ) );
    
    iCPSInterface->ExecuteCmdL( KGetList, *inparam, *outparam );
    
    RemoveWidgetsL( outparam );
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( outparam );
    CleanupStack::PopAndDestroy( inparam );
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::RemoveWidgetsL
// MMC watcher callback.
// ----------------------------------------------------------------------------
//
void CCHSwiPlugin::RemoveWidgetsL( CLiwGenericParamList* aWidgets )
    {
    TInt pos ( 0 );
    aWidgets->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        {
        TLiwVariant variant = (*aWidgets)[pos].Value();
        variant.PushL();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        if( iterable->NextL( variant ) )
            {
            CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
            variant.Get( *map );
            if ( map->FindL( KDataMap, variant) )
                {
                CLiwDefaultMap *dataMap = CLiwDefaultMap::NewLC();
                variant.Get( *dataMap );
                if ( dataMap->FindL( KWidgetUid, variant ) )
                    {
                    TApaAppInfo appInfo;
                    
                    if( KErrNotFound == iApaLsSession.GetAppInfo( 
                            appInfo, TUid::Uid( variant.AsTInt32() ) ) )
                        {
                        if( map->FindL( KContentId, variant ) )
                            {
                            RemoveWidgetL( KPublisher, variant.AsDes() );
                            RemoveWidgetL( KCpData, variant.AsDes() );
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( dataMap );
                }
            CleanupStack::PopAndDestroy( map );
            }
        CleanupStack::PopAndDestroy(&variant);
        }
    }

// ----------------------------------------------------------------------------
// CCHSwiPlugin::RemoveWidgetL
// MMC watcher callback.
// ----------------------------------------------------------------------------
//
void CCHSwiPlugin::RemoveWidgetL( const TDesC& aType, 
        const TDesC& aContentId )
    {
    CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC( );
    CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC( );
    inparam->AppendL( TLiwGenericParam( KType, TLiwVariant( aType ) ) );
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KHsWidgetPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KHsPublisher ));
    filter->InsertL( KContentId, TLiwVariant( aContentId ));
    inparam->AppendL( TLiwGenericParam( KFilter, TLiwVariant( filter ) ) );
    
    iCPSInterface->ExecuteCmdL( KDelete, *inparam, *outparam );
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( outparam );
    CleanupStack::PopAndDestroy( inparam );
    }

//  End of File  
