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
DEPENDPATH += . \
              inc \
              src
INCLUDEPATH += . \
               inc 
               
symbian: {
TARGET.UID3 = 0x20022F3B
}

# Sources
include(statemodel.pri)
include(../../common.pri)

DEFINES += HSSTATEMODEL_LIB

QT += xml
