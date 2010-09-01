/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __MCSPLUGINHANDLER_H__
#define __MCSPLUGINHANDLER_H__

#include <mcsmenuhandlerplugin.h>


#include <e32base.h>
#include <viewcli.h> // For CVwsSessionWrapper
#include <msvapi.h>  // For MMsvSessionObserver
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif

#include <vwsdef.h>  // For TVwsViewId


class CEikonEnv;
class CVwsSessionWrapper;
class CMsvSession;

/**
 *  @ingroup group_mcsplugin
 *
* Handler for menu items of type "menu:shortcut" "menu:folder" and "menu:suite".
 *
 *  @since S60 9.1
*/
NONSHARABLE_CLASS( CMCSPluginHandler ): public CMenuHandlerPlugin, public MMsvSessionObserver
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMCSPluginHandler();

    /**
    * Two-phased constructor. Leaves on failure.
    * @param aMenu Menu.
    * @return The constructed object.
    */
    static CMCSPluginHandler* NewL( RMenu &aMenu );

protected:  // construction

    /**
    * Constructor.
    * @param aMenu Menu.
    */
    CMCSPluginHandler( RMenu &aMenu );

    /**
    * Second-phase constructor.
    */
    void ConstructL();

public:     // from CMenuHandler

    /**
    * Query support for item type.
    * @param aType Type.
    * @return ETrue if handler supports this type, EFalse otherwise.
    */
    TBool SupportsType( const TDesC& aType );

    /**
    * Handle command.
    * @param aItem Item of supported type.
    * @param aCommand Command.
    * @param aParams. Command parameters.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    */
    CMenuOperation* HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus );

public:     // from MMsvSessionObserver

    /**
     * Handles an event from the message server.
     * Not used, but must be defined to be able to use the messaging server.
     *
     * @since S60 v3.2
     * @param aEvent Indicates the event type.
     * @param aArg1 Event type-specific argument value
     * @param aArg2 Event type-specific argument value
     * @param aArg3 Event type-specific argument value
     */
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, 
        TAny* aArg3 );

    
    
private:    // internal

    void LaunchShortcutL( CMenuItem& aItem );
    
    TInt GetEmailAccountCountL();

private:    // data
    
    CEikonEnv* iEikEnv;
    
    /**
     * View server session wrapper.
     * Own
     */
    CVwsSessionWrapper* iVwsSession;

    /**
     * Message server session
     * Own.
     */
    CMsvSession* iMsvSession;

    };

#endif // __MCSPLUGINHANDLER_H__
