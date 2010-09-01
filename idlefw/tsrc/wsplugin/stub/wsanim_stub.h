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

// wsanim.h

#ifndef _WSANIM_STUB_H
#define _WSANIM_STUB_H

#include <w32adll.h>

class MAnimGeneralFunctions;

/**
 * Stub of CWsAnim for CAiWsPluginAnim unit test.
 */
class CWsAnim : public CBase
    {
public:
    static void SetFunctions
            (CAnim& aAnim, MAnimGeneralFunctions* aFunctions)
        {
        // CWsAnim is friend of CAnim which enables setting its protected members
        aAnim.iFunctions = aFunctions;        
        }
        
private:
    // Private constructor to prevent instantiation of this class
    CWsAnim() {}
    };
    
    
#endif // _WSANIM_STUB_H
