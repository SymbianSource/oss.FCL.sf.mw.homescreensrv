/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of different constants
*  Version     : %version: e003sa35#2 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef __MCSINTERNALTYPES_H__
#define __MCSINTERNALTYPES_H__

#include <e32base.h>

//
// Limit constants.
//

LOCAL_D const TInt KMenuMaxTypeLen = 64; ///< Maximum length of item type.
LOCAL_D const TInt KMenuMaxAttrNameLen = 64; ///< Maximum length of attribute name.
LOCAL_D const TInt KMenuMaxAttrValueLen = 512; ///< Maximum length of attribute value.

//
// Types
//
typedef TBuf<KMenuMaxAttrNameLen> TAttributeName;




#endif // __MCSINTERNALTYPES_H__
