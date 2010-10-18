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
 * Description: canotifier.h
 *
 */

#ifndef CAUNINSTALLNOTIFIER_P_H
#define CAUNINSTALLNOTIFIER_P_H

#include <QObject>
#include <QVariantMap>
#include <QSharedPointer>
class CaUninstallObserver;
class CCaProgresScanner;
class CaUninstallNotifier;
class CaSoftwareRegistry;

class CaUninstallNotifierPrivate: public QObject
{
Q_OBJECT

public:

    explicit CaUninstallNotifierPrivate();

    virtual ~CaUninstallNotifierPrivate();
    
    void makeConnect();
    
    QVariantMap getLastNotification();
    
private slots:

    void progressChange(int componentId, int valueOfProgress);

private:
    CaUninstallObserver* mUninstallObserver;
    CCaProgresScanner* mProgressScanner;
    QSharedPointer<CaSoftwareRegistry> mCaSoftwareRegistry;
    
    CaUninstallNotifier *m_q;
    friend class CaUninstallNotifier;
    friend class T_CaSoftwareRegistry;

    int mLastComponentId;
    int mLastValueOfProgress;

};


#endif // CAUNINSTALLNOTIFIER_P_H
