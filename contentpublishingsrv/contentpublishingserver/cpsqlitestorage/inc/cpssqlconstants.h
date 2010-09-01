/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_CPSSQLCONSTS_H
#define C_CPSSQLCONSTS_H
#ifdef CONTENT_PUBLISHER_STORAGE_MT
_LIT( KCPpathdatabase, "c:\\cpdatastorage.sq" );
#endif

_LIT( KCPdatabase, "[20016B7B]cpdatastorage.sq" );
_LIT( KCPdatabaseDrive, "C:" );
_LIT8( KMdsSqlDbaConfig,
    "cache_size=10000; page_size=4096; encoding=\"UTF-8\";" );

//Create Tables
_LIT( KSQLCreateMainTable,
    "CREATE TABLE CP_MAIN_TABLE( Id INTEGER PRIMARY KEY ,\
                                 Publisher TEXT NOT NULL,\
                                 Content_type TEXT NOT NULL,\
                                 Content_id TEXT NOT NULL,\
                                 Expiration_date BIGINT NOT NULL,\
                                 Access_List BLOB,\
                                 Data BLOB, \
                                 Action BLOB, \
                                 UNIQUE( Publisher, Content_type, Content_id ));" );

_LIT( KSQLCreatePublisherTable,
    "CREATE TABLE CP_PUBLISHER_TABLE( Id INTEGER PRIMARY KEY ,\
                                      Publisher TEXT NOT NULL,\
                                      Content_type TEXT NOT NULL,\
                                      Content_id TEXT NOT NULL,  \
                                      Flag INTEGER NOT NULL, \
                                      Access_List BLOB,\
                                      Data BLOB, \
                                      Action BLOB, \
                                      UNIQUE( Publisher, Content_type, Content_id ));" );

//Create Indexes
_LIT( KSQLIndexOnMain_Publisher,
    "CREATE INDEX Main_PublisherIndex ON CP_MAIN_TABLE(Publisher);" );
_LIT( KSQLIndexOnMain_Content_type,
    "CREATE INDEX Main_Content_typeIndex ON CP_MAIN_TABLE(Content_type);" );

_LIT( KSQLIndexOnPub_Content_type,
    "CREATE INDEX Pub_Content_typeIndex ON CP_PUBLISHER_TABLE(Content_type);" );
_LIT( KSQLIndexOnPub_Content_type_Content_id,
    "CREATE INDEX Pub_Content_type_Content_Id_Index ON CP_PUBLISHER_TABLE(Content_type,Content_id);" );

#endif //C_CPSSQLCONSTS_H
