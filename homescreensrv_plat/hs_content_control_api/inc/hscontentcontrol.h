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

#ifndef HSCONTENTCONTROL_H
#define HSCONTENTCONTROL_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>

// Forward declarations

/**
 *  Content control interface
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
class MHsContentControl
    {
public:
    /**
     *
     */
    virtual void NotifyWidgetListChanged() = 0;

    /**
     *
     */
    virtual void NotifyViewListChanged() = 0;

    /**
     *
     */
    virtual void NotifyAppListChanged() = 0;

    };

#endif // HSCONTENTCONTROL_H

// End of file
