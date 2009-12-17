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
* Description:  Shortcut settings plug-in main class.
*
*/


#ifndef CAISCUTSETTINGS_H
#define CAISCUTSETTINGS_H

#include <gsbaseview.h>

class CAiScutSettingsContainer;
class CAiScutSettingsModel;

/**
 * Shortcut settings plug-in main class
 *
 * @lib aiscutsettings.lib
 * @since S60 v3.2
 */
class CAiScutSettings : public CGSBaseView
{

public:

    static CAiScutSettings* NewL(TAny* aInitParams);

    virtual ~CAiScutSettings();

    // from base class CAknView

    /**
     * From CAknView
     * Returns view id.
     *
     * @since S60 3.2
     * @return TUid
     */
    TUid Id() const;

    /**
     * From CAknView
     * Handles commands.
     *
     * @since S60 3.2
     * @param aCommand Command to be handled.
     */
    void HandleCommandL(TInt aCommand);

    /**
     * See base class in AknView.h
     */
    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);

    /**
     * See base class.
     */
    void DoDeactivate();

    // from base class CGSPluginInterface

    /**
     * From CGSPluginInterface
     *
     * @since S60 3.2
     * @see CGSPluginInterface header file.
     */
    void GetCaptionL(TDes& aCaption) const;

    /**
     * From CGSPluginInterface
     *
     * @since S60 3.2
     * @see CGSPluginInterface header file.
     */
    TInt PluginProviderCategory() const;

    /**
     * From CGSPluginInterface
     *
     * @since S60 3.2
     * @see CGSPluginInterface header file.
     */
    TBool Visible() const;

    // from base class CGSBaseView

    /**
     * From CGSBaseView
     *
     * @since S60 3.2
     * Returns the container this view has.
     * @return pointer to container, does not transfer ownership.
     */
    CAiScutSettingsContainer* Container();
    
    /**
    * See base class
    */
    void HandleForegroundEventL(TBool aForeground);

    /**
    * Check if view is activate
    * @since S60 5.1
    * @return ETrue if activated, EFalse otherwise
    */    
    TBool Activated() const;

protected:

private: // From MEikMenuObserver

    /**
     * Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:

    CAiScutSettings();

    void ConstructL();


    // from base class CGSBaseView

    /**
     * From CGSBaseView
     * Functionality for creating a container. Called by DoActivateL().
     */
    void NewContainerL();

    /**
     * From CGSBaseView
     * Handles listbox selection. Called by HandleListBoxEventL.
     * Implement by sub-class.
     */
    void HandleListBoxSelectionL();
    
    static TInt DoHandleListBoxSelectionL( TAny* aAny );


private:  // data

    /**
     * Settings plug-in model.
     * Own.
     */
    CAiScutSettingsModel*       iModel;

    /**
     * Resource loader for common text resources.
     */
    RConeResourceLoader         iResourceLoaderTexts;

    /***
     * Idle timer to delay event handling
     * Own.
     */
    CIdle* iListBoxTimer;

};

#endif // CAISCUTSETTINGS_H

// End of File.
