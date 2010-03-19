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
* Description:  Base class for all bagets.
*
*/

#ifndef BAGET_H
#define BAGET_H

#include <QGraphicsWidget>

#include "bagetmodel_global.h"

enum BagetState
{
    BagetStateConstructed = 0,
    BagetStateRunning,
    BagetStateSuspended,
    BagetStateStopped
};

class BagetPrivate;

class BAGETMODEL_EXPORT Baget : public QGraphicsWidget
{
    Q_OBJECT

public:

    Baget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~Baget();

    BagetState currentState();

public slots:

    void start();
    void stop();
    void resume();
    void suspend();

signals:

    void faulted();

protected:

    virtual bool onStart() = 0;
    virtual bool onStop() = 0;
    virtual bool onResume();
    virtual bool onSuspend();

private:

    Q_DISABLE_COPY(Baget)

private:

    BagetPrivate * const m_d;
    friend class BagetPrivate;

};

#endif // BAGET_H
