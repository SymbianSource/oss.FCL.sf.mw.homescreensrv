/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base content object parser interface definition
*
*/



#ifndef CGECOOBJECTBASE_H
#define CGECOOBJECTBASE_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION
/**
*  Generic Content Object Base Class
*  Base class that all content objects derive from. 
*
*  @lib XCFW.lib
*  @since Series 60 3.1
*/
class CGECOObjectBase :     public CBase
    {
    public: // New functions
        
        /**
        * Returns Object type identifier for this object
        * @since Series 60 3.1
        * @return Object identifier
        */
        virtual const TDesC& TypeIdentifier() = 0;
    };

#endif      // CXCFWOBJECTPARSER_H   
            
// End of File
