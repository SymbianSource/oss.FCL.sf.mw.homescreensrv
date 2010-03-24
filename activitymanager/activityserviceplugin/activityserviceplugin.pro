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

TEMPLATE = lib
TARGET = activityserviceplugin

CONFIG += plugin 
include(activityserviceplugin.pri)

HEADERS +=  activityserviceplugin.h \
            activitydatastorage.h \
            activityclient.h \
            activityclient_p.h \
            activitymanager.h \
            activitymanager_p.h \
            applicationlauncher.h \

SOURCES +=  activityserviceplugin.cpp \
            activitydatastorage.cpp \
            activityclient.cpp \
            activityclient_p.cpp \
            activitymanager.cpp \
            activitymanager_p.cpp \
            applicationlauncher.cpp \

symbian {
    INCLUDEPATH += ./s60/
    SOURCES += ./s60/applicationlauncher_p.cpp
    
    HEADERS += ./s60/applicationlauncher_p.h

    LIBS += -lhsactivitydbclient \
            -lxqutils \
 
}

win32 {
    INCLUDEPATH += ./win/
    SOURCES += ./win/applicationlauncher_p.cpp

    HEADERS += ./win/applicationlauncher_p.h
}            
            
symbian {
    load(data_caging_paths) 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    plugin.sources = activityserviceplugin.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR     
}
