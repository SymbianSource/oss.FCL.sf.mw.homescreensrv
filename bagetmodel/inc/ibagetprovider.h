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
* Description:  Baget provider interface.
*
*/

#ifndef IBAGETPROVIDER_H
#define IBAGETPROVIDER_H

#include <QtPlugin>
#include <QVariant>

/*!
    \class BagetToken
    \brief Represents a Baget that can be created.

    Bagets are located in baget providers. Each provider
    exposes its contained Bagets as baget tokens. A token has all
    the needed information for Baget selection and creation.
*/
class BagetToken
{

public:

    /*!
        The name of the library/plugin that contains the Baget.
    */
    QString mLibrary;

    /*!
        Uniquely identifies the Baget.
    */
    QString mUri;

};

class Baget;

/*!
    \class IBagetProvider
    \brief Defines a mechanism for retrieving a Baget.

    Interface that defines a mechanism for retrieving a Baget. 
    Each Baget provider implements this interface. 
*/
class IBagetProvider
{

public:

    /*!
        Destructor.
    */
    virtual ~IBagetProvider() {}

public:

    /*!
        Returns contained Bagets as a list of baget tokens.
        \return Contained Bagets as tokens.
    */
    virtual QList<BagetToken> bagets() = 0;

    /*!
        Creates and returns a Baget based on the given token.
        \param token Identifies the Baget to be created.
        \return The created Baget or null in failure cases.
    */
    virtual Baget *createBaget(const BagetToken &token) = 0;

};

Q_DECLARE_METATYPE(BagetToken)
Q_DECLARE_INTERFACE(IBagetProvider, "com.nokia.baget.ibagetprovider/1.0")

#endif // IBAGETPROVIDER_H
