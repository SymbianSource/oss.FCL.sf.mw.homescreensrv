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

TEMPLATE = app
TARGET = hsactivitydbserver 

QT        += core \
             gui \
             sql 

HEADERS   += ../inc/hsactivitydbclientinterface.h \
             inc/hsactivity.h \
             inc/hsactivityserver.h \
             inc/hsactivitystorage.h \
			 inc/hsactivitystorage_p.h

SOURCES   += src/hsactivityserver.cpp \
			 src/hsactivitystorage.cpp \
             src/hsactivitystorage_p.cpp

symbian{
TARGET.UID3 = 0x200267B4
HEADERS   += ../s60/inc/hsactivityglobals.h \
             ../s60/inc/hsserializer.h \
             s60/inc/hsactivityserver_p.h \
             s60/inc/hsactivitysession_p.h
			 

SOURCES   += s60/src/main.cpp \
             ../s60/src/hsserializer.cpp \
             s60/src/hsactivityserver_p.cpp \
			 s60/src/hsactivitysession_p.cpp

TARGET.CAPABILITY = ALL -TCB
}

