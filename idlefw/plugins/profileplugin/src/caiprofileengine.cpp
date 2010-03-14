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


// System includes
#include <w32std.h> 
#include <ProEngFactory.h>
#include <MProfileEngine.h>
#include <MProEngEngine.h>
#include <MProEngProfile.h>
#include <MProEngProfileName.h>
#include <MProEngProfileNameArray.h>
#include <MProEngNotifyHandler.h>
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
#include <startupdomainpskeys.h>

// User includes
#include <aiprofilepluginres.rsg>
#include "caiprofileengine.h"
#include "maiprofilepluginnotifier.h"
#include "aiprofileplugincontentmodel.h"

// Constants
const TInt KMaxProfileNameLength( 64 );
const TInt KGeneralProfileId( 0 );
const TInt KSilentProfileId( 1 );
const TInt KOfflineProfileId( 5 );

const TInt KMaxActiveProfileLength( 64 );
const TUid KUidProfileApp = { 0x100058F8 }; 
const TUid KProfileAppSettingViewId = { 2 };

_LIT( KAiProfilePluginResourceFileName, "z:aiprofilepluginres.rsc");

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAiProfileEngine::CAiProfileEngine
//
// ----------------------------------------------------------------------------
//
CAiProfileEngine::CAiProfileEngine( 
    MAiProfilePluginNotifier* aProfilePluginNotifier )
    : iProfilePluginNotifier ( aProfilePluginNotifier ),    
    iResourceLoader( *CCoeEnv::Static() )
    {
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::NewL
// 
// ----------------------------------------------------------------------------
//
CAiProfileEngine* CAiProfileEngine::NewL( 
    MAiProfilePluginNotifier* aProfilePluginNotifier )
    {
    CAiProfileEngine* self = 
        new( ELeave ) CAiProfileEngine( aProfilePluginNotifier );
    CleanupStack::PushL( self );
    
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::ConstructL
//
// ----------------------------------------------------------------------------
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

    // Register to listen ALS activation, if ALS status changes,
    // profile must be republished.
    TInt err( iSSSettings.Register( ESSSettingsAls, *this ) );
        
    if( err == KErrNotSupported || err == KErrAlreadyExists )
        {
        // ALS not supported or already registered, that's fine
        err = KErrNone;
        }

    User::LeaveIfError( err );
    
    iProfileEngine = ProEngFactory::NewEngineL();
    
    // Start to listen profile changes.
    iProfileNotifier = ProEngFactory::NewNotifyHandlerL();
    
    iProfileNotifier->RequestActiveProfileNotificationsL( *this );
    iProfileNotifier->RequestProfileNameArrayNotificationsL( *this );
    iProfileNotifier->RequestProfileActivationNotificationsL( *this ); 
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::~CAiProfileEngine
//
// ----------------------------------------------------------------------------
//
CAiProfileEngine::~CAiProfileEngine()
    {
    iSSSettings.CancelAll( *this );
    iSSSettings.Close();
    
	delete iActiveProfileName;
	delete iSwapProfileName;
	
	iProfileNamePointerArray.ResetAndDestroy();
	
	if ( iProfileNotifier )
	    {
        iProfileNotifier->CancelAll();
	    }
	
	delete iProfileNotifier;
	
    if( iProfileEngine )
        {
        iProfileEngine->Release();
        }
            
    iResourceLoader.Close();
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::UpdateProfileNamesL()
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::UpdateProfileNamesL()
	{
    // Update active profile name
    MProEngProfile* profile( iProfileEngine->ActiveProfileLC() );	
	
	const MProEngProfileName& name( profile->ProfileName() );	

	HBufC* activeProfileName( name.Name().AllocLC() );

	SetActiveProfileNameL( *activeProfileName );
      
    // Update profile name list	
	MProEngProfileNameArray* profileNamesArray( 
        iProfileEngine->ProfileNameArrayLC() );
    
    SetProfileNameListL( *profileNamesArray );
    
    // Update swap profile name
    HBufC* swapProfileName( NULL );
    
    TInt activeProfileId( iProfileEngine->ActiveProfileId() );
    
    if( activeProfileId == KSilentProfileId )
    	{
        TInt generalProfileIndex( 
            profileNamesArray->FindById( KGeneralProfileId ) );
        
		swapProfileName = 
            profileNamesArray->MdcaPoint( generalProfileIndex ).AllocLC() ;		        
    	}
    else
        {
        TInt silentProfileIndex( 
            profileNamesArray->FindById( KSilentProfileId ) );	
		
        swapProfileName = 
            profileNamesArray->MdcaPoint( silentProfileIndex ).AllocLC() ;                
        }
    
    TPtrC swapProfileNamePtr( *swapProfileName );
	   
	HBufC* activateProfileString( StringLoader::LoadLC( 
        R_AI_PERS_PROF_TOGGLE, swapProfileNamePtr ) );    	
    
    SetSwapProfileNameL( *activateProfileString );

    CleanupStack::PopAndDestroy( 5 ); //profile, profileName, // profileNamesArray, swapProfileName, activateProfileString           
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::ShowOfflineMessageL
//
// ----------------------------------------------------------------------------
//
TBool CAiProfileEngine::ShowOfflineMessageL()
	{
	TInt result( ETrue );
	
    TInt simCardStatus( ESimNotPresent );
        
    RProperty simStatus;
    User::LeaveIfError( simStatus.Attach( KPSUidStartup, KPSSimStatus ) );
    User::LeaveIfError( simStatus.Get( simCardStatus ) );
    simStatus.Close();

 	if ( simCardStatus == ESimNotPresent )
		{
		// SIM card does not exist.
	    HBufC* infoNoteText = StringLoader::LoadLC( R_SU_NOTE_INSERT_SIM_AND_RESTART );
		
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
								  
// ----------------------------------------------------------------------------
// CAiProfileEngine::SetActiveProfileNameL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::SetActiveProfileNameL( const TDesC& aName )
    {
    HBufC* temp( aName.AllocL() );
    
    TPtr profileNamePtr( temp->Des() );
    
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( profileNamePtr );
    
    delete iActiveProfileName;
    iActiveProfileName = NULL;
    
    iActiveProfileName = temp;  
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::ActiveProfileName
//
// ----------------------------------------------------------------------------
//
const TDesC& CAiProfileEngine::ActiveProfileName() const
    {
    if( iActiveProfileName )
        {
        return *iActiveProfileName;
        }
    
    return KNullDesC();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::SetSwapProfileNameL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::SetSwapProfileNameL( const TDesC& aName )
    {
    HBufC* temp( aName.AllocL() );
        
    TPtr profileNamePtr( temp->Des() );
    
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( profileNamePtr );
    
    delete iSwapProfileName;    
    iSwapProfileName = NULL;

    iSwapProfileName = temp;  
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::SwapProfileName
//
// ----------------------------------------------------------------------------
//
const TDesC& CAiProfileEngine::SwapProfileName() const
    {
    if( iSwapProfileName )
        {
        return *iSwapProfileName;
        }
    
    return KNullDesC();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::SetProfileNameListL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::SetProfileNameListL( 
    const MProEngProfileNameArray& aArray )
    {
    iProfileNamePointerArray.ResetAndDestroy();

    const TInt count( aArray.MdcaCount() );
    
    TBufC<KMaxProfileNameLength> profileName;
         
    for ( TInt i = 0; i < count; i++ )
    	{
    	profileName = aArray.MdcaPoint( i );
    	
    	TPtr profileNamePtr( profileName.Des() );
    	
    	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( 
    	        profileNamePtr );
    	
    	HBufC* profile( profileNamePtr.AllocLC() );
    	
    	iProfileNamePointerArray.AppendL( profile );    	
    	CleanupStack::Pop( profile );
    	}
    }
    
// ----------------------------------------------------------------------------
// CAiProfileEngine::ProfileNameByIndex
//
// ----------------------------------------------------------------------------
//
const TDesC& CAiProfileEngine::ProfileNameByIndex( TInt aIndex ) const
    {
    TInt count( iProfileNamePointerArray.Count() );
    
 	if(  aIndex < count && aIndex >= 0 )
 		{
 		return *iProfileNamePointerArray[ aIndex ];
 		}
 	
 	return KNullDesC();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::IsActiveProfileSilentL
//
// ----------------------------------------------------------------------------
//
TBool CAiProfileEngine::IsActiveProfileSilentL() const
	{
    MProEngProfile* profile( iProfileEngine->ActiveProfileLC() );
    	
	TBool silent( profile->IsSilent() );
	
	CleanupStack::PopAndDestroy();
	
	return silent; 
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::IsActiveProfileTimedL
// 
// ----------------------------------------------------------------------------
//
TBool CAiProfileEngine::IsActiveProfileTimedL() const
	{    
    MProfileEngine* engine = CreateProfileEngineL();
        
    TBool retval( EFalse );
    
    TRAP_IGNORE( retval = engine->IsActiveProfileTimedL() );
    
    engine->Release();
    
    return retval;	
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::NumberOfProfiles
//
// ----------------------------------------------------------------------------
//
TInt CAiProfileEngine::NumberOfProfiles() const
	{	
	return iProfileNamePointerArray.Count();
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleAiEventL
//
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleSwitchByIndexL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwitchByIndexL( const TDesC& aParam )
	{
    if ( aParam.Length() > 0 )
    	{
        TInt index( KErrNotFound );
		TPtrC ptr( aParam );
		TLex lexer( ptr );
			
    	TInt err( lexer.Val( index ) );
    	
		if ( err == KErrNone )
			{
            MProEngProfileNameArray* profileNamesArray( 
                iProfileEngine->ProfileNameArrayLC() );
		
            TInt profileId( profileNamesArray->ProfileId( index ) );
             
            if ( profileId != KErrNotFound )
                {
                SetActiveProfileL( profileId );
                }            
		    
		    CleanupStack::PopAndDestroy();
		   	}
    	}
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleSwitchByNameL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwitchByNameL( const TDesC& aParam )
	{
    if ( aParam.Length() > 0 )
    	{    	
        MProEngProfileNameArray* profileNamesArray( 
            iProfileEngine->ProfileNameArrayLC() );
            
        TInt index( profileNamesArray->FindByName( aParam ) );
        
        if( index != KErrNotFound )
        	{
        	SetActiveProfileL( profileNamesArray->ProfileId( index ) );
        	}
        
    	CleanupStack::PopAndDestroy();
    	}
	}

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleSwapL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleSwapL( const TDesC& aParam )
	{
	if ( aParam.Length() > 0 )
		{
		TInt profileId( KErrNotFound );
		TPtrC ptr( aParam );
		TLex lexer( ptr );
			
    	TInt err( lexer.Val( profileId ) );
    	
		if ( err == KErrNone )
			{
			TInt activeProfile( iProfileEngine->ActiveProfileId() );
			
			if( activeProfile != profileId )
				{
                MProEngProfileNameArray* profileNamesArray( 
                    iProfileEngine->ProfileNameArrayLC() );
	            						
        		profileId = profileNamesArray->FindById( profileId );
		    
		    	if( profileId != KErrNotFound )
        			{
		    		SetActiveProfileL( profileId );
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

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleEditActiveProfileL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleEditActiveProfileL()
    {
    RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );

    // Find the task with uid
    TApaTaskList taskList( ws );
    TApaTask task( taskList.FindApp( KUidProfileApp ) );

    if ( task.Exists() )
        {
        task.EndTask();
        User::After( 500000 );
        }
    
    CleanupStack::PopAndDestroy( &ws );
    
    TVwsViewId viewid( KUidProfileApp, KProfileAppSettingViewId );
    
    TInt profileId( iProfileEngine->ActiveProfileId() );
    
    TBuf8<KMaxActiveProfileLength> buf;
    buf.AppendNum( profileId );
    
    CEikonEnv::Static()->AppUi()->ActivateViewL( 
        viewid, KProfileAppSettingViewId, buf );
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::SetActiveProfileL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::SetActiveProfileL( const TInt aProfileId )
	{
	TInt activeProfileId( iProfileEngine->ActiveProfileId() );
	
	if ( activeProfileId == KOfflineProfileId && 
        aProfileId != KOfflineProfileId )
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

// ----------------------------------------------------------------------------
// CAiProfileEngine::IsOffline
//
// ----------------------------------------------------------------------------
//
TBool CAiProfileEngine::IsOffline() const
    {
    return iProfileEngine->ActiveProfileId() == KOfflineProfileId;
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::PhoneSettingChanged
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::PhoneSettingChanged( TSSSettingsSetting aSetting, 							
    TInt /*aNewValue*/ )
    {
    if( aSetting == ESSSettingsAls )
        {
        NotifyContentUpdate();
        }
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleActiveProfileModifiedL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleActiveProfileModifiedL()
    {
    NotifyContentUpdate();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleProfileNameArrayModificationL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleProfileNameArrayModificationL()
    {
    NotifyContentUpdate();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::HandleProfileActivatedL
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::HandleProfileActivatedL( TInt /*aProfileId*/ )
    {
    NotifyContentUpdate();
    }

// ----------------------------------------------------------------------------
// CAiProfileEngine::NotifyContentUpdate
//
// ----------------------------------------------------------------------------
//
void CAiProfileEngine::NotifyContentUpdate() 
    {
    TRAP_IGNORE( UpdateProfileNamesL() );
    
    iProfilePluginNotifier->NotifyContentUpdate();    
    }

// End of file
