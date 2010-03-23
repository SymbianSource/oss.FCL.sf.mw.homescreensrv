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
# Description: Example of home screen widget
#

TEMPLATE = lib

CONFIG += plugin hb

CONFIG(debug, debug|release) {
  DESTDIR = debug
  LIBS += -L../../../../bin/debug
}
else {
  DESTDIR = release
  LIBS += -L../../../../bin/release
}

LIBS += -lhswidgetmodel

HEADERS += ./inc/*.h \
           ./helloworldwidget/inc/*.h

SOURCES += ./src/*.cpp \
           ./helloworldwidget/src/*.cpp

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
               ./helloworldwidget/inc 
               

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    # get your own symbian uid    
    TARGET.UID3 = 0xEABCFE12
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    # add your symbian uid here
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/EABCFE12
    
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    qtplugins.sources += resource/$${TARGET}.manifest
    qtplugins.sources += resource/$${TARGET}.png
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)" 
}

win32 {
    # add platfrom API for windows
    INCLUDEPATH += ../../../homescreensrv_plat/hswidgetmodel_api
}