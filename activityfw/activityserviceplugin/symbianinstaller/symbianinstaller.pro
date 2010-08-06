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

TEMPLATE = subdirs
SUBDIRS = activityserviceinstaller

symbian {
    load(data_caging_paths) 

    plugin.sources = afservice.dll
    plugin.path = $$QT_PLUGINS_BASE_DIR 
    
    installer.pkg_postrules += "\"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/activityserviceinstaller.exe\" \  
                                         - \"!:\sys\bin\activityserviceinstaller.exe\",FR,RB,RW"
    
    DEPLOYMENT += plugin installer
}
