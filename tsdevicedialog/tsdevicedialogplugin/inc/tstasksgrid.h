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
#ifndef TSTASKSGRID_H
#define TSTASKSGRID_H

#include <HbGridView>

class TsTasksGrid : public HbGridView
{
    Q_OBJECT

signals:
    void deleteButtonClicked(const QModelIndex &index);

protected:
    virtual void polish(HbStyleParameters &params);

};

#endif // TSTASKSGRID_H
