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
symbian:{
DEPENDPATH += ./inc \
			./src
INCLUDEPATH += ./inc \
            ../inc

TARGET.UID2 = 0x1000008D
TARGET.UID3 = 0x2001CB55

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.VENDORID = VID_DEFAULT

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

LIBS += -leuser \
        -lbafl \
        -lestor
        
CONFIG -= stl
include(cautils.pri)
}