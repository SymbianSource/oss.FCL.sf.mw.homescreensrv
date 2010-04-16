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

#include <cadefs.h>
#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>
#include "cahandlerloader.h"

#include "caapphandler.h"
#include "caapphandlerplugin.h"
#include "cas60handleradapter.h"

/*!
    \class CaAppHandlerPlugin
    \ingroup
    \brief Implementation of Qt SF plugin for application command handler.
*/

/*!
    \param descriptor Service descriptor.
    \param context Ignored.
    \param session Ignored.
    \return An instance of the CaS60HandlerAdapter<CCaAppHandler> when descriptor interface name
    is "com.nokia.homescreen.ICommandHandler", NULL otherwise.
*/
QObject *CaAppHandlerPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
        QServiceContext *context,
        QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() ==
            "com.nokia.homescreen.ICommandHandler") {
        return new CaS60HandlerAdapter<CCaAppHandler>;
    } else {
        return 0;
    }
}
// TODO: uncomment when it will be used in plugin
//Q_EXPORT_PLUGIN2(caapphandlerplugin, CaAppHandlerPlugin)


/*!
    \class CaHandler
    \ingroup
    \brief Interface for command handlers
*/

/*!
    \fn int CaHandler::execute(const CaEntry &entry, const QString &commandName) = 0
    \param entry Subject of the command.
    \param commandName The name of the command to execute.
    Returns 0 on success, error code otherwise.
    \sa e32err.h for error code descriptions.
*/

/*!
    \class CaS60HandlerAdapter<typename Plugin>
    \ingroup
    \brief Adapter for S60 command handlers.

    Adapts S60 command handlers to CaHandler interface

    \sa CaHandler
*/

/*!
    \fn int CaS60HandlerAdapter<typename Plugin>::execute(const CaEntry &entry, const QString &commandName) 
    \param entry Subject of the command.
    \param commandName The name of the command to execute.
    \return 0 on success, error code otherwise.
    \sa e32err.h for error code descriptions.
*/
