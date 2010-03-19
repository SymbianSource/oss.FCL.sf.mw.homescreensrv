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
* Description:  HsRuntime factory private implementation..
*
*/


#ifndef HSRUNTIMEFACTORY_P_H
#define HSRUNTIMEFACTORY_P_H

#include "hsiruntimeprovider.h"

class HsRuntimeFactory;
class IHsRuntimeProvider;

class HsRuntimeFactoryPrivate : public QObject
{
    Q_OBJECT
    
public:
    HsRuntimeFactoryPrivate(HsRuntimeFactory* aPublic);
    ~HsRuntimeFactoryPrivate();
    
    QList<HsRuntimeToken> runtimes();
    HsRuntime* createRuntime(const HsRuntimeToken& aToken);
    IHsRuntimeProvider* loadProviderFromPlugin(const QString& aPluginName);


private:
    Q_DISABLE_COPY(HsRuntimeFactoryPrivate)

public:
    HsRuntimeFactory* mQ;
    QString mPluginManifestDirectory;
    QString mPluginDirectory;
};

#endif
