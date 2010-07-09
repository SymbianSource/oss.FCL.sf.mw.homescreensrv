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
# Description: tsdevicedialogplugin.pro
#

TEMPLATE = lib
CONFIG += hb plugin mobility
MOBILITY = serviceframework publishsubscribe

include (../common.pri)

HEADERS +=  inc/tsdevicedialogcontainer.h \
            inc/tsdevicedialogplugin.h \
            inc/tsmodel.h \
            inc/tsmodelitem.h \
            inc/tsentrymodelitem.h \
            inc/tsactivitymodelitem.h \
            inc/tstasksgrid.h \
            inc/tstasksgriditem.h \
            inc/tsdocumentloader.h \
            inc/tsdataroles.h \

SOURCES +=  src/tsdevicedialogcontainer.cpp \
            src/tsdevicedialogplugin.cpp \
            src/tsmodel.cpp \
            src/tsentrymodelitem.cpp \
            src/tsactivitymodelitem.cpp \
            src/tstasksgrid.cpp \
            src/tstasksgriditem.cpp \
            src/tsdocumentloader.cpp \

INCLUDEPATH += . \
               ./inc \

LIBS +=     -ltstaskmonitorclient
            
TRANSLATIONS = taskswitcher.ts

RESOURCES += tsdevicedialogplugin.qrc
               
symbian {
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    TARGET.UID3 = 0x2002677F
    pluginstub.sources = tsdevicedialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    
    LIBS += -lxqsettingsmanager -lapparc -lapgrfx
    
    # ugly hack
    translation.sources = ./*.qm
    translation.path = $$QT_PLUGINS_BASE_DIR/../translations
    
    crml.sources = ./resource/*.qcrml
    crml.path = /resource/qt/crml
    
    DEPLOYMENT += pluginstub translation crml
    
    BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "conf/tsdevicedialog.confml APP_LAYER_CONFML(tsdevicedialog.confml)" \
    "conf/tsdevicedialog_2002677F.crml APP_LAYER_CRML(tsdevicedialog_2002677F.crml)"
}

win32 {
    include ($$[QMAKE_MKSPECS]/features/hb_install.prf)
    DESTDIR = $$HB_PLUGINS_DIR/devicedialogs
}