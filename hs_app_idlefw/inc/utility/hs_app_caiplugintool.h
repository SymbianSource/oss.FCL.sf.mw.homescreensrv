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
* Description:  Collection of content plugin helper functions
*
*/


#ifndef C_AIPLUGINTOOL_H
#define C_AIPLUGINTOOL_H

#include <e32base.h>
#include "aiplugintool.h"

class TAiPublisherInfo;
class CAiContentPublisher;
class MAiContentItemIterator;
class MAiPropertyExtension;
class MAiEventHandlerExtension;

/**
*  @ingroup group_aiutils
* 
*  Plugin tool.
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CAiPluginTool ) :
    public CBase, public MAiPluginTool
	{
public:
	
    static CAiPluginTool* NewL();
    
private:

    CAiPluginTool();
    
    void ConstructL();
    
    void Release();

    const TAiPublisherInfo* PublisherInfoL(
                                CAiContentPublisher& aContentPublisher );

    MAiContentItemIterator* ContentItemIteratorL(
                                CAiContentPublisher& aContentPublisher,
                                TInt aContentType = EAiPublisherContent );

    MAiPropertyExtension* PropertyExt(
                                CAiContentPublisher& aContentPublisher );

    MAiEventHandlerExtension* EventHandlerExt(
                                CAiContentPublisher& aContentPublisher );

    };

#endif // M_AIPLUGINTOOL_H





