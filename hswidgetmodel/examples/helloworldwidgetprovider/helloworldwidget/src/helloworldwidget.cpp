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


#include <QGraphicsProxyWidget>
#include <QPushButton>
#include "helloworldwidget.h"

/*!


@page page_developing_home_screen_widget Creating Home Screen Widget

<b>Since the home screen widget concept is still open, these APIs may still be subject to changes and additions.</b>

Creating a home screen widget includes the following three steps:

<ul>
  <li> @ref page_developing_home_screen_widget </li>
  <li> @ref page_creating_widget_provider </li>
  <li> @ref page_deploying_widget_provider </li>
</ul>

Each home screen widget is inherited from HsWidget base class and HsWidget itself inherits HbWidget.
Usually you need to re-implement \c paint, \c boundingRect and \c shape functions from QGraphicsWidget
and HsWidget::onStart, HsWidget::onStop, HsWidget::onSuspend, HsWidget::onResume functions 
from HsWidget.

Instructions how to add widget to provider plugin can be found from @ref page_creating_widget_provider.

See @ref section_widget_state_machine for widget life cycle in detail.

The steps to create a widget are:
<ol>
<li> Declare a widget class that inherits from \c HsWidget.

<li> Implement constructor for the class where needed children are created.

<li> Implement destructor.

<li> Implement HsWidget::onStart() function. This is called by widget runtime to start the widget.

<li> Implement HsWidget::onStop() function. When called widget should stop all the processing started in \c onStart() function.

<li> Implement HsWidget::onSuspend() if needed.

<li> Implement HsWidget::onResume() if needed.

</ol>

Example of the home screen widget is HelloWorldWidget.


@section section_widget_resources Widget resources

Currently widgets are lacking of a unified way to handle widget resources.
This functionality will be added in the near future. Meanwhile we recommend using compiled-in resources.
How to do it is explained <a href="http://doc.trolltech.com/4.5/resources.html">here</a>.

*/

/*!

    \ingroup group_helloworld_widget
    \class HelloWorldWidget
    \brief Example implementation for home screen widget.

    HelloWorldWidget derived from the HsWidget and implements 
    needed functions for the home screen widget. 
    
    
    
*/


/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
HelloWorldWidget::HelloWorldWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HsWidget(parent, flags),
      mButton(0)
{
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    mButton = new QPushButton("Hello World");
    proxy->setWidget(mButton);        
}

/*!
    Destructor
*/
HelloWorldWidget::~HelloWorldWidget()
{

}

/*!
    Return bounding rect
*/
QRectF HelloWorldWidget::boundingRect() const
{
    return childrenBoundingRect();
}

/*!
    Return shape
*/
QPainterPath HelloWorldWidget::shape() const
{   
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

/*!
    Widget runtime starts widget
*/
HsWidget::StartResult HelloWorldWidget::onStart()
{
    return StartResultRunning;
}

/*!
    Widget runtime stops widget
*/
HsWidget::StopResult HelloWorldWidget::onStop()
{
    return StopResultFinished;
}

/*!
    Widget runtime suspends widget
*/
HsWidget::SuspendResult HelloWorldWidget::onSuspend()
{
    return SuspendResultSuspended;
}

/*!
    Widget runtime resumes widget
*/
HsWidget::ResumeResult HelloWorldWidget::onResume()
{
    return ResumeResultRunning;
}
