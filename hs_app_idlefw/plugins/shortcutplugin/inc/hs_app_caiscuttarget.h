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
* Description:  Base class for all shortcut targets.
*
*/


#ifndef CAISCUTTARGET_H
#define CAISCUTTARGET_H

#include <e32base.h>

#include "hs_app_taiscutparser.h"
#include "aiscutdefs.h"
#include <AknsItemID.h>

class CAiScutEngine;
class CGulIcon;

_LIT(KBitmapFile, "aiscutplugin.mif");

/**
 *  Base class for shortcut targets
 *
 *  @since S60 v3.2
 */
class CAiScutTarget : public CBase
{

public:

    virtual ~CAiScutTarget();

    /**
     * Returns the shortcut definition string.
     *
     * @since S60 v3.2
     * @return Shortcut definition string
     */
    virtual TPtrC Definition() const = 0;

    /**
     * Returns the shortcut target caption.
     * Caption can be either a descriptor or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the descriptor reference parameter is used.
     * Default implementation returns KErrNotSupported;
     *
     * Getting the target caption and icon utilizes the "lazy evaluation" idiom
     * which means they are created only when they are first needed. This is because
     * there are (atleast) two scenarios where they are never needed:
     * 1. The active theme does not support shortcut captions or icons.
     * 2. Shortcuts launched using LaunchByValue are never published.
     *
     * @since S60 v3.2
     * @param aDes On return, the descriptor if used
     * @return The resource id or zero to indicate that the descriptor is used.
     */
    virtual TInt GetCaption( TPtrC& aDes, TAiScutAppTitleType aTitleType ) const;

    /**
     * Returns the shortcut target icon.
     * Icon can be either an icon pointer or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the icon pointer is used.
     * Default implementation returns the icon that has possibly
     * been set with @c SetOverrideIcon() and returns
     * KErrNotSupported if no override icon has been set.
     *
     * Uses the "lazy evaluation" idiom to get the icon. See Caption()
     *
     * @since S60 v3.2
     * @param aIcon On return, the icon pointer if used
     * @return The resource id or zero to indicate that the icon pointer is used.
     */
    virtual TInt GetIcon( CGulIcon*& aIcon ) const;
    
     /**
     * Returns the shortcut icon that can be published to softkeys
     * Icon can be either an icon pointer or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the icon pointer is used.
     * Default implementation returns the icon that has possibly
     * been set with @c SetSoftkeyIcon() and returns
     * KErrNotSupported if no softkey icon has been set.
     *
     * Uses the "lazy evaluation" idiom to get the icon. See Caption()
     *
     * @since S60 v3.2
     * @param aIcon On return, the icon pointer if used
     * @return The resource id or zero to indicate that the icon pointer is used.
     */
    virtual TInt GetSoftkeyIcon( CGulIcon*& aIcon ) const;

     /**
     * Returns the shortcut icon that can be published to toolbar
     * Icon can be either an icon pointer or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the icon pointer is used.
     * Default implementation returns the icon that has possibly
     * been set with @c SetSoftkeyIcon() and returns
     * KErrNotSupported if no softkey icon has been set.
     *
     * Uses the "lazy evaluation" idiom to get the icon. See Caption()
     *
     * @since S60 v3.2
     * @param aIcon On return, the icon pointer if used
     * @return The resource id or zero to indicate that the icon pointer is used.
     */
    virtual TInt GetToolbarIcon( CGulIcon*& aIcon ) const;

    /**
     * Checks if the shortcut target is accessible.
     *
     * @since S60 v3.2
     * @param aCheckType Specifies which kind of shortcuts should check its access
     * @return ETrue if accessible, EFalse if not
     */
    virtual TBool IsAccessibleL( TInt aCheckType ) = 0;

    /**
     * Launches the shortcut.
     *
     * @since S60 v3.2
     */
    virtual void LaunchL() = 0;

    /**
     * Launches the shortcut.
     *
     * @since S60 v5.1
     * @param aMessage A message to pass
     */
    virtual void LaunchL(const TDesC8& aMessage) { (void)aMessage; };

    /**
     * Return application uid this target launches.
     *
     * @since S60 v3.2
     */
    virtual TUid AppUid() const = 0;

    /**
     * Return an additional id this target may use when 
     * launching. Default implementation return TUid(-1)
     *
     * @since S60 v3.2
     */
    virtual TUid AdditionalUid() const;

    /**
     * Return shortcut type.
     *
     * @since S60 v3.2
     */
    TShortcutType Type() const;
    
    /**
     * Starts a full screen effect.
     */
    void BeginEffectL();

    /**
     * Set the icon that overrides the shortcut default
     * icon. Instead of fetching the icon from for example an
     * application the icon defined here can be used. The shortcut
     * can use the @c GetIcon() to fetch the 
     * icon defined here. Currently we support
     * icon loading from skin or from MIF/MBM-file. 
     * The file must reside under z:\resource altough
     * it can be under some subdirectory. 
     *
     * @since S60 v3.2
     * @param aIcon The overriding icon
     */
    void SetOverrideIcon(TAiScutIcon aIcon);
    
    /**
     * Set the icon that is to be published to softkeys
     * The @c GetSoftkeyIcon() can be used to fetch the 
     * icon defined here. Currently we support
     * icon loading from skin or from MIF/MBM-file. 
     * The file must reside under z:\resource altough
     * it can be under some subdirectory. 
     *
     * @since S60 v3.2
     * @param aIcon The softkey icon
     */
    void SetSoftkeyIcon(TAiScutIcon aIcon);

    /**
     * Set the icon that is to be published to touch toolbar
     * The @c GetSoftkeyIcon() can be used to fetch the 
     * icon defined here. Currently we support
     * icon loading from skin or from mif-file. 
     * The mif-file must reside under z:\resource altough
     * it can be under some subdirectory. 
     *
     * @since S60 v5.0
     * @param aIcon The softkey icon
     */
    void SetToolbarIcon(TAiScutIcon aIcon);
    
protected:

    CAiScutTarget( CAiScutEngine& aEngine, TShortcutType aType );

private:
    
    /**
     * Returns the icon defined in aIconToLoads. Loads the icon
     * from MIF/MBM-file or from skin.
     *
     * @return KErrNone if no error
    */
    TInt GetIcon(const TAiScutIcon &aIconToLoad, CGulIcon *&aIcon ) const;
    
    /**
    * Loads an icon defined in aIconToLoad from skin
    *
    * @return KErrNone if no error
    */
    void LoadIconFromSkinL(const TAiScutIcon &aIconToLoad, CGulIcon*& aIcon) const;
    
    /**
    * Loads an icon defined in aIconToLoad from MIF/MBM-file
    * The file must reside under z:\resource. It can be under
    * some subdirectory.
    *
    * @return KErrNone if no error
    */
    void LoadIconFromFileL(const TAiScutIcon &aIconToLoad, CGulIcon*& aIcon) const;


protected:  // data

    /**
     * Reference to shortcut plug-in engine.
     */
    CAiScutEngine&  iEngine;

    /**
     * Shortcut type.
     */
    TShortcutType   iType;
    
    /**
    * Icon that is meant to override the default icon
    */
    TAiScutIcon iOverrideIcon;
   
    /**
    * Icon that is to be published to softkeys
    */
    TAiScutIcon     iSoftkeyIcon;
    
   /**
    * Icon that is to be published to toolbar
    */
    TAiScutIcon     iToolbarIcon;
   
};

#endif // CAISCUTTARGET_H

// End of File.
