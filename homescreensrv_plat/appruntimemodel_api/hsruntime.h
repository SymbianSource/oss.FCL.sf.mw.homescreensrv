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
* Description:  Base class for all homescreen runtimes.
*
*/


#ifndef HSRUNTIME_H
#define HSRUNTIME_H

#include <QObject>

#include "hsruntimemodel_global.h"

/*! 
    \class HsRuntime
    \ingroup group_hsruntimemodel
    \brief Base class for all home screen runtimes.

    Provides the common functionality for all home screen runtime
    implementations.
        
*/
class HSRUNTIMEMODEL_EXPORT HsRuntime : public QObject
{
    Q_OBJECT

public:
    HsRuntime(QObject* aParent = 0) : QObject(aParent) {}
    virtual ~HsRuntime() {}


signals:
/*!
    Emitted when the runtime has started.         
*/
    void started();

/*!
    Emitted when the runtime has stopped.         
*/
    void stopped();
    
/*!
    Emitted when the runtime has faulted.         
*/
    void faulted();


public slots:

/*!
    Starts the runtime.         
*/
    virtual void start() = 0;

/*!
    Stops the runtime.         
*/
    virtual void stop() = 0;



private:

    Q_DISABLE_COPY(HsRuntime)
};

#endif
