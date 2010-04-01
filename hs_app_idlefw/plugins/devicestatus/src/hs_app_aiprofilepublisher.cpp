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
* Description:  Profile publisher
*
*/


#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileName.h>
#include <Profile.hrh>
#include <CProfileChangeNotifyHandler.h>
#include <aidevstaplgres.rsg>
#include <PUAcodes.hrh>
#include "hs_app_aiprofilepublisher.h"
#include "hs_app_aiprioritizer.h"
#include "hs_app_ainwidpriorities.h"

// ======== MEMBER FUNCTIONS ========

// PUA code for the timed profile, missing from PUAcodes.hrh
#define KAiTimedProfilePUA 0xF815

CAiProfilePublisher::CAiProfilePublisher() : 
    iLastPublishedProfileId(ENothingPublished),
    iLastPublishedProfileSilent( EFalse ),
    iLastPublishedProfileTimed( EFalse )
    {
    }


void CAiProfilePublisher::ConstructL()
    {
    User::LeaveIfError( iSSSettings.Open() );

    iProfileEngine = CreateProfileEngineL();
    }


CAiProfilePublisher* CAiProfilePublisher::NewL()
    {
    CAiProfilePublisher* self = new( ELeave ) CAiProfilePublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiProfilePublisher::~CAiProfilePublisher()
    {
    //TRAP_IGNORE: leaving function called in non-leaving function
    TRAP_IGNORE(CleanLastProfileL());
    iSSSettings.CancelAll( *this );
    iSSSettings.Close();
    delete iProfileNotifier;

    if( iProfileEngine )
        {
        iProfileEngine->Release();
        }     
    }


void CAiProfilePublisher::ResumeL()
    {
  User::LeaveIfError( iSSSettings.Open() );
  //Register to listen ALS activation, if ALS status changes,
  //profile must be republished.
    TInt err = iSSSettings.Register( ESSSettingsAls, *this );

    if( err == KErrNotSupported ||
        err == KErrAlreadyExists )
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


void CAiProfilePublisher::Subscribe( MAiContentObserver& aObserver, 
                      MAiPropertyExtension& aExtension,
                                        MAiPublishPrioritizer& aPrioritizer,
                                        MAiPublisherBroadcaster& aBroadcaster )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    iPrioritizer = &aPrioritizer;
    iBroadcaster = &aBroadcaster;
    }


void CAiProfilePublisher::RefreshL( TBool aClean )
    {
    TRAP_IGNORE( RefreshProfileL( aClean ) );
    }


void CAiProfilePublisher::PhoneSettingChanged( TSSSettingsSetting aSetting,
                        TInt /*aNewValue*/ )
    {
    if( aSetting == ESSSettingsAls )
        {
        //refresh profile, can't do much if it doesn't work
        TRAP_IGNORE( RefreshProfileL( ETrue ) );
        }
    }


void CAiProfilePublisher::HandleActiveProfileEventL(
              TProfileEvent aProfileEvent,
              TInt /*aProfileId*/ )
    {
  //Profile activated or modified.
    if( ( aProfileEvent == EProfileNewActiveProfile ) ||
      ( aProfileEvent == EProfileActiveProfileModified ) )
        {
        iActiveProfilePublish = ETrue; 
        RefreshProfileL( ETrue );
        }
    }


void CAiProfilePublisher::CleanLastProfileL()
    {
    if ( iLastPublishedProfileSilent )
        {
        iContentObserver->Clean( *iExtension,
                                  EAiDeviceStatusContentSilentIndicator,
                                  0 );
        iLastPublishedProfileSilent = EFalse;
        }
    
    if ( iLastPublishedProfileTimed )
        {
        iContentObserver->Clean( *iExtension,
                                  EAiDeviceStatusContentTimedProfileIndicator,
                                  0 );
        iLastPublishedProfileTimed = EFalse;
        }
    
    if( iLastPublishedProfileId == EGeneralProfilePublished )
      {
        iContentObserver->Clean( *iExtension,
                                  EAiDeviceStatusContentGeneralProfileName,
                                  0 );
        }
    else if( iLastPublishedProfileId == EOfflineProfilePublished )
      {
      iPrioritizer->TryToCleanL( *iBroadcaster,
                                  EAiDeviceStatusContentNetworkIdentity,
                                  EAiOfflineProfile );
      }
    else
      {
      iContentObserver->Clean( *iExtension,
                                EAiDeviceStatusContentProfileName,
                                0 );  
      }

    iLastPublishedProfileId = ENothingPublished;
    iActiveProfilePublish = EFalse;
    }


void CAiProfilePublisher::RefreshProfileL( TBool aClean )
    {
    // Profile is changed or Refresh is called for some other reason
    //  -> re-publish profile
    iSuccess = EFalse;
    if( !iContentObserver )
        {
        return;
        }

    MProfile* profile = iProfileEngine->ActiveProfileLC();

    const MProfileName& name = profile->ProfileName();
    
    TInt profileNameId = name.Id();
    
    if( ( aClean && iLastPublishedProfileId != profileNameId ) || iActiveProfilePublish ) 
        {
        CleanLastProfileL();
        }

    switch( profileNameId )
        {
        case EProfileGeneralId:
            {
            iContentObserver->Publish( *iExtension,
                                        EAiDeviceStatusContentGeneralProfileName,
                                        name.Name(),
                                        0 );
            break;
            }
            
        case EProfileOffLineId:
            {
            iContentObserver->Publish( *iExtension,
                                        EAiDeviceStatusContentGeneralProfileName,
                                        name.Name(),
                                        0 );

            iPrioritizer->TryToPublishL( *iBroadcaster,
                                          EAiDeviceStatusContentNetworkIdentity,
                                          name.Name(),
                                          EAiOfflineProfile );
            iSuccess = ETrue;
            break;
            }
          
        default:
            {
            iContentObserver->Publish( *iExtension,
                                        EAiDeviceStatusContentProfileName,
                                        name.Name(),
                                        0 );
            break;
            }
        }

    iLastPublishedProfileId = (TLastPublishedProfile)profileNameId;
        
    if ( iProfileEngine->IsActiveProfileTimedL() )
        {
        TBuf<1> timed; // one character
        timed.Append( KAiTimedProfilePUA );
        iContentObserver->Publish( *iExtension,
                                    EAiDeviceStatusContentTimedProfileIndicator,
                                    timed,
                                    0 );
        iLastPublishedProfileTimed = ETrue;
        }

  //If profile is silent, publish silent indicator.
    if( profile->IsSilent() )
        {
        TBuf<1> silent; // one character
        silent.Append( KPuaCodeSilentSymbol );
        iContentObserver->Publish( *iExtension,
                                    EAiDeviceStatusContentSilentIndicator,
                                    silent,
                                    0 );
        iLastPublishedProfileSilent = ETrue;
        }

    CleanupStack::PopAndDestroy();//profile
    }


TBool CAiProfilePublisher::RefreshL( TInt aContentId, TBool aClean )
  {
    switch( aContentId )
        {
        case EAiDeviceStatusContentProfileName:
        case EAiDeviceStatusContentGeneralProfileName:
        case EAiDeviceStatusContentNetworkIdentity:
          {
          RefreshProfileL( aClean );
          return ETrue;
          }
      }
    return EFalse;
  }


TBool CAiProfilePublisher::RefreshContentWithPriorityL( TInt aContentId, 
                                                        TInt aPriority )
  {
  if( aContentId == EAiDeviceStatusContentNetworkIdentity &&
      aPriority == EAiOfflineProfile )
        {
      RefreshProfileL( EFalse );
      if( iSuccess )
          {
          return ETrue;    
          }
        }
    return EFalse;
  }
