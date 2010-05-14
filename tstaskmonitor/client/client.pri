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

    HEADERS +=  $$PWD/s60/inc/tstaskmonitorobserver.h \
                $$PWD/s60/inc/tstaskmonitorclient.h \
                $$PWD/s60/inc/tstaskmonitorclientimpl.h \
                $$PWD/s60/inc/tstaskmonitorsession.h \
                $$PWD/../utils/inc/tsfswentry.h \
                $$PWD/../utils/inc/tsutils.h \
                $$PWD/s60/inc/tstaskmonitor_p.h \
                $$PWD/s60/inc/tsapplicationtask.h \
                
    SOURCES +=  $$PWD/s60/src/tstaskmonitorclient.cpp \
                $$PWD/s60/src/tstaskmonitorclientimpl.cpp \
                $$PWD/s60/src/tstaskmonitorsession.cpp \
                $$PWD/../utils/src/tsfswentry.cpp \
                $$PWD/s60/src/tstaskmonitor_p.cpp \
                $$PWD/s60/src/tstask.cpp \
                $$PWD/s60/src/tsapplicationtask.cpp \

    LIBS    +=  -lxqutils \
                -lestor \
                -lfbscli

} else {
    INCLUDEPATH +=  $$PWD/stub/inc \

    HEADERS +=  $$PWD/stub/inc/tstaskmonitor_p.h \
                $$PWD/stub/inc/tsfswentry.h \
                
    SOURCES +=  $$PWD/stub/src/tstaskmonitor_p.cpp \
                $$PWD/stub/src/tstask.cpp \
                $$PWD/stub/src/tsfswentry.cpp \
                
}
