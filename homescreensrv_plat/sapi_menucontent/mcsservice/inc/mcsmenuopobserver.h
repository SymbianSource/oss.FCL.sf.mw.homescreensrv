/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMCSMENUOPOBSERVER_H_
#define __MMCSMENUOPOBSERVER_H_

#include <e32base.h>

// Observe menu operations. TRequestStatus -> callback
class MMCSMenuOpObserver
    {
public:
    virtual void MenuOpCompletedL( TInt /*aStatus*/ ) = 0;
    };

#endif /*__MMCSMENUOPOBSERVER_H_*/
