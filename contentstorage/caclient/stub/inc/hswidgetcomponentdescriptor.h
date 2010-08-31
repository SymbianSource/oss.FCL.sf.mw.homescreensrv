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
* Description:
*
*/

#ifndef HSWIDGETCOMPONENTDESCRIPTION_H
#define HSWIDGETCOMPONENTDESCRIPTION_H


#include <QDataStream>
#include <QMetaType>

class HsWidgetComponentDescriptor
{
public:
    HsWidgetComponentDescriptor();
    HsWidgetComponentDescriptor(const HsWidgetComponentDescriptor& copyFrom);
    QString installationPath;
    QString uri;
    QString title;
    QString description;
    QString iconUri;
    QString hidden;
    QString serviceXml;
    QString version;    
    QString library;
    QString translationFilename;
    int uid;
    QString previewImage;
    bool isValid();
};

Q_DECLARE_METATYPE(HsWidgetComponentDescriptor)

#endif 
