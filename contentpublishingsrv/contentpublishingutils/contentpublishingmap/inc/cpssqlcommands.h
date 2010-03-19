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
 *  Version     : %version: ou1s60ui#7.1.1 % << Don't touch! Updated by Synergy at check-out.
 *
*/


#ifndef C_CPSSQL_H
#define C_CPSSQL_H

//Insert Data
//Update Data


_LIT( KSQLUpdate, "UPDATE CP_MAIN_TABLE SET " );
_LIT( KSQLUpdateData, "Data = :Data " );
_LIT( KSQLUpdateAction, "Action = :Action " );
_LIT( KSQLUpdateExpirationDate, "Expiration_date = :Expiration_date " );
_LIT( KSQLUpdateFlag, "Flag = :Flag " );
_LIT( KSQLUpdateWhere, "WHERE Id = :Id  " );
_LIT( KComma, ",  " );

_LIT( KSQLUpdateRegistry, "UPDATE CP_PUBLISHER_TABLE SET " );

_LIT( KSQLInsert,
    "INSERT INTO CP_MAIN_TABLE (Publisher,Content_type,Content_id,Expiration_date,Access_List,Data,Action) VALUES ( \
					            :Publisher,\
                                :Content_type,\
                                :Content_id,\
                                :Expiration_date,\
                                :Access_List,\
                                :Data,\
                                :Action )" );

_LIT( KSQLInsertToRegistry,
    "INSERT INTO CP_PUBLISHER_TABLE (Publisher,Content_type,Content_id,Flag,Access_List,Data,Action) VALUES ( \
					            :Publisher,\
                                :Content_type,\
                                :Content_id,\
                                :Flag,\
                                :Access_List,\
                                :Data,\
                                :Action )" );

//Delete Data
_LIT( KSQLDelete, "DELETE FROM CP_MAIN_TABLE" );
_LIT( KSQLDeleteFromRegistry, "DELETE FROM CP_PUBLISHER_TABLE" );

//Get Data
_LIT( KSQLGetLastInsertId, "SELECT last_insert_rowid() ");

_LIT( KSQLGetList, 
    "SELECT Id,Publisher,Content_type,Content_id,Expiration_date,Access_List,Data,Action FROM CP_MAIN_TABLE");
_LIT( KSQLGetIds,
    "SELECT Id,Publisher,Content_type,Content_id,Access_List FROM CP_MAIN_TABLE");
_LIT( KSQLGetIdsRegistry, 
    "SELECT Id,Publisher,Content_type,Content_id,Access_List,Flag FROM CP_PUBLISHER_TABLE" );
_LIT( KSQLGetAction, 
    "SELECT Action FROM CP_MAIN_TABLE WHERE ");

_LIT( KSQLGetListFromRegistry, 
    "SELECT Id,Publisher,Content_type,Content_id,Access_List,Flag,Data,Action FROM CP_PUBLISHER_TABLE");
_LIT( KSQLGetIdsFromRegistry, 
    "SELECT Id,Publisher,Content_type,Content_id FROM CP_PUBLISHER_TABLE");
_LIT( KSQLGetActionFromRegistry, 
    "SELECT Id,Action FROM CP_PUBLISHER_TABLE WHERE ");

_LIT( KSQLGetFlag ,"Flag <> 0");
_LIT( KSQLGetPublisher ,"Publisher = :Publisher");
_LIT( KSQLGetContentType ,"Content_type = :Content_type");
_LIT( KSQLGetContentId ,"Content_id = :Content_id");
_LIT( KSQLGetId ,"Id = :Id");
_LIT( KAnd," AND ");

_LIT( KSQLFetchId, 
    "SELECT Id FROM CP_MAIN_TABLE WHERE Publisher =:Publisher AND Content_type=:Content_type AND Content_id =:Content_id");
_LIT( KSQLFetchIds, 
    "SELECT Publisher,Content_type,Content_id,Access_List FROM CP_MAIN_TABLE WHERE Id =:Id");

_LIT( KSQLFetchIdFromRegistry,
    "SELECT Id FROM CP_PUBLISHER_TABLE WHERE Publisher =:Publisher AND Content_type=:Content_type AND Content_id =:Content_id" );
_LIT( KSQLFetchIdsFromRegistry,
    "SELECT Publisher,Content_type,Content_id,Access_List FROM CP_PUBLISHER_TABLE WHERE Id =:Id" );

_LIT( KSQLPublisher, ":Publisher" );
_LIT( KSQLContent_type, ":Content_type" );
_LIT( KSQLContent_id, ":Content_id" );
_LIT( KSQLExpiration_date, ":Expiration_date" );
_LIT( KSQLFlag, ":Flag" );
_LIT( KSQLData, ":Data" );
_LIT( KSQLAction, ":Action" );
_LIT( KSQLAccessList, ":Access_List" );
_LIT( KSQLId, ":Id" );

_LIT( KSQLOrderRecent, " ORDER BY Id DESC" );
_LIT( KSQLOrderPublisher, " ORDER BY Publisher" );
_LIT( KSQLOrderContent, " ORDER BY Content_type" );
_LIT( KSQLOrderContentId, " ORDER BY Content_id" );

_LIT( KColumnId, "Id" );
_LIT( KColumnPublisher, "Publisher" );
_LIT( KColumnContent_type, "Content_type" );
_LIT( KColumnContent_id, "Content_id" );
_LIT( KColumnAccessList, "Access_List" );
_LIT( KColumnExpiration_date, "Expiration_date" );
_LIT( KColumnFlag, "Flag" );
_LIT( KColumnData, "Data" );
_LIT( KColumnAction, "Action" );

_LIT( KWhere, " WHERE ");

_LIT8( KList, "List" );

#endif //C_CPSSQL_H
