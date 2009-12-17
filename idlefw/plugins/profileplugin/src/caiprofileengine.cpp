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
* Description:  The engine class of profile plugin. 
*
*/


// INCLUDE FILES
#include "caiprofileengine.h"
#include "maiprofilepluginnotifier.h"
#include "aiprofileplugincontentmodel.h"

#include <w32std.h> 
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfilesNamesArray.h>
#include <MProfileName.h>
#include <Profile.hrh>
#include <CProfileChangeNotifyHandler.h>
#include <PUAcodes.hrh> 
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <RSSSettings.h>


#include <aiprofilepluginres.rsg>

#include <startupdomainpskeys.h>

const TInt KMaxProfileNameLength( 64 );
const TInt KGeneralProfileId( 0 );
const TInt KSilentProfileId( 1 );
const TInt KOfflineProfileId( 5 );

const TInt KMaxActiveProfileLength( 64 );
const TUid KUidProfileApp = { 0x100058F8 }; 
const TUid KProfileAppSettingViewId = { 2 };

_LIT( KAiProfilePluginResourceFileName, "z:aiprofilepluginres.rsc");

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------
// Default constructor
// ---------------------------------------------------------
//
CAiProfileEngine::CAiProfileEngine( MAiProfilePluginNotifier* aProfilePluginNotifier ) :
    iProfilePluginNotifier ( aProfilePluginNotifier ),
    iResourceLoader( *CCoeEnv::Static() )
    {
    }
    
// ---------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------
//
CAiProfileEngine* CAiProfileEngine::NewL( MAiProfilePluginNotifier* aProfilePluginNotifier )
    {
    CAiProfileEngine* self = new( ELeave ) CAiProfileEngine( aProfilePluginNotifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------
//
void CAiProfileEngine::ConstructL()
    {
    TParse parse;
    parse.Set( KAiProfilePluginResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file.
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), fileName );

    // Open resource file.
    TRAP_IGNORE( iResourceLoader.OpenL( fileName ) );
    
    User::LeaveIfError( iSSSettings.Open() );

    iProfileEngine = CreateProfileEngineL();    
   }
    
    
// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CAiProfileEngine::~CAiProfileEngine()
    {
    iSSSettings.CancelAll( *this );
    iSSSettings.Close();
    delete iProfileNotifier;
	delete iActiveProfileName;
	delete iSwapProfileName;
	
	if( iProfileNamePointerArray.Count() )
    	{
        iProfileNamePointerArray.ResetAndDestroy();
        }

    if( iProfileEngine )
        {
        iProfileEngine->Release();
        }
    
    iResourceLoader.Close();            	 
    }
    
// ---------------------------------------------------------
// Updates profiles
// ---------------------------------------------------------
//
void CAiProfileEngine::UpdateProfileNamesL()
	{
	//update active profile name
	HBufC* activeProfileName = NULL;
	
	MProfile* profile = iProfileEngine->ActiveProfileLC();	
	const MProfileName& name = profile->ProfileName();	
	activeProfileName = name.Name().AllocLC();

	SetActiveProfileNameL( *activeProfileName );
      
    //update profile name list	
	MProfilesNamesArray* profileNamesArray = iProfileEngine->ProfilesNamesArrayLC();
    
    SetProfileNameListL( *profileNamesArray );
    
    //update swap profile name
    HBufC* swapProfileName = NULL;
    
    TInt activeProfileId = iProfileEngine->ActiveProfileId();
    
    if( activeProfileId == KSilentProfileId )
    	{
        const MProfileName* generalProfileName = profileNamesArray->ProfileName( KGeneralProfileId );	
		swapProfileName = generalProfileName->Name().AllocLC();	
    	}
    else
        {
        const MProfileName* silentProfileName = profileNamesArray->ProfileName( KSilentProfileId );	
		swapProfileName = silentProfileName->Name().AllocLC();	
        }
    
    TPtrC swapProfileNamePtr( *swapProfileName );
	HBufC* activateProfileString = NULL;
    activateProfileString = StringLoader::LoadLC( R_AI_PERS_PROF_TOGGLE, swapProfileNamePtr );    	
    
    SetSwapProfileNameL( *activateProfileString );

    CleanupStack::PopAndDestroy( 5 ); //profile, profileName, profileNamesArray, swapProfileName, activateProfileString 
	}

// ---------------------------------------------------------
// Checks SIM card status
// ---------------------------------------------------------
//
TBool CAiProfileEngine::ShowOfflineMessageL()
	{
	TInt result = ETrue;
	
    TInt simCardStatus( ESimNotPresent );
        
    RProperty simStatus;
    User::LeaveIfError( simStatus.Attach( KPSUidStartup, KPSSimStatus ) );
    User::LeaveIfError( simStatus.Get( simCardStatus ) );
    simStatus.Close();

 	if( simCardStatus == ESimNotPresent )
		{
		// SIM card does not exist.
	    HBufC* infoNoteText = StringLoader::LoadLC( R_SU_NOTE_INSERT_SIM );
	    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
	    note->ExecuteLD( *infoNoteText );
	    CleanupStack::PopAndDestroy( infoNoteText );
	    result = EFalse;	
		}
	else
		{
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		result = dlg->ExecuteLD( R_AI_LEAVE_OFFLINE_MODE_QUERY );	
		}
       
	return result;
	}

									  
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAiProfileEngine::SetActiveProfileNameL( const TDesC& aName )
    {
    HBufC* temp = aName.AllocL();
    delete iActiveProfileName;
    iActiveProfileName = NULL;
    TPtr profileNamePtr = temp->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( profileNamePtr );
    iActiveProfileName = temp;  
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
const TDesC& CAiProfileEngine::ActiveProfileName() const
    {
    if( iActiveProfileName )
        {
        return *iActiveProfileName;
        }
    
    return KNullDesC();
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAiProfileEngine::SetSwapProfileNameL( const TDesC& aName )
    {
    HBufC* temp = aName.AllocL();
    delete iSwapProfileName;
    iSwapProfileName = NULL;
    TPtr profileNamePtr = temp->Des();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( profileNamePtr );
    iSwapProfileName = temp;  
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
const TDesC& CAiProfileEngine::SwapProfileName() const
    {
    if( iSwapProfileName )
        {
        return *iSwapProfileName;
        }
    
    return KNullDesC();
    }


// ---------------------------------------------------------
// Set profile names
// ---------------------------------------------------------
//
void CAiProfileEngine::SetProfileNameListL( const MProfilesNamesArray& aArray )
    {
    if( iProfileNamePointerArray.Count() )
    	{
        iProfileNamePointerArray.ResetAndDestroy();
        }
      
    const TInt count = aArray.MdcaCount();
    TBufC<KMaxProfileNameLength> profileName;
         
    for( TInt i = 0; i < count; i++ )
    	{
    	profileName = aArray.MdcaPoint( i );
    	TPtr profileNamePtr = profileName.Des();
    	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( profileNamePtr );
    	HBufC* profile = profileNamePtr.AllocLC();
    	User::LeaveIfError( iProfileNamePointerArray.Append( profile ));
    	CleanupStack::Pop( profile );
    	}
    }
    

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
const TDesC& CAiProfileEngine::ProfileNameByIndex( TInt aIndex ) const
    {
 	if( iProfileNamePointerArray.Count() )
 		{
 		return *iProfileNamePointerArray[aIndex];
 		}
 	
 	return KNullDesC();
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAiProfileEngine::IsActiveProfileSilentL()
	{
	TBool isSilent = EFalse;
	MProfile* profile = iProfileEngine->ActiveProfileLC();
	isSilent = profile->IsSilent();
	CleanupStack::PopAndDestroy();
	return isSilent; 
	}

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TBool CAiProfileEngine::IsActiveProfileTimedL()
	{
	return iProfileEngine->IsActiveProfileTimedL();
	}

// ---------------------------------------------------------
// Number of profiles
// ---------------------------------------------------------
//
TInt CAiProfileEngine::NumberOfProfiles()
	{	
	return iProfileNamePointerArray.Count();
	}


// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CAiProfileEngine::HandleAiEventL( TInt aEvent, const TDesC& aParam )
	{
	switch ( aEvent )
        {
        case EAiProfileEventSwitchByIndex:
            HandleSwitchByIndexL( aParam );
            break;

        case EAiProfileEventSwitchByName:
            HandleSwitchByNameL( aParam );
            break;

        case EAiProfileEventSwap:
            HandleSwapL( aParam );
            break;
            
        case EAiProfileEditActive:
            HandleEditActiveProfileL();
            break;
        default:
            break;
        }
	}


// -----------------------------------------------------------------------------
// Handles profile switch by index event
// -----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwitchByIndexL( const TDesC& aParam )
	{
    if ( aParam.Length() > 0 )
    	{
        TInt profileId = KErrNotFound;
		TPtrC ptr( aParam );
		TLex lexer( ptr );
			
    	TInt err = lexer.Val( profileId );  
		if ( err == KErrNone )
			{
			MProfilesNamesArray* profileNamesArray = iProfileEngine->ProfilesNamesArrayLC();
			const MProfileName* profileName = profileNamesArray->ProfileName( profileId );
		 
		    if( profileName )
        		{
	            profileId = profileName->Id();
		    	SetActiveProfileL( profileId );
        		}
		    
		    CleanupStack::PopAndDestroy();
		   	}
    	}
	}

// -----------------------------------------------------------------------------
// Handles profile switch by name event
// -----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwitchByNameL( const TDesC& aParam )
	{
    if ( aParam.Length() > 0 )
    	{
    	TInt profileId = KErrNotFound;
        MProfilesNamesArray* profileNamesArray = iProfileEngine->ProfilesNamesArrayLC();
        profileId = profileNamesArray->FindByName( aParam );
        if( profileId != KErrNotFound )
        	{
        	SetActiveProfileL( profileId );
        	}
    	
    	CleanupStack::PopAndDestroy();
    	}
	}

// -----------------------------------------------------------------------------
// Handles profile swap event
// -----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwapL( const TDesC& aParam )
	{
	if ( aParam.Length() > 0 )
		{
		TInt profileId = KErrNotFound;
		TPtrC ptr( aParam );
		TLex lexer( ptr );
			
    	TInt err = lexer.Val( profileId );  
		if ( err == KErrNone )
			{
			TInt activeProfile = iProfileEngine->ActiveProfileId();
			
			if( activeProfile != profileId )
				{
				MProfilesNamesArray* profileNamesArray = iProfileEngine->ProfilesNamesArrayLC();
        		profileId = profileNamesArray->FindById( profileId );
		    
		    	if( profileId != KErrNotFound )
        			{
		    		TRAP_IGNORE( SetActiveProfileL( profileId ) );
        			}
        		
        		CleanupStack::PopAndDestroy();	
				}
			else
				{
				SetActiveProfileL( KGeneralProfileId );	
				}
		   	}
		}
	}

// -----------------------------------------------------------------------------
// Handles edit active profile event
// -----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleEditActiveProfileL()
    {
    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    CleanupClosePushL(ws);

    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KUidProfileApp );

    if ( task.Exists() )
        {
        task.EndTask();
        User::After( 500000 );
        }
    CleanupStack::PopAndDestroy(&ws);
    
    TVwsViewId viewid( KUidProfileApp, KProfileAppSettingViewId );
    TInt profileId = iProfileEngine->ActiveProfileId();
    TBuf8<KMaxActiveProfileLength> buf;
    buf.AppendNum(profileId);
    CEikonEnv::Static()->AppUi()->ActivateViewL( viewid ,KProfileAppSettingViewId,buf);
    

    }

// -----------------------------------------------------------------------------
//  Set active profile
// -----------------------------------------------------------------------------
//
void CAiProfileEngine::SetActiveProfileL( const TInt aProfileId )
	{
	TInt activeProfileId = iProfileEngine->ActiveProfileId();
	
	if ( activeProfileId == KOfflineProfileId && aProfileId != KOfflineProfileId )
   		{
    	if( !ShowOfflineMessageL() )
    		{
    		// User doesn't want to activate RF or
    		// SIM card does not exist.
    		return;
    		}
    	}
    	
   	iProfileEngine->SetActiveProfileL( aProfileId );   	
 	}
		
// ---------------------------------------------------------------------------
// Resumes the engine
// ---------------------------------------------------------------------------
//
void CAiProfileEngine::ResumeL()
    {
    User::LeaveIfError( iSSSettings.Open() );
 
    //Register to listen ALS activation, if ALS status changes,
	//profile must be republished.
    TInt err = iSSSettings.Register( ESSSettingsAls, *this );

    if( err == KErrNotSupported || err == KErrAlreadyExists )
        {
        //ALS not supported or already registered, that's fine
        err = KErrNone;
        }

    User::LeaveIfError( err );

	//Start to listen profile changes.
	delete iProfileNotifier;
    iProfileNotifier = NULL;	
		
    iProfileNotifier = CProfileChangeNotifyHandler::NewL( this );
   	}

// ---------------------------------------------------------------------------
// Suspends the engine
// ---------------------------------------------------------------------------
//
void CAiProfileEngine::Suspend()
    {
 	iSSSettings.CancelAll( *this );
    iSSSettings.Close();
   	delete iProfileNotifier;
   	iProfileNotifier = NULL;	
    }

// ---------------------------------------------------------------------------
// From class MProfileChangeObserver
// ---------------------------------------------------------------------------
//

void CAiProfileEngine::HandleActiveProfileEventL(
							TProfileEvent aProfileEvent,
							TInt /*aProfileId*/ )
    {
	//Profile activated or modified.
    if( ( aProfileEvent == EProfileNewActiveProfile ) ||
    	( aProfileEvent == EProfileActiveProfileModified ) )
        {
        UpdateProfileNamesL();
        iProfilePluginNotifier->NotifyContentUpdate();
        }
    }
    
// ---------------------------------------------------------------------------
// From class MSSSettingsObserver.
// ---------------------------------------------------------------------------
//
void CAiProfileEngine::PhoneSettingChanged( 
								TSSSettingsSetting aSetting,
								TInt /*aNewValue*/ )
    {
    if( aSetting == ESSSettingsAls )
        {
        TRAP_IGNORE( UpdateProfileNamesL() );
        iProfilePluginNotifier->NotifyContentUpdate();
        }
    }


TBool CAiProfileEngine::IsOffline()
	{
	return iProfileEngine->ActiveProfileId() == KOfflineProfileId;
	}

