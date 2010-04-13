/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*        Header file for MGSTabbedView class.
*
*/


#ifndef GSTABBEDVIEW_H
#define GSTABBEDVIEW_H

#include <e32cmn.h>

class CGulIcon;

/**
* Interface to be used when creating tab groups. Used by CGSTabHelper.
*
*/
class MGSTabbedView
    {
    public: // New
        
        /**
        * Creates new icon for tab. Ownership is transferred to client.
        */
        virtual CGulIcon* CreateTabIconL() = 0;
        
        /**
        * UID of the tab.
        */
        virtual TUid Id() const = 0;
    
    };
    
#endif // GSTABBEDVIEW_H
//End of File
