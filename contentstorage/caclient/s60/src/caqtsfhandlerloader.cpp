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

#include <QMap>
#include <QScopedPointer>
#include <QString>
#include <qservice.h>
#include <qstringlist.h>
#include <qservicemanager.h>

#include "cahandler.h"
#include "caqtsfhandlerloader.h"
#include "caclient_defines.h"
#include "caapphandler.h"
#include "caurlhandler.h"
#include "catapphandler.h" 
#include "cas60handleradapter.h"

QTM_USE_NAMESPACE

/*!
    \class CaQtSfHandlerLoader
    \ingroup 
    \brief Loads handlers implementation

    The purpose of the class is to find Qt SF plugins implementing command handlers.
    Temporarily because of issues with Qt SF this is replaced by immediate object
    creation.
    \sa CaHandlerLoader
*/

/*!
    Loads handler implementations appropriate for the requested entry type name and command.
    
    The caller takes ownership of the returned pointer.
        
    \param entryTypeName Entry type name.
    \param commandName Name of the command to be handled.
    \return A pointer to handler serving the entry type and command if found, NULL otherwise.
*/
CaHandler *CaQtSfHandlerLoader::loadHandler(const QString &entryTypeName,
        const QString &commandName)
{
    Q_UNUSED(commandName);

    CaHandler *implementation(0);

    if (entryTypeName == APPLICATION_ENTRY_TYPE_NAME
        || entryTypeName == WIDGET_ENTRY_TYPE_NAME) {
        implementation = new CaS60HandlerAdapter<CCaAppHandler>;
    } else if (entryTypeName == URL_ENTRY_TYPE_NAME) {
        implementation = new CaS60HandlerAdapter<CCaUrlHandler>;
    } else if (entryTypeName == TEMPLATED_APPLICATION_ENTRY_TYPE_NAME) {
        implementation = new CaTappHandler;
    }

    return implementation;
}


