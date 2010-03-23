/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QList>
#include <QDebug>
#include <QString>
#include <QtSql>
#include <QMap>
#include <QMapIterator>

#include "caclientproxy.h"
#include "caobjectadapter.h"

#include "caentry.h"
#include "caquery.h"
#include "cadefs.h"
#include "canotifier.h"
#include "canotifier_p.h"
#include "canotifiers.h"

#include "hswidgetregistryservice.h"

const char* DATABASE_CONNECTION_NAME = "CaService";
const char* DATABASE_TYPE = "QSQLITE";
const char* DATABASE_NAME = "castoragedb";

static QSqlDatabase dbConnection()
{
    return QSqlDatabase::database(DATABASE_CONNECTION_NAME, false);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
CaClientProxy::CaClientProxy() :
    mWidgetRegistryPath("hsresources/import/widgetregistry")
{
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
CaClientProxy::~CaClientProxy()
{
    QSqlDatabase db = dbConnection();
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(DATABASE_CONNECTION_NAME);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::connect()
{
    ErrorCode errorCode = NotFoundErrorCode;
    QSqlDatabase db = QSqlDatabase::addDatabase(DATABASE_TYPE,
        DATABASE_CONNECTION_NAME);
    if (db.isValid()) {
        db.setDatabaseName(DATABASE_NAME);
        if (db.open()) {
            errorCode = NoErrorCode;
            updateWidgets();
        }
    }
    if (errorCode) {
        qDebug("CaClientProxy::CaClientProxy FAILED");
    }
    return errorCode;
}

/*!
 Updates widgets.
 */
void CaClientProxy::updateWidgets()
{
    qDebug("CaClientProxy::updateWidgets start");
    
    HsWidgetRegistryService *rs =
        new HsWidgetRegistryService(mWidgetRegistryPath);
    QList<HsWidgetToken> widgets = rs->widgets();
    
    // Read widgets in order to add synchronize the content of the widgets
    // registry with Content Storage database.
    foreach (const HsWidgetToken &widgetToken, widgets) {
        int uid = widgetToken.mUid;
        
        if (!hsWidgetExists(uid)) {
            // The given widget does not have a corresonding entry
            // in the databse, so such an entry needs do be created.
            addWidgetEntry(widgetToken);
        }
    }
    
    delete rs;
    qDebug("CaClientProxy::updateWidgets end");
}

/*!
 Returns true if a widget with the given uid exists in the database.
 */
void CaClientProxy::addWidgetEntry(const HsWidgetToken &widgetToken)
{
    QString description = widgetToken.mDescription;
    QString iconUri = widgetToken.mIconUri;
    QString library = widgetToken.mLibrary;
    QString title = widgetToken.mTitle;
    int uid = widgetToken.mUid;
    QString uri = widgetToken.mUri;
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    QString hexUid;
    hexUid.setNum(uid,16);
    QDir currentDir = QDir::current();
    
    // Add icon.
    QString queryAddIcon =
        "INSERT INTO CA_ICON " \
            "(IC_FILENAME) " \
        "VALUES " \
            "(?)";

    query.prepare(queryAddIcon);
    query.addBindValue(iconUri);
    query.exec();
    qDebug() << query.executedQuery();
    
    // Add entry.
    QString queryAddEntry =
        "INSERT INTO CA_ENTRY " \
            "(EN_TEXT, EN_DESCRIPTION, EN_ROLE, EN_TYPE_NAME, EN_ICON_ID) " \
        "VALUES " \
            "(?, ?, 1, 'widget', last_insert_rowid())";

    query.prepare(queryAddEntry);    
    query.addBindValue(title);
    query.addBindValue(description);
    query.exec();
    qDebug() << query.executedQuery();
    
    // Get last id 
    QString queryLastId = "SELECT last_insert_rowid() AS LAST_ID";
    query.prepare(queryLastId);
    query.exec();
    query.next();
    int lastId = query.value(query.record().indexOf("LAST_ID")).toInt();
    
    // Add attribute packageuid
    QString queryAddAttribute1 =
        "INSERT INTO CA_ATTRIBUTE " \
            "(AT_ENTRY_ID, AT_NAME, AT_VALUE) " \
        "VALUES " \
            "(?, 'packageuid', ?)";

    query.prepare(queryAddAttribute1);
    query.addBindValue(lastId);
    query.addBindValue(hexUid);
    query.exec();
    qDebug() << query.executedQuery();
    
    // Add attribute widget uri
    QString queryAddAttribute2 =
        "INSERT INTO CA_ATTRIBUTE " \
            "(AT_ENTRY_ID, AT_NAME, AT_VALUE) " \
        "VALUES " \
            "(?, 'widget:uri', ?)";

    query.prepare(queryAddAttribute2);
    query.addBindValue(lastId);
    query.addBindValue(uri);
    query.exec();
    qDebug() << query.executedQuery();
    
    // Add attribute widget library
    QString queryAddAttribute3 =
        "INSERT INTO CA_ATTRIBUTE " \
            "(AT_ENTRY_ID, AT_NAME, AT_VALUE) " \
        "VALUES " \
            "(?, 'widget:library', ?)";

    query.prepare(queryAddAttribute3);
    query.addBindValue(lastId);
    query.addBindValue(library);
    query.exec();
    qDebug() << query.executedQuery();
}

/*!
 Returns true if a widget with the given uid exists in the database.
 */
bool CaClientProxy::hsWidgetExists(int uid)
{
    bool exists(false);
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    QString hexUid;    
    hexUid.setNum(uid,16);
    
    QString queryString =
        "SELECT " \
            "AT_ENTRY_ID " \
        "FROM " \
            "CA_ATTRIBUTE " \
        "WHERE " \
            "AT_VALUE LIKE ?";
    
    query.prepare(queryString);
    query.addBindValue(hexUid);
    
    if (query.exec() && query.next()) {
        // Query returned a non empty result.
        exists = true;
    } else {
        // The widget with the given uid was not found.
        exists = false;
    }
    
    qDebug() << query.executedQuery();
    
    return exists;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::addData(const CaEntry &entryToAdd,
    CaEntry &targetEntry)
{
    qDebug() << "CaClientProxy::addData" << "entry id: "
        << entryToAdd.id();

    targetEntry = entryToAdd;
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    if (entryToAdd.id() == 0) {
        CaObjectAdapter::setId(targetEntry, 0);
    }
    query.exec("begin");
    bool success = (setIconInDb(&targetEntry)
        && setEntryInDb(&targetEntry)
        && setAttributesInDb(&targetEntry));
    if (success) {
        query.exec("commit");
        QList<int> parentIds;
        GetParentsIds(QList<int>() << targetEntry.id(), parentIds);
        if (entryToAdd.id() == 0) {
            CaNotifiers::Notify(targetEntry, AddChangeType, parentIds);
        } else {
            CaNotifiers::Notify(targetEntry, UpdateChangeType, parentIds);
        }
        return NoErrorCode;
    } else {
        query.exec("rollback");
        return UnknownErrorCode;
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::removeData(const QList<int> &entryIdList)
{
    qDebug() << "CaClientProxy::removeData" << "entryIdList: "
        << entryIdList;

    QList<CaEntry*> entryList;
    getData(entryIdList, entryList);
    QList<QList<int> > parentsIds;
    foreach (CaEntry *entry, entryList) {
        QList<int> parentIds;
        GetParentsIds(QList<int>() << entry->id(), parentIds);
        parentsIds.append(parentIds);
    }

    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    //begin transaction
    bool success(false);
    query.exec("begin");
    foreach(int entryId, entryIdList) {
        query.prepare(
            "SELECT ENTRY_ID FROM CA_ENTRY WHERE ENTRY_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success && query.next()) {
            success
                = query.value(query.record().indexOf("ENTRY_ID")).toInt()
                    > 0;
        }
        if (!success) {
            break;
        }

        query.prepare(
            "SELECT EN_ICON_ID FROM CA_ENTRY WHERE ENTRY_ID = ?");
        query.addBindValue(entryId);
        int iconId(0);
        success = query.exec();
        if (success && query.next()){
            iconId
                = query.value(query.record().indexOf("EN_ICON_ID")).toInt();
        } else {
            break;
        }

        query.prepare("DELETE FROM CA_LAUNCH WHERE LA_ENTRY_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success) {
            qDebug() << query.lastQuery() << " rows deleted: "
                << query.numRowsAffected();
        } else {
            break;
        }

        query.prepare(
            "DELETE FROM CA_GROUP_ENTRY WHERE GE_ENTRY_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success) {
            qDebug() << query.lastQuery() << " rows deleted: "
                << query.numRowsAffected();
        } else {
            break;
        }

        query.prepare(
            "DELETE FROM CA_GROUP_ENTRY WHERE GE_GROUP_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success) {
            qDebug() << query.lastQuery() << " rows deleted: "
                << query.numRowsAffected();
        } else {
            break;
        }

        query.prepare("DELETE FROM CA_ATTRIBUTE WHERE AT_ENTRY_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success) {
            qDebug() << query.lastQuery() << " rows deleted: "
                << query.numRowsAffected();
        } else {
            break;
        }

        query.prepare("DELETE FROM CA_ENTRY WHERE ENTRY_ID = ?");
        query.addBindValue(entryId);
        success = query.exec();
        if (success) {
            qDebug() << query.lastQuery() << " rows deleted: "
                << query.numRowsAffected();
        } else {
            break;
        }

        if (iconId != 0) {
            query.prepare("DELETE FROM CA_ICON WHERE ICON_ID = ?");
            query.addBindValue(iconId);
            success = query.exec();
            if (success) {
                qDebug() << query.lastQuery() << " rows deleted: "
                    << query.numRowsAffected();
            } else {
                // ignore, this means that the icon cannot be removed
                // because some other entry has the same icon.
                success = true;
            }
        }
    }

    ErrorCode error(NoErrorCode);
    if (success) {
        query.exec("commit");
        if (parentsIds.count() == entryList.count()) {
            for (int i = 0; i < entryList.count(); i++) {
                GetParentsIds(QList<int>() << entryList[i]->id(), parentsIds[i]);
                CaNotifiers::Notify(*entryList[i], RemoveChangeType, parentsIds[i]);
            }
        }
    } else {
        query.exec("rollback");
        error = UnknownErrorCode;
    }

    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::insertEntriesIntoGroup(int groupId,
    const QList<int> &entryIdList, int beforeEntryId)
{
    qDebug() << "CaClientProxy::insertEntriesIntoGroup" << "groupId: "
        << groupId << "beforeEntryId: " << beforeEntryId << "entryIdList: "
        << entryIdList;

    removeEntriesFromGroup(groupId, entryIdList, false);
    QString queryText;
    if (beforeEntryId == AfterTheLastEntry) {
        queryText = QString("INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID,GE_ENTRY_ID,GE_POSITION) "
            "VALUES ( ?, ?,(SELECT MAX(DATA) FROM ( SELECT MAX(GE_POSITION)+ 1 AS DATA FROM CA_GROUP_ENTRY "
            "WHERE GE_GROUP_ID = %1 UNION SELECT 1 AS DATA FROM CA_GROUP_ENTRY ) ) )").arg(groupId);
    } else if (beforeEntryId == BeforeTheFirstEntry) {
        queryText = "INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID,GE_ENTRY_ID,GE_POSITION) VALUES ( ?, ?, 0 ) ";
    } else {
        queryText = QString("INSERT INTO CA_GROUP_ENTRY (GE_GROUP_ID,GE_ENTRY_ID,GE_POSITION) VALUES ( "
            "?, ?, ( SELECT GE_POSITION FROM CA_GROUP_ENTRY WHERE GE_ENTRY_ID = %1 ) ) ").arg(beforeEntryId);
    }

    bool success = true;
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    query.exec("begin");
    for (int i = 0; i < entryIdList.count() && success; ++i) {
        query.prepare(queryText);
        query.addBindValue(groupId);
        query.addBindValue(entryIdList.at(i));
        success = query.exec();
    }

    ErrorCode error(NoErrorCode);
    if (success) {
        query.exec("commit");
        CaNotifiers::Notify(groupId);
    } else {
        query.exec("rollback");
        error = UnknownErrorCode;
    }
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::removeEntriesFromGroup(int groupId,
    const QList<int> &entryIdList,
    bool calledDirectly)
{
    qDebug() << "CaClientProxy::removeEntriesFromGroup" << "groupId: "
        << groupId << "entryIdList: " << entryIdList;

    bool success = true;
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    query.exec("begin");
    for (int i = 0; i < entryIdList.count() && success; ++i) {
        query.prepare(
            "DELETE FROM CA_GROUP_ENTRY WHERE GE_ENTRY_ID = ? AND GE_GROUP_ID = ? ");
        query.addBindValue(entryIdList.at(i));
        query.addBindValue(groupId);
        success = query.exec();
    }

    ErrorCode error(NoErrorCode);
    if (success) {
        query.exec("commit");
        if (calledDirectly) {
            CaNotifiers::Notify(groupId);
        }
    } else {
        query.exec("rollback");
        error = UnknownErrorCode;
    }
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::getData(const QList<int> &entryIdList,
    QList<CaEntry*> &sourceList)
{
    qDebug() << "CaClientProxy::getData" << "entryIdList: "
        << entryIdList;

    QSqlDatabase db = dbConnection();

    bool success(true);
    foreach (int i, entryIdList) {
        QSqlQuery query(db);
        query.prepare(
            "SELECT ENTRY_ID, EN_TEXT, EN_DESCRIPTION, EN_TYPE_NAME, EN_FLAGS, EN_ROLE, EN_UID,  \
                  ICON_ID, IC_BITMAP_ID, IC_MASK_ID, IC_SKINMAJOR_ID, IC_SKINMINOR_ID, IC_FILENAME \
                  FROM CA_ENTRY LEFT JOIN CA_ICON ON EN_ICON_ID = ICON_ID WHERE ENTRY_ID = ?");
        query.addBindValue(i);

        success = query.exec();
        if (success && query.next()) {
            qDebug() << query.executedQuery();
            int role =
                query.value(query.record().indexOf("EN_ROLE")).toInt();
            CaEntry* entry = new CaEntry((EntryRole) role);
            CaObjectAdapter::setId(*entry,
                query.value(query.record().indexOf("ENTRY_ID")).toInt());
            entry->setText(query.value(
                query.record().indexOf("EN_TEXT")).toString());
            entry->setDescription(query.value(
                query.record().indexOf("EN_DESCRIPTION")).toString());
            entry->setEntryTypeName(query.value(query.record().indexOf(
                "EN_TYPE_NAME")).toString());
            entry->setFlags(static_cast<EntryFlag> (query.value(
                query.record().indexOf("EN_FLAGS")).toUInt()));

            CaIconDescription icon;
            CaObjectAdapter::setId(icon,
                query.value(query.record().indexOf("ICON_ID")).toInt());
            icon.setBitmapId(query.value(query.record().indexOf(
                "IC_BITMAP_ID")).toInt());
            icon.setMaskId(query.value(query.record().indexOf(
                "IC_MASK_ID")).toInt());
            icon.setSkinMajorId(query.value(query.record().indexOf(
                "IC_SKINMAJOR_ID")).toInt());
            icon.setSkinMinorId(query.value(query.record().indexOf(
                "IC_SKINMINOR_ID")).toInt());
            icon.setFilename(query.value(query.record().indexOf(
                "IC_FILENAME")).toString());
            entry->setIconDescription(icon);

            // attributes
            // UID as attribute
            if (query.value(query.record().indexOf("EN_UID")).toString().length()
                > 0) {
                entry->setAttribute("application:uid", query.value(
                    query.record().indexOf("EN_UID")).toString());
            }

            // fetch from DB attributes
            QSqlQuery attributesQuery(db);
            attributesQuery.prepare(
                "SELECT AT_NAME, AT_VALUE FROM CA_ATTRIBUTE WHERE AT_ENTRY_ID  = ?");
            attributesQuery.addBindValue(i);
            success = attributesQuery.exec();
            if (success) {
                while (attributesQuery.next()) {
                    entry->setAttribute(
                        attributesQuery.value(0).toString(),
                        attributesQuery.value(1).toString());
                }
                sourceList << entry;
            }
        } else {
            break;
        }
    }

    ErrorCode error = NoErrorCode;
    if (!success) {
        error = UnknownErrorCode;
    } else if ((entryIdList.count() != sourceList.count())) {
        error = NotFoundErrorCode;
    }
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::getData(const CaQuery &query,
    QList<CaEntry*> &sourceList)
{
    QList<int> entryIdList;
    ErrorCode errorCode = getEntryIds(query, entryIdList);
    if (errorCode == NoErrorCode) {
        errorCode = getData(entryIdList, sourceList);
    }
    return errorCode;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::getEntryIds(const CaQuery &query,
    QList<int> &sourceIdList)
{
    qDebug() << "CaClientProxy::getEntryIds";

    QSqlDatabase db = dbConnection();

    bool success(true);
    QString whereStatement;
    if (query.flagsOn() != 0)
        whereStatement.append(" AND ").append(QString().setNum(
            query.flagsOn())).append(" & EN_FLAGS == ").append(
            QString().setNum(query.flagsOn()));
    if (query.flagsOff() != 0)
        whereStatement.append(" AND ").append(QString().setNum(
            query.flagsOff())).append(" & (~EN_FLAGS) == ").append(
            QString().setNum(query.flagsOff()));
    if (query.entryRoles() != 0)
        whereStatement.append(" AND ").append(QString().setNum(
            query.entryRoles())) .append(" | EN_ROLE == ").append(
            QString().setNum(query.entryRoles()));
    //TODO: by uid???

    if (query.entryTypeNames().count()) {
        whereStatement.append(" AND EN_TYPE_NAME IN (");
        for (int i = 0; i < query.entryTypeNames().count(); i++) {
            whereStatement.append("\'" + query.entryTypeNames()[i] + "\'");
            if (i < query.entryTypeNames().count() - 1)
                whereStatement.append(",");
        }
        whereStatement.append(") ");
    }

    QSqlQuery sqlquery(db);
    if (query.parentId() == 0) {
        // sort
        QString queryString("SELECT ENTRY_ID from CA_ENTRY where 1=1 ");
        queryString.append(whereStatement);
        modifyQueryForSortOrder(queryString, query, false);
        if (query.count() > 0)
            queryString.append(" LIMIT ").append(query.count());
        qDebug() << "CaServicePrivate::getEntryIds query text: "
            << queryString;
        success = sqlquery.prepare(queryString);
        success = sqlquery.exec();
        if (success) {
            while (sqlquery.next()) {
                sourceIdList << sqlquery.value(sqlquery.record().indexOf(
                    "ENTRY_ID")).toInt();
            }
        }
    }
    else {
        QString
            queryString(
                "SELECT ENTRY_ID FROM CA_ENTRY \
        LEFT JOIN CA_GROUP_ENTRY ON GE_ENTRY_ID = ENTRY_ID WHERE GE_GROUP_ID  = ? ");
        queryString.append(whereStatement);
        modifyQueryForSortOrder(queryString, query, true);
        if (query.count() > 0)
            queryString.append(" LIMIT ").append(query.count());
        qDebug() << "CaServicePrivate::getEntryIds query text: "
            << queryString;
        sqlquery.prepare(queryString);
        sqlquery.addBindValue(query.parentId());
        success = sqlquery.exec();
        if (success) {
            while (sqlquery.next()) {
                sourceIdList << sqlquery.value(sqlquery.record().indexOf(
                    "ENTRY_ID")).toInt();
            }
        }
    }
    ErrorCode error = UnknownErrorCode;
    if (success) {
            error = NoErrorCode;
    }
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::executeCommand(const CaEntry &entry,
    const QString &command)
{
    qDebug() << "CaClientProxy::executeCommand" << "entry id: "
        << entry.id() << "command: " << command;
    return touch(entry);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::touch(const CaEntry &entry)
{
    const int id = entry.id();

    qDebug() << "CaClientProxy::touch" << "id: " << id;

    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    query.exec("begin");

    query.prepare(
        "INSERT INTO CA_LAUNCH (LA_ENTRY_ID,LA_LAUNCH_TIME) VALUES ( ?,? )");
    query.addBindValue(id);
    query.addBindValue(QDateTime::currentDateTime().toTime_t());
    bool success = query.exec();

    if (success) {
        query.prepare(
            "UPDATE CA_ENTRY SET EN_FLAGS = EN_FLAGS | ? WHERE ENTRY_ID = ?");
        query.addBindValue((int) UsedEntryFlag);
        query.addBindValue(id);
        success = query.exec();
    }

    ErrorCode error = NoErrorCode;
    if (success) {
        query.exec("commit");
        QList<CaEntry *> entryList;
        if (getData(QList<int>() << id, entryList) == NoErrorCode) {
            QList<int> parentIds;
            GetParentsIds(QList<int>() << id, parentIds);
            CaNotifiers::Notify(*entryList[0], UpdateChangeType, parentIds);
        }
    } else {
        query.exec("rollback");
        error = UnknownErrorCode;
    }
    
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaClientProxy::customSort(const QList<int> &entryIdList,
        int groupId)
{
    bool success = true;
    QSqlDatabase db = dbConnection();
    QSqlQuery query(db);
    query.exec("begin");
    for (int i = 0; i < entryIdList.count(); i++) {
        int position = i+1;
        query.prepare(
            "UPDATE CA_GROUP_ENTRY SET GE_POSITION = ? WHERE GE_ENTRY_ID = ? AND GE_GROUP_ID = ?" );
        query.addBindValue(position);
        query.addBindValue(entryIdList.at(i));
        query.addBindValue(groupId);
        success = query.exec();
        if (!success) {
            break;
        }
    }

    ErrorCode error(NoErrorCode);
    if (success) {
        query.exec("commit");
        CaNotifiers::Notify(groupId);
    } else {
        query.exec("rollback");
        error = UnknownErrorCode;
    }
    return error;
}

/*!
 //TODO:
 */
void CaClientProxy::modifyQueryForSortOrder(QString& queryString,
    const CaQuery &query, bool parent) const
{
    SortAttribute sortAttribute;
    Qt::SortOrder sortOrder;
    query.getSort(sortAttribute, sortOrder);
    QString oldQueryString(queryString);
    queryString.clear();

    if (sortAttribute == NameSortAttribute) {
        queryString.append(oldQueryString).append(" ORDER BY EN_TEXT ");
    }
    else if (sortAttribute == CreatedTimestampSortAttribute) {
        queryString.append(oldQueryString).append(
            " ORDER BY EN_CREATION_TIME ");
    }
    else if (sortAttribute == MostUsedSortAttribute) {
        queryString.append("SELECT ENTRY_ID FROM (").append(oldQueryString).append(
            " \
                ) LEFT JOIN \
                (SELECT LA_ENTRY_ID, COUNT(*) AS USAGE_DATA FROM CA_LAUNCH GROUP BY LA_ENTRY_ID) \
                ON ENTRY_ID = LA_ENTRY_ID ORDER BY USAGE_DATA ");
    }
    else if (sortAttribute == LastUsedSortAttribute) {
        queryString.append("SELECT ENTRY_ID FROM (").append(oldQueryString).append(
            " \
                ) LEFT JOIN \
                (SELECT LA_ENTRY_ID, MAX(LA_LAUNCH_TIME) AS USAGE_DATA FROM CA_LAUNCH GROUP BY LA_ENTRY_ID) \
                ON ENTRY_ID = LA_ENTRY_ID ORDER BY USAGE_DATA ");
    }
    else if (parent && sortAttribute == DefaultSortAttribute) {
        queryString.append(oldQueryString).append(
            " ORDER BY GE_GROUP_ID, GE_POSITION ");
    }
    else if (!parent && sortAttribute == DefaultSortAttribute) {
        queryString.append(oldQueryString).append(" ORDER BY ENTRY_ID ");
    }
    else {
        queryString.append(oldQueryString);
    }

    if (sortAttribute == NameSortAttribute || sortAttribute
        == CreatedTimestampSortAttribute || sortAttribute
        == MostUsedSortAttribute || sortAttribute == LastUsedSortAttribute
        || (sortAttribute == DefaultSortAttribute && parent)) {
        if (sortOrder == Qt::AscendingOrder)
            queryString.append(" ASC ");
        else
            queryString.append(" DESC ");
    }

}

/*!
 //TODO:
 */
bool CaClientProxy::setIconInDb(CaEntry *entryClone) const
{
    //set icon information into db
    QSqlQuery query(dbConnection());
    query.prepare(
        "SELECT ICON_ID FROM CA_ICON WHERE IC_FILENAME = :IC_FILENAME \
            AND IC_BITMAP_ID = :IC_BITMAP_ID \
            AND IC_MASK_ID = :IC_MASK_ID \
            AND IC_SKINMAJOR_ID = :IC_SKINMAJOR_ID \
            AND IC_SKINMINOR_ID = :IC_SKINMINOR_ID");
    query.bindValue(":IC_FILENAME",
        entryClone->iconDescription().filename());
    query.bindValue(":IC_BITMAP_ID",
        entryClone->iconDescription().bitmapId());
    query.bindValue(":IC_MASK_ID", entryClone->iconDescription().maskId());
    query.bindValue(":IC_SKINMAJOR_ID",
        entryClone->iconDescription().skinMajorId());
    query.bindValue(":IC_SKINMINOR_ID",
        entryClone->iconDescription().skinMinorId());

    bool success = query.exec();
    if (success && query.next()) {
        qDebug() << "query.executedQuery() : " << query.executedQuery();
        int iconId = query.value(query.record().indexOf("ICON_ID")).toInt();
        qDebug() << "iconId = " << iconId;
        CaIconDescription iconDescription = entryClone->iconDescription();
        if (iconId <= 0 && (iconDescription.filename() != ""
            || iconDescription.bitmapId() != 0 || iconDescription.maskId() != 0
            || iconDescription.skinMajorId() != 0
            || iconDescription.skinMinorId() != 0)) {
            query.prepare(
                "INSERT INTO CA_ICON \
                           (IC_FILENAME,IC_BITMAP_ID,IC_MASK_ID,IC_SKINMAJOR_ID,IC_SKINMINOR_ID) \
                            VALUES ( ? , ? , ? , ? , ? )");
            query.addBindValue(iconDescription.filename());
            query.addBindValue(iconDescription.bitmapId());
            query.addBindValue(iconDescription.maskId());
            query.addBindValue(iconDescription.skinMajorId());
            query.addBindValue(iconDescription.skinMinorId());
            success = query.exec();
            qDebug() << query.executedQuery();
            iconId = query.lastInsertId().toInt();
        }
        CaObjectAdapter::setId(iconDescription, iconId);
        entryClone->setIconDescription(iconDescription);
    }
    return success;
}

/*!
 //TODO:
 */
bool CaClientProxy::setEntryInDb(CaEntry *entryClone) const
{
    QSqlQuery query(dbConnection());
    bool isNewEntry(entryClone->id() <= 0);
    QString
        queryText(
            "INSERT INTO CA_ENTRY \
        (EN_TEXT,EN_ROLE,EN_TYPE_NAME,EN_FLAGS,EN_ICON_ID ) VALUES ( ?, ?, ?, ?, ");
    if (!isNewEntry)
        queryText
            = "REPLACE INTO CA_ENTRY \
        (ENTRY_ID,EN_TEXT,EN_ROLE,EN_TYPE_NAME,EN_FLAGS,EN_ICON_ID ) VALUES ( ?, ?, ?, ?, ?, ";
    if (entryClone->iconDescription().id() > 0) {
        queryText.append("?");
    }
    else {
        queryText.append("NULL");
    }
    queryText.append(")");
    query.prepare(queryText);
    if (!isNewEntry)
        query.addBindValue(entryClone->id());
    query.addBindValue(entryClone->text());
    query.addBindValue((int) entryClone->role());
    query.addBindValue(entryClone->entryTypeName());
    query.addBindValue((int) entryClone->flags());
    if (entryClone->iconDescription().id() > 0) {
        query.addBindValue(entryClone->iconDescription().id());
    }

    bool success = query.exec();
    if (success) {
        qDebug() << query.executedQuery();
        int newEntryId(0);
        //set entry creation time if new entry
        if (isNewEntry) {
            newEntryId = query.lastInsertId().toInt();

            CaObjectAdapter::setId(*entryClone, newEntryId);
            uint timestamp = QDateTime::currentDateTime().toTime_t();
            query.prepare(
                "UPDATE CA_ENTRY SET EN_CREATION_TIME = ? WHERE ENTRY_ID = ?");
            query.addBindValue(timestamp);
            query.addBindValue(newEntryId);
            success = query.exec();
            qDebug() << "CaServicePrivate::setEntryInDb"
                << query.executedQuery();
        }
    }
    return success;
}

/*!
 //TODO:
 */
bool CaClientProxy::setAttributesInDb(CaEntry *entryClone) const
{
    bool success = true;
    QSqlQuery query(dbConnection());
    if (entryClone->attributes().count() > 0) {
        QMap<QString, QString> attributesMap = entryClone->attributes();
        foreach (QString key, attributesMap.keys()) {
                query.prepare(
                    "REPLACE INTO CA_ATTRIBUTE (AT_ENTRY_ID,AT_NAME,AT_VALUE) VALUES ( \
                :AT_ENTRY_ID,\
                :AT_NAME,\
                :AT_VALUE )");
                query.bindValue(":AT_ENTRY_ID", entryClone->id());
                query.bindValue(":AT_NAME", key);
                query.bindValue(":AT_VALUE", attributesMap.value(key));
                success = query.exec();
                if (!success) {
                    break;
                }
                qDebug() << "CaServicePrivate::setAttributesInDb"
                    << query.boundValues();
            }
    }
     return success;
}


void CaClientProxy::CreateGetParentsIdsQuery(
    const QList<int> &entryIds,
    const QList<int> &parentIds,
    QString &query)
{
    QString entryIdList;
    int lastItemIndex = entryIds.count()-1;
    for (int i = 0; i < lastItemIndex; i++) {
        entryIdList.append(QString::number(entryIds[i]));
        entryIdList.append(",");
    }
    if (lastItemIndex >= 0) {
        entryIdList.append(QString::number(entryIds[lastItemIndex]));
    }
    query = QString("SELECT GE_GROUP_ID FROM CA_GROUP_ENTRY "
        "WHERE GE_ENTRY_ID IN ( %1 )").arg(entryIdList);

    int lastParentIndex = parentIds.count()-1;
    if (lastParentIndex >= 0) {
        QString parentIdList;
        for (int i = 0; i < lastParentIndex; i++) {
            parentIdList.append(QString::number(parentIds[i]));
            parentIdList.append(",");
        }
        parentIdList.append(QString::number(parentIds[lastParentIndex]));
        query.append(QString(" AND GE_GROUP_ID NOT IN( %1 )").arg(parentIdList));
    }
}


bool CaClientProxy::GetParentsIds( const QList<int> &entryIds,
    QList<int> &parentIds)
{
    QString getParentIdsQuery;
    CreateGetParentsIdsQuery(entryIds, parentIds, getParentIdsQuery);
    QSqlQuery query(dbConnection());
    bool success = query.exec(getParentIdsQuery);

    if (success && query.next()) {
        QList<int> newParentIds;
        do {
            newParentIds << query.value(query.record().indexOf("GE_GROUP_ID")).toInt();
        } while (query.next());
        parentIds.append(newParentIds);
        GetParentsIds(newParentIds, parentIds);
    }
    return success;
}
