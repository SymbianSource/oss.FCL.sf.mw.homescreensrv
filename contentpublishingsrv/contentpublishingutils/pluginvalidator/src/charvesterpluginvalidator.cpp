/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32property.h>
#include <ecom/ecom.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <contentharvesterplugin.h> // harvesting plugin

#include "charvesterpluginvalidator.h"
#include "cblacklisthandler.h"

const TUint32 KInProgressPropertyKey	= { 0x1028000E };
const TUint32 KHarvesterUid	= { 0x10282E5A };

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CHarvesterPluginValidator* CHarvesterPluginValidator::NewL( TUid aUid, 
                                                   TAny *aParameter )
    {
    CHarvesterPluginValidator* self = 
					CHarvesterPluginValidator::NewLC( aUid , aParameter );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CHarvesterPluginValidator* CHarvesterPluginValidator::NewLC( TUid aUid,
                                                    TAny *aParameter )
    {
    CHarvesterPluginValidator* self = new( ELeave ) 
				CHarvesterPluginValidator( aUid, aParameter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }



// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CHarvesterPluginValidator::~CHarvesterPluginValidator()
    {
    iInProgressProperty.Close( );
    delete iBlacklist;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CHarvesterPluginValidator::CHarvesterPluginValidator( TUid aUid, TAny* aParameter ): 
											CPluginValidator(aUid, aParameter),
                                            iStartup( ETrue )
    {
    
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::ConstructL()
    {
    iBlacklist = CBlacklistHandler::NewL( );    
    CPluginValidator::ConstructL();
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::ManagePluginsL()    
    {
    TInt errorCode = iInProgressProperty.Define( TUid::Uid( KHarvesterUid ), 
        		KInProgressPropertyKey, RProperty::EInt );
    
    if ( KErrAlreadyExists == errorCode )
    	{
    	TInt value(-1);
    	iInProgressProperty.Get( TUid::Uid( KHarvesterUid ), 
        		KInProgressPropertyKey, value);
    	if ( value == 1 )
    		{
    		// property value == inprogress
    		// there was a panic in the previous startup
    		// so we make unofficial blacklist official
    		iBlacklist->CopyBlacklistL( EFalse );
    		}
    	}
    else
    	{
    	User::LeaveIfError( errorCode );
    	}
    // copy blacklisted plugins to unoffical blacklist at startup
    iBlacklist->CopyBlacklistL( ETrue );
    
    // set property value to 1 (which means "in progress")
	iInProgressProperty.Set( TUid::Uid( KHarvesterUid ), 
    		KInProgressPropertyKey, 1 );
    
    CPluginValidator::ManagePluginsL();
    
    if ( iStartup )
    	{
    	RProperty::Set( KPSUidActiveIdle2, 
    		KActiveIdleCpsPluginsUpdated , EPSAiPluginsUpdated );
    	iStartup = EFalse;
    	}
    // set property value to 0 (which means "finished")
	iInProgressProperty.Set( TUid::Uid( KHarvesterUid ), 
    		KInProgressPropertyKey, 0 );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CHarvesterPluginValidator::LoadPluginL( TPluginInfo& aPluginInfo )    
    {
    if ( !iBlacklist->IsPresentL( aPluginInfo.iImplementationUid ) )
    	{
    	//first we append UID to the blacklist
    	iBlacklist->AppendL( aPluginInfo.iImplementationUid );
    	TAny* plug ( NULL );
    	TInt err( KErrNone );
    	TRAP( err, plug = REComSession::CreateImplementationL( 
										aPluginInfo.iImplementationUid, 
										aPluginInfo.iDtor_ID_Key, iParameter ) );
    	if( err==KErrNone && plug ) 
    		{
			TRAP_IGNORE( 
				CleanupStack::PushL( plug );
				aPluginInfo.iPlugin = plug;
				iPluginArray.AppendL( aPluginInfo );
				CleanupStack::Pop( plug );
   				static_cast<CContentHarvesterPlugin*>( plug )->UpdateL()
						);
    		}
    	//no panic during load so we can remove UID from blacklist
    	iBlacklist->RemoveL( aPluginInfo.iImplementationUid );
    	}
    }




