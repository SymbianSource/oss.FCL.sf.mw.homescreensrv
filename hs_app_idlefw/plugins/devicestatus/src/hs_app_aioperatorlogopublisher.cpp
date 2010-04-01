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
* Description:  Operator logo publisher.
*
*/


#include <RPhCltServer.h>

#include <CPhCltImageHandler.h>
#include <CPhCltBaseImageParams.h>

#include <PhoneClientServerDomainPStypes.h>
#include <fbs.h>
#include <e32cmn.h>
#include <gulicon.h>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <settingsinternalcrkeys.h>
#include <PhoneClientServerDomainPSkeys.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <bitdev.h>
#include <AknStatuspaneUtils.h>
#include <AknUtils.h>
#include <aipspropertyobserver.h>
#include "hs_app_aioperatorlogopublisher.h"
#include "hs_app_ainetworkinfolistener.h"
#include "hs_app_aidevicestatuscontentmodel.h"
#include "hs_app_aiprioritizer.h"
#include "hs_app_ainwidpriorities.h"
#include "debug.h"

// Operator Logo showing state is OFF.
const TInt KAIDisplayOperatorLogoOff = 0;

// Operator Logo showing state is ON.
const TInt KAIDisplayOperatorLogoOn = 1;

const TInt KAddTwoPixels = 2;
const TInt KMaskBlitStartOffset = -1;
const TInt KMaskBlitEndOffset = 2;


// ======== MEMBER FUNCTIONS ========

CAiOperatorLogoPublisher::CAiOperatorLogoPublisher()
: iPriority( EAiInvalidPriority )
    {
   
    }


void CAiOperatorLogoPublisher::ConstructL()
    {
    __PRINTS("CAiOperatorLogoPublisher:: ConstructL >>");
    iListener = CAiNetworkInfoListener::InstanceL();
    
    iOperatorLogoObserver = AiUtility::CreatePSPropertyObserverL( 
    	TCallBack( HandleOperatorLogoUpdateL, this ), 
    	KPSUidPhoneClientServerInformation, KTelOperatorLogoUpdated );
    
    iCenRep = CRepository::NewL( KCRUidPersonalizationSettings );
    
    iCenRepNotify = CCenRepNotifyHandler::NewL( *this, 
    											*iCenRep,
    											CCenRepNotifyHandler::EIntKey, 
    											KSettingsDisplayOperatorLogo );
    											
    iCenRepNotify->StartListeningL();
    __PRINTS("CAiOperatorLogoPublisher:: ConstructL <<");
    }


CAiOperatorLogoPublisher* CAiOperatorLogoPublisher::NewL()
    {
    CAiOperatorLogoPublisher* self = new( ELeave ) CAiOperatorLogoPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiOperatorLogoPublisher::~CAiOperatorLogoPublisher()
    {
    __PRINTS("CAiOperatorLogoPublisher:: ~CAiOperatorLogoPublisher >>");
    if( iCenRepNotify )
        {
        iCenRepNotify->StopListening();
        }
    delete iCenRepNotify;
    delete iCenRep;
    if( iListener )
        {
	    iListener->RemoveObserver( *this );
        iListener->Release();
        }
    delete iIcon;
    Release( iOperatorLogoObserver );
    __PRINTS("CAiOperatorLogoPublisher:: ~CAiOperatorLogoPublisher <<");
    }


void CAiOperatorLogoPublisher::ResumeL()
    {
    __PRINTS("CAiOperatorLogoPublisher:: ResumeL >>");
    iListener->AddObserverL( *this );
    __PRINTS("CAiOperatorLogoPublisher:: ResumeL <<");
    }


void CAiOperatorLogoPublisher::HandleNetworkInfoChange(
							const MNWMessageObserver::TNWMessages& /*aMessage*/,
							const TNWInfo& /*aInfo*/,
							const TBool aShowOpInd )
    {
    __PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: HandleNetworkInfoChange >> aShowOpInd %d "), aShowOpInd);
        if( aShowOpInd )
        	{
        	TRAP_IGNORE(UpdateOperatorLogoL( ETrue ));
        	}
        else
        	{
        	TRAP_IGNORE (iPrioritizer->TryToCleanL( *iBroadcaster,
            							EAiDeviceStatusContentNetworkIdentity,
            							iPriority ));
        	}
        
    __PRINTS("CAiOperatorLogoPublisher:: HandleNetworkInfoChange <<");
    }


void CAiOperatorLogoPublisher::HandleNotifyInt( TUint32 aId, TInt /*aNewValue*/ )
	{
	__PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: HandleNotifyInt >> aId %d "), aId);
    if( aId == KSettingsDisplayOperatorLogo )
        {
        TRAP_IGNORE( UpdateOperatorLogoL( ETrue ) );
        }
    __PRINTS("CAiOperatorLogoPublisher:: HandleNotifyInt <<");
	}
	
	
void CAiOperatorLogoPublisher::Subscribe( MAiContentObserver& aObserver, 
									        MAiPropertyExtension& aExtension,
                                            MAiPublishPrioritizer& aPrioritizer,
                                            MAiPublisherBroadcaster& aBroadcaster )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    iPrioritizer = &aPrioritizer;
    iBroadcaster = &aBroadcaster;
    }


void CAiOperatorLogoPublisher::RefreshL( TBool aClean )
    {
    __PRINTS("CAiOperatorLogoPublisher:: RefresL >> ");
    TRAP_IGNORE( UpdateOperatorLogoL( aClean ) );
    __PRINTS("CAiOperatorLogoPublisher:: RefresL << ");
    }


CFbsBitmap* CAiOperatorLogoPublisher::LoadLogoL( TInt aMCC, 
                                                 TInt aMNC )
    {
    __PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: LoadLogo >> aMCC %d, aMNC %d"), aMCC, aMNC);
    CPhCltImageHandler* imageHandler = CPhCltImageHandler::NewL();       
    User::LeaveIfNull( imageHandler );
    
    CleanupStack::PushL( imageHandler );
    
    // Create image parameter class
    CPhCltImageParams* params = 
    	imageHandler->CPhCltBaseImageParamsL( EPhCltTypeOperatorLogo );
    CleanupStack::PushL( params );
    
    TInt err = KErrNotFound;
    iPriority = EAiOTAOperatorLogo;
    
    CPhCltExtOperatorLogoParams* opLogoParams = 
    	static_cast<CPhCltExtOperatorLogoParams*>(params);
	
	if( opLogoParams )
        {
		opLogoParams->SetCodesL( aMCC, aMNC, EPhCltLogoTypeOTA );
		err = imageHandler->LoadImages( params );
		
		if( err == KErrNotFound )
       		{
            // OTA logo wasn't found, try programmable logo
            opLogoParams->SetCodesL( aMCC, aMNC, EPhCltLogoTypeProgrammable );
            err = imageHandler->LoadImages( params );
            iPriority = EAiProgOperatorLogo;
            }        
        }
    else
        {
        err = KErrNotFound;
        }
    __PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: LoadLogo >> err %d"), err);
	User::LeaveIfError( err );
	 
	//take always first image, we can show only one logo
    TInt bitmapHandle = params->GetImageL( 0 );
    
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Duplicate( bitmapHandle ) );

    CleanupStack::Pop( bitmap );
    
    CleanupStack::PopAndDestroy( params );
    CleanupStack::PopAndDestroy( imageHandler );
    __PRINTS("CAiOperatorLogoPublisher:: LoadLogo << ");
    return bitmap;
    }


void CAiOperatorLogoPublisher::UpdateOperatorLogoL( TBool aClean )
    {
    __PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: UpdateOperatorLogoL >> aClean %d"), aClean);
    iSuccess = EFalse;
    if( aClean )
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							iPriority );
        }
        
    if( !AllowToShowLogoL() )
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							iPriority );

        delete iIcon;
        iIcon = NULL;
        __PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL : not allowed to show logo");
        return;
        }

    const TNWInfo& info = iListener->NetworkInfo();

    if ( info.iRegistrationStatus == ENWNotRegisteredNoService ||
    		info.iRegistrationStatus ==ENWRegistrationUnknown )
    	{
    	__PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL : registrationstatus unknown");
    	return;
    	}

    TInt mcc;

    TLex lex( info.iCountryCode );
    User::LeaveIfError( lex.Val( mcc ) );

    TInt mnc;
    lex.Assign( info.iNetworkId );
    User::LeaveIfError( lex.Val( mnc ) );

    CFbsBitmap* opLogo = NULL;
	CFbsBitmap* opLogoMask = NULL;

    TRAPD( err, opLogo = LoadLogoL( mcc,
    								mnc ) );

	if( err )
        {
        opLogo = NULL;
        }

	if ( opLogo && opLogo->IsMonochrome() )
        {
        __PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL : monochrome icon");
        CFbsBitmap* bitmap = opLogo;
        opLogo = NULL;

        CleanupStack::PushL( bitmap );
        CreateMonochromeBitmapsL( *bitmap, opLogo, opLogoMask );
        
        CleanupStack::PopAndDestroy( bitmap );
        }

    //clean always, might be that there is old logo published
    //and now the logo isn't valid anymore. So publish clean
    //even if we didn't get new logo
    if( opLogo )
        {
        delete iIcon;
        iIcon = NULL;

        if( opLogoMask )
        	{
        	iIcon = CGulIcon::NewL( opLogo, opLogoMask );
        	}
        else
        	{
        	iIcon = CGulIcon::NewL( opLogo, NULL );
        	}

        opLogo = NULL;
        opLogoMask = NULL;

        TPckg<CGulIcon*> pckg( iIcon );

        iPrioritizer->TryToPublishL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							pckg,
        							iPriority );
        __PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL : logo published ");
        iSuccess = ETrue;
        }
    else
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							iPriority );
        __PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL : logo cleaned ");
        }
    __PRINTS("CAiOperatorLogoPublisher:: UpdateOperatorLogoL << ");
    }


TBool CAiOperatorLogoPublisher::AllowToShowLogoL() const
    {
    TInt val;
    iCenRep->Get( KSettingsDisplayOperatorLogo, val );
    return val && iListener->IsOperatorIndicatorAllowed();
    }


void CAiOperatorLogoPublisher::CreateMonochromeBitmapsL(
    const CFbsBitmap& aBitmap,
    CFbsBitmap*& aNewLogoMain,
    CFbsBitmap*& aNewLogoMask )
    {
    
    __PRINTS("CAiOperatorLogoPublisher:: CreateMonochromeBitmapsL >> ");
    TSize size = aBitmap.SizeInPixels();
    const TRgb white = TRgb::Gray2( 1 );

    // New size 2 pixels larger in every dimension.
    TSize newSize( size.iWidth + KAddTwoPixels, size.iHeight + KAddTwoPixels );
    TPoint delta( 1, 1 ); // adjust image position.

    TSize opLogoSize;
    TInt err = GetTitlePaneSize( opLogoSize );
    if ( err != KErrNone )
        {
        return;
        }

    if ( newSize.iWidth > opLogoSize.iWidth &&
        size.iWidth <= opLogoSize.iWidth )
        {
        newSize.iWidth = size.iWidth;
        delta.iX = 0;
        }
    if ( newSize.iHeight > opLogoSize.iHeight &&
        size.iHeight <= opLogoSize.iHeight )
        {
        newSize.iHeight = size.iHeight;
        delta.iY = 0;
        }

    // Create mask image
    CFbsBitmap* bitmapLargeMain =
        new (ELeave) CFbsBitmap;
    CleanupStack::PushL( bitmapLargeMain );
    User::LeaveIfError(
        bitmapLargeMain->Create( newSize, EGray2 ) );

    CFbsBitmapDevice* device =
        CFbsBitmapDevice::NewL( bitmapLargeMain );
    CleanupStack::PushL( device );

    CFbsBitGc* gc = NULL;
    User::LeaveIfError( device->CreateContext( gc ) );

    gc->SetPenStyle(CGraphicsContext::ENullPen);
    gc->SetBrushColor( white );
    gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->Clear();

    gc->BitBlt( TPoint() + delta, &aBitmap );

    delete gc;
    gc = NULL;

    CleanupStack::PopAndDestroy( device );

    // Create main image
    CFbsBitmap* bitmapLargeMask =
        new (ELeave) CFbsBitmap;
    CleanupStack::PushL( bitmapLargeMask );
    User::LeaveIfError(
        bitmapLargeMask->Create( newSize, EGray2 ) );

    CFbsBitmapDevice* deviceMask =
        CFbsBitmapDevice::NewL( bitmapLargeMask );
    CleanupStack::PushL( deviceMask );

    User::LeaveIfError( deviceMask->CreateContext( gc ) );

    gc->SetPenStyle(CGraphicsContext::ENullPen);
    gc->SetBrushColor( white );
    gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc->Clear();
    gc->SetBrushStyle(CGraphicsContext::ENullBrush);

    TPoint point;
    TRect rect( point, size );

    // Blit with mask 3 points from top left to down right.
    for ( TInt j = KMaskBlitStartOffset;
          j < KMaskBlitEndOffset;
          j++ )
        {
        for ( TInt i = KMaskBlitStartOffset;
              i < KMaskBlitEndOffset;
              i++ )
            {
            point = TPoint( i, j );
            point += delta;
            gc->BitBltMasked(
                point, &aBitmap, rect, &aBitmap, ETrue );
            }
        }

    delete gc;
    gc = NULL;

    CleanupStack::PopAndDestroy( deviceMask );

    CleanupStack::Pop( bitmapLargeMask );
    CleanupStack::Pop( bitmapLargeMain );

    aNewLogoMain = bitmapLargeMain;
    aNewLogoMask = bitmapLargeMask;
    __PRINTS("CAiOperatorLogoPublisher:: CreateMonochromeBitmapsL << ");
    }


TInt CAiOperatorLogoPublisher::GetTitlePaneSize( TSize& aSize )
    {
   	TRect titlePaneRect;
    TBool hasTitlePane = AknLayoutUtils::LayoutMetricsRect( 
        AknLayoutUtils::ETitlePane, titlePaneRect );
    
    if ( !hasTitlePane )
        {
        return KErrNotFound;            
        }
    
    TAknLayoutRect oplogoLayout;
    
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        // stacon
        oplogoLayout.LayoutRect( titlePaneRect, 
            AknLayoutScalable_Avkon::title_pane_stacon_g2(0).LayoutLine() );    
        }
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        // flat
        oplogoLayout.LayoutRect( titlePaneRect, 
            AknLayoutScalable_Avkon::title_pane_stacon_g2(0).LayoutLine() );    
        }   
    else
        {
        // usual idle
        oplogoLayout.LayoutRect( titlePaneRect, 
            AknLayout::Title_pane_elements_Line_1() );
        }             
     
    aSize = oplogoLayout.Rect().Size(); 
    
    
    return KErrNone;  
    }


TBool CAiOperatorLogoPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
	
    if(aContentId == EAiDeviceStatusContentNetworkIdentity )
        {
        __PRINTS("CAiOperatorLogoPublisher:: RefreshL >> ");
   	    RefreshL( aClean );
   	    if( iSuccess )
   	        {
   	        __PRINTS("CAiOperatorLogoPublisher:: RefreshL << success ");
   	        return ETrue;
   	        }   
   	    __PRINTS("CAiOperatorLogoPublisher:: RefreshL << failed ");	    
    	}    	
    return EFalse;
	}


TInt CAiOperatorLogoPublisher::HandleOperatorLogoUpdateL( TAny *aPtr )
	{
	__PRINTS("CAiOperatorLogoPublisher::  HandleOperatorLogoUpdateL >> ");
	CAiOperatorLogoPublisher* self = static_cast<CAiOperatorLogoPublisher*>( aPtr );
    
    TTelOTALogoUpdate logoUpd;
    TTelOTALogoUpdate::TOTALogoUpdatePckg logoUpdPckg( logoUpd );

    if ( RProperty::Get( 
    		KPSUidPhoneClientServerInformation, 
        	KTelOperatorLogoUpdated, 
        	logoUpdPckg ) == KErrNone )
    	{
        __PRINT(__DBG_FORMAT("CAiOperatorLogoPublisher:: HandleOperatorLogoUpdateL >> logoUpd.iEvent %d"), logoUpd.iEvent);
        if ( logoUpd.iEvent == TTelOTALogoUpdate::EPSTelephonyLogoChanged )
        	{
        	if(self)
        		{
        		__PRINTS("CAiOperatorLogoPublisher::  HandleOperatorLogoUpdateL: logo changed ");
        		self->iCenRep->Set( KSettingsDisplayOperatorLogo, KAIDisplayOperatorLogoOn );
        		self->UpdateOperatorLogoL( ETrue );        			
        		}
        	}
        else if ( logoUpd.iEvent == TTelOTALogoUpdate::EPSTelephonyLogoDeleted ||
                  logoUpd.iEvent == TTelOTALogoUpdate::EPSTelephonyAllLogosDeleted )
            {
            if(self)
                {  
                __PRINTS("CAiOperatorLogoPublisher::  HandleOperatorLogoUpdateL: logo deleted ");
        		self->iCenRep->Set( KSettingsDisplayOperatorLogo, KAIDisplayOperatorLogoOff );
        		self->UpdateOperatorLogoL( ETrue );        			            
                }
            }
  
       	}
    __PRINTS("CAiOperatorLogoPublisher::  HandleOperatorLogoUpdateL << ");
    return KErrNone;
	}


TBool CAiOperatorLogoPublisher::RefreshContentWithPriorityL( TInt aContentId, 
                                                                TInt aPriority )
	{
	if( aContentId == EAiDeviceStatusContentNetworkIdentity &&
	    ( aPriority == EAiOTAOperatorLogo || 
	    aPriority == EAiProgOperatorLogo ) )
        {
        __PRINTS("CAiOperatorLogoPublisher::  RefreshContentWithPriorityL >> ");
	    RefreshL( EFalse );
	    if( iSuccess )
	        {
	        __PRINTS("CAiOperatorLogoPublisher::  RefreshContentWithPriorityL << success");
	        return ETrue;	        
	        }
	    __PRINTS("CAiOperatorLogoPublisher::  RefreshContentWithPriorityL << failed");
        }
    return EFalse;
	}
	
