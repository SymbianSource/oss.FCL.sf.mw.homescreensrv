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
* Description:  Numeric key forwarding handler implementation for Active Idle 
*                WS Plug-in.
*
*/


#include "numerickeyhandler.h"
#include "uistate.h"

#include <e32property.h>
#include <centralrepository.h>
#include <w32adll.h>

#include <easydialingcrkeys.h>
#include <spsettings.h>
#include <featmgr.h>

#include <PtiEngine.h>
#include <activeidle2domainpskeys.h>
#include <AvkonInternalCRKeys.h>
#include <AknFepInternalCRKeys.h>

#include <aiutility.h>
#include <aipspropertyobserver.h>

namespace AiWsPlugin {


CNumericKeyHandler::CNumericKeyHandler
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt ) :
    iTargetWgId( aTargetWgId ), 
    iWindowExt( aWindowExt )
    {
    }
    
void CNumericKeyHandler::ConstructL()
    {
    // Read capability: ReadUserData.
    _LIT_SECURITY_POLICY_C1( KReadUserPolicy, ECapabilityReadUserData ); 
    // Write capability: WriteDeviceData.
    _LIT_SECURITY_POLICY_C1( KWriteDevicePolicy, ECapabilityWriteDeviceData );

    RProperty::Define( 
         	KPSUidAiInformation,
         	KActiveIdleState,
         	RProperty::EInt,
            KReadUserPolicy,
            KWriteDevicePolicy );
                    
	iQwertyObserver = AiUtility::CreatePSPropertyObserverL(
			TCallBack( HandleQwertyModeChanged, this ),
        	KCRUidAvkon, KAknQwertyInputModeActive );
	
	iInputLanguageRepository = CRepository::NewL( KCRUidAknFep );
	iInputLanguageObserver = CCenRepNotifyHandler::NewL(*this, *iInputLanguageRepository);
	iInputLanguageObserver->StartListeningL(); 
	
	RProperty::Get(KCRUidAvkon, KAknQwertyInputModeActive, iQwertyMode);
	iInputLanguageRepository->Get( KAknFepInputTxtLang, iInputLanguage );
          
    // Load numeric keys mapping for qwerty 
    if ( iQwertyMode )
        {
        LoadInputLanguageKeyBindings( iInputLanguage );            
        }
    }

CNumericKeyHandler* CNumericKeyHandler::NewLC
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt )
    {
    CNumericKeyHandler* self = new(ELeave) CNumericKeyHandler
        ( aTargetWgId, aWindowExt );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CNumericKeyHandler::~CNumericKeyHandler()
    {
    if( iInputLanguageObserver )
        {
        iInputLanguageObserver->StopListening();
        }
    delete iInputLanguageObserver;
    delete iInputLanguageRepository;
    Release(iQwertyObserver);
    iNumericKeys.Close();
    }

void CNumericKeyHandler::SetUiStateQuery( MUiState& aUiState )
    {
    iUiState = &aUiState;
    }

void CNumericKeyHandler::FocusChanged( TBool /*aState*/ )
    {
    }
    
TBool CNumericKeyHandler::OfferRawEvent(const TRawEvent& aRawEvent)
    {
    switch( aRawEvent.Type() )
        {
        case TRawEvent::EKeyDown:
            {
            if ( iUiState->HasFocus() && CheckPostToTarget( aRawEvent ) )
                {
                TInt forwardKeys = EPSAiForwardNumericKeysToPhone;       
                
                TInt err = RProperty::Get(
                        KPSUidAiInformation,
                        KActiveIdleForwardNumericKeysToPhone,
                        forwardKeys);
                // Key event forwarding disabled => Ignore the keys
                if ( err == KErrNone && 
                        forwardKeys == EPSAiDontForwardNumericKeysToPhone )
                    {
                    return EFalse;
                    }
    			// Phone app is listening this key and knows that next
                // focus event from window server is becuase key events
                // are coming to it.                        
                RProperty::Set(
                 	KPSUidAiInformation, 
                    KActiveIdleState,
                	EPSAiNumberEntry );

                // Switch focus to Phone app
                if( iWindowExt )
                    {
                    iWindowExt->SetOrdinalPosition( iTargetWgId, 0, 0 );
                    }
                }
            break;
            }
        }
    
    // Never consume the key event as we want target application to process it
    return EFalse;
    }

/**
 * Returns true if event should be forwarded to Phone app.
 */
TBool CNumericKeyHandler::CheckPostToTarget(const TRawEvent& aRawEvent ) const
    {
    const TInt scanCode = aRawEvent.ScanCode();
    const TUint modifiers = iUiState->Modifiers();

    if ( iQwertyMode ) 
        {
        // Don't pass the check if shift is pressed.		
        if(( modifiers & EModifierShift ) == 0 )
            {
            TInt numericKeysCount = iNumericKeys.Count();
            while( numericKeysCount-- )
                {
                TPtiNumericKeyBinding numKeyBind = iNumericKeys[numericKeysCount];
                if( numKeyBind.iKey == scanCode )
                    {
                    return ETrue;
                    }
                }
            }
        }
    else
        {
        // Normal check ignore modifiers
        static const TInt KIdPlgScanCodes[] = 
            { '1','2','3','4','5','6','7','8','9','0',
              '*',EStdKeyNkpAsterisk,EStdKeyHash,EStdKeyNkpPlus };
        static const TInt KIdPlgScanCodeCount = 
            sizeof( KIdPlgScanCodes ) / sizeof( KIdPlgScanCodes[0] );
        TInt count = KIdPlgScanCodeCount;
        while( count-- )
            {
            const TInt refCode = KIdPlgScanCodes[ count ];
            // Is one of [0,1,2,3,4,5,6,7,8,9,*,#]?
            if( refCode == scanCode )
                {
                return ETrue;
                }
            }
        }
    
    // Homescreen should open dialer also with alpha characters, if dialer is in 
    // mode that accepts alpha characters into number entry (ou1cimx1#299396)    
    
    const TInt  KPhoneKeyStart = 33;
    const TInt  KPhoneKeyEnd   = 127;
    
    return ( ( AllowAlphaNumericMode() ) && ( ( scanCode >= KPhoneKeyStart &&
             scanCode <= KPhoneKeyEnd ) || modifiers & EModifierSpecial ) );
    }


/**
 * Load input locales.
 */
void CNumericKeyHandler::LoadInputLanguageKeyBindings( TInt aLanguage )
    {
    iNumericKeys.Reset();
    
    TRAPD( err, 
        {
        CPtiEngine* ptiEngine = CPtiEngine::NewL();
        CleanupStack::PushL( ptiEngine );
        ptiEngine->GetNumericModeKeysForQwertyL( aLanguage, 
                                                 iNumericKeys );         
        CleanupStack::PopAndDestroy( ptiEngine );                                                 
        } ); // TRAP
        
    if ( err )        
        {
        iNumericKeys.Reset();
        iQwertyMode = 0; // To default mode
        }
    else
        {
        // remove numeric chars that not open number entry
        TInt numericKeysCount = iNumericKeys.Count();
        while ( numericKeysCount-- )
            {
            TPtiNumericKeyBinding numKeyBind = iNumericKeys[numericKeysCount];
            
            if ( numKeyBind.iChar == 'w' ||
                 numKeyBind.iChar == 'p' 
#ifndef RD_INTELLIGENT_TEXT_INPUT
                 /*
                 * The following code is commented because
                 * For GQF requirement Space is mapped to "0"
                 */
                 || numKeyBind.iKey == EPtiKeyQwertySpace  
#endif
               )
                 {
                 iNumericKeys.Remove( numericKeysCount );                    
                 }
            }    
        }
    }
    
/**
 * Handles qwerty mode change.
 */
TInt CNumericKeyHandler::HandleQwertyModeChanged( TAny *aPtr )
    {
    CNumericKeyHandler* self = static_cast<CNumericKeyHandler*>( aPtr );
    if( self )
        {
        TInt value = self->iQwertyMode;
        TInt err = self->iQwertyObserver->Get( value );

        if( err == KErrNone )
            {
            self->SetQwertyMode( value );
            }
        // Load key bindings if not already loaded
#ifdef RD_INTELLIGENT_TEXT_INPUT
        if ( self->iQwertyMode )
            {
            self->iNumericKeys.Reset();
#else 
        if ( self->iQwertyMode && !self->iNumericKeys.Count() )
            {
#endif
        	self->LoadInputLanguageKeyBindings( self->iInputLanguage );
            }
        }
    return KErrNone;
    }

/**
 * Handles input language change.
 */
TInt CNumericKeyHandler::HandleInputLanguageChanged( TInt aNewValue )
	{    	
	SetInputLanguage( aNewValue );
	LoadInputLanguageKeyBindings( aNewValue );
    return KErrNone;
	}
    
/**
 * Set qwerty mode.
 */
void CNumericKeyHandler::SetQwertyMode( TInt aValue )
	{
	iQwertyMode = aValue;
	}
    
/**
 * Set input language.
 */
void CNumericKeyHandler::SetInputLanguage( TInt aValue )
    {
    iInputLanguage = aValue;
    }

/**
 * Check alpha numeric mode.
 */
TBool CNumericKeyHandler::AllowAlphaNumericMode() const
    {
    return ( EasyDialingEnabled() || VoIPSupported() );
    }

/**
 * Check if voip supported.
 */
TBool CNumericKeyHandler::VoIPSupported() const
    {
    TBool voipSupported( EFalse );
    CSPSettings* serviceProviderSettings( NULL );

    TRAP_IGNORE( serviceProviderSettings = CSPSettings::NewL() );   

    if ( serviceProviderSettings )
        {
        voipSupported = serviceProviderSettings->IsFeatureSupported( 
                ESupportInternetCallFeature );
        
        delete serviceProviderSettings;
        }

    return voipSupported;
    }

/**
 * Check if easy dialing enabled.
 */
TBool CNumericKeyHandler::EasyDialingEnabled() const
    {
    TBool easyDialingEnabled( EFalse );
    if ( FeatureManager::FeatureSupported( 
            KFeatureIdProductIncludesHomeScreenEasyDialing ) )
        {
        CRepository* cenrep( NULL );
        TInt easyDialingSetting;

        TRAP_IGNORE( cenrep = CRepository::NewL( KCRUidEasyDialSettings ) );

        if ( cenrep )
            {
            TInt err = cenrep->Get( KEasyDialing, easyDialingSetting );
            if ( !err && easyDialingSetting )
                {
                easyDialingEnabled = ETrue;
                }

            delete cenrep;        
            }
        }

    return easyDialingEnabled;
    }

void CNumericKeyHandler::HandleNotifyGeneric(TUint32 aKey)
    {
    if( aKey == KAknFepInputTxtLang )
        {
        TInt newValue = iInputLanguage;
        iInputLanguageRepository->Get( KAknFepInputTxtLang, newValue );
        HandleInputLanguageChanged( newValue );
        }
    }
    
void CNumericKeyHandler::HandleNotifyError
        (TUint32 /*aKey*/, TInt /*aError*/, CCenRepNotifyHandler* /*aHandler*/)
    {
    }

} // namespace AiWsPlugin
