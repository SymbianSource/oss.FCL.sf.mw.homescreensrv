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
* Description:  Shortcut plug-in xSP extension data
*
*/


#ifndef MAISCUTEXTDATA_H
#define MAISCUTEXTDATA_H

#include <e32base.h>

class CGulIcon;

/**
 *  @since S60 v3.2
 */
class MAiScutExtData
    {
    public:
        /**
         * @return Pointer to popup line array or NULL if not set
         * @since S60 v3.2
         */
        virtual const MDesCArray* PopupLineArray() const = 0;
        
        /**
         * return Pointer to icon or NULL if not set
         * @since S60 v3.2
         */
        virtual const CGulIcon* Icon() const = 0;
    };

#endif // MAISCUTEXTDATA_H

// End of File.
