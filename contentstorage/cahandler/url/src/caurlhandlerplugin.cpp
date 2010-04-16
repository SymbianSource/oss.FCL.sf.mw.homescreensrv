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

#include "caurlhandlerplugin.h"
#include "cas60urlhandleradapter.h"
#include "caurlhandler.h"

/*!
    \class CaUrlHandlerPlugin
    \ingroup
    \brief Implementation of Qt SF plugin for url command handler.
*/

/*!
    \param descriptor Service descriptor.
    \param context Ignored.
    \param session Ignored.
    \return An instance of the CaS60UrlHandlerAdapter<CCaUrlHandler> when descriptor interface name
    is "com.nokia.homescreen.ICommandHandler", NULL otherwise.
*/
QObject *CaUrlHandlerPlugin::createInstance(const QServiceInterfaceDescriptor &descriptor,
        QServiceContext *context,
        QAbstractSecuritySession *session)
{
    Q_UNUSED(context);
    Q_UNUSED(session);

    if (descriptor.interfaceName() ==
            "com.nokia.homescreen.ICommandHandler") {
        return new CaS60UrlHandlerAdapter();
    } else {
        return 0;
    }
}
Q_EXPORT_PLUGIN2(caurlhandlerplugin, CaUrlHandlerPlugin)
