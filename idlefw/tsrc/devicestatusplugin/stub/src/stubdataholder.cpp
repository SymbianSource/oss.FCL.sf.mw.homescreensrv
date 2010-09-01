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


#include "stubdataholder.h"


CProfileName::~CProfileName()
    {
    delete iName;
    }


TInt CProfileName::Id() const
    {
    return iId;
    }


const TDesC& CProfileName::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

const TDesC& CProfileName::NaviName() const
    {
    if( iNaviName )
        {
        return *iNaviName;
        }
    return KNullDesC;
    }

const TDesC& CProfileName::TitleName() const
    {
    if( iTitleName )
        {
        return *iTitleName;
        }
    return KNullDesC;
    }


void CProfileName::SetL( const TDesC& aName, TInt aId )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    iId = aId;
    }


const TDesC& CProfileName::ShortName() const
	{
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
	}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CStubDataHolder::CStubDataHolder()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CStubDataHolder::ConstructL()
    {
    iProfile = new( ELeave ) CProfileName;

    User::LeaveIfError( Dll::SetTls( this ) );
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
EXPORT_C CStubDataHolder* CStubDataHolder::InstanceL()
    {
    CStubDataHolder* ptr = static_cast<CStubDataHolder*>( Dll::Tls() );

    if( ptr )
        {
        return ptr;
        }

    CStubDataHolder* self = new( ELeave ) CStubDataHolder;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CStubDataHolder* CStubDataHolder::Instance()
    {
    CStubDataHolder* ptr = static_cast<CStubDataHolder*>( Dll::Tls() );

    if( !ptr )
        {
        User::Panic( _L("StubDataHolder"), 1 );
        }

    return ptr;
    }


EXPORT_C void CStubDataHolder::Release()
    {
    CStubDataHolder* ptr = static_cast<CStubDataHolder*>( Dll::Tls() );

    if( ptr )
        {
        delete ptr;
        }
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CStubDataHolder::~CStubDataHolder()
    {
    iSSSettingValues.Reset();
    iSSSettingsObservers.Reset();
    iProfileChangeObservers.Reset();
    delete iProfile;
    Dll::SetTls( NULL );
    }


// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CStubDataHolder::AddProfileChangeObserverL( MProfileChangeObserver& aObserver, CBase* aOwner )
    {
    TProfileObserver obs;
    obs.iOwner = aOwner;
    obs.iObserver = &aObserver;
    User::LeaveIfError( iProfileChangeObservers.Append( obs ) );
    }


EXPORT_C void CStubDataHolder::RemoveProfileChangeObserver( CBase* aOwner )
    {
    const TInt count = iProfileChangeObservers.Count();
    
    for( TInt i( 0 ); i < count; i++ )
        {
        if( iProfileChangeObservers[i].iOwner == aOwner )        
            {
            iProfileChangeObservers.Remove( i );
            break;
            }
        }
    }


EXPORT_C RArray<TProfileObserver>& CStubDataHolder::ProfileChangeObservers()
    {
    return iProfileChangeObservers;
    }


EXPORT_C void CStubDataHolder::SetCurrentProfileL( const TDesC& aName, TInt aId )
    {
    iProfile->SetL( aName, aId );
    }


EXPORT_C MProfileName* CStubDataHolder::CurrentProfile()
    {
    return iProfile;
    }


EXPORT_C void CStubDataHolder::SetNWMessageObserver( MNWMessageObserver& aObserver, TNWInfo& aInfo )
    {
    iNWObserver = &aObserver;
    iNWInfo = &aInfo;
    }


EXPORT_C MNWMessageObserver* CStubDataHolder::NWMessageObserver()
    {
    return iNWObserver;
    }


EXPORT_C TNWInfo* CStubDataHolder::NWInfo()
    {
    return iNWInfo;
    }

// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CStubDataHolder::AddSSSettingsObserverL( MSSSettingsObserver& aObserver )
    {
    User::LeaveIfError( iSSSettingsObservers.Append( &aObserver ) );    
    };


TInt CStubDataHolder::GetSSSettingsValueL( TSSSettingsSetting aSetting )
    {
    const TInt count = iSSSettingValues.Count();
    
    for( TInt i( 0 ); i < count; i++ )    
        {
        if( iSSSettingValues[i].iSetting == aSetting )
            {
            return iSSSettingValues[i].iValue;
            }
        }
        
    User::Leave( KErrNotFound );
    return KErrNotFound;
    }
    

void CStubDataHolder::RemoveSSSettingsObserver( MSSSettingsObserver& aObserver )
    {
    const TInt count = iSSSettingsObservers.Count();
    
    for( TInt i( 0 ); i < count; i++ )
        {
        if( iSSSettingsObservers[i] == &aObserver )
            {
            iSSSettingsObservers.Remove( i );
            break;
            }
        }
    }
    

EXPORT_C RArray<MSSSettingsObserver*>& CStubDataHolder::SSSettingsObservers()
    {
    return iSSSettingsObservers;    
    }


EXPORT_C void CStubDataHolder::SetSSSettingsValueL( TSSSettingsSetting aSetting, TInt aValue )
    {
    TSSSettingsValue ss;
    ss.iSetting = aSetting;
    ss.iValue = aValue;
    User::LeaveIfError( iSSSettingValues.Append( ss ) );
    }


EXPORT_C void CStubDataHolder::RemoveSSSetting( TSSSettingsSetting aSetting )
    {
    const TInt count = iSSSettingValues.Count();
    
    for( TInt i( count - 1 ); i >= 0; i-- )    
        {
        if( iSSSettingValues[i].iSetting == aSetting )
            {
            iSSSettingValues.Remove( i );
            }
        }
    }
