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
TARGET = hsactivitydbclient

QT        += core \
             sql

QT        -= gui 

HEADERS   += inc/hsactivitydbclient.h

SOURCES   += src/hsactivitydbclient.cpp

DEFINES += ACTIVITY_LIB

symbian{
TARGET.UID3 = 0xE467C21E

HEADERS   += ../s60/inc/hsserializer.h \
             s60/inc/hsactivitydbclient_p.h \
			 s60/inc/hsactivitydbasyncrequest_p.h

SOURCES   += ../s60/src/hsserializer.cpp \
             s60/src/hsactivitydbclient_p.cpp \
			 s60/src/hsactivitydbasyncrequest_p.cpp


hsactivitydbclientdll.sources = hsactivitydbclient.dll
hsactivitydbclientdll.path = $$SHARED_LIB_DIR
DEPLOYMENT += hsactivitydbclientdll

BLD_INF_RULES.prj_exports += "../inc/hsactivitydbclientinterface.h /epoc32/include/hsactivitydbclientinterface.h" \
                             "inc/hsactivitydbclient.h /epoc32/include/hsactivitydbclient.h" \
							 "inc/hsactivitydbasyncrequestobserver.h /epoc32/include/hsactivitydbasyncrequestobserver.h"
DEPLOYMENT += api
TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += EXPORTUNFROZEN
}
