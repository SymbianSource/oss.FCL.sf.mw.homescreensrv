/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Profile plug-in publisher
*
*/

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <PUAcodes.hrh>
#include <AknUtils.h>

// User includes
#include <aiprofilepluginuids.hrh>
#include <aicontentobserver.h>
#include <aiutility.h>

#include "aiprofileplugincontentmodel.h"
#include "caiprofileplugin.h"
#include "caiprofileengine.h"
#include "aipluginsettings.h"

// Constants

// PUA code for the timed profile, missing from PUAcodes.hrh
#define KAiTimedProfilePUA 0xF815
#define KAiRTL 0x200F

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidProfilePlugin, CAiProfilePlugin::NewL ) 
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CAiProfilePlugin::NewL
//
// ---------------------------------------------------------------------------
//
CAiProfilePlugin* CAiProfilePlugin::NewL()
    {
    CAiProfilePlugin* self = new ( ELeave ) CAiProfilePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ---------------------------------------------------------------------------
// CAiProfilePlugin::CAiProfilePlugin()
// 
// ---------------------------------------------------------------------------
//
CAiProfilePlugin::CAiProfilePlugin()
    {
    }
    
// ---------------------------------------------------------------------------
// CAiProfilePlugin::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::ConstructL()
    { 
    iContent = 
        AiUtility::CreateContentItemArrayIteratorL( KAiProfileContent );
    
    iEvents = 
        AiUtility::CreateContentItemArrayIteratorL( KAiProfileEvents );
    
    iResources = 
        AiUtility::CreateContentItemArrayIteratorL( KAiProfileResources );
    
    iEngine = CAiProfileEngine::NewL( this );
    
    iEngine->UpdateProfileNamesL();        
    }
    
// ---------------------------------------------------------------------------
// CAiProfilePlugin::~CAiProfilePlugin
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CAiProfilePlugin::~CAiProfilePlugin()
    {    	
    Release( iContent );
    Release( iEvents );
    Release( iResources );
    
    delete iActiveProfileAndChar;
    delete iPreviousProfileNameAndChar;
    delete iEngine;
    
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::PublishL
// Publishes the profiles
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::PublishL()
    {
    TInt err( KErrNone );
    TInt observers( iObservers.Count() );        
    TInt transactionId ( reinterpret_cast<TInt>( this ) );

    iCurrentCount = iEngine->NumberOfProfiles();
    
    for ( TInt i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer( iObservers[i] );
        
        err = observer->StartTransaction( transactionId );
		
		if ( err == KErrNotSupported )
            {
            // The observer does not support transactions, check for real errors.
            return;
            }
        
        //Active Profile name    
        if ( observer->CanPublish( *this, EAiProfileContentActiveProfileName, EAiProfileContentActiveProfileName ) )
        	{        	
       		observer->Publish( *this, EAiProfileContentActiveProfileName, iEngine->ActiveProfileName(), EAiProfileContentActiveProfileName );
        	}
        
        if ( observer->CanPublish( *this, EAiProfileActiveProfileNameAndIconChar, 0 ) )
            {
            delete iActiveProfileAndChar;
            iActiveProfileAndChar = NULL;
            
            // silent/non-silent icon + timed icon + space + possible RTL*2 = 5
            TInt maxChars( iEngine->ActiveProfileName().Length() + 5 );
            
            iActiveProfileAndChar = HBufC::NewL( maxChars );
            TPtr profileNamePtr( iActiveProfileAndChar->Des() );
           
            if( AknLayoutUtils::LayoutMirrored() )
                {
                profileNamePtr.Append( KAiRTL );
                }

            if ( iEngine->IsActiveProfileTimedL() )
                {
                profileNamePtr.Append( KAiTimedProfilePUA );                
                }
            
            if ( iEngine->IsActiveProfileSilentL() )
                {                
                profileNamePtr.Append( KPuaCodeSilentSymbol );
                }
            else
                {
                profileNamePtr.Append( KPuaCodeAprofSound );
                }
            
            _LIT( KSpace, " " ); 
            
            profileNamePtr.Append( KSpace );
            
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                profileNamePtr.Append( KAiRTL );
                }
            
            profileNamePtr.Append( iEngine->ActiveProfileName() );
                        
            if ( !iPreviousProfileNameAndChar || 
                    iPreviousProfileNameAndChar->CompareC( *iActiveProfileAndChar ) != 0 )
                {
                delete iPreviousProfileNameAndChar;
                iPreviousProfileNameAndChar = NULL;
                observer->Publish( *this, EAiProfileActiveProfileNameAndIconChar, profileNamePtr, 0 );
                
                iPreviousProfileNameAndChar = iActiveProfileAndChar->AllocL();
                }
            }
        
        //Swap Profile name ( 1.phase: General or Silent )
       	if ( observer->CanPublish( *this, EAiProfileContentSwapProfileName, EAiProfileContentSwapProfileName ) )
        	{
        	observer->Publish( *this, EAiProfileContentSwapProfileName, iEngine->SwapProfileName(), EAiProfileContentSwapProfileName );
        	}      
          
		//All profile names
       	// clean profiles that are already deleted.
       	// Cleans the array blindly from the end, because in the next
       	// step all the profiles are republished
        if ( iPreviousCount > iCurrentCount )
            {
            for( TInt k = iCurrentCount; k < iPreviousCount; k++ )
                {
                observer->Clean( *this, EAiProfileContentProfileName, k + 1 );
                }
            }
        
		for ( TInt j = 0; j < iCurrentCount; j++ )
			{
			if ( observer->CanPublish( *this, EAiProfileContentProfileName, j + 1 ) )
				{
				observer->Publish(*this, EAiProfileContentProfileName, iEngine->ProfileNameByIndex( j ), j + 1 );
				}
			}
      	
      	//Active Profile silent indicator char
        if ( observer->CanPublish( *this, EAiProfileActiveProfileSilentChar, EAiProfileActiveProfileSilentChar ) )
        	{
        	if ( iEngine->IsActiveProfileSilentL() )
        		{
        		TBuf<1> silent; // one character
        		silent.Append( KPuaCodeSilentSymbol );
        		
        		observer->Publish( *this, EAiProfileActiveProfileSilentChar, silent, EAiProfileActiveProfileSilentChar );
        		}
        	else
        		{
        		observer->Clean( *this, EAiProfileActiveProfileSilentChar, EAiProfileActiveProfileSilentChar );
        		}
        	}
        
        //Active Profile silent indicator resource
        if ( observer->CanPublish( *this, EAiProfileActiveProfileIcon, EAiProfileActiveProfileIcon ) )
        	{
    		observer->Clean( *this, EAiProfileActiveProfileIcon, EAiProfileActiveProfileSilentIconResource );
    		observer->Clean( *this, EAiProfileActiveProfileIcon, EAiProfileActiveProfileGeneralIconResource );
    		observer->Clean( *this, EAiProfileActiveProfileIcon, EAiProfileActiveProfileTimedIconResource );
        	
    		if ( iEngine->IsActiveProfileTimedL() )
        		{
        		observer->Publish( *this, 
        							EAiProfileActiveProfileIcon, 
        							EAiProfileActiveProfileTimedIconResource, 
        							EAiProfileActiveProfileTimedIconResource );
        		}
        	else if ( iEngine->IsActiveProfileSilentL() )
        		{
        		observer->Publish( *this, 
        							EAiProfileActiveProfileIcon, 
        							EAiProfileActiveProfileSilentIconResource, 
        							EAiProfileActiveProfileSilentIconResource );
        		}
        	else
        		{
        		observer->Publish( *this, 
        							EAiProfileActiveProfileIcon, 
        							EAiProfileActiveProfileGeneralIconResource, 
        							EAiProfileActiveProfileGeneralIconResource );
        		}
        	}

        if ( err == KErrNone )
            {
            err = observer->Commit( transactionId );
            
            if ( err == KErrNotSupported)
                {
                return;
                }
            }                
        }
    
    iPreviousCount = iCurrentCount;
    }
    
// ---------------------------------------------------------------------------
// CAiProfilePlugin::Start
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Start( TStartReason /*aReason*/ )
    {   
    iPublishRequired = ETrue;
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::Stop
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Stop( TStopReason /*aReason*/ )
    {    
    CleanPublishedProfileNames();
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::Resume
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Resume( TResumeReason aReason )
    {
    if ( aReason == EForeground )
        {
        TRAP_IGNORE( DoResumeL() );
        }
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::Suspend
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Suspend( TSuspendReason /*aReason*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::SubscribeL
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::SubscribeL( MAiContentObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::ConfigureL
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::ConfigureL( RAiSettingsItemArray& /*aSettings*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::GetProperty
// 
// ---------------------------------------------------------------------------
//
TAny* CAiProfilePlugin::GetProperty( TProperty aProperty )
    {
    if ( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );        
        }
    else if ( aProperty == EPublisherEvents )
        {
        return static_cast< MAiContentItemIterator* >( iEvents );
        }
    else if ( aProperty == EPublisherResources )
        {
        return static_cast< MAiContentItemIterator* >( iResources );        
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::HandleEvent
// 
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::HandleEvent( TInt aEvent, const TDesC& aParam )
    {    
    TRAP_IGNORE( iEngine->HandleAiEventL( aEvent, aParam ) );    
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::DoResumeL
//
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::DoResumeL()
    {
    if ( iPublishRequired )
        {
        iPublishRequired = EFalse;
        
        PublishL();
        }
    }
 
// ---------------------------------------------------------------------------
// CAiProfilePlugin::NotifyContentUpdate()
// This method is called from the engine, when the profile data has changed
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::NotifyContentUpdate()
    {        
    iPublishRequired = ETrue;
    
    TRAP_IGNORE( DoResumeL() );
    }

// ---------------------------------------------------------------------------
// CAiProfilePlugin::CleanPublishedProfileNames
// Clean profile names from content
// ---------------------------------------------------------------------------
//    
void CAiProfilePlugin::CleanPublishedProfileNames()
    {
    TInt obsCount( iObservers.Count() );
    
    for ( TInt i( 0 ); i < obsCount; i++ )
        {
        MAiContentObserver* observer( iObservers[i] );
        
        for ( TInt j( 0 ); j < iCurrentCount && observer; j++ )
            {
            observer->Clean( *this, EAiProfileContentProfileName, j + 1 );
            }
        }
    
    delete iPreviousProfileNameAndChar;
    iPreviousProfileNameAndChar = NULL;
    
    iPreviousCount = 0;
    }

// ======== GLOBAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / 
        sizeof( TImplementationProxy );
    
    return KImplementationTable;
    }
