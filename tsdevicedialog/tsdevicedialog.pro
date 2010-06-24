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
    SUBDIRS += ../tstaskmonitor
}

SUBDIRS += tsdevicedialogplugin

tests {
    SUBDIRS += internal/tstestrunner    
    SUBDIRS += tsdevicedialogplugin/tsrc
    SUBDIRS += ../activityfw/activityserviceplugin/tsrc
    SUBDIRS += ../activityfw/storage/client/t_afstorageclient
    SUBDIRS += ../hsappkeyhandler/tsrc/t_hsappkeyhandler
    SUBDIRS += ../tstaskmonitor/client/tsrc
    SUBDIRS += ../tstaskmonitor/server/tsrc
    SUBDIRS += ../tstaskmonitor/screenshotplugin/tsrc
    SUBDIRS += ../activityfw/tsutils/tsrc
    SUBDIRS += ../tstaskmonitor/utils/tsrc
    SUBDIRS += ../tstaskmonitor/backstepping/tsrc
}

symbian:include(rom.pri)

CONFIG += ordered
