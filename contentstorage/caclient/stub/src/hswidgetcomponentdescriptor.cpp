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


#include "hswidgetcomponentdescriptor.h"

bool HsWidgetComponentDescriptor::isValid()
{
    bool ret(false);
    if (version == "1.0") {
        ret = !(uri.isEmpty() || serviceXml.isEmpty() || title.isEmpty()); 
    } else if (version.isEmpty()) {
        ret = !(uri.isEmpty() || title.isEmpty()); 
    }
    return ret;
}

