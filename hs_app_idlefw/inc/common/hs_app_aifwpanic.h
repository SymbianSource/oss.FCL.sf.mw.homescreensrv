/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AI2 debug panic helper
*
*/


#ifndef AIFWPANIC_H
#define AIFWPANIC_H

#ifdef _DEBUG

namespace AiFwPanic
    {
    
    enum TAiPanicCodes
        {
        EAiFwPanic_FwCreationFailed,
        EAiFwPanic_UnsupportedContentType,
        EAiFwPanic_NullPointerReference,
        EAiFwPanic_IllegalPluginStateChange
        };
        
    void Panic(TAiPanicCodes aPanicCode);
    
    }

#endif // _DEBUG

#endif // AIFWPANIC_H

