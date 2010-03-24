/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef HSACTIVITY_H
#define HSACTIVITY_H


const char ActivityOrganization [] = "nokia";

const char ActivityApplication [] = "hsactivitydbserver";

const char ActivityStorageDriver [] = "QSQLITE";

const char ActivityStorageName [] = "ActivitySQLClient";

const char ActivityStorageProperty [] = "activitydatabase";

const char ActivityDefaultStorage [] = "activitydatabase.db";

const char ActivityTableKeyword [] = ":Table";
const char ActivityDataKeyword [] = ":Data";



const char ActivityCreateQuery [] = "CREATE TABLE Activities(" \
                                    "ApplicationId INTEGER NOT NULL," \
                                    "ActivityName TEXT NOT NULL," \
                                    "Data BLOB NOT NULL," \
                                    "PRIMARY KEY ( ApplicationId, ActivityName ) )";

const char ActivityDropQuery [] = "DROP TABLE :Table";

const char ActivitySelectActivityQuery [] = "SELECT ApplicationId FROM Activities WHERE " \
                                            "ApplicationId = :ApplicationId AND " \
                                            "ActivityName = :ActivityName";

const char ActivityUpdateActivityQuery [] = "UPDATE Activities SET Data = :Data WHERE " \
                                            "ApplicationId = :ApplicationId AND " \
                                            "ActivityName = :ActivityName";

const char ActivityInsertActivityQuery [] = "INSERT INTO Activities( ApplicationId, ActivityName, Data ) VALUES " \
                                            "( :ApplicationId, :ActivityName, :Data )";

const char ActivitySelectActiveQuery [] = "SELECT ActivityName FROM Activities WHERE " \
                                          "ApplicationId = :ApplicationId";

const char ActivityActivitiesQuery [] = "SELECT Data FROM Activities";

const char ActivityApplicationActivitiesQuery [] = "SELECT Data FROM Activities " \
                                                   "WHERE ApplicationId = :ApplicationId";

const char ActivityDeleteActivityQuery [] = "DELETE FROM Activities WHERE " \
                                            "ApplicationId = :ApplicationId AND " \
                                            "ActivityName = :ActivityName";

const char ActivityDeleteApplicationActivitiesQuery [] = "DELETE FROM Activities WHERE " \
                                                         "ApplicationId = :ApplicationId";

#endif //HSACTIVITY_H
