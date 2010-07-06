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

INCLUDEPATH +=  $$PWD/inc

HEADERS +=  $$PWD/inc/tstaskmonitor_global.h \
            $$PWD/inc/tstaskmonitor.h \
            $$PWD/inc/tstask.h \
            
SOURCES +=  $$PWD/src/tstaskmonitor.cpp \
            
symbian {
    INCLUDEPATH +=  $$PWD/s60/inc \
                    $$PWD/../inc \
                    $$PWD/../utils/inc \   
                    $$PWD/../../inc \   

    HEADERS +=  $$PWD/s60/inc/tstaskmonitorobserver.h \
                $$PWD/s60/inc/tstaskmonitorclient.h \
                $$PWD/s60/inc/tstaskmonitorclientimpl.h \
                $$PWD/s60/inc/tssession.h \
                $$PWD/../utils/inc/tsentry.h \
                $$PWD/../utils/inc/tsentrykey.h \
                $$PWD/../utils/inc/tsutils.h \
                $$PWD/s60/inc/tstaskmonitor_p.h \
                $$PWD/s60/inc/tsapplicationtask.h \
                $$PWD/s60/inc/tsscreenshotclient.h \
                $$PWD/s60/inc/tsscreenshotclientimpl.h \
                $$PWD/s60/inc/tstasksettings_p.h \
                $$PWD/../../homescreensrv_plat/taskswitcher_api/tstasksettings.h \
                
    SOURCES +=  $$PWD/s60/src/tstaskmonitorclient.cpp \
                $$PWD/s60/src/tstaskmonitorclientimpl.cpp \
                $$PWD/s60/src/tssession.cpp \
                $$PWD/../utils/src/tsentry.cpp \
                $$PWD/../utils/src/tsentrykey.cpp \                              
                $$PWD/s60/src/tstaskmonitor_p.cpp \
                $$PWD/s60/src/tstask.cpp \
                $$PWD/s60/src/tsapplicationtask.cpp \
                $$PWD/s60/src/tsscreenshotclient.cpp \
                $$PWD/s60/src/tsscreenshotclientimpl.cpp \
                $$PWD/src/tstasksettings.cpp \
                $$PWD/s60/src/tstasksettings_p.cpp \

    LIBS    +=  -lxqutils \
                -lestor \
                -lfbscli

} else {
    INCLUDEPATH +=  $$PWD/stub/inc \

    HEADERS +=  $$PWD/stub/inc/tstaskmonitor_p.h \
                $$PWD/stub/inc/tsentry.h \
                $$PWD/stub/inc/tstasksettings_p.h \
                
    SOURCES +=  $$PWD/stub/src/tstaskmonitor_p.cpp \
                $$PWD/stub/src/tstask.cpp \
                $$PWD/stub/src/tsentry.cpp \
                $$PWD/stub/src/tstasksettings_p.cpp \
                
}
