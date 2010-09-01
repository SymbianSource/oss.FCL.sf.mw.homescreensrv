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


#include "stub.h"
#include "cprofilechangenotifyhandler.h"
#include "stubdataholder.h"
#include "profileengine.h"
#include "networkhandling.h"


EXPORT_C RSSSettings::RSSSettings()
    {    
    }
    
    
EXPORT_C TInt RSSSettings::Open()
    {    
    TRAPD( err, CStubDataHolder::InstanceL() );
    return err;
    }
    
    
TInt RSSSettings::Get( TSSSettingsSetting aSetting, TInt& aValue )
    {    
    TRAPD( err, aValue = CStubDataHolder::Instance()->GetSSSettingsValueL( aSetting ) );
    return err;
    }


TBool RSSSettings::IsValueValidCugIndex( const TInt& /*aValue*/ ) const
    {
    TRAPD( err, CStubDataHolder::Instance()->GetSSSettingsValueL( ESSSettingsCug ) );
    return ( err == KErrNone );    
    }

    
EXPORT_C MProfileEngine* CreateProfileEngineL(void)
    {        
    CStubDataHolder::InstanceL();
    return CProfileEngine::NewL();
    }

    
EXPORT_C void RSSSettings::CancelAll(MSSSettingsObserver &aObserver)
    {    
    CStubDataHolder::Instance()->RemoveSSSettingsObserver( aObserver );
    }


EXPORT_C void RSSSettings::Close()
    {    
    }


EXPORT_C TInt RSSSettings::Register(TSSSettingsSetting /*aSetting*/, MSSSettingsObserver& aObserver)
    {    
    TRAPD( err, CStubDataHolder::Instance()->AddSSSettingsObserverL( aObserver ) );
    return err;
    }


EXPORT_C CNWSession* CreateL(class MNWMessageObserver& aObserver, struct TNWInfo& aInfo)
    {    
    CStubDataHolder::InstanceL();
    return new( ELeave )CNWSessionStub( aObserver, aInfo );
    };


GLDEF_C TInt E32Dll(TInt)
    {
    return KErrNone;
    }
// End of File.
