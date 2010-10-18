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
 * Description: cahandlerspreloader.h
 *
 */

#ifndef CAHANDLERSPRELOADER_H
#define CAHANDLERSPRELOADER_H

#include <QObject>
#include <QStringList>

class QTimer;
class CaHandlerProxy;

class CaHandlersPreloader: public QObject
{    
    Q_OBJECT
    
public:
    
    CaHandlersPreloader(
        CaHandlerProxy *handlerProxy,
        QObject *parent = 0);
    ~CaHandlersPreloader();
    
private slots:
    void preloadHandlersWhenIdle();
    
private:
    QTimer *mTimer; // Own.
    CaHandlerProxy *mHandlerProxy; // Not own.
    QStringList mHandlersToLoad;
};

#endif // CAHANDLERSPRELOADER_H
