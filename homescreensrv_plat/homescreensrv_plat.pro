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
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"

include(appruntimemodel_api/appruntimemodel_api.pri)
include(contentstorage_api/contentstorage_api.pri)
include(hswidgetmodel_api/hswidgetmodel_api.pri)
include(servicemodel_api/servicemodel_api.pri)
include(statemodel_api/statemodel_api.pri)
include(menu_sat_interface_api/menu_sat_interface_api.pri)

for(filename,PLATFORM_HEADERS){
    BLD_INF_RULES.prj_exports *= "$$filename MW_LAYER_PLATFORM_EXPORT_PATH($$basename(filename))"
  }


BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include \"group/bld.inf\""