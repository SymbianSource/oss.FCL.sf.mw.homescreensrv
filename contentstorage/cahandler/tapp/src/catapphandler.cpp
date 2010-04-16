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
 * Description:  Default implementation of the home screen runtime.
 *
 */

#include <qservicemanager.h>
#include <qservicefilter.h>
#include <qserviceinterfacedescriptor.h>
#include <XQConversions>
#include <xqappmgr.h>

#include "caentry.h"
#include "catapphandler.h"
#include "cainnerentry.h"
#include "caclient_defines.h"

_LIT(hsitemLaunchUri, "item:launchuri");

QTM_USE_NAMESPACE

/*!
    \class CaTappHandler
    \ingroup
    \brief Implementation of templated application command handler.
*/

/*!
 Constructor.
 \param parent Parent object of this object.
 */
CaTappHandler::CaTappHandler(QObject *parent)
{
	Q_UNUSED(parent);
    mAiwMgr = new XQApplicationManager();
}

/*!
 Destructor.
 */
CaTappHandler::~CaTappHandler()
{
    delete mAiwMgr;
}

/*!
 Executes \a command by default "open" on template application represented by \a entry.
 Uses XQApplicationManager
 \param entry Subject of the \a command.
 \param command Description of the command.
 \return Error code described in QSERVICEMANAGER.H
 */
int CaTappHandler::execute(CCaInnerEntry& innerEntry, const QString& command)
{
    int error = 0; // this returns Error enum from QSERVICEMANAGER.H
    if (command == caCmdOpen) {
        TPtrC attribute;
        innerEntry.FindAttribute(hsitemLaunchUri, attribute);

        // e.g. QUrl url ("application://101F7AE7?");
        QUrl url(QString::fromUtf16(attribute.Ptr(), attribute.Length()));
        QScopedPointer<XQAiwRequest> request(mAiwMgr->create(url, true));
        if (!request.isNull()) {
            bool res = request->send();
            if (!res) {
                error = request->lastError();
            }
        }
    }
    return error;
}
