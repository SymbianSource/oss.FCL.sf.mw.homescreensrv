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
* Description:  Psln Active Idle settings plugin.
*
*/


#ifndef C_GSACTIVEIDLEPLUGIN_H
#define C_GSACTIVEIDLEPLUGIN_H

#include <gsplugininterface.h>
#include <coneresloader.h>

class CGSActiveIdlePluginContainer;
class CPslnFWPluginInterface;
class CPslnFWPluginHandler;


/**
 *  CGSActiveIdlePlugin view class for Psln Active Idle settings.
 *  @since S60 3.2
 *
 */
class CGSActiveIdlePlugin : public CGSPluginInterface
{

public: // Constructors and destructor

    /**
     * Symbian OS two-phased constructor
     *
     * @param aInitParams param not used.
     * @return GS connection view.
     */
    static CGSActiveIdlePlugin* NewL(TAny* aInitParams);

    /**
     * Destructor
     */
    ~CGSActiveIdlePlugin();

// From CGSPluginInterface

    /**
     * @see CGSPluginInterface header file.
     */
    void GetCaptionL(TDes& aCaption) const;

    /**
     * @see CGSPluginInterface header file.
     */
    TInt PluginProviderCategory() const;

    /**
     * @see CGSPluginInterface header file.
     */
    TBool Visible() const;

// from base classes

    /**
     * Returns view id.
     * @return TUid
     */
    TUid Id() const;

    /**
     * Handles commands.
     * @param aCommand Command to be handled.
     *
     */
    void HandleCommandL(TInt aCommand);

protected: // From CAknView

    /**
     * @see CAknView.
     */
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage);

    /**
     * @see CAknView.
     */
    void DoDeactivate();

// Construction

    /**
     * C++ default constructor.
     */
    CGSActiveIdlePlugin();

    /**
     * Symbian OS default constructor.
     *
     */
    void ConstructL();

private: // data

    /// Resource loader.
    RConeResourceLoader             iResourceLoader;

    /// Container. Owned.
    CGSActiveIdlePluginContainer*   iContainer;

    /// Previous view id.
    TVwsViewId                      iPrevViewId;

    /// Plugin loader. Owned.
    CPslnFWPluginHandler*           iPluginLoader;

};

#endif // C_GSACTIVEIDLEPLUGIN_H

// End of File.
