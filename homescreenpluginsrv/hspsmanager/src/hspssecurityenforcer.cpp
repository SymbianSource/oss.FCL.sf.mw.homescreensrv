/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <centralrepository.h>
#include <e32std.h>

#include "hspsthememanagement.h"
#include "hspsodt.h"
#include "hspsdefinitionrepository.h"
#include "hspssecurityenforcer.h"
#include "hspsserverutil.h"
#include "hspsthemeserver.h"
#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif
    

// ============================ LOCAL FUNCTIONS ===============================  

// -----------------------------------------------------------------------------
// ResetAndDestroyArray Callback function for cleaning up the CArrayPtr.
// Returns: void
// -----------------------------------------------------------------------------
//
LOCAL_C void ResetAndDestroyArray( TAny* aArray )
    {
    CArrayPtrSeg<HBufC>* tmp = reinterpret_cast<CArrayPtrSeg<HBufC>*>( aArray );
    tmp->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::ChspsSecurityEnforcer
// C++ default constructor can not contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsSecurityEnforcer::ChspsSecurityEnforcer(
    ChspsDefinitionRepository& aDefRep, CRepository& aCenRep )
: iDefRep( aDefRep ), iCentralRepository( aCenRep )
    {
    }

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsSecurityEnforcer::ConstructL()
    {
    iAccessControlList.Reset();
    TBuf8<KCenRepBufferSize> buf;
    iCentralRepository.Get(KCenrepKeyAccessControlList, buf );
    HandleAccessControlListL(buf, iAccessControlList);
    }

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsSecurityEnforcer* ChspsSecurityEnforcer::NewL( ChspsDefinitionRepository& aDefRep, CRepository& aCenRep )
    {
    ChspsSecurityEnforcer* self = new( ELeave ) ChspsSecurityEnforcer( aDefRep, aCenRep );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
ChspsSecurityEnforcer::~ChspsSecurityEnforcer()
    {
    iAccessControlList.Close();
    }

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::CheckIfLicenseeDefaultExistsL()
// Checks if Licensee Default theme exists. Leaves if there doesn't exist a licensee 
// default theme that has the same AppUid and ThemeUid as aOdt.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsSecurityEnforcer::CheckIfLicenseeDefaultExistsL( const ChspsODT& aOdt )
    {
    CArrayPtrSeg<HBufC8>* headerDataList = new( ELeave ) CArrayPtrSeg<HBufC8>( 
        KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyArray, headerDataList ) );
    TBool found( EFalse );

    ChspsODT* tempMask = ChspsODT::NewL();
    CleanupStack::PushL( tempMask );
    tempMask->SetRootUid( aOdt.RootUid() );
    tempMask->SetThemeUid( aOdt.ThemeUid() );   

    iDefRep.GetThemeListAsStreamL( *headerDataList, *tempMask );
    CleanupStack::PopAndDestroy( tempMask );
    
    for( TInt j = headerDataList->Count(); --j>=0; )
        {
        TPtr8 h = headerDataList->At( j )->Des();
        ChspsODT* odt = ChspsODT::UnMarshalHeaderLC( h );
        if ( odt->Flags() & EhspsThemeStatusLicenceeDefault )
            {
            found = ETrue;
            }           
        CleanupStack::PopAndDestroy( odt );
        }                         

    CleanupStack::Pop( headerDataList );
    if ( headerDataList )
        {
        headerDataList->ResetAndDestroy();
        delete headerDataList;
        }
    
    if ( !found )
        {
        User::LeaveIfError( KErrGeneral );//TO DO: Use HSPS error space
        }
    }


// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::CheckThemeLockingL()
// Checks if there exists a licencee default theme on ROM which has the same 
// identity and has the theme status locked.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsSecurityEnforcer::CheckThemeLockingL( const ChspsODT& aOdt )
    {
    CArrayPtrSeg<HBufC8>* headerDataList = new( ELeave ) CArrayPtrSeg<HBufC8>( 
        KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyArray, headerDataList ) );
    TBool found( EFalse );

    ChspsODT* tempMask = ChspsODT::NewL();
    CleanupStack::PushL( tempMask );
    tempMask->SetRootUid( aOdt.RootUid() );
    tempMask->SetThemeUid( aOdt.ThemeUid() );   

    iDefRep.GetThemeListAsStreamL( *headerDataList, *tempMask );
    CleanupStack::PopAndDestroy( tempMask );
    
    for( TInt j = headerDataList->Count(); --j>=0; )
        {
        TPtr8 h = headerDataList->At( j )->Des();
        ChspsODT* odt = ChspsODT::UnMarshalHeaderLC( h );
        if ( odt->Flags() & EhspsThemeStatusLocked && odt->Flags() & EhspsThemeStatusLicenceeDefault )
            {
            found = ETrue;
            }           
        CleanupStack::PopAndDestroy( odt );
        }                         

    CleanupStack::Pop( headerDataList );
    if ( headerDataList )
        {
        headerDataList->ResetAndDestroy();
        delete headerDataList;
        }
    
    return found;
    }

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::SetLogBus()
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE        
void ChspsSecurityEnforcer::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#endif

//------------------------------------------------------------------------------
// ChspsSecurityEnforcer::HandleAccessControlListL()
//------------------------------------------------------------------------------
//
void ChspsSecurityEnforcer::HandleAccessControlListL( const TDesC8& aStrBuf, RArray<TInt>& aArray )
    {
    TLex8 input( aStrBuf );
    input.Mark(); 
        
    while ( !input.Eos() )
        {
        if( input.Peek() == ';')
            {
            TUint uid;
            User::LeaveIfError( hspsServerUtil::HexString2Uint( input.MarkedToken(), uid ) );
            aArray.AppendL(uid);              
            input.Inc();
            input.Mark( );
            }
        input.Inc();        
        }
    
    }

//------------------------------------------------------------------------------
// ChspsSecurityEnforcer::CheckAccessControlListL()
//------------------------------------------------------------------------------
//
TBool ChspsSecurityEnforcer::CheckAccessControlListL( TInt aUid )
    {
    
    TBool status(EFalse);
#ifdef _hsps_INTERNAL_
    if( aUid == KSecureId_EUnit.iUid )
        {
        status = ETrue;
        return status; 
        }           
#endif   
    for( TInt i = 0; i < iAccessControlList.Count(); i++ )
        {
        if( aUid == iAccessControlList[i] )
            {
            status = ETrue;
            break;
            }
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// ChspsSecurityEnforcer::CheckAccessRightsL()
// This function is called by CPolicyServer when hspsThemeServer receives user request. 
// Access rights are hard-coded here for S60 3.1, however, in later versions 
// support for dynamic configuration of access rights must taken care. 
// This would be appropriate to solve together with TARM-policy implementation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
#ifdef _hsps_SECURITY_NOT_IN_USE_
TBool ChspsSecurityEnforcer::CheckAccessRightsL( const RMessage2& /*aMessage*/ )
    {
    return ETrue;
    }
#else
TBool ChspsSecurityEnforcer::CheckAccessRightsL( const RMessage2& aMessage )
    {
    TBool passed = EFalse;
    TInt function = aMessage.Function();
    TInt secure_id = aMessage.SecureId().iId;
    TInt vendor_id = aMessage.VendorId().iId;

#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsSecurityEnforcer::CheckAccessRightsL(): - SID: %d, VID: %d, function: %d" ),
                secure_id,
                vendor_id,
                function );
        }
#endif

    switch ( function )
        {
        // installation:
        case EhspsInstallTheme:
        case EhspsInstallNextPhase:
        case EhspsCancelInstallTheme:
        case EhspsReinstallConf:
            {
            if( CheckAccessControlListL( secure_id ) )    
                {
                passed = ETrue;
                }
            else
                {
                passed = aMessage.HasCapability( ECapabilityWriteDeviceData );
                }
            break;
            }
        // maintenance
        case EhspsGetListHeaders:
        case EhspsGetNextHeader:
        case EhspsCancelGetListHeaders:
        case EhspsSetActiveTheme:
        case EhspsRestoreDefault:
        case EhspsAddPlugin:
        case EhspsRemovePlugin:
        case EhspsSetPluginSettings:
        case EhspsGetPluginOdt:    
        case EhspsSetActivePlugin:
        case EhspsMovePlugins:
        case EhspsReplacePlugin:
        case EhspsSetConfState:
        case EhspsRestoreActiveAppConf:
        case EhspsUpdatePluginConf:    
        case EhspsRestoreConfigurations:
            {
            if( CheckAccessControlListL( secure_id ) )
                {
                passed = ETrue;
                }
            else
                {
                passed = aMessage.HasCapability( ECapabilityWriteDeviceData );
                }
            break;
            }
        case EhspsRemoveTheme:
            {
            if( CheckAccessControlListL( secure_id ) )
                {
                passed = ETrue;
                }
            else
                {
                passed = aMessage.HasCapability( ECapabilityWriteDeviceData );
                }
            break;
            }
        // theme usage
        case EhspsGetODT:
        case EhspsGetODTUpdate:
        case EhspsCancelGetODTUpdate:
        case EhspsAccessResourceFile:
        case EhspsCopyResources:            
            {
            if( CheckAccessControlListL( secure_id ) )
                {
                passed = ETrue;
                }
            else
                {
                passed = aMessage.HasCapability( ECapabilityWriteDeviceData );
                }
            break;
            }

        default:
            {
            passed = EFalse;
            break;
            }
        }
    
#ifdef HSPS_LOG_ACTIVE
    if ( !passed )
        {
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsSecurityEnforcer::CheckAccessRightsL(): - security violation, client process is now frozen." ) );
            }
        }
#endif
    
    return passed;    
    }
#endif
    
// end of file
