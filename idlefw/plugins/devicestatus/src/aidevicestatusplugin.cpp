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

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <coemain.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <aiutility.h>
#include <featmgr.h>

// User includes
#include "aidevicestatusplugin.h"
#include "aidevicestatuspluginengine.h"
#include "aipublishprioritizer.h"
#include "aidevicestatuscontentmodel.h"
#include "aimulticontentobserver.h"
#include "aipluginsettings.h"
#include "ainetworkinfolistener.h"

// Constants
_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "aidevstaplgres.rsc" );

#define KResourcePath KDC_RESOURCE_FILES_DIR

// ECOM implementation table
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KImplUidDevStaPlugin, CAiDeviceStatusPlugin::NewL)
    };


// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::CAiDeviceStatusPlugin
//
// ----------------------------------------------------------------------------
//
CAiDeviceStatusPlugin::CAiDeviceStatusPlugin() 
    : iResourceOffset( KErrNotFound )   	
    {
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::ConstructL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::ConstructL()
    {     
    FeatureManager::InitializeLibL();
    
    // Create master instance to prevent deletion on Stop()
    iListener = CAiNetworkInfoListener::InstanceL();
    
	//Create content here since this is needed in optimization phase.
    iContent = AiUtility::CreateContentItemArrayIteratorL( KAiDeviceStatusContent );
    iResources = AiUtility::CreateContentItemArrayIteratorL( KAiDeviceStatusResources );
    
    iContentObservers = CAiMultiContentObserver::NewL();
    
    iPrioritizer = CAiPublishPrioritizer::NewL( *iContentObservers, *this );
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::NewL
//
// ----------------------------------------------------------------------------
//
CAiDeviceStatusPlugin* CAiDeviceStatusPlugin::NewL()
    {
    CAiDeviceStatusPlugin* self = CAiDeviceStatusPlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::NewLC
//
// ----------------------------------------------------------------------------
//
CAiDeviceStatusPlugin* CAiDeviceStatusPlugin::NewLC()
    {
    CAiDeviceStatusPlugin* self = new( ELeave ) CAiDeviceStatusPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::~CAiDeviceStatusPlugin
//
// ----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::AllocateResourcesL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::AllocateResourcesL()
    {
    //create engine
    if( !iEngine )
    	{
   	    iEngine = CAiDeviceStatusPluginEngine::NewL( 
            *iContentObservers, *this, *iPrioritizer );   	                                                      	                                                  
    	}

	if( iResourceOffset < 0 )
		{
		CCoeEnv* coe( CCoeEnv::Static() );

		if ( coe )
		    {
	        //Add resource file to cone
	        TFullName resourceFile( KResourceDrive );
	        resourceFile.Append( KResourcePath );
	        resourceFile.Append( KResourceFile );
	        BaflUtils::NearestLanguageFile( coe->FsSession(), resourceFile );
	        iResourceOffset = coe->AddResourceFileL( resourceFile );		    
		    }
		}
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::FreeResources
//
// ----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::Start
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::Start( TStartReason aReason )     
    {    
    if ( aReason == EPageStartup || aReason == ESystemStartup )
        {
        iRequirePublish = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::Stop
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::Stop( TStopReason /*aReason*/ )     
    {    
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::Resume
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::Resume( TResumeReason aReason )     
    {          
    if ( aReason == EForeground )
        {
        if ( iRequirePublish )
            {
            TRAP_IGNORE( DoResumeL() );
            
            iRequirePublish = EFalse;
            }          
        }
    }
  
// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::Suspend
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::Suspend( TSuspendReason /*aReason*/ )    
    {
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::SubscribeL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::SubscribeL( MAiContentObserver& aObserver )
    {
    iContentObservers->AddObserverL( aObserver );
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::ConfigureL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
	aSettings.ResetAndDestroy();
	
	AllocateResourcesL();
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::GetProperty
//
// ----------------------------------------------------------------------------
//
TAny* CAiDeviceStatusPlugin::GetProperty( TProperty aProperty )     
    {
    if ( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );
        }
    else if ( aProperty ==  EPublisherResources )
        {
        return static_cast< MAiContentItemIterator* >( iResources );
        }
    else if ( aProperty == EContentRequest )
        {
        return static_cast< MAiContentRequest* >( this );
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::DoResumeL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPlugin::DoResumeL()
    {    
    iEngine->ResumePublishersL();
    iEngine->RefreshPublishersL( EFalse );
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::RefreshContent
//
// ----------------------------------------------------------------------------
//
TBool CAiDeviceStatusPlugin::RefreshContent( TInt aContentId )
    {
    TBool result( EFalse );

    TRAP_IGNORE( result = iEngine->RefreshPublishersL( aContentId, EFalse ) );
    
    return result;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPlugin::SuspendContent
//
// ----------------------------------------------------------------------------
//
TBool CAiDeviceStatusPlugin::SuspendContent( TInt aContentId )
    {
    TBool result( EFalse );
    
    TRAP_IGNORE( result = iEngine->SuspendPublishersL( aContentId, EFalse ) );
    
    return result;
    }

// ======== GLOBAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// ImplementationGroupProxy
//
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof(KImplementationTable) / sizeof( TImplementationProxy );

    return KImplementationTable;
    }

// End of file
