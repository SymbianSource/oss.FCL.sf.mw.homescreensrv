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

TEMPLATE = subdirs

coverage {
    SUBDIRS += ../activityfw
    SUBDIRS += ../hsappkeyhandler
    SUBDIRS += ../taskswitcher
}

SUBDIRS += tsdevicedialogplugin

tests {
    SUBDIRS += ../internal/tstestutils
    SUBDIRS += internal/tstestrunner    
    SUBDIRS += tsdevicedialogplugin/tsrc
    SUBDIRS += ../activityfw/activityserviceplugin/tsrc
    SUBDIRS += ../activityfw/storage/client/t_afstorageclient
    SUBDIRS += ../activityfw/storage/server/tsrc
    SUBDIRS += ../activityfw/afactivitylauncher/tsrc
    SUBDIRS += ../hsappkeyhandler/tsrc/t_hsappkeyhandler
    SUBDIRS += ../taskswitcher/client/tsrc
    SUBDIRS += ../taskswitcher/server/tsrc
    SUBDIRS += ../taskswitcher/screenshotplugin/tsrc
    SUBDIRS += ../activityfw/tsutils/tsrc
    SUBDIRS += ../taskswitcher/utils/tsrc
    SUBDIRS += ../taskswitcher/backstepping/tsrc
}

symbian:include(rom.pri)

CONFIG += ordered
