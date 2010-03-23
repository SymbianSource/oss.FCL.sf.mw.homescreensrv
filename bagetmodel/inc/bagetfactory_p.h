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
* Description:  Baget factory's private implementation.
*
*/

#ifndef BAGETFACTORY_P_H
#define BAGETFACTORY_P_H

class BagetFactory;

class BagetFactoryPrivate
{

public:

    BagetFactoryPrivate(const QString& pluginDirectory, 
                        BagetFactory* bagetFactoryPublic);
    ~BagetFactoryPrivate();

    Baget *createBaget(const BagetToken &token);

private:

    Q_DISABLE_COPY(BagetFactoryPrivate)

public:

    BagetFactory *m_q;
    QString mPluginDirectory;

};

#endif // BAGETFACTORY_P_H
