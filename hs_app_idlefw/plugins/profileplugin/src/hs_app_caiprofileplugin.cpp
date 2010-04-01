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


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <aicontentobserver.h>
#include <aiutility.h>
#include <PUAcodes.hrh>
#include <AknUtils.h>

#include "aiprofileplugincontentmodel.h"
#include <aiprofilepluginuids.hrh>
#include "hs_app_caiprofileplugin.h"
#include "hs_app_caiprofileengine.h"
#include "aipluginsettings.h"

// PUA code for the timed profile, missing from PUAcodes.hrh
#define KAiTimedProfilePUA 0xF815
#define KAiRTL 0x200F

// CONST CLASS VARIABLES
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidProfilePlugin, CAiProfilePlugin::NewL ) 
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CAiProfilePlugin* CAiProfilePlugin::NewL()
    {
    CAiProfilePlugin* self = new (ELeave) CAiProfilePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CAiProfilePlugin::CAiProfilePlugin()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::ConstructL()
    { 
    iInfo.iUid.iUid = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN; 
   
    iContent = AiUtility::CreateContentItemArrayIteratorL( KAiProfileContent );
    iEvents = AiUtility::CreateContentItemArrayIteratorL( KAiProfileEvents );
    iResources = AiUtility::CreateContentItemArrayIteratorL( KAiProfileResources );
    
    iIsUpdated = ETrue;
    iAlive = EFalse;        
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CAiProfilePlugin::~CAiProfilePlugin()
    {
    CleanPublishedProfileNames();
    Release( iContent );
    Release( iEvents );
    Release( iResources );   
    delete iActiveProfileAndChar;
    delete iPreviousProfileNameAndChar;
    delete iEngine;
    iObservers.Close();
    }

// ---------------------------------------------------------------------------
// Publishes the profiles
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::PublishL()
    {
    TInt err( KErrNone );
    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );

    iCurrentCount = iEngine->NumberOfProfiles();
    for ( int i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = iObservers[i];
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
            TInt maxChars = iEngine->ActiveProfileName().Length() + 5;
            
            iActiveProfileAndChar = HBufC::NewL( maxChars );
            TPtr profileNamePtr = iActiveProfileAndChar->Des();
           
            if( AknLayoutUtils::LayoutMirrored() )
                {
                profileNamePtr.Append( KAiRTL );
                }

            if ( iEngine->IsActiveProfileTimedL() )
                {
                profileNamePtr.Append( KAiTimedProfilePUA );                
                }
            if( iEngine->IsActiveProfileSilentL() )
                {                
                profileNamePtr.Append( KPuaCodeSilentSymbol );
                }
            else
                {
                profileNamePtr.Append( KPuaCodeAprofSound );
                }
            _LIT( KSpace, " " );   
            profileNamePtr.Append( KSpace );
            
            if( AknLayoutUtils::LayoutMirrored() )
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
        if( iPreviousCount > iCurrentCount )
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

		// in case of Offline profile profile indicator is not shown
        if ( observer->CanPublish( *this, EAiProfileContentActiveProfileName, EAiProfileContentActiveProfileName ) &&
        		iEngine->IsOffline() )
        	{
    		observer->Clean( *this, EAiProfileActiveProfileSilentChar, EAiProfileActiveProfileSilentChar );
    		observer->Clean( *this, EAiProfileActiveProfileIcon, 1 );
    		observer->Clean( *this, EAiProfileActiveProfileIcon, 2 );
            // uncomment also this and respective policy lines in profiles.xml if whole widget needs to be hidden in AI3 
    		//observer->Clean( *this, EAiProfileContentActiveProfileName, EAiProfileContentActiveProfileName );        		    		
    		}
        if ( err == KErrNone )
            {
            err = observer->Commit( transactionId );
            
            if ( err == KErrNotSupported)
                {
                return;
                }
            }
        
        iIsUpdated = EFalse;
        }
    iPreviousCount = iCurrentCount;
    }
    
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is requested to unload its engines due backup operation
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Stop( TAiTransitionReason /*aReason*/ )
    {
    FreeEngine();
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Resume( TAiTransitionReason aReason )
    {
    TRAP_IGNORE( DoResumeL( aReason ) ); 
    }
    
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is not allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::Suspend( TAiTransitionReason /*aReason*/ )
    {
    if ( iEngine && iAlive )
        {
        iEngine->Suspend();
        }
        
    iAlive = EFalse;
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// The plug-in MUST maintain a registry of subscribers and send 
// notification to all of them whenever the state changes or new content
// is available
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
    
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-ins take ownership of the settings array, so it must either
// store it in a member or free it.
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    aSettings.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Returns the extension interface. Actual type depends on the passed 
// aUid argument.
// ---------------------------------------------------------------------------
//
TAny* CAiProfilePlugin::Extension( TUid aUid )
    {    
    if (aUid == KExtensionUidProperty)
   		{
        return static_cast<MAiPropertyExtension*>(this);
    	}
    else if (aUid == KExtensionUidEventHandler)
    	{
        return static_cast<MAiEventHandlerExtension*>(this);
    	}
    else
    	{	
        return NULL;
    	}
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Read property of publisher plug-in.
// ---------------------------------------------------------------------------
//
TAny* CAiProfilePlugin::GetPropertyL( TInt aProperty )
    {
    TAny* property = NULL;
    
    switch ( aProperty )
        {
    case EAiPublisherInfo:
        {
         property = static_cast<TAiPublisherInfo*>( &iInfo );
        break;  
        }       

    case EAiPublisherContent:
        {
        property = static_cast<MAiContentItemIterator*>( iContent );
        break;    
        }        

    case EAiPublisherEvents:
        {
        property = static_cast<MAiContentItemIterator*>( iEvents );
        break;
        }
    
    case EAiPublisherResources:
        property = static_cast<MAiContentItemIterator*>( iResources );
        break;
       
    default:
        break;
        }

    return property;
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Write property value to optimize the content model.
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::SetPropertyL( TInt aProperty, TAny* aValue )
    {  
    if( aProperty == EAiPublisherInfo )
        {
        ASSERT( aValue );
        
        const TAiPublisherInfo* info( 
                static_cast<const TAiPublisherInfo*>( aValue ) );
        
        iInfo = *info;
        }
    }
 
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::HandleEvent(TInt aEvent, const TDesC& aParam)
	{
    if ( iEngine )
    	{
        // We have no way of reporting errors to framework so just ignore them.
        TRAP_IGNORE( iEngine->HandleAiEventL( aEvent, aParam ) );
    	}
	} 
    
// ---------------------------------------------------------
// This method is called from the engine, when the profile
// data content has been changed. Method call is made through
// the MAiProfilePluginNotifier interface.
// ---------------------------------------------------------
//
void CAiProfilePlugin::NotifyContentUpdate()
    {
    iIsUpdated = ETrue;
    
    TRAP_IGNORE( PublishL() );
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// framework instructs plug-in that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CAiProfilePlugin::DoResumeL( TAiTransitionReason aReason )
    {
    if ( !iEngine )
        {
        iEngine = CAiProfileEngine::NewL( this );
        }
    
	//update in startup phase and idle is on foreground.
    if( aReason != EAiBacklightOff && aReason != EAiIdleBackground )
    	{
    	// force republish in case layout has changed
      if ( aReason == EAiScreenLayoutChanged )
          {
          delete iPreviousProfileNameAndChar;
          iPreviousProfileNameAndChar = NULL;
          }

    	if ( !iAlive )
    	    {
            iEngine->ResumeL();
    	    }
    	
	    iEngine->UpdateProfileNamesL();
	    
        PublishL();
	    iAlive = ETrue;     
    	}   
    }
    
// ---------------------------------------------------------------------------
// Frees engine resources
// ---------------------------------------------------------------------------
//    
void CAiProfilePlugin::FreeEngine()
    {
    delete iEngine;
    iEngine = NULL;
    iAlive = EFalse;   
    }

// ---------------------------------------------------------------------------
// Clean profile names from content
// ---------------------------------------------------------------------------
//    
void CAiProfilePlugin::CleanPublishedProfileNames()
    {
    TInt obsCount( iObservers.Count() );
    for ( TInt i( 0 ); i < obsCount; i++ )
        {
        MAiContentObserver* observer = iObservers[i];
        for( TInt j( 0 ); j < iCurrentCount && observer; j++ )
            {
            observer->Clean( *this, EAiProfileContentProfileName, j + 1 );
            }
        }
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
