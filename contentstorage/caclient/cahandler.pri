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

INCLUDEPATH += ../cahandler/inc \
    ../cahandler/app/inc \
    ../cahandler/url/inc \
	../cahandler/tapp/inc

HEADERS += ../cahandler/inc/*.h \
    ../cahandler/app/inc/*.h \
    ../cahandler/url/inc/*.h \
	../cahandler/tapp/inc/catapphandler.h
    
SOURCES += ../cahandler/app/src/*.cpp \
    ../cahandler/url/src/*.cpp  \
		../cahandler/tapp/src/catapphandler.cpp

LIBS += -lswinstcli \
        -lapgrfx \
        -lws32 \
        -lapparc \
        -lsisregistryclient \
        -leikcore \
        -lcone \
        -lefsrv \
        -lxqservice
		

