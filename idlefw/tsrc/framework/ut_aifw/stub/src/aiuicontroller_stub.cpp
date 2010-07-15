/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UI controller stub
*
*/

// System includes
#include <coemain.h>

// User includes
#include "aiuicontroller_stub.h"


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiUiControllerStub::CAiUiControllerStub()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerStub::CAiUiControllerStub()
    {
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::NewL()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerStub* CAiUiControllerStub::NewL()
    {
    CAiUiControllerStub* self = 
        new ( ELeave ) CAiUiControllerStub;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    
    return self;    
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::NewL()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerStub* CAiUiControllerStub::NewL( TUid /*aImpUid*/ )
    {
    return CAiUiControllerStub::NewL();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::~CAiUiControllerStub()
//
// ----------------------------------------------------------------------------
//
CAiUiControllerStub::~CAiUiControllerStub()
    {    
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::ConstructL()
    {    
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::LoadUIDefinitionL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::LoadUIDefinitionL()
    {
    iUiDefinition = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::GetSettingsL()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::GetSettingsL( 
    const THsPublisherInfo& /*aPublisherInfo*/, 
    RAiSettingsItemArray& /*aSettings*/ )          
    {
    iSettings = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::ActivateUI()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::ActivateUI()
    {    
    iActive = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::GetContentObserver()
//
// ----------------------------------------------------------------------------
//
MAiContentObserver& CAiUiControllerStub::GetContentObserver()
    {
    return *this;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SetEventHandler()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::SetEventHandler( 
    MAiFwEventHandler& /*aEventHandler*/ )
    {
    iEventHandler = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SetStateHandler()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::SetStateHandler( 
    MAiFwStateHandler& /*aStateHandler*/ )
    {
    iStateHandler = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::MainInterface()
//
// ----------------------------------------------------------------------------
//
MAiMainUiController* CAiUiControllerStub::MainInterface()
    {
    if ( iMain )
        {
        return this;
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SecondaryInterface()
//
// ----------------------------------------------------------------------------
//
MAiSecondaryUiController* CAiUiControllerStub::SecondaryInterface()
    {
    if ( !iMain )
        {
        return this;
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SecondaryInterface()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::RunApplicationL()
    {
    iRunning = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SecondaryInterface()
//
// ----------------------------------------------------------------------------
//
CCoeEnv& CAiUiControllerStub::CoeEnv()
    {
    return *CCoeEnv::Static();
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SecondaryInterface()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::Exit()
    {
    iExit = ETrue;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::IsMenuOpen()
//
// ----------------------------------------------------------------------------
//
TBool CAiUiControllerStub::IsMenuOpen()
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SetCoeEnv()
//
// ----------------------------------------------------------------------------
//
void CAiUiControllerStub::SetCoeEnv( CCoeEnv& /*aCoeEnv*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::StartTransaction()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::StartTransaction( TInt /*aTxId*/ )
    {    
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Commit()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Commit( TInt /*aTxId*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::CancelTransaction()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::CancelTransaction( TInt /*aTxId*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::CanPublish()
//
// ----------------------------------------------------------------------------
//
TBool CAiUiControllerStub::CanPublish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, TInt /*aIndex*/ )     
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Publish()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Publish()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, const TDesC16& /*aText*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Publish()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, const TDesC8& /*aBuf*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Publish()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Publish( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Clean()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::Clean( CHsContentPublisher& /*aPlugin*/, 
    TInt /*aContent*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::Extension()
//
// ----------------------------------------------------------------------------
//
TAny* CAiUiControllerStub::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::RequiresSubscription()
//
// ----------------------------------------------------------------------------
//
TBool CAiUiControllerStub::RequiresSubscription( 
    const THsPublisherInfo& /*aPublisherInfo*/ ) const     
    {
    if ( iMain )
        {
        return ETrue;
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SetProperty()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::SetProperty( CHsContentPublisher& /*aPlugin*/,
        const TDesC8& /*aElementId*/,
        const TDesC8& /*aPropertyName*/,
        const TDesC8& /*aPropertyValue*/ )
    {
    return KErrNotSupported;
    }

// ----------------------------------------------------------------------------
// CAiUiControllerStub::SetProperty()
//
// ----------------------------------------------------------------------------
//
TInt CAiUiControllerStub::SetProperty( CHsContentPublisher& /*aPlugin*/,
        const TDesC8& /*aElementId*/,
        const TDesC8& /*aPropertyName*/,
        const TDesC8& /*aPropertyValue*/,  
        MAiContentObserver::TValueType /*aValueType*/ )
    {
    return KErrNotSupported;
    }

// End of file
