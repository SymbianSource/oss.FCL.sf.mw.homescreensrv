/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_AIPLUGINSETTINGSIMPL_H
#define M_AIPLUGINSETTINGSIMPL_H

#include "aipluginsettings.h"

class CAiPluginSettingsItem;
class MAiPluginContentItem;

/**
 * Implementation of Active Idle framework settings access interface.
 *
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CAiPluginSettings) : public CBase, public MAiPluginSettings
    {
public:

    static CAiPluginSettings* NewL();

    ~CAiPluginSettings();

    MAiPluginSettingsItem& AiPluginSettingsItem();
    
    TInt AiPluginItemType();
            
    MAiPluginContentItem& AiPluginContentItem();
    
    MAiPluginConfigurationItem& AiPluginConfigurationItem();

private:

    CAiPluginSettings();

    void ConstructL();

private:

    CAiPluginSettingsItem* iSettingsItem;
    
    TInt                    iItemType;

    };

#endif // M_AIPLUGINSETTINGSIMPL_H


