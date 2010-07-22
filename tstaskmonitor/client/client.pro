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
TARGET = tstaskmonitorclient

CONFIG += dll
    
DEFINES += TSTASKMONITOR_LIB
            
symbian {
    dll.sources = tstaskmonitorclient.dll
    dll.path = $$SHARED_LIB_DIR
    DEPLOYMENT += dll
    LIBS += -ltsutils -lws32 -lapgrfx -lcone

    BLD_INF_RULES.prj_exports +=    "s60\inc\tstaskmonitorclient.h" \
                                    "s60\inc\tstaskmonitorobserver.h" \
                                    "inc\tstaskmonitor.h" \
                                    "inc\tstask.h" \
                                    "inc\tstaskmonitor_global.h" \
                                    "s60\inc\tsscreenshotclient.h" \

    TARGET.UID3 = 0x200267AF    
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1    
    
    # temporary, remove when API is stable enough
    MMP_RULES += EXPORTUNFROZEN
}

include(client.pri)
