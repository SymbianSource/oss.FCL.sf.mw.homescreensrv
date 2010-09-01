/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AIWSPLUGINANIMDLL_H
#define AIWSPLUGINANIMDLL_H

#include    <e32std.h>
#include    <w32adll.h>

/** 
*  @ingroup group_wsplugin
*
* Active Idle Window Server plug-in.
*
* @since S60 3.2
*/
class CAiWsPluginAnimDll 
    : public CAnimDll
    {
public: // From base classes
    
    /** 
    * From CAnimDll, creates an instance of requested plugin.
    *
    * @param aType type of the plug-in. AI WS implements EAnimKeyForward.
    * @return instance of animation service.
    */
    virtual CAnim* CreateInstanceL( TInt aType );

    };

/** 
* Creates an instance of CPhoneAnimDll. 
* Factory function. 
*
* @since S60 3.2
* @return instance of anim dll.
*/
IMPORT_C CAnimDll* CreateCAnimDllL();

#endif // AIWSPLUGINANIMDLL_H

// End of File
