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

#Exporting ibys
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}ifndef CONTENT_ARSENAL_STORAGE_UT" \
     "../castorage/data/castorage.db z:/private/20022F90/castorage.db" \
     "$${LITERAL_HASH}else " \
     "../castorage/data/castorage.db z:/castorage.db" \
     "$${LITERAL_HASH}endif" \
     "../data/backup_registration.xml z:/private/20022F90/backup_registration.xml"

BLD_INF_RULES.prj_mmpfiles += "casathandler.mmp" \
    "../castorage/group/castorage.mmp" \
    "../casatinterface/group/MenuInterface.mmp" \
    "../casrv/casrvmgr/group/casrvmanager.mmp" \
    "camenu.mmp" \
    "camenusrv.mmp" \
    "../casrv/caappscanner/group/caappscanner.mmp" \
    "../casrv/casatmonitor/group/casatmonitor.mmp" \
    "../casrv/cawidgetscanner/group/cawidgetscanner.mmp" \
    "../casrv/carunningappmonitor/group/carunningappmonitor.mmp" \
    "../casrv/causifscanner/group/causifscanner.mmp" \
    
tests_krakow {
        BLD_INF_RULES.prj_mmpfiles += \
#        "$${LITERAL_HASH}include \"../castorage/internal/tsrc/castoragetester/group/bld.inf\" " \  # disabled, needs CONTENT_ARSENAL_STORAGE_UT flag
#        "$${LITERAL_HASH}include \"../internal/MT_casrv/group/bld.inf\" " \ # disabled, needs CONTENT_ARSENAL_STORAGE_UT flag
        "$${LITERAL_HASH}include \"../internal/MT_sathandler/group/bld.inf\" " \
        "$${LITERAL_HASH}include \"../internal/MT_handler/group/bld.inf\" " \
#        "$${LITERAL_HASH}include \"../casrv/caappscanner/internal/MT_cappscanner/group/bld.inf\" " \ # disabled, needs CONTENT_ARSENAL_STORAGE_UT flag
        "$${LITERAL_HASH}include \"../casrv/casatmonitor/internal/MT_satmonitor/group/bld.inf\" " \
#        "$${LITERAL_HASH}include \"../casrv/cawidgetscanner/internal/MT_hswidgetscanner/group/bld.inf\" " # disabled, needs CONTENT_ARSENAL_STORAGE_UT flag
}