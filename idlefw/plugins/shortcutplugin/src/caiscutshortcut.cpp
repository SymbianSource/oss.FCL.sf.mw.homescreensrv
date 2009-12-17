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
* Description:  Implementation for a shortcut.
*
*/


#include <aicontentobserver.h>
#include <gulicon.h>            // For CGulIcon
#include <fbs.h>                // For CFbsBitmap
#include <e32property.h>            // For RProperty

#include <activeidle2domainpskeys.h>
#include "aiscutcontentmodel.h"
#include "caiscutshortcut.h"
#include "aiscutdefs.h"
#include "caiscuttargetapp.h"
#include "caiscuttargetbkm.h"
#include "caiscuttargethttp.h"
#include "caiscuttargetmessagingview.h"
#include "caiscuttargetnewmsg.h"
#include "caiscuttargetkeylock.h"
#include "caiscuttargetempty.h"
#include "aiscutpluginprivatecrkeys.h"

#include "debug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut::CAiScutShortcut(TInt aId, CAiScutEngine& aEngine)
	: CTimer( CActive::EPriorityLow )
	, iId(aId)
    , iEngine(aEngine)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcut::ConstructL(const TDesC& aTarget)
{
    iDefaultTarget = CreateTargetL(aTarget, EFalse);

    if (!iDefaultTarget)
    {
        iDefaultTarget = CAiScutTargetEmpty::NewL(iEngine, EScutUnknown, aTarget);
    }
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut* CAiScutShortcut::NewLC(TInt aId, const TDesC& aTarget,
    CAiScutEngine& aEngine)
{
    CAiScutShortcut* self = new (ELeave) CAiScutShortcut(aId, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aTarget);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut* CAiScutShortcut::NewL(TInt aId, const TDesC& aTarget,
    CAiScutEngine& aEngine)
{
    CAiScutShortcut* self = CAiScutShortcut::NewLC(aId, aTarget, aEngine);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut::~CAiScutShortcut()
{
	Cancel();
    delete iDefaultTarget;
    delete iUserTarget;
    delete iRetiredTarget;
}

// -----------------------------------------------------------------------------
// Returns the shortcut id.
// -----------------------------------------------------------------------------
//
TInt32 CAiScutShortcut::Id() const
{
    return iId;
}

// -----------------------------------------------------------------------------
// Publishes the shortcut content, non leaving version.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::Publish(
    MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver)
{
    TRAPD(err, PublishL(aPlugin, aObserver));
    //Possible forcing done already so reset the flag
    iForcePublish = EFalse;
    if (err == KErrNone)
    {
        delete iRetiredTarget;
        iRetiredTarget = NULL;
        iLastPublishedTarget = iActiveTarget;
    }
    else
    {
        // Publish failed, roll back to previous content.
        TInt transactionId = reinterpret_cast<TInt>(this);
        aObserver.CancelTransaction(transactionId);

        // Delete the new target and put the retired one back to work.
        if (iRetiredTarget)
        {
            delete iUserTarget;
            iUserTarget = iRetiredTarget;
            iRetiredTarget = NULL;
        }

        iActiveTarget = iLastPublishedTarget;
    }

    
}

// -----------------------------------------------------------------------------
// Checks if the application or messaging view pointed to can be launched.
// -----------------------------------------------------------------------------
//
TBool CAiScutShortcut::CheckAccessL(TInt aCheckType)
{
    TBool userTargetAccessible = EFalse;

	__PRINT( __DBG_FORMAT( "XAI: CAiScutShortcut::CheckAccessL( %d )"), aCheckType);

    // First try the user setting if it is defined.
    if (iUserTarget && iUserTarget->IsAccessibleL(aCheckType))
    {
        iActiveTarget = iUserTarget;
        userTargetAccessible = ETrue;
    }

    if (!userTargetAccessible)
    {

		__PRINTS("XAI:   *** user target NOT accessible ***");
        // User setting was not accessible or not defined, try the default.
        if (iDefaultTarget->IsAccessibleL(aCheckType))
        {
            iActiveTarget = iDefaultTarget;
        }
        else
        {
            // The default is not accessible either, the shortcut is empty.
            iActiveTarget = NULL;

			__PRINTS( "XAI:   *** default target NOT accessible ***");
        }
    }

    if (iActiveTarget)
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CAiScutShortcut::IsTargetChanged() const
{
    // We need to publish if target changed during access checking.
    return (iLastPublishedTarget != iActiveTarget);
}

// -----------------------------------------------------------------------------
// Changes the shortcut target to the user defined setting.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::SetUserTarget(const TDesC& aNewTarget)
{

	TPtrC defaultTarget(iDefaultTarget->Definition());
	__PRINTS( "XAI: CAiScutShortcut::SetUserTarget");
	__PRINT( __DBG_FORMAT( "XAI:   id             = 0x%x"), iId);
	__PRINT( __DBG_FORMAT( "XAI:   default target = '%S'"),   &defaultTarget);
	__PRINT( __DBG_FORMAT("XAI:   new target     = '%S'"),   &aNewTarget);
	if (iUserTarget)
		{
		TPtrC userTarget(iUserTarget->Definition());
		__PRINT( __DBG_FORMAT("XAI:   user target    = '%S'"),   &userTarget);
		}
		
	
    if (iUserTarget && (aNewTarget.CompareC(iUserTarget->Definition()) == 0))
    {
        return;
    }


    // Creating the new target might leave, so it is done before the old target
    // is deleted to ensure that the shortcut object remains in a consistent state
    // in case of a leave. If target creation leaves, nothing has changed.
    //
    // As a side effect this means that the new target is allocated to a different
    // memory address than the old one, the address of the old target will not be re-used
    // by this target. That doesn't mean anything for the plug-in, but it helps the
    // plug-in tester to notice premature target deletion, since the address will still
    // contain 0xDEDEDEDE instead of the new target.
    CAiScutTarget* tempTarget = NULL;

    TRAPD(err, tempTarget = CreateTargetL(aNewTarget, ETrue));

    if (err == KErrNone)
    {
        DeleteUserTarget();
        iUserTarget = tempTarget;
    }
}

// -----------------------------------------------------------------------------
// Deletes the user target.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::DeleteUserTarget()
{
    if (iUserTarget)
    {

		__PRINTS("XAI: CAiScutShortcut::DeleteUserTarget");

        if (iLastPublishedTarget == iUserTarget && !(iId & KScutFlagBitNonVisible))
        {
            // Previous user target was the last published target so the Ai framework
            // is still using the icon pointer. Deleting the target now would cause a
            // KERN-EXEC 3 panic when the framework tries to access the deleted icon.
            // The target must be kept alive until the new target has been successfully
            // published and the framework is no longer using its icon.
            // This is unnecessary for non-visible shortcuts because they are not published.
            iRetiredTarget = iUserTarget;
            iUserTarget = NULL;

			__PRINTS("XAI:   iUserTarget = NULL");

        }

        if (iActiveTarget == iUserTarget)
        {
            // Previous user target was the active target. We don't know if the new
            // user target is accessible, so the shortcut is effectively empty until
            // the access check has been run.
            iActiveTarget = NULL;
        }

        delete iUserTarget;
        iUserTarget = NULL;

		__PRINTS( "XAI:   delete iUserTarget");
    }
}

// -----------------------------------------------------------------------------
// Launches the shortcut.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::LaunchL()
{
	Cancel();
	RProperty::Set( 
        KPSUidAiInformation, 
        KActiveIdleLaunch, 
        EPSAiLaunchIsActive );

    if (iActiveTarget)
    {
    	TRAP_IGNORE( iActiveTarget->BeginEffectL() ); //start a full screen effect
        iActiveTarget->LaunchL();
    }
  
    // When preparing for backup, the plugin is suspended and calling After() would
    // cause a crash
    if ( IsAdded() )
    {	
    	After(1000000);
    }
}

// -----------------------------------------------------------------------------
// Launches the shortcut.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::LaunchL(const TDesC8& aMessage)
{
	Cancel();
	RProperty::Set( 
        KPSUidAiInformation, 
        KActiveIdleLaunch, 
        EPSAiLaunchIsActive );

    if (iActiveTarget)
    {
    	TRAP_IGNORE( iActiveTarget->BeginEffectL() ); //start a full screen effect
        iActiveTarget->LaunchL(aMessage);
    }
  
    // When preparing for backup, the plugin is suspended and calling After() would
    // cause a crash
    if ( IsAdded() )
    {	
    	After(1000000);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::SetToBePublished(TBool aFlag)
{
    if ( !iForcePublish )
        {
        iNeedsToBePublished = aFlag;                
        }
    else
        {
        iNeedsToBePublished = iForcePublish;
        }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CAiScutShortcut::NeedsToBePublished()
{
    return iNeedsToBePublished;
}

// -----------------------------------------------------------------------------
// Return shortcut type.
// -----------------------------------------------------------------------------
//
TShortcutType CAiScutShortcut::Type() const
{
    if (iUserTarget)
    {
        return iUserTarget->Type();
    }
    else
    {
        return iDefaultTarget->Type();
    }
}

// ---------------------------------------------------------------------------
// Return application uid of this shortcut.
// ---------------------------------------------------------------------------
//
TUid CAiScutShortcut::AppUid() const
{
    if (iUserTarget)
    {
        return iUserTarget->AppUid();
    }
    else
    {
        return iDefaultTarget->AppUid();
    }
}
void CAiScutShortcut::SetIcon(TAiScutIcon aIcon)
    {
    switch(aIcon.iDestination)
        {      
        case EScutDestinationSoftkey:
            SetSoftkeyIcon(aIcon);
            break;
        case EScutDestinationToolbar:
            SetToolbarIcon(aIcon);
            break;
        case EScutDestinationNormal:
            SetOverrideIcon(aIcon);
            break;
        default:
            break;
        }
    }
void CAiScutShortcut::SetOverrideIcon(TAiScutIcon aIcon)
{   
    if (iUserTarget)
    {
        iUserTarget->SetOverrideIcon(aIcon);
    }
    else
    {
        iDefaultTarget->SetOverrideIcon(aIcon);
    }
}

void CAiScutShortcut::SetSoftkeyIcon(TAiScutIcon aIcon)
{   
    if (iUserTarget)
    {
        iUserTarget->SetSoftkeyIcon(aIcon);
    }
    else
    {
        iDefaultTarget->SetSoftkeyIcon(aIcon);
    }
}


void CAiScutShortcut::SetToolbarIcon(TAiScutIcon aIcon)
{   
    if (iUserTarget)
    {
        iUserTarget->SetToolbarIcon(aIcon);
    }
    else
    {
        iDefaultTarget->SetToolbarIcon(aIcon);
    }
}


// -----------------------------------------------------------------------------
// Creates a shortcut target object.
// -----------------------------------------------------------------------------
//
CAiScutTarget* CAiScutShortcut::CreateTargetL(
    const TDesC& aDefinition, TBool aCreateUserTarget)
{

	__PRINTS("XAI: CAiScutShortcut::CreateTargetL");
	__PRINT( __DBG_FORMAT("XAI:   id = 0x%x, target = '%S'"), iId, &aDefinition);

    CAiScutTarget* target = NULL;
    TAiScutParser parser;
    TInt err = parser.Parse(aDefinition);

    if (parser.IsValid())
    {
        TShortcutType type = parser.Type();
        
        switch (type)
        {
        case EScutApplication:
        case EScutChangeTheme:
        case EScutApplicationView:
        case EScutApplicationWithParams:
        case EScutLogsMissedCallsView:
        case EScutLogsDialledCallsView:
        case EScutLogsReceivedCallsView:
        case EScutLogsMainView:
            target = CAiScutTargetApp::NewL(iEngine, type, parser);
            break;

        case EScutNewMsgType:
        case EScutNewMessage:
        case EScutNewEmail:
#ifdef __SYNCML_DS_EMAIL
        case EScutNewSyncMLMail:
#endif
        case EScutNewPostcard:
        case EScutNewAudioMsg:
            target = CAiScutTargetNewMsg::NewL(iEngine, type, parser);
            break;

        case EScutMailbox:
            target = CAiScutTargetMessagingView::NewL(iEngine, type, parser);
            break;

        case EScutWebAddress:
            target = CAiScutTargetHttp::NewL(iEngine, type, parser);
            break;

        case EScutKeylock:
            target = CAiScutTargetKeyLock::NewL(iEngine, type, aDefinition);
            break;

        case EScutNoEffect:
            target = CAiScutTargetEmpty::NewL(iEngine, type, aDefinition);
            break;

        case EScutBookmark:
            target = CAiScutTargetBkm::NewL(iEngine, type, parser);
            break;

        case EScutConnectivityStatusView:
            target = CAiScutTargetApp::NewL(iEngine, EScutApplicationView, parser);
            break;

        case EScutApplicationManagerView:
            target = CAiScutTargetApp::NewL(iEngine, EScutApplicationView, parser);
            break;

        default:
            break;
        }
    }

    if (!target && aCreateUserTarget)
    {
        // treat unknown user targets as web addresses.
        target = CAiScutTargetHttp::NewL(iEngine, EScutWebAddress, parser);
    }

    if (!target)
    {
		__PRINTS( "XAI:   *** empty target ***");
    }
    
    
    return target;
}

// -----------------------------------------------------------------------------
// Publishes the shortcut content, leaving version.
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::PublishL(
    MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver)
    {
    TInt err = KErrNone;
    TBool cbaIconPublished = EFalse;

	__PRINT( __DBG_FORMAT("XAI: CAiScutShortcut::PublishL 0x%x"), iId);

    // Publish caption if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutCaption].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishCaption(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutCaption].id);

        if (err != KErrNone)
        	__PRINT( __DBG_FORMAT( "XAI:   publish caption err = %d"), err);
        }

    // Publish short caption if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutShortCaption].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishCaption(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutShortCaption].id);

        if (err != KErrNone)
	        __PRINT( __DBG_FORMAT( "XAI:   publish short caption err = %d"), err);
        }

    /** softkeys **/
      // Publish short caption if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutSkIcon].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishIcon(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutSkIcon].id);
        
        if ( err == KErrNone )
            {
            cbaIconPublished = ETrue;
            }
        if (err != KErrNone) 
        	__PRINT( __DBG_FORMAT("XAI:   publish icon err = %d"), err);
        }
    
    // No CBA icon published so publish the caption
    if ( !cbaIconPublished )
        {        
        // Publish sk caption if the framework can handle it.
        if (!aObserver.CanPublish(
            aPlugin, KAiScutContent[EAiScutContentShortcutSkCaption].id, iId))
            {
            err = KErrNotSupported;
            }
        else
            {
            err = PublishCaption(
                aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutSkCaption].id);

            if (err != KErrNone)
    	        __PRINT( __DBG_FORMAT( "XAI:   publish short caption err = %d"), err);
            }
        }
    
    /** end of softkeys **/
    
    // Publish MSK caption if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutMskCaption].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishCaption(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutMskCaption].id);

        if (err != KErrNone)
        	__PRINT( __DBG_FORMAT( "XAI:   publish msk caption err = %d"), err);
        }

    // Publish icon if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutIcon].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishIcon(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutIcon].id);

        if (err != KErrNone) 
        	__PRINT( __DBG_FORMAT("XAI:   publish icon err = %d"), err);
        }

    // Publish toolbar caption if the framework can handle it.
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutToolbarCaption].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishCaption(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutToolbarCaption].id);

        if (err != KErrNone)
        	__PRINT( __DBG_FORMAT( "XAI:   publish toolbar caption err = %d"), err);
        }

    // Publish the toolbar icon if the framework can handle it    
    if (!aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentShortcutToolbarIcon].id, iId))
        {
        err = KErrNotSupported;
        }
    else
        {
        err = PublishIcon(
            aPlugin, aObserver, KAiScutContent[EAiScutContentShortcutToolbarIcon].id);
        
        if (err != KErrNone) 
        	__PRINT( __DBG_FORMAT("XAI:   publish toolbar icon err = %d"), err);
        }
    }

// -----------------------------------------------------------------------------
// Publishes shortcut caption.
// -----------------------------------------------------------------------------
//
TInt CAiScutShortcut::PublishCaption(MAiPropertyExtension& aPlugin,
    MAiContentObserver& aObserver, TInt aCaptionContentId) const
{
    TInt err = KErrNone;

    if (iId == KRightSoftkeyId)
    {
        if (iActiveCall)
        {
            TInt backCaptionResId = KAiScutResources[EAiScutResourceBackCaption].id;
            err = aObserver.Publish(aPlugin, aCaptionContentId, backCaptionResId, iId);
            return err;
        }
    }

    if (!iActiveTarget)
    {
        // Publish the EmptyCaption resource id.
        TInt emptyCaptionResId = KAiScutResources[EAiScutResourceEmptyCaption].id;
        err = aObserver.Publish(aPlugin, aCaptionContentId, emptyCaptionResId, iId);
    }
    else
    {
        TPtrC captionDes;
        TInt captionResId = 0;
        TAiScutAppTitleType titleType = EAiScutLongTitle;
        if (aCaptionContentId == KAiScutContent[EAiScutContentShortcutShortCaption].id ||
            aCaptionContentId == KAiScutContent[EAiScutContentShortcutSkCaption].id || 
            aCaptionContentId == KAiScutContent[EAiScutContentShortcutToolbarCaption].id)
        {
            titleType = EAiScutSkeyTitle;
        }
        else if (aCaptionContentId == KAiScutContent[EAiScutContentShortcutMskCaption].id)
        {
            titleType = EAiScutMskTitle;
        }

        captionResId = iActiveTarget->GetCaption(captionDes, titleType);

        if (captionResId == 0)
        {
            // Publish descriptor.
            err = aObserver.Publish(aPlugin, aCaptionContentId, captionDes, iId);
        }
        else if (captionResId > 0)
        {
            // Publish resource.
            err = aObserver.Publish(aPlugin, aCaptionContentId, captionResId, iId);
            
            __PRINT( __DBG_FORMAT("XAI:   publish resource = %d"), captionResId);
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
// Publishes shortcut icon.
// -----------------------------------------------------------------------------
//
TInt CAiScutShortcut::PublishIcon(MAiPropertyExtension& aPlugin,
    MAiContentObserver& aObserver, TInt aIconContentId )
{
    TInt emptyIconResId = KAiScutResources[EAiScutResourceEmptyIcon].id;
    TInt err = KErrNone;
    // In case of an active call cancel the SK icon publication
    // to RSK
    if ( iActiveCall && iId == KRightSoftkeyId )
        {
        return KErrCancel;
        }
    if (!iActiveTarget)
    {
        // Publish the EmptyIcon resource id.

        __PRINT( __DBG_FORMAT( "XAI: PublishIcon publish empty #1 = %d"), emptyIconResId);

        err = aObserver.Publish(aPlugin, aIconContentId, emptyIconResId, iId);
    }
    else
    {
        CGulIcon* iconPtr = NULL;
        TInt iconResId = 0;
        //  Normal icon
        if (aIconContentId == KAiScutContent[EAiScutContentShortcutIcon].id)
            {
            iconResId = iActiveTarget->GetIcon(iconPtr);
            }
        // Soft key icon
        else if ( aIconContentId == KAiScutContent[EAiScutContentShortcutSkIcon].id )
            {
            iconResId = iActiveTarget->GetSoftkeyIcon(iconPtr);
            }
        else if ( aIconContentId == KAiScutContent[EAiScutContentShortcutToolbarIcon].id )
            {
            iconResId = iActiveTarget->GetToolbarIcon(iconPtr);            
            }

        // No error, continue with the publish
        if (iconResId == 0)
        {
            if (iconPtr)
            {
                // Publish pointer.
                err = aObserver.PublishPtr(aPlugin, aIconContentId, iconPtr, iId);
                    
                if( err != KErrNone )
                    {
                    delete iconPtr;
                    }
            }
            else
            {
                // The target hasn't been able to initialize its icon, publish the empty icon.

                __PRINT( __DBG_FORMAT("XAI: PublishIcon publish empty #2 = %d"), emptyIconResId);

                err = aObserver.Publish(aPlugin, aIconContentId, emptyIconResId, iId);
            }
        }
        // Publish by resource
        else if (iconResId > 0)
        {
            // Publish resource.

            __PRINT( __DBG_FORMAT("XAI: PublishIcon publish resource = %d"), iconResId);

            err = aObserver.Publish(aPlugin, aIconContentId, iconResId, iId);
        }
        // < 0 error occurred, return it
        else
            {
            err = iconResId;
            }
    }

    return err;
}

// -----------------------------------------------------------------------------
// Set call state
// -----------------------------------------------------------------------------
//
void CAiScutShortcut::SetCallState(TBool aStatus)
{
    // Call state changed force the publish of RSK    
    if ( iActiveCall != aStatus )
        {
        if ( iId == KRightSoftkeyId )
            {
            iForcePublish = ETrue;
            }
        }
    iActiveCall = aStatus;
}

TPtrC CAiScutShortcut::ActiveDefinition()
    {
    if( iUserTarget )
        {
        return iUserTarget->Definition();
        }
    if( iDefaultTarget )
        {
        return iDefaultTarget->Definition();
        }
    return TPtrC();
    }
    
// ---------------------------------------------------------------------------
// Return the possible additional id
// ---------------------------------------------------------------------------
//    
TUid CAiScutShortcut::AdditionalUid() const
    {
    if( iUserTarget )
        {
        return iUserTarget->AdditionalUid();
        }
    if( iDefaultTarget )
        {
        return iDefaultTarget->AdditionalUid();
        }
    return TUid::Uid(-1);
    }

// ---------------------------------------------------------------------------
// CActive
// ---------------------------------------------------------------------------
//   
TInt CAiScutShortcut::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CActive
// ---------------------------------------------------------------------------
//   
void CAiScutShortcut::DoCancel()
    {
    CTimer::DoCancel();
    RProperty::Set( 
        KPSUidAiInformation, 
        KActiveIdleLaunch, 
        EPSAiLaunchNotActive );
    }

// ---------------------------------------------------------------------------
// CActive
// ---------------------------------------------------------------------------
//
void CAiScutShortcut::RunL()
    {
    RProperty::Set( 
        KPSUidAiInformation, 
        KActiveIdleLaunch, 
        EPSAiLaunchNotActive );
    }
// End of File.
