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
#ifndef AFSTORAGEENTRY_H
#define AFSTORAGEENTRY_H

#include <qvariant.h>
#include <qsharedpointer.h>

class CAfEntry;

#ifdef ACTIVITY_LIB
    #define ACTIVITY_EXPORT Q_DECL_EXPORT
#else
    #define ACTIVITY_EXPORT Q_DECL_IMPORT
#endif

class ACTIVITY_EXPORT AfStorageEntry
{
public:
    AfStorageEntry(QSharedPointer<CAfEntry> data = QSharedPointer<CAfEntry>(0));

    AfStorageEntry(const AfStorageEntry &templateEntry);
    
    AfStorageEntry(int appId, 
                   const QString &actId = QString(), 
                   const QVariant &privateData = QVariant(), 
                   const QVariantHash& publicData = QVariantHash());

    const CAfEntry* data() const;

    int flags() const;

    int applicationId() const;

    QString activityId() const;

    QString imageSrc() const;
    
    QVariantHash publicData() const;
    
    QVariant privateData() const;
    
private:
    QSharedPointer<CAfEntry> mData;

};

#endif //AFSTORAGEENTRY_H
