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
* Description:  Device status plugin interface.
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <aiutility.h>
#include <featmgr.h>
#include "aidevicestatusplugin.h"
#include "aidevicestatuspluginengine.h"
#include "aipublishprioritizer.h"
#include "aidevicestatuscontentmodel.h"
#include "aimulticontentobserver.h"
#include "aipluginsettings.h"
#include "ainetworkinfolistener.h"

_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "aidevstaplgres.rsc" );
#define KResourcePath KDC_APP_RESOURCE_DIR

// ECOM implementation table
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KImplUidDevStaPlugin, CAiDeviceStatusPlugin::NewL)
    };


// ======== MEMBER FUNCTIONS ========

CAiDeviceStatusPlugin::CAiDeviceStatusPlugin() :
   	iResourceOffset( KErrNotFound )
    {
    }


void CAiDeviceStatusPlugin::ConstructL()
    {
    iInfo.iUid.iUid = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN; 

    FeatureManager::InitializeLibL();
    
    // Create master instance to prevent deletion on Stop()
    iListener = CAiNetworkInfoListener::InstanceL();
    
	//Create content here since this is needed in optimization phase.
    iContent = AiUtility::CreateContentItemArrayIteratorL( KAiDeviceStatusContent );
    iResources = AiUtility::CreateContentItemArrayIteratorL( KAiDeviceStatusResources );
    iContentObservers = CAiMultiContentObserver::NewL();
    iPrioritizer = CAiPublishPrioritizer::NewL( *iContentObservers, *this );
    }


CAiDeviceStatusPlugin* CAiDeviceStatusPlugin::NewL()
    {
    CAiDeviceStatusPlugin* self = CAiDeviceStatusPlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


CAiDeviceStatusPlugin* CAiDeviceStatusPlugin::NewLC()
    {
    CAiDeviceStatusPlugin* self = new( ELeave ) CAiDeviceStatusPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CAiDeviceStatusPlugin::~CAiDeviceStatusPlugin()
    {
    delete iPrioritizer;
    FreeResources();
    delete iContentObservers;
    Release( iResources );
    Release( iContent );
    FeatureManager::UnInitializeLib();
    if( iListener )
        {
        iListener->Release();
        }
    }


/**
 * Allocates all resourcers required for plug-in operation.
 */
void CAiDeviceStatusPlugin::AllocateResourcesL()
    {
    //create engine
    if( !iEngine )
    	{
   	    iEngine = CAiDeviceStatusPluginEngine::NewL( *iContentObservers,
   	                                                    *this,
   	                                                    *iPrioritizer);
    	}

	if( iResourceOffset < 0 )
		{
		CCoeEnv* coe = CCoeEnv::Static();

		if( !coe )
		    {
		    User::Leave( KErrNotReady );
		    }

		//Add resource file to cone
		TFullName resourceFile( KResourceDrive );
		resourceFile.Append( KResourcePath );
		resourceFile.Append( KResourceFile );
		BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resourceFile );
		iResourceOffset = coe->AddResourceFileL( resourceFile );
		}
    }


/**
 * Frees all allocated resources.
 */
void CAiDeviceStatusPlugin::FreeResources()
    {
	if( iResourceOffset >= 0 )
	    {
	    CCoeEnv* coe = CCoeEnv::Static();
    	//If device status plugin is created when CCoeEnv is available and
    	//destroyed when it is not anymore available, cone will inform about
    	//resource leak.
	    if ( coe )
	        {
        	coe->DeleteResourceFile( iResourceOffset );
        	iResourceOffset = KErrNotFound;
        	}
	    }

	delete iEngine;
	iEngine = NULL;
    }


void CAiDeviceStatusPlugin::Resume(TAiTransitionReason aReason)
    {
    if( IgnoreReason( aReason ) )
        {
        return;
        }
    // resume all publishers only in startup
    if( iEngine )
        {
        if (  aReason == EAiIdleForeground || aReason == EAiKeylockDisabled )
        	{
        	// not much can be done if some publisher cannot be refreshed
        	TRAP_IGNORE( iEngine->RefreshPublishersL( 
        	                EAiDeviceStatusContentNetworkIdentity, ETrue ) );
        	
        	TRAP_IGNORE( iEngine->RefreshPublishersL( 
        					EAiDeviceStatusContentCUGMCNIndicator, ETrue ) );
        	}
        // if layout changed republish some information
        else if ( aReason == EAiScreenLayoutChanged )
            {
            TRAP_IGNORE( iEngine->RefreshPublishersL( EAiDeviceStatusContentDate, ETrue )); 
            TRAP_IGNORE( iEngine->RefreshPublishersL( EAiDeviceStatusContentCUGMCNIndicator, ETrue ));
            TRAP_IGNORE( iEngine->RefreshPublishersL( EAiDeviceStatusContentVHZText, ETrue ));
            TRAP_IGNORE( iEngine->RefreshPublishersL( EAiDeviceStatusContentNetworkIdentity, ETrue ));
            }
        return;
        }
    // If engine has been deleted. create it again.
    else
        {
        iContentObservers->StartTransaction( KImplUidDevStaPlugin );

        TRAPD( err, DoResumeL(aReason) );

        if( err == KErrNone )
            {
            iContentObservers->Commit( KImplUidDevStaPlugin );
            }
        else
            {
            iContentObservers->CancelTransaction( KImplUidDevStaPlugin );
            }
        }
    }


void CAiDeviceStatusPlugin::DoResumeL(TAiTransitionReason /*aReason*/)
    {
    AllocateResourcesL( );
    iEngine->ResumePublishersL();
    iEngine->RefreshPublishersL( EFalse );
    }
    

TBool CAiDeviceStatusPlugin::IgnoreReason( TAiTransitionReason aReason )
    {
    switch( aReason )
        {
        case EAiBacklightOff:
            return ETrue;
        }
    return EFalse;
    }


void CAiDeviceStatusPlugin::Stop(TAiTransitionReason /*aReason*/)
    {
    FreeResources();
    }


void CAiDeviceStatusPlugin::Suspend(TAiTransitionReason /*aReason*/)
    {
    }


void CAiDeviceStatusPlugin::SubscribeL(MAiContentObserver& aObserver)
    {
    iContentObservers->AddObserverL( aObserver );
    }


TAny* CAiDeviceStatusPlugin::Extension(TUid aUid)
    {
	//Access to extensions
    if( aUid == KExtensionUidProperty )
        {
        return static_cast<MAiPropertyExtension*>(this);
        }

    return NULL;
    }


void CAiDeviceStatusPlugin::ConfigureL(RAiSettingsItemArray& aSettings)
    {
	aSettings.ResetAndDestroy();
    }

TAny* CAiDeviceStatusPlugin::GetPropertyL(TInt aProperty)
    {
	//Return properties.
    switch (aProperty)
        {
        case EAiPublisherInfo:
                return &iInfo;

        case EAiPublisherContent:
            return static_cast<MAiContentItemIterator*>(iContent);

        case EAiPublisherResources:
            return static_cast<MAiContentItemIterator*>(iResources);

        case EAiContentRequest:
            return static_cast<MAiContentRequest*>(this);
        }

    return NULL;
    }

void CAiDeviceStatusPlugin::SetPropertyL(TInt aProperty, TAny* aValue)
    {
    if( aProperty == EAiPublisherInfo )
        {
        ASSERT( aValue );
        
        const TAiPublisherInfo* info( 
                static_cast<const TAiPublisherInfo*>( aValue ) );
        
        iInfo = *info;
        }
    }

TBool CAiDeviceStatusPlugin::RefreshContent( TInt aContentId )
    {
    TBool result = EFalse;

    TRAP_IGNORE( result = iEngine->RefreshPublishersL( aContentId, EFalse ) );
    return result;
    }


/**
 * ECom component entry point.
 */
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
    }
