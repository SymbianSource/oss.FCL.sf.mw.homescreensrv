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
* Description:  Base class for shortcut target
*
*/


#include "hs_app_caiscutengine.h"
#include "hs_app_caiscuttarget.h"
#include <AknIconUtils.h>
#include <AknsUtils.h>          // For AknsUtils
#include <mifconvdefs.h>
#include <gulicon.h>
#include <bautils.h>
#include <AknTaskList.h>        // For CAknTaskList
#include <apgwgnam.h>           // For CApaWindowGroupName
#include <gfxtranseffect/gfxtranseffect.h>		// For Transition effect
#include <akntranseffect.h>
#include <akntransitionutils.h>

// ======== MEMBER FUNCTIONS ========
_LIT(KFileLoadDir,"z:\\resource\\");
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTarget::CAiScutTarget(CAiScutEngine& aEngine, TShortcutType aType)
    : iEngine(aEngine)
    , iType(aType)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTarget::~CAiScutTarget()
{
    
}

// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTarget::GetCaption(TPtrC& /*aDes*/, TAiScutAppTitleType /*aTitleType*/) const
{
    // Default implementation.
    return KErrNotSupported;
}

TUid CAiScutTarget::AdditionalUid() const
    {
    return TUid::Uid(-1);
    }

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTarget::GetIcon(const TAiScutIcon &aIconToLoad, CGulIcon*& aIcon ) const
{   
    TInt err = KErrNotSupported;
    
    if ( aIconToLoad.iType == EScutIconSkin )
        {
        TRAP(err, LoadIconFromSkinL(aIconToLoad, aIcon));
        }
    else if ( aIconToLoad.iType == EScutIconMif )
        {
        TRAP(err, LoadIconFromFileL(aIconToLoad, aIcon));
        }
    else if ( aIconToLoad.iType == EScutIconMbm )
        {
        TRAP(err, LoadIconFromFileL(aIconToLoad, aIcon));
        }
    
    return err;
}

TInt CAiScutTarget::GetIcon(CGulIcon*& aIcon) const
    {
    return GetIcon(iOverrideIcon, aIcon);
    }

TInt CAiScutTarget::GetSoftkeyIcon(CGulIcon*& aIcon) const
    {
    return GetIcon(iSoftkeyIcon, aIcon);
    }

TInt CAiScutTarget::GetToolbarIcon(CGulIcon*& aIcon) const
    {
    return GetIcon(iToolbarIcon, aIcon);
    }
    
void CAiScutTarget::SetOverrideIcon(TAiScutIcon aIcon)
    {
    iOverrideIcon = aIcon;
    }

void CAiScutTarget::SetSoftkeyIcon(TAiScutIcon aIcon)
    {
    iSoftkeyIcon = aIcon;
    }

void CAiScutTarget::SetToolbarIcon(TAiScutIcon aIcon)
    {
    iToolbarIcon = aIcon;
    }

void CAiScutTarget::LoadIconFromSkinL(const TAiScutIcon &aIconToLoad, CGulIcon*& aIcon) const
    {
    CFbsBitmap* bitmap = NULL;
	CFbsBitmap* bitmapMask = NULL;
	
    // With colour group support
    if ( aIconToLoad.iColourGroup >= EAknsCIQsnTextColorsCG1 &&
           aIconToLoad.iColourGroup <= EAknsCIQsnTextColorsCG62)
        {
        
        AknsUtils::CreateColorIconLC(
            AknsUtils::SkinInstance(),
            aIconToLoad.iSkinId,
            KAknsIIDQsnTextColors,
            aIconToLoad.iColourGroup,
            bitmap,
            bitmapMask,
            KNullDesC,  /* no backup */
            0,          /* no backup */
            0,          /* no backup */
            KRgbBlack );
            
        if ( bitmap )
            {
            aIcon = CGulIcon::NewL( bitmap, bitmapMask );              
            }
        CleanupStack::Pop( 2 ); // bitmap, bitmapMask
        }
    // no colour group support
    else
        {
        aIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), aIconToLoad.iSkinId, KNullDesC, 0, 0 );
        }
    if ( !aIcon )
        {
        User::Leave( KErrNotFound );
        }
    }

void CAiScutTarget::LoadIconFromFileL(const TAiScutIcon &aIconToLoad, CGulIcon*& aIcon) const 
    {
    CFbsBitmap* bitmap = NULL;
	CFbsBitmap* bitmapMask = NULL;
    TFileName actualPath;
    TInt iconIndexStart = 0;
    
    if ( aIconToLoad.iType == EScutIconMif )
        {
        iconIndexStart = KMifIdFirst;
        }
    actualPath.Append(KFileLoadDir);
    actualPath.Append(aIconToLoad.iPath);
    
    RFs fs; fs.Connect();
	if ( aIconToLoad.iIconId < 0 || !BaflUtils::FileExists(fs, actualPath))
		{
		fs.Close();
		User::Leave( KErrNotFound );
		}
    fs.Close();
	// Mask is next after bitmap
    AknIconUtils::CreateIconLC(bitmap,bitmapMask, actualPath,
        aIconToLoad.iIconId+iconIndexStart, aIconToLoad.iIconId+iconIndexStart+1);
      

    aIcon = CGulIcon::NewL(bitmap, bitmapMask);

    CleanupStack::Pop(bitmapMask);    
    CleanupStack::Pop(bitmap);
    
    if ( !aIcon )
        {
        User::Leave( KErrNotFound );        
        }
    
    }

// ---------------------------------------------------------------------------
// Returns the shortcut target type.
// ---------------------------------------------------------------------------
//
TShortcutType CAiScutTarget::Type() const
{
    return iType;
}

// ---------------------------------------------------------------------------
// Determines which effect should be shown, and starts it.
// ---------------------------------------------------------------------------
//
void CAiScutTarget::BeginEffectL()
	{
	RWsSession& aWs = iEngine.Env()->WsSession();
	TUid appUid = AppUid();
	TInt effectType = AknTransEffect::EApplicationStart;
	CAknTaskList* taskList = CAknTaskList::NewL( aWs );
	TApaTask task = taskList->FindRootApp( appUid );
	delete taskList;
	
	if ( task.Exists() )
		{
		TBool inHiddenList = iEngine.IsHiddenFromFSW(appUid);
		CApaWindowGroupName* wgName = CApaWindowGroupName::NewL( aWs, task.WgId() );
		wgName->SetAppUid( appUid );
		const TBool isHidden = wgName->Hidden() || inHiddenList;
		delete wgName;
				
		if (!isHidden)
			{
			effectType = AknTransEffect::EApplicationStartSwitchRect;
			}
		
		//start a full screen effect
		GfxTransEffect::BeginFullScreen( effectType,
		TRect(0,0,0,0), 
		AknTransEffect::EParameterType, 
		AknTransEffect::GfxTransParam( appUid, 
		AknTransEffect::TParameter::EActivateExplicitContinue ) );
		}
	}

// End of File.
