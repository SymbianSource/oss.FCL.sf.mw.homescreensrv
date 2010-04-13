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
* Description:   Header file for MGSTabbedViewOwner class.
*
*/


#ifndef GSTABBEDVIEWOWNER_H
#define GSTABBEDVIEWOWNER_H

#include <e32base.h>

class CGSPluginInterface;

/**
* Interface for MGSTabbedView owner class. Views owning tabbed sub-views
* implement this.
*/
class MGSTabbedViewOwner
    {
    public:
        /**
        * @return Reference to owned tabbed views which should be included in
        *         the sub-view's tab group.
        */
        virtual CArrayPtrFlat<CGSPluginInterface>* TabbedViews() = 0;

        /**
        * Parent view implements this to handle switching tabs. This is needed
        * in case parent view needs to update selected item index according to
        * tab selection.
        *
        * @param selectedTabUid UID of the selected tab item.
        */
        virtual void TabChangedL( TUid selectedTabUid ) = 0;
    };

#endif // GSTABBEDVIEWOWNER_H
//End of File
