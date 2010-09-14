/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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


//#include <?include_file>
#include <profile.hrh>
#include <cprofilechangenotifyhandler.h>
#include "profileengine.h"
#include "stubdataholder.h"

// ======== MEMBER FUNCTIONS ========

CProfileChangeNotifyHandler::CProfileChangeNotifyHandler(MProfileChangeObserver* /*aProfileChangeObserver*/)
    {    
    }
    
    
CProfileChangeNotifyHandler::~CProfileChangeNotifyHandler()
    {    
    }


CProfileChangeNotifyHandler* CProfileChangeNotifyHandler::NewL(MProfileChangeObserver* aProfileChangeObserver)
    {
    CProfileChangeNotifyHandler* _this = new( ELeave )CProfileChangeNotifyHandler( aProfileChangeObserver);
    CleanupStack::PushL( _this );
    CStubDataHolder::Instance()->AddProfileChangeObserverL( *aProfileChangeObserver, _this );
    CleanupStack::Pop();
    return _this;
    }


CProfile::~CProfile()
    {    
    }

    
void CProfile::Release()
    {
    delete this;
    }


const TArray<TContactItemId> CProfile::AlertForL()
    {    
    RArray<TContactItemId> array;
    return array.Array();
    }


TBool CProfile::IsSilent() const
    {    
    return ( CStubDataHolder::Instance()->CurrentProfile()->Id() == EProfileSilentId );
    }


const MProfileName& CProfile::ProfileName() const
    {    
    return *CStubDataHolder::Instance()->CurrentProfile();
    }


const MProfileTones& CProfile::ProfileTones() const
    {    
    MProfileTones* ret = NULL;
    return *ret;
    }


const MProfileExtraTones& CProfile::ProfileExtraTones() const
    {    
    MProfileExtraTones* ret = NULL;
    return *ret;
    }


const MProfileExtraSettings& CProfile::ProfileExtraSettings() const
    {    
    MProfileExtraSettings* ret = NULL;
    return *ret;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CProfileEngine::CProfileEngine()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CProfileEngine::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CProfileEngine* CProfileEngine::NewL()
    {
    CProfileEngine* self = CProfileEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CProfileEngine* CProfileEngine::NewLC()
    {
    CProfileEngine* self = new( ELeave ) CProfileEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CProfileEngine::~CProfileEngine()
    {
    }


// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CProfileEngine::Release()
    {    
    delete this;
    }

    
MProfile* CProfileEngine::ActiveProfileLC()
    {
    if( CStubDataHolder::Instance()->CurrentProfile()->Name().Length() == 0 )
        {
        User::Leave( KErrNotReady );
        }
    CProfile* profile = new( ELeave )CProfile;
    CleanupStack::PushL( profile );
    return profile;
    }


MProfile* CProfileEngine::ActiveProfileL()
    {    
    CProfile* profile = new( ELeave )CProfile;
    return profile;
    }


TInt CProfileEngine::ActiveProfileId()
    {    
    return CStubDataHolder::Instance()->CurrentProfile()->Id();
    }


MProfilesNamesArray* CProfileEngine::ProfilesNamesArrayLC()
    {   
    return NULL; 
    }


void CProfileEngine::SetActiveProfileL( TInt /*aId*/ )
    {    
    }


void CProfileEngine::SetTempRingingVolumeL( TProfileRingingVolume /*aVolume*/ )
    {    
    }


TProfileRingingVolume CProfileEngine::TempRingingVolumeL() const
    {
    TProfileRingingVolume vol = EProfileRingingVolumeLevel1;
    return vol;
    }


void CProfileEngine::SetTempMediaVolumeL( TProfileRingingVolume /*aVolume*/ )
    {    
    }


TProfileRingingVolume CProfileEngine::TempMediaVolumeL() const
    {
    TProfileRingingVolume vol = EProfileRingingVolumeLevel1;
    return vol;
    }


TBool CProfileEngine::IsFeatureSupported( TProfileFeatureId /*aFeatureId*/ ) const
    { 
    return EFalse;   
    }

TBool CProfileEngine::IsActiveProfileTimedL()
    {
    return EFalse;
    }
    
