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

INCLUDEPATH += ./inc \

HEADERS +=  ./inc/activityserviceplugin.h \
            ./inc/activitydatastorage.h \
            ./inc/activityclient.h \
            ./inc/activityclient_p.h \
            ./inc/activitymanager.h \
            ./inc/activitymanager_p.h \
            ./inc/applicationlauncher.h \

SOURCES +=  ./src/activityserviceplugin.cpp \
            ./src/activitydatastorage.cpp \
            ./src/activityclient.cpp \
            ./src/activityclient_p.cpp \
            ./src/activitymanager.cpp \
            ./src/activitymanager_p.cpp \
            ./src/applicationlauncher.cpp \

symbian {
    INCLUDEPATH += ./s60/inc \

    SOURCES += ./s60/src/applicationlauncher_p.cpp
    
    HEADERS += ./s60/inc/applicationlauncher_p.h

    LIBS += -lapparc \
            -lapgrfx \
            -lcone \
            -lhsactivitydbclient \
            -lxqutils
 
}

win32 {
    INCLUDEPATH += ./win/inc \

    SOURCES += ./win/src/applicationlauncher_p.cpp

    HEADERS += ./win/inc/applicationlauncher_p.h
}            
            
symbian {
    load(data_caging_paths) 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x200267B2

    plugin.sources = activityserviceplugin.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR     
    
    xml.sources = ./data/activityserviceplugin.xml
    xml.path = $$RESOURCE_FILES_DIR/activity
    
    DEPLOYMENT += xml

    #temporary workaround
    BLD_INF_RULES.prj_exports += "data/activityserviceplugin.xml z:/resource/activity/activityserviceplugin.xml"
    
}
