/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine class for Device Status Plug-in
*
*/

// System includes

// User includes
#include <aicontentobserver.h>
#include "aidevicestatuspluginengine.h"
#include "aipublisherfactory.h"
#include "aidevicestatuspublisher.h"

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::CAiDeviceStatusPluginEngine
//
// ----------------------------------------------------------------------------
//
CAiDeviceStatusPluginEngine::CAiDeviceStatusPluginEngine( 
    MAiContentObserver& aObserver, CHsContentPublisher& aExtension,
    MAiPublishPrioritizer& aPrioritizer )
    : iContentObserver( &aObserver ), iExtension( &aExtension ),          
      iPrioritizer( &aPrioritizer )
	{	
	}

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::ConstructL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPluginEngine::ConstructL()
    {
    // Instantiate all publishers via factory
    MAiDeviceStatusPublisher* publisher = AiPublisherFactory::CreateDatePublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateSimRegPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateNWSPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateBTSAPPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateOperatorLogoPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateOperatorNamePublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateMCNPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateCUGPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    publisher = AiPublisherFactory::CreateVHZPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );
    
    publisher = AiPublisherFactory::CreateCUGMCNPublisherL();
    CleanupStack::PushL( publisher );
    AddPublisherL( publisher );
    CleanupStack::Pop( publisher );

    // Subscribe all publishers once they are instantiated
    const TInt count( iPublishers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        iPublishers[i]->Subscribe( 
            *iContentObserver, *iExtension, *iPrioritizer, *this );
        }    
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::NewL
//
// ----------------------------------------------------------------------------
//
CAiDeviceStatusPluginEngine* CAiDeviceStatusPluginEngine::NewL( 
    MAiContentObserver& aObserver, CHsContentPublisher& aExtension,
    MAiPublishPrioritizer& aPrioritizer )											                                           
    {
    CAiDeviceStatusPluginEngine* self = 
    	new( ELeave ) CAiDeviceStatusPluginEngine( aObserver, aExtension, aPrioritizer ); 
    	        											                                                          
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::~CAiDeviceStatusPluginEngine
//
// ----------------------------------------------------------------------------
//

CAiDeviceStatusPluginEngine::~CAiDeviceStatusPluginEngine()
    {
    iPublishers.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::AddPublisherL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPluginEngine::AddPublisherL( 
    MAiDeviceStatusPublisher* aPublisher )
    {
	//Add publisher to list.
    if ( aPublisher )
        {
        CleanupDeletePushL( aPublisher );
        User::LeaveIfError( iPublishers.Append( aPublisher ) );
        CleanupStack::Pop(aPublisher);//aPublisher
        }
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::ResumePublishersL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPluginEngine::ResumePublishersL()
	{
    const TInt count( iPublishers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        iPublishers[i]->ResumeL();
        }    
	}

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::RefreshPublishersL
//
// ----------------------------------------------------------------------------
//
void CAiDeviceStatusPluginEngine::RefreshPublishersL( TBool aClean )
	{
	iContentObserver->StartTransaction( KImplUidDevStaPlugin );

    const TInt count( iPublishers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        iPublishers[i]->RefreshL( aClean );
        }    
        
	iContentObserver->Commit( KImplUidDevStaPlugin );
	}

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::RefreshPublishersL
//
// ----------------------------------------------------------------------------
//
TBool CAiDeviceStatusPluginEngine::RefreshPublishersL( TInt aContentId, 
    TBool aClean )
	{
	TBool success( EFalse );
	
	iContentObserver->StartTransaction( KImplUidDevStaPlugin );

    const TInt count( iPublishers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iPublishers[i]->RefreshL( aContentId, aClean ) )
            {
            success = ETrue;
            }
        }

    if ( success )
    	{
    	iContentObserver->Commit( KImplUidDevStaPlugin );
    	}
    else
        {
        iContentObserver->CancelTransaction( KImplUidDevStaPlugin );
        }

    return success;
	}

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::SuspendPublishersL
//
// ----------------------------------------------------------------------------
//
TBool CAiDeviceStatusPluginEngine::SuspendPublishersL( TInt aContentId, 
    TBool aClean )
    {
    TBool success( EFalse );
    
    const TInt count( iPublishers.Count() );
    
    for ( TInt i( 0 ); i < count; i++ )
        {
        if ( iPublishers[i]->SuspendL( aContentId, aClean ) )
            {
            success = ETrue;
            }
        }
    
    return success;
    }

// ----------------------------------------------------------------------------
// CAiDeviceStatusPluginEngine::RefreshPriorizedPublishersL
//
// ----------------------------------------------------------------------------
//
TBool CAiDeviceStatusPluginEngine::RefreshPriorizedPublishersL( TInt aContentId,
    TInt aPriority )
	{
	iContentObserver->StartTransaction( KImplUidDevStaPlugin );
	
	TBool success( EFalse );
    
	const TInt count( iPublishers.Count() );

    for ( TInt i( 0 ); i < count; i++ )
        {
        MAiDeviceStatusPublisher* publisher( iPublishers[i] );
        
        if( publisher->RefreshContentWithPriorityL( aContentId, aPriority ) )                                                                           
            {
            success = ETrue;
            break;
            }
        }
    
    if ( success )
        {
        iContentObserver->Commit( KImplUidDevStaPlugin );
        }
    else
        {
        iContentObserver->CancelTransaction( KImplUidDevStaPlugin );
        }
    
    return success;
	}

// End of file
