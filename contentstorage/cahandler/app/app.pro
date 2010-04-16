# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = lib
TARGET = caapphandlerplugin
CONFIG += plugin hb mobility
MOBILITY = serviceframework

PLUGIN_SUBDIR = /hsresources/plugins/commandhandler

include(../../../common.pri)


DEPENDPATH += ./inc \
    ./src
	
INCLUDEPATH += ./inc \
    ../../inc \
    ../../cautils/inc \
    ../../caclient/inc \
    ../../caclient/s60/inc \
    $$MW_LAYER_SYSTEMINCLUDE
	
include(app.pri)

symbian: {
    TARGET.UID3 = 0x20022F83

    LIBS += \
    -leuser \
    -lecom \
    -lswinstcli \
    -lcautils \
    -lapgrfx \
    -lws32 \
    -lapparc \
    -lsisregistryclient \
    -leikcore \
    -lcone \
    -lefsrv \
    -lsif \
    -lscrclient \
    -lxqutils

    exists($${EPOCROOT}epoc32/include/mw/XQSettingsManager):LIBS += -lxqsettingsmanager
}

