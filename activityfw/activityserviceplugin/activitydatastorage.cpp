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
#include "activitydatastorage.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>

const char KConnectionName[] = "ActivityDataStorage";

ActivityDataStorage::ActivityDataStorage()
{
    QSqlDatabase database;
    if (QSqlDatabase::contains(KConnectionName)) {
        database = QSqlDatabase::database(KConnectionName);
    } else {   
        database = QSqlDatabase::addDatabase("QSQLITE", KConnectionName);
        database.setDatabaseName("activitydatastorage.db");    
        if (!database.open()) {
            qFatal(qPrintable(database.lastError().text()));
        }
    }

    if (!checkTables()) {
        recreateTables();
    }
}

ActivityDataStorage::~ActivityDataStorage()
{
    QSqlDatabase::database(KConnectionName).close();
}

bool ActivityDataStorage::addActivity(const QString &activityId, const QVariant &data)
{
    QSqlDatabase database = QSqlDatabase::database(KConnectionName);
    
    QByteArray streamedData;
    {
        QDataStream stream(&streamedData, QIODevice::WriteOnly);
        stream << data;
    }    
    
    // insert data
    QSqlQuery query(database);
    if (!query.prepare("INSERT INTO Activities(Name, Data) VALUES(:Name, :Data)")) {
        qCritical(qPrintable(query.lastError().text()));
        return false;
    }
    query.bindValue(":Name", activityId);
    query.bindValue(":Data", streamedData);
    if (!query.exec()) {
        qCritical(qPrintable(query.lastError().text()));
        return false;
    }        
    
    return true;
}

bool ActivityDataStorage::removeActivity(const QString &activityId)
{
    QString sqlCommand = QString("DELETE FROM Activities WHERE Name = '%1'").arg(activityId);
    QSqlQuery query(QSqlDatabase::database(KConnectionName));
    if (!query.exec(sqlCommand)) {
        qCritical(qPrintable(query.lastError().text()));
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool ActivityDataStorage::updateActivity(const QString &activityId, const QVariant &data)
{
    QSqlDatabase database = QSqlDatabase::database(KConnectionName);
    
    QByteArray streamedData;
    {
        QDataStream stream(&streamedData, QIODevice::WriteOnly);
        stream << data;
    }    
    
    // update data
    QSqlQuery query(database);
    if (!query.prepare("UPDATE Activities SET Data = :Data WHERE Name = :Name")) {
        qCritical(qPrintable(query.lastError().text()));
        return false;
    }
    query.bindValue(":Data", streamedData);
    query.bindValue(":Name", activityId);
    if (!query.exec()) {
        qCritical(qPrintable(query.lastError().text()));
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

QVariant ActivityDataStorage::activityData(const QString &activityId) const
{
    QSqlDatabase database = QSqlDatabase::database(KConnectionName);
    QSqlQuery query(database);
    if (!query.exec(QString("SELECT Data FROM Activities WHERE Name = '%1'").arg(activityId))) {
        qCritical(qPrintable(query.lastError().text()));
        return QVariant();
    }
    
    QVariant result;
    if (query.next()) {
        QByteArray data(query.value(0).toByteArray());
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> result;
    }   
    return result;
}

bool ActivityDataStorage::checkTables()
{
    QStringList expectedTables("Activities");
    QStringList actualTables = QSqlDatabase::database(KConnectionName).tables();
    return (expectedTables == actualTables);    
}

void ActivityDataStorage::recreateTables()
{
    QSqlDatabase database = QSqlDatabase::database(KConnectionName);
    
    if (!database.transaction()) {
        qFatal(qPrintable(database.lastError().text()));
    }
    
    // drop any existing tables
    {
        QSqlQuery dropQuery(database);
        foreach(const QString &tableName, database.tables()) {
            if (!dropQuery.exec(QString("DROP TABLE %1").arg(tableName))) {
                qFatal(qPrintable(dropQuery.lastError().text()));
            }
        }
    }

    // create new table
    {
        QSqlQuery createQuery(database);
        QString statement(
            "CREATE TABLE Activities("
            "Name TEXT NOT NULL PRIMARY KEY UNIQUE,"
            "Data BLOB NOT NULL)");    
        
        if (!createQuery.exec(statement)) {
            qFatal(qPrintable(createQuery.lastError().text()));
        }
    }
    
    if (!database.commit()) {
        qFatal(qPrintable(database.lastError().text()));
    }    
}
