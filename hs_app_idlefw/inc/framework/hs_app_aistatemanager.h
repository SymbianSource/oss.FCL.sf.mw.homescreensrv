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
* Description:  State manager base class
*
*/


#ifndef M_AISTATEMANAGER_H
#define M_AISTATEMANAGER_H

#include "hs_app_aipluginstatemachine.h"

/**
 *  description
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
class MAiStateManager
    {
    
    public:
    
        /**
         * System status observers use this method to report status variable 
         * changes to state manager.
         *
         * @param TAiStateChanges aState the state that has changed.
         */
        virtual void ReportStateChange( TAiStateChanges aState ) = 0;
    
    protected:
    
        ~MAiStateManager(){}
    
    };
 
#endif // M_AISTATEMANAGER_H

// End of File.
