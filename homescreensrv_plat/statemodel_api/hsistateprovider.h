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
* Description:  State provider interface.
*
*/


#ifndef HSISTATEPROVIDER_H
#define HSISTATEPROVIDER_H

#include <QtPlugin>
#include <QList>
#include <QString>

class QState;

/*!
    \class HsStateToken
    \ingroup group_hsstatemodel
    \brief Represents a home screen state that can be created.
    Home screen states are located in state providers. Each provider
    exposes its contained states as state tokens. A token has all
    the needed information for state selection and creation.
*/
class HsStateToken
{

public:

/*!
    The name of the library/plugin that contains the state.
 */
QString mLibrary;

/*!
    Uniquely identifies the state.
*/
    QString mUri;

};

/*!
    \class IHsStateProvider
    \ingroup group_hsstatemodel
    \brief Defines a mechanism for retrieving a home screen state.
    Interface that defines a mechanism for retrieving a home screen
    state. Each home screen state provider implements this interface.
*/
class IHsStateProvider
{

public:

/*!
    Destructor.    
*/
    virtual ~IHsStateProvider() {}

public:

/*!
    Returns contained states as a list of state tokens.    
*/
    virtual QList<HsStateToken> states() = 0;

/*!
    Creates and returns a state based on the given token.
    \a aToken Identifies the state to be created.

    Returns The created state or null in failure cases.
*/
    virtual QState* createState(const HsStateToken& aToken) = 0;

};

Q_DECLARE_INTERFACE(IHsStateProvider, "com.nokia.homescreen.istateprovider/1.0")

#endif
