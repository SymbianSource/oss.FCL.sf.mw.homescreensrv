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
* Description: Example of home screen widget
*
*/

#ifndef HELLOWORLDWIDGETPROVIDER_H
#define HELLOWORLDWIDGETPROVIDER_H


#include <hsiwidgetprovider.h>

class HsWidget;

class HelloWorldWidgetProvider : public QObject, public IHsWidgetProvider
{
    Q_OBJECT
    Q_INTERFACES(IHsWidgetProvider)

public:
    HelloWorldWidgetProvider();
	virtual ~HelloWorldWidgetProvider();

public:
    QList<HsWidgetToken> widgets();
    HsWidget* createWidget(const HsWidgetToken& token);

private:
    HsWidgetToken mWidgetToken;
};

#endif // HELLOWORLDWIDGETPROVIDER_H
