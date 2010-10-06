#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

PLATFORM_HEADERS += taskswitcher_api/tstasksettings.h \
                    taskswitcher_api/tstaskmonitor_global.h \
                    taskswitcher_api/tspropertydefs.h \

symbian {
    crml.sources = tsdevicedialog.qcrml
    crml.path = /resource/qt/crml
    DEPLOYMENT += crml
}
