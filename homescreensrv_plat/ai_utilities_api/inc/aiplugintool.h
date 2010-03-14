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


#ifndef M_AIPLUGINTOOL_H
#define M_AIPLUGINTOOL_H

// System includes
#include <e32base.h>

// User incldues
#include <hscontentpublisher.h>

// Forward declarations
class THsPublisherInfo;
class MAiContentItemIterator;

/**
*  Plugin tool.
*
*  @since S60 3.2
*/
class MAiPluginTool 
	{
	
public:

    /**
     * Get the content item iterator for the passed publisher.
     *
     * @param reference to content publisher
     * @param content type indication
     * @return pointer to content iterator 
     */
    virtual MAiContentItemIterator* ContentItemIterator(
        CHsContentPublisher& aContentPublisher,
        CHsContentPublisher::TProperty aType = CHsContentPublisher::EPublisherContent ) = 0;

    /**
     * Release the tool.
     */
    virtual void Release() = 0;
	    
protected:
	    
	MAiPluginTool() { }    
    };

inline void Release( MAiPluginTool* aSelf )
    {
    if ( aSelf )
        {
        aSelf->Release();
        }
    }

#endif // M_AIPLUGINTOOL_H





