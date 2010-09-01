/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates name and value
*
*/

#ifndef MHOMESCREENSETTINGSIF_H_
#define MHOMESCREENSETTINGSIF_H_

#include <e32base.h>

namespace HSPluginSettingsIf{

class CItemMap;

class MHomescreenSettingsIf
    {
public:
    /**
     * Stores defined plugin’s settings. 
     * Plugin is identified with unique plugin ID used in application’s
     * configuration 
     *
     * @param aPluginId Pluging’s ID which settings are stored
     * @param aSettings List of item maps
     * @return Operation status. KErrNone (success), KErrNotFound
     */  
    virtual TInt GetSettingsL(const TDesC8& aPluginId, RPointerArray< CItemMap >& aSettings) = 0;
    
    /**
     * From MHomescreenSettingsIf
     * Stores defined plugin’s settings. 
     * Plugin is identified with unique plugin ID used in application’s 
     * configuration 
     *
     * @param aPluginId Pluging’s ID which settings are stored
     * @param aSettings List of item maps
     * @param aStoringParam parameter to tell whether plugin configuration need to be saved,if parameter is ETrue.
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    virtual TInt SetSettingsL(const TDesC8& aPluginId, const RPointerArray< CItemMap >& aSettings, const TBool aStoringParam) = 0;
    };
}
#endif /*MHOMESCREENSETTINGSIF_H_*/
