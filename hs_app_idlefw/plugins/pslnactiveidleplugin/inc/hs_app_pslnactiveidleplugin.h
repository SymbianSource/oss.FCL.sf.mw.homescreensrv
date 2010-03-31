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
* Description:  View for Active Idle skinning.
*
*/


#ifndef C_PSLNACTIVEIDLEPLUGIN_H
#define C_PSLNACTIVEIDLEPLUGIN_H

#include <aknsettingpage.h>
#include <pslnfwbaseview.h>
#include <coneresloader.h>
#include <pslnfwplugininterface.h>
#include <mpslnfwmsklabelobserver.h>


class CAknViewAppUi;
class CPslnActiveIdlePluginContainer;
class CPslnActiveIdlePluginEngine;

enum TAiUIControllers
{
    EAiUICUnknown = -1,
    EAiUICNative = 0,
    EAiUICXML
};

// View item text length
const TInt KPslnItemMaxTextLength = 128;


/**
 *  CPslnActiveIdlePlugin view class for Active Idle Theme settings
 *
 *  @since S60 3.2
 */
class CPslnActiveIdlePlugin :
    public CPslnFWBaseView,
    public MPslnFWMSKObserver
{
public: // Constructors and destructor

    /**
     * Symbian OS two-phased constructor
     *
     * @param aAppUi pointer to app ui, TAny static cast'd.
     * @return Psln AI skinning view.
     */
    static CPslnActiveIdlePlugin* NewL(TAny* aAppUi);

    /**
     * Destructor
     */
    ~CPslnActiveIdlePlugin();

// From CPslnFWnInterface

    /**
     * @see CPslnFWPluginInterface.
     */
    void GetCaptionL(TDes& aCaption) const;


    /**
     * @see CPslnFWPluginInterface.
     */
    void GetTabTextL(TDes& aCaption) const;

    /**
     * @see CPslnFWPluginInterface.
     */
    CGulIcon* CreateIconL();

    /**
     * @see CPslnFWPluginInterface.
     */
    void GetLocationTypeAndIndex(TPslnFWLocationType& aType, TInt& aIndex) const;

// from AknView

    /**
     * Returns view id.
     * @return TUid
     */
    TUid Id() const;

    /**
     * Handles commands.
     * @param aCommand Command to be handled.
     */
    void HandleCommandL(TInt aCommand);

// new methods

    /**
     * Get CPslnActiveIdlePlugin's ccontainer.
     * @return pointer to container.
     */
    CPslnActiveIdlePluginContainer* Container();

    /**
     * From MPslnFWMSKObserver.
     * Checks if the MSK label needs to be adjusted.
     */
    void CheckMiddleSoftkeyLabelL();
    
    /**
     * Return engine instance.
     */
    CPslnActiveIdlePluginEngine* Engine();

protected: // From CAknView

    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage);

    void DoDeactivate();

// Construction

    CPslnActiveIdlePlugin(CAknViewAppUi* aAppUi);

    void ConstructL();

private: // From MEikMenuObserver

    /**
     * Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

// from CPslnFWBaseView

    void NewContainerL();

    void HandleListBoxSelectionL();

    /**
     * Remove command and label from MSK
     */
    void RemoveCommandFromMSK();
    
    /**
     * @see CPslnFWBaseView.
     */
    void SetTitlePaneL( TInt& aResourceId );

private: // data

    /// Resource loader for own resources.
    RConeResourceLoader         iResourceLoader;

    /// Resource loader for common psln resources.
    RConeResourceLoader         iResourceLoaderCommon;

    /// Resource loader for Psln app resources.
    RConeResourceLoader         iResourceLoaderPsln;

    /// Application UI pointer. Not owned.
    CAknViewAppUi*              iAppUi;

    /// Navi pane. Not owned.
    CAknNavigationDecorator*    iNaviPaneContext;
    
    /// Ai2 perso engine
    CPslnActiveIdlePluginEngine* iEngine;

};

#endif // C_PSLNACTIVEIDLEPLUGIN_H

// End of File.
