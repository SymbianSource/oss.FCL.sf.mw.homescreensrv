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
#ifndef TSNOITEMSLABEL_H
#define TSNOITEMSLABEL_H

#include <HbLabel>

class TsNoItemsLabel : public HbLabel
{
    Q_OBJECT

public:
    explicit TsNoItemsLabel(QGraphicsItem *parent = 0);  
    
private:
    Q_DISABLE_COPY(TsNoItemsLabel)    
    
};

#endif //TSNOITEMSLABEL_H
