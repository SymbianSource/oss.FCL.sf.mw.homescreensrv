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
* Description:  Event handling extension for the CAiContentPublisher interface.
*
*/


#ifndef M_AIEVENTHANDLEREXTENSION_H
#define M_AIEVENTHANDLEREXTENSION_H

#include <e32std.h>

/**
 *  Extension of the CAiContentPublisher interface, which allows Active Idle
 *  Framework to notify their plug-ins that they must to handle event.
 *  Plug-ins must provide implementation of interface only if they are
 *  supporting event model (e.g. provides possibility to manipulate engine 
 *  properties via UI).
 *
 *  @since S60 3.2
 */
class MAiEventHandlerExtension
    {
public:
    /**
     * Invoked by the framework when plug-in must handle an event.
     *
     * @param aEvent - unique identifier of event from plug-in content model.
     * @param aParam - parameters associated with event. Each UI Definition
     *        declares events in the format: <event name>(<event params>),
     *        where <event name> is mapped by the framework to unique
     *        identifier supplied in aEvent, <event params> are provided to
     *        plug-in as-is in the descriptor.
     */
    virtual void HandleEvent(TInt aEvent, const TDesC& aParam) = 0;

    /**
    * Invoked by the framework when plug-in must handle an event.
    *
    * @param aEventName - name of the event from plug-in content model.
    * @param aParam - parameters associated with event. Each UI Definition
    *        declares events in the format: <event name>(<event params>),
    *        where  <event name> mapping to unique identifier supplied by event 
    *        is failed by the frame work then the  <event name> and  
    *        <event params>  are provied to plug-in as-is in the descriptor.
    */
    virtual void HandleEvent(const TDesC& /*aEventName*/, const TDesC& /*aParam*/) { };
    
    /**
    * Invoked by the framework for querying if plugin has menu item
    *
    * @param aMenuItem menu item name
    * @return ETrue if plugin has specific menu item, EFalse otherwise
    */    
    virtual TBool HasMenuItem(const TDesC& /*aMenuItem*/) { return EFalse; }
    
protected:
    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiEventHandlerExtension() { }
    };

#endif // M_AIEVENTHANDLEREXTENSION_H
