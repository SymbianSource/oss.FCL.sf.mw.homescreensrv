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
* Description:  Content item refresh request interface
*
*/


#ifndef M_AICONTENTREQUEST_H
#define M_AICONTENTREQUEST_H

/**
 * Interface for requesting a content publisher to refresh its published
 * content to the UI.
 *
 * @since S60 3.2
 * @see MAiPropertyExtension
 */
class MAiContentRequest
    {
public:
    /**
     * Requests AI content publisher to publish specific content item.
     *
     * @param aContentId content identifier in target plug-ins content model.
     * @return ETrue if the plugin will refresh the content by calling its 
     *         content observer, EFalse otherwise.
     */
    virtual TBool RefreshContent( TInt aContentId ) = 0;
    
protected:
    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiContentRequest() { }
    };
    
#endif

