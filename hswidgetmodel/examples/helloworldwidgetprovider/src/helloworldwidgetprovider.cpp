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

#include <hswidget.h>

#include "helloworldwidgetprovider.h"
#include "helloworldwidget.h"


/*!
    \ingroup group_helloworld_widget
    \class HelloWorldWidgetProvider
    \brief Example implementation for home screen widget provider.

    @page page_creating_widget_provider Creating Home Screen Widget Provider

    Widgets are exposed to the home screen through widget provider plugins.
    A widget can be added to an existing provider or new one can be created.
    Widget provider plugins are implemented according to
    the <a href="http://doc.trolltech.com/4.4/plugins-howto.html">Qt plugin model</a>.

    See @ref page_developing_home_screen_widget for the instructions how to create widget for the home screen.

    The steps to create a widget provider are:
    <ol>
    <li> Declare a plugin class that inherits from QObject and from the \c IHsWidgetProvider interface.
    
    <li> Use the Q_INTERFACES() macro to tell Qt's meta-object system about the \c IHsWidgetProvider interface.
    
    <li> Export the plugin using the Q_EXPORT_PLUGIN2() macro.
    
    <li> Build the plugin using an suitable .pro file. See @ref page_deploying_widget_provider.
    </ol>
    
    An example (full example source code can be found from section @ref page_developing_home_screen_widget):


Each widget provider plugin has a manifest file that allows searching widgets from the plugin without first loading it.
The manifest file contains information on widgets inside the plugin:

\li \c library The name of the provider plugin binary.
\li \c uri     Uniquely identifies the widget.
\li \c title   Widget's human-readable name.
\li \c iconuri URI of the widget's icon image file.

Example: Manifest for a widget provider.

@code
<?xml version="1.0" encoding="utf-8"?>
<widgetprovider>
    <widget library="helloworldwidgetprovider.dll"
            uri="homescreen.nokia.com/widget/helloworld"
            title="HelloWorld"
            iconuri="helloworldwidgetprovider.png"/>
</widgetprovider>
@endcode


@page page_deploying_widget_provider Deploying Home Screen Widget Provider

Widget's binaries and manifest file(s) must be deployed to correct folders on emulator and in target. 
Below are the needed .pro file for the \c helloworldwidgetprovider.

For example:
@code
# helloworldwidgetprovider.pro

TEMPLATE = lib

CONFIG += plugin hb
LIBS += -lhswidgetmodel

HEADERS += ./inc/ *.h \
           ./helloworldwidget/inc/ *.h

SOURCES += ./src/ *.cpp \
           ./helloworldwidget/src/ *.cpp

DEPENDPATH += ./inc \
              ./src

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    # get your own symbian uid    
    TARGET.UID3 = 0xEABCFE12
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    # add your symbian uid here
    PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/EABCFE12
    
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    qtplugins.sources += resource/$${TARGET}.manifest
    qtplugins.sources += resource/$${TARGET}.png
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)" 
}

@endcode

For detailed information on DEPLOYMENT macro, see <a HREF="http://pepper.troll.no/s60prereleases/doc/qmake-variable-reference.html#deployment">here</a>.

*/


/*!
    Initialize token for hello world widget. Contains necessary information about 
    the hello world widget that can be loaded with this provider.
*/
HelloWorldWidgetProvider::HelloWorldWidgetProvider()
{
    mWidgetToken.mLibrary = QString("helloworldwidgetprovider.dll");
    mWidgetToken.mUri = QString("homescreen.nokia.com/widget/helloworld");
    mWidgetToken.mTitle = QString("HelloWorld");
    mWidgetToken.mIconUri = QString("helloworldwidgetprovider.png");
}

/*!
    Destructor
*/
HelloWorldWidgetProvider::~HelloWorldWidgetProvider()
{

}

/*!
    Returns list of loadable widgets' tokens.
*/
QList<HsWidgetToken> HelloWorldWidgetProvider::widgets()
{
    return QList<HsWidgetToken>() << mWidgetToken;
}

/*!
    Create widget based on uri \a token
*/
HsWidget* HelloWorldWidgetProvider::createWidget(const HsWidgetToken& token)
{
    HsWidget *widget = 0;
    if (token.mUri == mWidgetToken.mUri) {
        widget =  new HelloWorldWidget();
    }
    return widget;
}

Q_EXPORT_PLUGIN2(helloworldwidgetprovider, HelloWorldWidgetProvider)
