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
#include "activitydatabase_p.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSettings>

#include <hsactivitydbclient.h>

const char KConnectionName[] = "ActivityDatabase";

ActivityDatabasePrivate::ActivityDatabasePrivate()
{
    // determine database localization
    QSettings settings("nokia", "activities");
    if (!settings.contains("activitydatabase")) {
        settings.setValue("activitydatabase", "C:\\activitydatabase.db");
    }
    QString databaseFile = settings.value("activitydatabase").toString();

    QSqlDatabase database;
    if (QSqlDatabase::contains(KConnectionName)) {
        database = QSqlDatabase::database(KConnectionName);
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE", KConnectionName);
        database.setDatabaseName(databaseFile);
        if (!database.open()) {
            qFatal(qPrintable(database.lastError().text()));
        }
    }

    if (!checkTables()) {
        recreateTables();
    }
}

ActivityDatabasePrivate::~ActivityDatabasePrivate()
{
    QSqlDatabase::database(KConnectionName).close();
}

void ActivityDatabasePrivate::saveActivity(const QString &applicationName, const QString &activityName, const QVariantHash &activityMetadata)
{
    if (applicationName.isEmpty() || activityName.isEmpty() || activityMetadata.value("screenshot").isNull()) {
        qWarning("Activity entry is invalid, aborting save");
        return;
    }

    QVariantHash activityData(activityMetadata);
    activityData.insert(ActivityApplicationKeyword, applicationName);
    activityData.insert(ActivityActivityKeyword, activityName);

    // stream whole entry to bytearray
    QByteArray streamedData;
    {
        QDataStream stream(&streamedData, QIODevice::WriteOnly);
        stream << activityData;
    }

    QSqlDatabase database = QSqlDatabase::database(KConnectionName);

    if (!database.transaction()) {
        qFatal(qPrintable(database.lastError().text()));
    }

    // update or insert data
    {
        QSqlQuery query(database);
        if (!query.exec(QString("SELECT ApplicationName FROM Activities WHERE ApplicationName = '%1' AND ActivityName = '%2'").arg(applicationName).arg(activityName))) {
            qFatal(qPrintable(query.lastError().text()));
        }

        if (query.next()) {
            if (!query.prepare("UPDATE Activities SET Data = :Data WHERE ApplicationName = :ApplicationName AND ActivityName = :ActivityName")) {
                qFatal(qPrintable(query.lastError().text()));
            }
            query.bindValue(":Data", streamedData);
            query.bindValue(":ApplicationName", applicationName);
            query.bindValue(":ActivityName", activityName);
            if (!query.exec()) {
                qFatal(qPrintable(query.lastError().text()));
            }
        } else {
            if (!query.prepare("INSERT INTO Activities(ApplicationName, ActivityName, Data) VALUES(:ApplicationName, :ActivityName, :Data)")) {
                qFatal(qPrintable(query.lastError().text()));
            }
            query.bindValue(":ApplicationName", applicationName);
            query.bindValue(":ActivityName", activityName);
            query.bindValue(":Data", streamedData);
            if (!query.exec()) {
                qFatal(qPrintable(query.lastError().text()));
            }
        }
    }

    if (!database.commit()) {
        qFatal(qPrintable(database.lastError().text()));
    }
}

void ActivityDatabasePrivate::deleteActivity(const QString &applicationName, const QString &activityName)
{
    QString sqlCommand = QString("DELETE FROM Activities WHERE ApplicationName = '%1' AND ActivityName = '%2'").arg(applicationName).arg(activityName);
    QSqlQuery query(QSqlDatabase::database(KConnectionName));
    if (!query.exec(sqlCommand)) {
        qFatal(qPrintable(query.lastError().text()));
    }
}

void ActivityDatabasePrivate::deleteApplicationActivities(const QString &applicationName)
{
    QString sqlCommand = QString("DELETE FROM Activities WHERE ApplicationName = '%1'").arg(applicationName);
    QSqlQuery query(QSqlDatabase::database(KConnectionName));
    if (!query.exec(sqlCommand)) {
        qFatal(qPrintable(query.lastError().text()));
    }
}

QList<QVariantHash> ActivityDatabasePrivate::allActivitiesList()
{
    return activitiesList("SELECT Data FROM Activities");
}

QList<QVariantHash> ActivityDatabasePrivate::applicationActivitiesList(const QString &applicationName)
{
    return activitiesList(QString("SELECT Data FROM Activities WHERE ApplicationName = '%1'").arg(applicationName));
}

QList<QVariantHash> ActivityDatabasePrivate::activitiesList(const QString &sqlCommand)
{
    QSqlQuery query(QSqlDatabase::database(KConnectionName));
    if (!query.exec(sqlCommand)) {
        qFatal(qPrintable(query.lastError().text()));
    }

    QList<QVariantHash> result;
    while (query.next()) {
        QVariantHash activityEntry;
        {
            QByteArray data(query.value(0).toByteArray());
            QDataStream stream(&data, QIODevice::ReadOnly);
            stream >> activityEntry;
        }
        result.append(activityEntry);
    }
    return result;
}

QString ActivityDatabasePrivate::requestedActivityName(const QString &applicationName)
{
    QSqlQuery query(QSqlDatabase::database(KConnectionName));
    if (!query.exec(QString("SELECT ActivityName FROM Activities WHERE ApplicationName = '%1' AND RequestFlag").arg(applicationName))) {
        qFatal(qPrintable(query.lastError().text()));
    }

    QString result;
    if (query.next()) {
        result = query.value(0).toString();
    }
    return result;
}

void ActivityDatabasePrivate::setActivityRequestFlag(const QString &applicationName, const QString &activityName)
{
    QSqlQuery query(QSqlDatabase::database(KConnectionName));

    // clear old requests for given application
    if (!query.exec(QString("UPDATE Activities SET RequestFlag=0 WHERE ApplicationName = '%1'").arg(applicationName))) {
        qFatal(qPrintable(query.lastError().text()));
    }

    // set new request
    if (!query.exec(QString("UPDATE Activities SET RequestFlag=1 WHERE ApplicationName = '%1' AND ActivityName = '%2'").arg(applicationName).arg(activityName))) {
        qFatal(qPrintable(query.lastError().text()));
    }
}

void ActivityDatabasePrivate::clearActivityRequestFlag(const QString &applicationName, const QString &activityName)
{
    QSqlQuery query(QSqlDatabase::database(KConnectionName));

    // just clear the given request
    if (!query.exec(QString("UPDATE Activities SET RequestFlag=0 WHERE ApplicationName = '%1' AND ActivityName = '%2'").arg(applicationName).arg(activityName))) {
        qFatal(qPrintable(query.lastError().text()));
    }
}

bool ActivityDatabasePrivate::checkTables()
{
    QStringList expectedTables("Activities");
    QStringList actualTables = QSqlDatabase::database(KConnectionName).tables();
    return (expectedTables == actualTables);
}

void ActivityDatabasePrivate::recreateTables()
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
            "ApplicationName TEXT NOT NULL,"
            "ActivityName TEXT NOT NULL,"
            "RequestFlag BOOL NOT NULL DEFAULT FALSE,"
            "Data BLOB NOT NULL,"
            "PRIMARY KEY(ActivityName, ApplicationName))");

        if (!createQuery.exec(statement)) {
            qFatal(qPrintable(createQuery.lastError().text()));
        }
    }

    if (!database.commit()) {
        qFatal(qPrintable(database.lastError().text()));
    }
}
