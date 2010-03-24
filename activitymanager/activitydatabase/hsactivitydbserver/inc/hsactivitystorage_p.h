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
#ifndef HSACTIVITYSTORAGEPRIVATE_H
#define HSACTIVITYSTORAGEPRIVATE_H
#include "hsactivitydbclientinterface.h"
#include <QSqlDatabase>

/**
 * Class implements HsActivityDbClientInterface and storage functionality using
 * database subsystem 
 */
class HsActivityStoragePrivate: public HsActivityDbClientInterface
{
public:
    /**
     * Constructor
     */
    HsActivityStoragePrivate();
    
    /**
     * Destructor
     */
    ~HsActivityStoragePrivate();
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::addActivity(const QVariantHash&)
     */
    int addActivity(const QVariantHash &);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash&)
     */
    int updateActivity(const QVariantHash &);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::removeActivity(const QVariantHash &)
     */
    int removeActivity(const QVariantHash &activity);

   /**
    * Interface implementation.
    * @see int HsActivityDbClientInterface::removeApplicationActivities(const QVariantHash &)
    */
    int removeApplicationActivities(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::requestedActivityName( QString &, const QVariantHash &)
     */
    int requestedActivityName(QString& result, 
                              const QVariantHash &activity);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::activities(QList<QVariantHash> &)
     */
    int activities(QList<QVariantHash> &);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::applicationActivities(QList<QVariantHash> &, const QVariantHash &)
     */
    int applicationActivities(QList<QVariantHash> &, const QVariantHash &);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::waitActivity(const QVariantHash &)
     */
    int waitActivity(const QVariantHash &activity);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::launchActivity(const QVariantHash &)
     */
    int launchActivity(const QVariantHash &activity);
private:
    /**
     * Function check database structure
     * @return true if structure is valid, false otherwise
     */
    bool checkTables();
    
    /**
     * Function create database structure
     */
    void recreateTables();
    
    /**
     * Function return last SQL query error code
     * @param querty - executed sql query
     * @return SQL query error code  
     */
    int getSqlErrorCode(const QSqlQuery& query);
    
    /**
     * Function exeute SQL query
     * @param query - SQL 
     * @return true on succees, false otherwise
     */
    bool exec(QSqlQuery& query);
    
    /**
     * Function exeute SQL query
     * @param query - SQL query that has to be executed
     * @return true on succees, false otherwise
     */
    bool exec(const QString &query, const QVariantHash& params);
    
    /**
     * Funciton bind provided filtering rules with prepared SQL query
     * @param query - SQL query 
     * @param activity - filtering rules
     * @param additionalData - additional filetering rules
     */
    void bind( QSqlQuery& query, 
               const QVariantHash &activity, 
               const QVariantHash &additionalData = QVariantHash());
    
    /**
     * Funciton get records that match conditions
     * @param result - output destination 
     * @param query - SQL query sting
     * @param conditions - query conditions
     */
    int activities(QList<QVariantHash> &results, 
                   const QString &query,
                   const QVariantHash &conditions);

private:
    /**
     * Connectioin to SQL engine
     */
    QSqlDatabase mConn;
};
#endif //HSACTIVITYSTORAGEPRIVATE_H
