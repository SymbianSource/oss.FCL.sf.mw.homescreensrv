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
* Description:  ?Description
 *  Version     : %version: ou1s60ui#6 % << Don't touch! Updated by Synergy at check-out.
 *
*/


#include <s32mem.h>
#include <LiwCommon.h>
#include <e32std.h>

#include "cpsecuritypolicy.h"
#include "cpdebug.h"
#include "cpglobals.h"

_LIT( KHex, "0x" );
_LIT( KTCB, "TCB" );
_LIT( KLocalServices, "LocalServices" );
_LIT( KLocation, "Location" );
_LIT( KNetworkServices, "NetworkServices" );
_LIT( KReadUserData, "ReadUserData" );
_LIT( KUserEnvironment, "UserEnvironment" );
_LIT( KWriteUserData, "WriteUserData" );
_LIT( KAllFiles, "AllFiles" );
_LIT( KCommDD, "CommDD" );
_LIT( KDiskAdmin, "DiskAdmin" );
_LIT( KDrm, "Drm" );
_LIT( KMultimediaDD, "MultimediaDD" );
_LIT( KNetworkControl, "NetworkControl" );
_LIT( KPowerMgmt, "PowerMgmt" );
_LIT( KProtServ, "ProtServ" );
_LIT( KReadDeviceData, "ReadDeviceData" );
_LIT( KSurroundingsDD, "SurroundingsDD" );
_LIT( KSwEvent, "SwEvent" );
_LIT( KTrustedUI, "TrustedUI" );
_LIT( KWriteDeviceData, "WriteDeviceData" );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPSecurityPolicy::CCPSecurityPolicy( const RMessage2& aMessage ): 
                                                           iMessage( aMessage )
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPSecurityPolicy::ConstructL()
    {
  
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPSecurityPolicy* CCPSecurityPolicy::NewL( const RMessage2& aMessage )
    {
    CCPSecurityPolicy* self = CCPSecurityPolicy::NewLC( aMessage );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPSecurityPolicy* CCPSecurityPolicy::NewLC( const RMessage2& aMessage )
    {
    CCPSecurityPolicy* self = new( ELeave ) CCPSecurityPolicy( aMessage );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPSecurityPolicy::~CCPSecurityPolicy()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPSecurityPolicy::GetDefaultSecurityL( RBuf8& aResult ) const
    {
    CLiwDefaultMap* aclMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* writeMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* deleteMap = CLiwDefaultMap::NewLC();
    
    TUint32 publisherUid = iMessage.SecureId();
    
    TBuf<10> uidString;
    uidString.Append( KHex );
    uidString.AppendNumFixedWidth(publisherUid,EHex,8);
    
    writeMap->InsertL( KUIDs, TLiwVariant( uidString ) );
    deleteMap->InsertL( KUIDs, TLiwVariant( uidString ) );
    
    aclMap->InsertL( KACLWrite, TLiwVariant( writeMap ) );
    aclMap->InsertL( KACLDelete, TLiwVariant( deleteMap ) );

    //externalize
    aResult.CreateL( aclMap->Size() );
    RDesWriteStream strm( aResult );
    CleanupClosePushL( strm );
    aclMap->ExternalizeL( strm );
    strm.CommitL( );
     
    CleanupStack::PopAndDestroy( &strm );    
    CleanupStack::PopAndDestroy( deleteMap );
    CleanupStack::PopAndDestroy( writeMap );
    CleanupStack::PopAndDestroy( aclMap );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPSecurityPolicy::CheckSecurityL( TPtrC8& aBinary, 
                                        TACLPolicy aPolicy ) const
    {
    TBool error( ETrue );    
    RDesReadStream datastrm( aBinary );
    CleanupClosePushL( datastrm );
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC( datastrm );
    TLiwVariant variant;
    variant.PushL();
    switch( aPolicy )
        {
        case ERead:
            if( map->FindL( KACLRead, variant ) )
                {
                error = DoCheckSecurityL( variant.AsMap() );
                }
        break;
        case EWrite:
            if( map->FindL( KACLWrite, variant ) )
                {
                error = DoCheckSecurityL( variant.AsMap() );
                }
        break;
        case EDelete:
            if( map->FindL( KACLDelete, variant ) )
                {
                error = DoCheckSecurityL( variant.AsMap() );
                }
        break;
        default:
            User::Leave( KErrNotSupported );
        }
    CleanupStack::PopAndDestroy( &variant );
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( &datastrm );
    return error;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//    
TBool CCPSecurityPolicy::DoCheckSecurityL( const CLiwMap* aMap ) const
    {
    TLiwVariant variant;
    variant.PushL();
    TBool denyAccess( ETrue );
    if( aMap->FindL( KUIDs, variant ) )
        {
        TPtrC uidList( KNullDesC );
        if( variant.Get( uidList) )
            {
            denyAccess = CheckUids( uidList );
            }
        }
    if( denyAccess && aMap->FindL( KCapability, variant ) )
        {
        TPtrC capList( KNullDesC );
        if( variant.Get( capList ) )
            {
            denyAccess = CheckCapabilities( capList );
            }
        }
    CleanupStack::PopAndDestroy( &variant );
    return !denyAccess;
    }

 // ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//   
TBool CCPSecurityPolicy::CheckUids( const TDesC& aList ) const
    {
    TInt i( 1 );
    TBool denyAccess( ETrue );
    TPtrC singleUid( KNullDesC );
    while( i )
        {
        ExctractNextToken( aList , singleUid, i );
        TLex converter( singleUid );
        converter.Inc( KHex().Length() );
        TUint32 aclUid( 0 );
        converter.Val( aclUid , EHex );
        if( iMessage.SecureId().iId == aclUid )
            {
            denyAccess = EFalse;
            break;
            }
        ++i;    
        }
    return denyAccess;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPSecurityPolicy::CheckCapabilities( const TDesC& aList ) const
    {
    TInt i( 1 );
    TBool denyAccess( EFalse );
    TPtrC singleCap( KNullDesC );
    while( i )
        {
        ExctractNextToken( aList , singleCap, i );
        if( !iMessage.HasCapability( GetCapability( singleCap ) ) )
            {
            denyAccess = ETrue;
            break;
            }  
        ++i;    
        }
    return denyAccess;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCPSecurityPolicy::ExctractNextToken( const TDesC& aList, 
                                            TPtrC& aResult , TInt& aPos ) const
    {
    TPtrC buf( aList );
    for( TInt x(0); x < aPos; x++ )
        {
        TInt pos = buf.Locate(':');
        if( pos!= KErrNotFound )
            {
            aResult.Set( buf.Left( pos ) );
            buf.Set( buf.Right( buf.Length() - pos - 1 ) );
            }
        else
            {
            aResult.Set( buf );
            aPos = -1;
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TCapability CCPSecurityPolicy::GetCapability( const TDesC& aCapability ) const
    {
    TCapability ret( ECapability_None );
    if( aCapability == KTCB )
        {
        ret = ECapabilityTCB;
        }
    else if( aCapability == KLocalServices )
        {
        ret = ECapabilityLocalServices;
        }
    else if( aCapability == KLocation )
        {
        ret = ECapabilityLocation;
        }
    else if( aCapability == KNetworkServices )
        {
        ret = ECapabilityNetworkServices;
        }
    else if( aCapability == KReadUserData )
        {
        ret = ECapabilityReadUserData;
        }
    else if( aCapability == KUserEnvironment )
        {
        ret = ECapabilityUserEnvironment;
        }
    else if( aCapability == KWriteUserData )
        {
        ret = ECapabilityWriteUserData;
        }
    else if( aCapability == KAllFiles )
        {
        ret = ECapabilityAllFiles;
        }
    else if( aCapability == KDiskAdmin )
        {
        ret = ECapabilityDiskAdmin;
        }
    else if( aCapability == KCommDD )
        {
        ret = ECapabilityCommDD;
        }
    else if( aCapability == KDrm )
        {
        ret = ECapabilityDRM;
        }
    else if( aCapability == KMultimediaDD )
        {
        ret = ECapabilityMultimediaDD;
        }
    else if( aCapability == KNetworkControl )
        {
        ret = ECapabilityNetworkControl;
        }
    else if( aCapability == KPowerMgmt )
        {
        ret = ECapabilityPowerMgmt;
        }
    else if( aCapability == KProtServ )
        {
        ret = ECapabilityProtServ;
        }                            
    else if( aCapability == KReadDeviceData )
        {
        ret = ECapabilityReadDeviceData;
        }                            
    else if( aCapability == KSurroundingsDD )
        {
        ret = ECapabilitySurroundingsDD;
        }
    else if( aCapability == KSwEvent )
        {
        ret = ECapabilitySwEvent;
        }
    else if( aCapability == KTrustedUI )
        {
        ret = ECapabilityTrustedUI;
        }                        
    else if( aCapability == KWriteDeviceData )
        {
        ret = ECapabilityWriteDeviceData;
        }
    else ret = ECapability_None;                        
    return ret;
    }   

