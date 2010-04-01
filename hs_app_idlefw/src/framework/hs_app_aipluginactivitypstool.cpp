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
* Description:  Plugin activity PS tool
*
*/


#include "hs_app_aipluginactivitypstool.h"
#include "aipropertyextension.h"
#include <activeidle2domainpskeys.h>
#include <e32std.h> // for User
#include <e32property.h> // for RProperty
#include <e32capability.h>

// Unnamed namespace for local definitions
namespace
    {

    const TInt KStartOrdinal( KAIActivePluginRangeStart );
    
    // All reads are allowed.
    _LIT_SECURITY_POLICY_PASS( KPluginActivityRegistryReadPolicy );

    // Write requires WriteDeviceData capability
    _LIT_SECURITY_POLICY_C1( KPluginActivityRegistryWritePolicy, ECapabilityWriteDeviceData );

    }

CAiPluginActivityRegistry::CAiPluginActivityRegistry()
  : iRegistryOrdinal( KStartOrdinal )
    {
    }

CAiPluginActivityRegistry* CAiPluginActivityRegistry::NewL()
    {
    CAiPluginActivityRegistry* self = 
        new (ELeave) CAiPluginActivityRegistry();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
void CAiPluginActivityRegistry::ConstructL()
    {
    }
    
CAiPluginActivityRegistry::~CAiPluginActivityRegistry()
    {
    CleanRegistry();
    }
        
TInt CAiPluginActivityRegistry::SetPluginActive( const TAiPublisherInfo& aPubInfo )
    {
    TInt psErr = KErrArgument;
    TInt uid = aPubInfo.iUid.iUid;
    
    // Make sure the keys are within their assigned ranges
    if( uid >= KAIPluginNameRangeStart &&
        uid <= KAIPluginNameRangeEnd &&
        iRegistryOrdinal  >= KAIActivePluginRangeStart &&
        iRegistryOrdinal  <= KAIActivePluginRangeEnd )
        {
        psErr = UpdateOrdinalRegister( uid );

        ++iPluginCount; // now there is partial data in registry for next item
                        // so update count allready here, so that
                        // the data may be cleaned in case on error
             
        psErr |= UpdateNameRegister( uid, aPubInfo.iName );
                        
        psErr |= UpdateCountRegister();
        
        if( psErr != KErrNone )
            {
            CleanLastEntry( uid,
                            iRegistryOrdinal,
                            iPluginCount - 1 );
            // Decrement only after rollback so failures may be cleaned properly
            // in case there is interrupting error situations
            --iPluginCount;
            return psErr;
            }
                        
        ++iRegistryOrdinal;
        }
    
    return psErr;
    }
    
void CAiPluginActivityRegistry::CleanRegistry()
    {
    // The count in p&s might not be updated before
    // we end up here that why we use iPluginCount for count.
    for( TInt i = 0; i < iPluginCount; ++i )
        {
        TInt categoryKey = i + KStartOrdinal;
        TInt pluginUid = 0;
        TInt err = RProperty::Get( 
                    KPSUidActiveIdle2,
                    categoryKey,
                    pluginUid );
        if( err == KErrNone )
            {
            // Delete name
            RProperty::Delete( KPSUidActiveIdle2, pluginUid );
            }
        // Delete ordinal
        RProperty::Delete( KPSUidActiveIdle2, categoryKey );
        }
    // Delete count
    RProperty::Delete( KPSUidActiveIdle2, KAIActivePluginCount );
    iRegistryOrdinal = KStartOrdinal;
    }

TInt CAiPluginActivityRegistry::UpdateCountRegister()
    {
    TInt err = RProperty::Define(
                        KPSUidActiveIdle2,
                        KAIActivePluginCount,
                        RProperty::EInt,
                        KPluginActivityRegistryReadPolicy,
                        KPluginActivityRegistryWritePolicy );
    if( err == KErrAlreadyExists &&
        iRegistryOrdinal == KStartOrdinal )
        {
        // Some error has occured
        CleanRegistry();
        err = RProperty::Define(
                        KPSUidActiveIdle2,
                        KAIActivePluginCount,
                        RProperty::EInt,
                        KPluginActivityRegistryReadPolicy,
                        KPluginActivityRegistryWritePolicy );
        }
    if( err != KErrAlreadyExists &&
        err != KErrNone )
        {
        return err;
        }
        
    // iRegistryOrdinal starts from 1, so it can be used as count, but only
    // before incrementation.
    err = RProperty::Set( 
                    KPSUidActiveIdle2,
                    KAIActivePluginCount,
                    iPluginCount );
    return err;
    }

TInt CAiPluginActivityRegistry::UpdateOrdinalRegister( TInt aPluginUid )
    {
    TInt categoryKey = iRegistryOrdinal;
    TInt err = RProperty::Define(
                KPSUidActiveIdle2,
                categoryKey,
                RProperty::EInt,
                KPluginActivityRegistryReadPolicy,
                KPluginActivityRegistryWritePolicy );
    
    if( err == KErrNone ||
        err == KErrAlreadyExists )
        {
        // Set plugin uid to ordinal key
        err = RProperty::Set( 
                    KPSUidActiveIdle2,
                    categoryKey,
                    aPluginUid );
        }
    return err;
    }

TInt CAiPluginActivityRegistry::UpdateNameRegister( TInt aPluginUid,
                                                    const TDesC& aName )
    {
    TInt err = RProperty::Define(
                    KPSUidActiveIdle2,
                    aPluginUid,
                    RProperty::EText,
                    KPluginActivityRegistryReadPolicy,
                    KPluginActivityRegistryWritePolicy );
                        
    if( err == KErrNone ||
        err == KErrAlreadyExists )
        {
        // Set plugin uid to ordinal key
        err = RProperty::Set( 
                        KPSUidActiveIdle2,
                        aPluginUid,
                        aName );
        }
    return err;
    }

void CAiPluginActivityRegistry::CleanLastEntry( TInt aPluginUid,
                                                TInt aOrdinal,
                                                TInt aLastCount )
    {
    RProperty::Delete( KPSUidActiveIdle2, aOrdinal );
    RProperty::Delete( KPSUidActiveIdle2, aPluginUid );
    if( aLastCount == 0 )
        {
        RProperty::Delete( KPSUidActiveIdle2, KAIActivePluginCount );
        }
    else
        {
        RProperty::Set( 
                    KPSUidActiveIdle2,
                    KAIActivePluginCount,
                    aLastCount );
        }
    }
