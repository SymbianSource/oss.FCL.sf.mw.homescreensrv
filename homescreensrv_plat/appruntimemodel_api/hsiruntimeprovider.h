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
* Description:  HsRuntime provider interface.
*
*/


#ifndef HSIRUNTIMEPROVIDER_H
#define HSIRUNTIMEPROVIDER_H

#include <QtPlugin>
#include <QList>
#include <QString>

/*!
    \class HsRuntimeToken
    \ingroup group_hsruntimemodel
    \brief Represents a home screen runtime that can be created. 

    Home screen runtimes are located in runtime providers. Each provider
    exposes its contained runtimes as runtime tokens. A token has all
    the needed information for runtime selection and creation
        
*/
class HsRuntimeToken
{

public:

/*!
    The name of the library/plugin that contains the runtime.
*/
    QString mLibrary;

/*!
    Uniquely identifies the runtime.
*/
    QString mUri;
    
};

class HsRuntime;

/*!
    \class IHsRuntimeProvider
    \ingroup group_hsruntimemodel
    \brief Defines a mechanism for retrieving a home screen runtime.
    Interface that defines a mechanism for retrieving a home screen
    runtime. Each home screen runtime provider implements this interface. 
        
*/
class IHsRuntimeProvider
{

public:

/*!
    Destructor.    
*/
    virtual ~IHsRuntimeProvider() {}

public:

/*!
    Returns contained runtimes as a list of runtime tokens.    
*/
    virtual QList<HsRuntimeToken> runtimes() = 0;

/*!
    Creates and returns a runtime based on the given token.
    
    \a aToken Identifies the runtime to be created.

    Returns The created runtime or null in failure cases.
*/
    virtual HsRuntime* createRuntime(const HsRuntimeToken& aToken) = 0;

};

Q_DECLARE_INTERFACE(IHsRuntimeProvider, "com.nokia.homescreen.iruntimeprovider/1.0")

#endif
