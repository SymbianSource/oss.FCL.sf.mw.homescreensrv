/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MCS settings plug-in main class.
*
*/


#ifndef MCSPLUGINSETTINGS_H
#define MCSPLUGINSETTINGS_H

#include <gsbaseview.h>

class CMCSPluginSettingsContainer;
class CMCSPluginSettingsModel;

/**
 *  @ingroup group_mcsplugin
 *
 * MCS settings plug-in main class
 *
 * @lib mcspluginsettings.lib
 * @since S60 9.1
 */
class CMCSPluginSettings : public CGSBaseView
{

public:
    /**
     * Part of the two phased constuction
     * 
     * @param aInitParams
     * @return CMCSPluginSettings*
     */
    static CMCSPluginSettings* NewL(TAny* aInitParams);

    /**
     * Destructor
     */
    virtual ~CMCSPluginSettings();

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
     * 
     * @param aPrevViewId
     * @param aCustomMessageId
     * @param aCustomMessage
     */
    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, 
        const TDesC8& aCustomMessage);

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
     * @param aCaption
     */
    void GetCaptionL(TDes& aCaption) const;

    /**
     * From CGSPluginInterface
     *
     * @since S60 3.2
     * @see CGSPluginInterface header file.
     * @return TInt
     */
    TInt PluginProviderCategory() const;

    /**
     * From CGSPluginInterface
     *
     * @since S60 3.2
     * @see CGSPluginInterface header file.
     * @return TBool
     */
    TBool Visible() const;

    /**
     * From CGSBaseView
     *
     * @since S60 3.2
     * Returns the container this view has.
     * @return pointer to container, does not transfer ownership.
     */
    CMCSPluginSettingsContainer* Container();

private: // From MEikMenuObserver

    /**
     * Changes MenuPane dynamically
     * 
     * @param aResourceId
     * @param aMenuPane
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
    /**
     * CMCSPluginSettings
     */
    CMCSPluginSettings();

    /**
     * Part of the two phased construction
     */
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

    /**
     * Callback function to be used with CPeriodic.
     */
    static TInt TimerCallbackL( TAny *aPtr );
    
private:  // data

    /**
     * Settings plug-in model.
     * Own.
     */
    CMCSPluginSettingsModel* iModel;
    
    /** 
     * Timer to change specified shortcut when view is activated.
     * Own. 
     */
    CPeriodic* iTimer;    
};

#endif // MCSPLUGINSETTINGS_H

// End of File.
