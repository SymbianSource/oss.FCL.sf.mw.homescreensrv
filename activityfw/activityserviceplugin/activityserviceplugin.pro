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
               ../../inc

HEADERS +=  ./inc/afserviceplugin.h \
            ./inc/afclient.h \
            ./inc/afmanager.h \
            ./inc/aflauncher.h \

SOURCES +=  ./src/afserviceplugin.cpp \
            ./src/afclient.cpp \
            ./src/afmanager.cpp \
            ./src/aflauncher.cpp \

symbian {
    INCLUDEPATH += ./s60/inc \

    SOURCES += ./s60/src/applicationlauncher_p.cpp
    
    HEADERS += ./s60/inc/applicationlauncher_p.h

    LIBS += -lapparc \
            -lapgrfx \
            -lcone \
            -lafstorageclient \
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
