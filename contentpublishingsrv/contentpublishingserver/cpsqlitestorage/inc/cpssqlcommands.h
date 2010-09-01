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


#ifndef C_CPSSQL_H
#define C_CPSSQL_H
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
                                 Data BLOB, \
                                 Action BLOB, \
                                 UNIQUE( Publisher, Content_type, Content_id ));" );

_LIT( KSQLCreatePublisherTable,
    "CREATE TABLE CP_PUBLISHER_TABLE( Id INTEGER PRIMARY KEY ,\
                                      Publisher TEXT NOT NULL,\
                                      Content_type TEXT NOT NULL,\
                                      Content_id TEXT NOT NULL,  \
                                      Data BLOB, \
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

//Insert Data
//Update Data
_LIT( KSQLUpdateComplete,
    "UPDATE CP_MAIN_TABLE SET  \
	                Data = :Data, Action = :Action WHERE Id = :Id " );

_LIT( KSQLUpdateData,
    "UPDATE CP_MAIN_TABLE SET  \
					Data = :Data WHERE Id = :Id " );

_LIT( KSQLUpdateAction,
    "UPDATE CP_MAIN_TABLE SET  \
					Action = :Action WHERE Id = :Id " );

_LIT( KSQLInsert,
    "INSERT INTO CP_MAIN_TABLE (Publisher,Content_type,Content_id,Expiration_date,Data,Action) VALUES ( \
					            :Publisher,\
                                :Content_type,\
                                :Content_id,\
                                :Expiration_date,\
                                :Data,\
                                :Action )" );

//Delete Data
_LIT( KSQLDelete, "DELETE FROM CP_MAIN_TABLE WHERE " );

//Get Data
_LIT( KSQLGetLastInsertId, "SELECT last_insert_rowid() " );
_LIT( KSQLGetList,
    "SELECT Id,Publisher,Content_type,Content_id,Expiration_date,Data,Action FROM CP_MAIN_TABLE WHERE " );
_LIT( KSQLGetIds,
    "SELECT Id,Publisher,Content_type,Content_id FROM CP_MAIN_TABLE WHERE " );
_LIT( KSQLGetAction, "SELECT Id, Action FROM CP_MAIN_TABLE WHERE " );
_LIT( KSQLGetPublisher, "Publisher = :Publisher" );
_LIT( KSQLGetContentType, "Content_type = :Content_type" );
_LIT( KSQLGetContentId, "Content_id = :Content_id" );
_LIT( KSQLGetId, "Id = :Id" );
_LIT( KAnd, " AND " );

_LIT( KSQLFetchId,
    "SELECT Id FROM CP_MAIN_TABLE WHERE Publisher =:Publisher AND Content_type=:Content_type AND Content_id =:Content_id" );
_LIT( KSQLFetchIds,
    "SELECT Publisher,Content_type,Content_id FROM CP_MAIN_TABLE WHERE Id =:Id" );

_LIT( KSQLPublisher, ":Publisher" );
_LIT( KSQLContent_type, ":Content_type" );
_LIT( KSQLContent_id, ":Content_id" );
_LIT( KSQLExpiration_date, ":Expiration_date" );
_LIT( KSQLData, ":Data" );
_LIT( KSQLAction, ":Action" );
_LIT( KSQLId, ":Id" );

_LIT( KSQLOrderRecent, " ORDER BY Id DESC" );
_LIT( KSQLOrderPublisher, " ORDER BY Publisher" );
_LIT( KSQLOrderContent, " ORDER BY Content_type" );

_LIT( KColumnId, "Id" );
_LIT( KColumnPublisher, "Publisher" );
_LIT( KColumnContent_type, "Content_type" );
_LIT( KColumnContent_id, "Content_id" );
_LIT( KColumnExpiration_date, "Expiration_date" );
_LIT( KColumnData, "Data" );
_LIT( KColumnAction, "Action" );

_LIT8( KList, "List" );

#endif //C_CPSSQL_H
