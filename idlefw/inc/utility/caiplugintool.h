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

// System includes
#include <e32base.h>

// User includes
#include "aiplugintool.h"

// Forward declarations
class THsPublisherInfo;
class CHsContentPublisher;
class MAiContentItemIterator;

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
    // Constructor
	
    static CAiPluginTool* NewL();
    
private:
    // Constructors
    
    /**
     * C++ default contructor
     */
    CAiPluginTool();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
private:
    // from MAiPluginTool

    MAiContentItemIterator* ContentItemIterator( 
        CHsContentPublisher& aContentPublisher,        
        CHsContentPublisher::TProperty aType = CHsContentPublisher::EPublisherContent );
    
    void Release();
    };

#endif // M_AIPLUGINTOOL_H

// End of file

