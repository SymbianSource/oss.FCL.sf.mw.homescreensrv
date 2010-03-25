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

#include <e32base.h>
#include <aipropertyextension.h>

class TAiPublisherInfo;
class CAiContentPublisher;
class MAiContentItemIterator;
class MAiPropertyExtension;
class MAiEventHandlerExtension;

/**
*  Plugin tool.
*
*  @since S60 3.2
*/
class MAiPluginTool 
	{
	
public:

    /**
     * Get the publisher info of the passed publisher.
     *
     * @since S60 3.2
     * @param reference to content publisher
     * @return pointer to publisher info 
     */
    virtual const TAiPublisherInfo* PublisherInfoL(
                                CAiContentPublisher& aContentPublisher ) = 0;

    /**
     * Get the content item iterator for the passed publisher.
     *
     * @param reference to content publisher
     * @param content type indication
     * @return pointer to content iterator 
     */
    virtual MAiContentItemIterator* ContentItemIteratorL(
                                CAiContentPublisher& aContentPublisher,
                                TInt aContentType = EAiPublisherContent ) = 0;


    /**
     * Get the property extension for passed publisher.
     *
     * @param reference to content publisher
     * @return property extension pointer
     */
    virtual MAiPropertyExtension* PropertyExt(
                                CAiContentPublisher& aContentPublisher ) = 0;

    /**
     * Get the event handler extension for passed publisher.
     *
     * @param reference to content publisher
     * @return event handler extension pointer
     */                                
    virtual MAiEventHandlerExtension* EventHandlerExt(
                                CAiContentPublisher& aContentPublisher ) = 0;

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




