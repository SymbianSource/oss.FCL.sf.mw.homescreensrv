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
TARGET = afstorageclient

QT        += core \
             sql

HEADERS   += inc/afstorageclient.h \
             inc/afstorageentry.h

SOURCES   += src/afstorageclient.cpp \
             src/afstorageentry.cpp

DEFINES += ACTIVITY_LIB

symbian{
TARGET.UID3 = 0xE467C21E

HEADERS   += ../s60/inc/afentry.h \
             ../s60/inc/afserializer.h \
             s60/inc/afstorageclient_p.h \
             s60/inc/afstorageclientimp.h \
			 s60/inc/afasyncrequest_p.h \
             s60/inc/afthumbnailrequest_p.h

SOURCES   += ../s60/src/afentry.cpp \
             ../s60/src/afserializer.cpp \
             s60/src/afstorageclient_p.cpp \
             s60/src/afstorageclientimp.cpp \
			 s60/src/afasyncrequest_p.cpp \
             s60/src/afthumbnailrequest_p.cpp

LIBS      += -lxqutils \
             -lestor \
             -lfbscli

afstorageclientdll.sources = afstorageclient.dll
afstorageclientdll.path = $$SHARED_LIB_DIR
DEPLOYMENT += afstorageclientdll

BLD_INF_RULES.prj_exports += "../inc/afstorageglobals.h /epoc32/include/afstorageglobals.h" \
                             "../inc/afcmd.h /epoc32/include/afcmd.h" \
                             "inc/afstorageclient.h |../../../inc/afstorageclient.h" \
                             "inc/afstorageentry.h |../../../inc/afstorageentry.h" \
							 "inc/afasyncrequestobserver.h |../../../inc/afasyncrequestobserver.h"
DEPLOYMENT += api
TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x200267B3
MMP_RULES += EXPORTUNFROZEN
}
