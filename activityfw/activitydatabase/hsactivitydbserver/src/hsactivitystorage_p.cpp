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

#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSettings>
#include <QSqlDriver>
#include <QDebug>
#include "hsactivitystorage_p.h"
#include "hsactivity.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityStoragePrivate::HsActivityStoragePrivate()
{
    // determine database localization
    QSettings settings(ActivityOrganization, ActivityApplication);
    settings.setValue(ActivityStorageProperty, ActivityDefaultStorage);
    if (!settings.contains(ActivityStorageProperty)) {
        settings.setValue(ActivityStorageProperty, ActivityDefaultStorage);
    }
    QString databaseFile = settings.value(ActivityStorageProperty).toString();
    
    if (QSqlDatabase::contains(ActivityStorageName)) {
        mConn = QSqlDatabase::database(ActivityStorageName);
    } else {   
        mConn = QSqlDatabase::addDatabase(ActivityStorageDriver, ActivityStorageName);
        mConn.setDatabaseName(databaseFile);    
        if (!mConn.open()) {
            qWarning(qPrintable(mConn.lastError().text()));
            return;
        }
    }
    if (!checkTables()) {
        recreateTables();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityStoragePrivate::~HsActivityStoragePrivate()
{
    mConn.close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::addActivity(const QVariantHash &activity)
{
    // stream whole entry to bytearray
    QByteArray streamedData;
    QDataStream stream(&streamedData, QIODevice::WriteOnly);
    stream << activity;
    
    //insert data
    QSqlQuery query(mConn);
    query.prepare(ActivitySelectActivityQuery);
    bind(query, activity);
    exec(query);
    if(query.next()) {
        return KErrGeneral;
    }
    query.prepare(ActivityInsertActivityQuery);
    QVariantHash additionalData;
    additionalData.insert(ActivityDataKeyword, streamedData);
    bind(query, activity, additionalData);
    exec(query);
    return 0>=query.numRowsAffected() ? KErrGeneral : getSqlErrorCode(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::updateActivity(const QVariantHash &activity)
{
    // stream whole entry to bytearray
    QByteArray streamedData;
    QDataStream stream(&streamedData, QIODevice::WriteOnly);
    stream << activity;
    
    // update
    QSqlQuery query(mConn);
    query.prepare(ActivityUpdateActivityQuery);
    QVariantHash additionalData;
    additionalData.insert(ActivityDataKeyword, streamedData);
    bind(query, activity, additionalData);
    exec(query);
    return 0>=query.numRowsAffected() ? KErrGeneral : getSqlErrorCode(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::removeActivity(const QVariantHash &activity)
{
    return exec(ActivityDeleteActivityQuery, activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::removeApplicationActivities(const QVariantHash &activity)
{
    return exec(ActivityDeleteApplicationActivitiesQuery, activity);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::requestedActivityName(QString& result, 
                                                    const QVariantHash &activity)
{
    QSqlQuery query(mConn);
    query.prepare(ActivitySelectActiveQuery);
    bind(query, activity);
    int retVal(KErrNone);
    if (exec(query) && query.next()) {
        result = query.value(0).toString();
    } else {
        retVal = KErrNotFound;
    }
    return retVal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::activities(QList<QVariantHash>& result)
{
    return activities(result, ActivityActivitiesQuery, QVariantHash());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::applicationActivities(QList<QVariantHash> & result, 
                                                    const QVariantHash & condition)
{
    return activities(result, ActivityApplicationActivitiesQuery, condition);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::waitActivity(const QVariantHash &)
{
    return KErrNotSupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::launchActivity(const QVariantHash &)
{
    return KErrNotSupported;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool HsActivityStoragePrivate::checkTables()
{
    return (QStringList("Activities") == mConn.tables()); 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityStoragePrivate::recreateTables()
{
    //start sql transaction
    if (!mConn.transaction()) {
        qErrnoWarning(qPrintable(mConn.lastError().text()));
        return;
    }
    
    // drop any existing tables
    QSqlQuery query(mConn);
    foreach (const QString &tableName, mConn.tables()) {
        query.prepare(ActivityDropQuery);
        query.bindValue(ActivityTableKeyword, tableName);
        exec(query);
    }
    
    // create new table
    query.prepare(ActivityCreateQuery);
    exec(query);
    
    //finish sql transaction
    if (!mConn.commit()) {
        qErrnoWarning(qPrintable(mConn.lastError().text()));
    }  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::getSqlErrorCode(const QSqlQuery& query)
{
    const QSqlError err(query.lastError());
    const QString errStr(err.text());
    
    if (QSqlError ::NoError == err.type()) {
        return 0;
    } else {
        qErrnoWarning(qPrintable(errStr));
        return err.number();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool HsActivityStoragePrivate::exec(QSqlQuery& query)
{
    const bool retVal = query.exec();
    qErrnoWarning(qPrintable(query.lastQuery()));
    if (!retVal) {
        getSqlErrorCode(query);
    }
    return retVal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool HsActivityStoragePrivate::exec(const QString &queryStr, const QVariantHash& params)
{
    QSqlQuery query(mConn);
    query.prepare(queryStr);
    bind(query, params);
    query.exec();
    return getSqlErrorCode(query);
    
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityStoragePrivate::bind( QSqlQuery& query, 
                                   const QVariantHash &activity,
                                   const QVariantHash &additionalData)
{
    const QChar tag(' ');
    QString queryString( query.lastQuery() );
    QVariantHash::const_iterator iter;
    int offset(0);
    QStringList tokens;
    
    //explode SQL query to tokens 
    do {
        offset = queryString.indexOf(tag, 0);
        if (0 < offset) {
            tokens << queryString.left(offset++);
            queryString = queryString.right(queryString.length() - offset);
        } else {
            if (0 < queryString.length()) {
                tokens << queryString;
            }
            break;
        }
    } while (true);
    
    //current Sql driver doesnt support proper query formating.
    //reuest filtering data has to be binded in right order.
    QStringList::iterator token = tokens.begin();
    //iterate all tokens
    for (; token != tokens.end(); token = tokens.erase(token)) {
        //iterate all provided data and chcek if it match pattern
        for ( iter = activity.constBegin();
             iter != activity.constEnd(); 
             ++iter ) {
             if( (*token).contains(iter.key()) ){
                query.bindValue(iter.key(), iter.value());
                break;
            }
        }
        for (iter = additionalData.constBegin(); 
            iter != additionalData.constEnd(); 
            ++iter) {
            if ((*token).contains(iter.key())) {
                query.bindValue(iter.key(), iter.value());
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStoragePrivate::activities(QList<QVariantHash> &results, 
                                         const QString &queryStr,
                                         const QVariantHash &conditions)
{
    results.clear();
    
    QSqlQuery query(mConn);
    query.prepare(queryStr);
    bind(query, conditions);
    if (exec(query)) {
        QVariantHash activityEntry;
        while (query.next()) {
            activityEntry.clear();
            QByteArray data(query.value(0).toByteArray());
            QDataStream stream(&data, QIODevice::ReadOnly);
            stream >> activityEntry;
            results.append(activityEntry);
        }
    }
    return getSqlErrorCode(query);
}
