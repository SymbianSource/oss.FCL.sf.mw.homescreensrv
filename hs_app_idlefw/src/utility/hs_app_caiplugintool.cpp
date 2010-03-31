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
* Description:  Collection of content plugin helper functions
*
*/


#include <aicontentpublisher.h>
#include "hs_app_caiplugintool.h"

CAiPluginTool::CAiPluginTool()
    {
    }

CAiPluginTool* CAiPluginTool::NewL()
    { 
    CAiPluginTool* self = new ( ELeave ) CAiPluginTool();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CAiPluginTool::ConstructL()
    {
    }

void CAiPluginTool::Release()
    {
    delete this;
    }
    
const TAiPublisherInfo* CAiPluginTool::PublisherInfoL(
                                        CAiContentPublisher& aContentPublisher )
    {
    const TAiPublisherInfo* result = NULL;
    MAiPropertyExtension* propExt = PropertyExt( aContentPublisher );
    if ( propExt )
        {
        result = propExt->PublisherInfoL();
        }
    return result;
    }
    
MAiContentItemIterator* CAiPluginTool::ContentItemIteratorL(
                                        CAiContentPublisher& aContentPublisher,
                                        TInt aContentType )
    {
    MAiContentItemIterator* result = NULL;
    MAiPropertyExtension* propExt = PropertyExt( aContentPublisher );
    if ( propExt )
        {
        TAny* prop = NULL;
        prop = propExt->GetPropertyL( aContentType );
        if ( prop )
            {
            result = static_cast<MAiContentItemIterator*>( prop );
            }
        }
    return result;
    }
    
MAiPropertyExtension* CAiPluginTool::PropertyExt(
                                        CAiContentPublisher& aContentPublisher )
    {
    return static_cast<MAiPropertyExtension*>(
                        aContentPublisher.Extension( KExtensionUidProperty ) );
    }

MAiEventHandlerExtension* CAiPluginTool::EventHandlerExt(
                                        CAiContentPublisher& aContentPublisher )
    {
    return static_cast<MAiEventHandlerExtension*>(
                        aContentPublisher.Extension( KExtensionUidEventHandler ) );
    }

 // End of File.


