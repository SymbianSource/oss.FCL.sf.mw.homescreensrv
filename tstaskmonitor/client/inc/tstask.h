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
#ifndef TSTASK_H
#define TSTASK_H

#include <QPixmap>
#include <QString>

#include "tstaskmonitor_global.h"

class CTsFswEntry;

class TsTask
{
    
public:
    TsTask(CTsFswEntry* entry);
    TSTASKMONITOR_EXPORT virtual ~TsTask();
    
    TSTASKMONITOR_EXPORT virtual void open() = 0;
    TSTASKMONITOR_EXPORT virtual void close() = 0;
    
    TSTASKMONITOR_EXPORT bool isClosable() const;
    TSTASKMONITOR_EXPORT QPixmap icon() const;
    TSTASKMONITOR_EXPORT QString name() const;
    
protected:
    CTsFswEntry* mEntry;

};

#endif //TSTASK_H
