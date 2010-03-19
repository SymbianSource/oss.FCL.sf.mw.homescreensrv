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
* Description:  Private implementation of Baget.
*
*/

#ifndef BAGET_P_H
#define BAGET_P_H

#include "baget.h"

class BagetPrivate
{

public:

    BagetPrivate(Baget * const bagetPublic);
    ~BagetPrivate();

    void start();
    void stop();
    void resume();
    void suspend();

    BagetState currentState();

private:

    Q_DISABLE_COPY(BagetPrivate)

private:

    Baget *m_q;
    BagetState mState;

};

#endif // BAGET_P_H
