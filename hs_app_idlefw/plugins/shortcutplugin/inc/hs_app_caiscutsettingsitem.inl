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
* Description:  Inline functions for the shortcut settings item.
*
*/


// ---------------------------------------------------------------------------
// Returns the setting item key.
// ---------------------------------------------------------------------------
//
inline TUint32 CAiScutSettingsItem::Key() const
{
    return iKey;
}

// ---------------------------------------------------------------------------
// Returns the target shortcut uid.
// ---------------------------------------------------------------------------
//
inline TUid CAiScutSettingsItem::Uid() const
{
    return iUid;
}

// ---------------------------------------------------------------------------
// Returns the setting item type.
// ---------------------------------------------------------------------------
//
inline TAiScutSettingType CAiScutSettingsItem::Type() const
{
    return iType;
}



// ---------------------------------------------------------------------------
// Returns the setting item title.
// ---------------------------------------------------------------------------
//
inline TPtrC CAiScutSettingsItem::Title() const
{
    return iTitle;
}

// End of File.
