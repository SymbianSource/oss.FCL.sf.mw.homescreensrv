/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network identity related prioritizer.
*
*/


#include "hs_app_aipublishprioritizer.h"
#include "hs_app_ainwidpriorities.h"
#include "hs_app_aidevicestatuscontentmodel.h"
#include <aicontentrequest.h>
#include "aipropertyextension.h"
#include "hs_app_aipublisherbroadcaster.h"
#include "debug.h"


CAiPublishPrioritizer::CAiPublishPrioritizer( 
                                        MAiContentObserver& aContentObserver,
                                        MAiPropertyExtension& aPropertyExtension )
    : iContentObserver( aContentObserver ),
        iPropertyExtension( aPropertyExtension ),
        iPriority( EAiInvalidPriority )
    {
    }


CAiPublishPrioritizer* CAiPublishPrioritizer::NewL( 
                                        MAiContentObserver& aContentObserver,
                                        MAiPropertyExtension& aPropertyExtension )
    {
    return new( ELeave ) CAiPublishPrioritizer( aContentObserver, 
                                                aPropertyExtension );
    }


CAiPublishPrioritizer::~CAiPublishPrioritizer()
    {
    }


void CAiPublishPrioritizer::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                            TInt aContent, 
                                            TInt aResource, 
                                            TInt aPriority )
    {
    if( aPriority >= iPriority )
        {
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aResource, 
                                            0 );
        }
    }


void CAiPublishPrioritizer::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                            TInt aContent, 
                                            const TDesC16& aText, 
                                            TInt aPriority )
    {
    __PRINT(__DBG_FORMAT("XAI: prio publish - content %d, priority %d, text %S"), aContent, aPriority, &aText );
    
    if( aPriority >= iPriority )
        {
        __PRINTS("XAI: prio publish - priority check ok");
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aText, 
                                            0 );
        }
        
    __PRINTS("XAI: prio publish - done");
    }


void CAiPublishPrioritizer::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                            TInt aContent, 
                                            const TDesC8& aBuf, 
                                            TInt aPriority )
    {
    if( aPriority >= iPriority )
        {
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aBuf, 
                                            0 );
        }
    }
                            

void CAiPublishPrioritizer::TryToCleanL( MAiPublisherBroadcaster& aBroadcaster, 
                                        TInt aContent, 
                                        TInt aPriority )
    {
    __PRINT(__DBG_FORMAT("XAI: prio clean - content %d, priority %d"), aContent, aPriority );
    if( aPriority == EAiInvalidPriority )
        {
        // do nothing
        return;
        }
        
    else if( aPriority == iPriority )
        {
        __PRINTS("XAI: prio clean - priority matched");
        // Clean the current content...
        (void)iContentObserver.Clean( iPropertyExtension, aContent, 0 );

        TInt currentPriority = iPriority;
        iPriority = EAiInvalidPriority;

        TBool refreshed = EFalse;
        while( --currentPriority != EAiInvalidPriority && !refreshed )
        	{
        	__PRINT(__DBG_FORMAT("XAI: prio clean - refreshing %d"), currentPriority );
        	refreshed = aBroadcaster.RefreshPriorizedPublishersL( aContent, 
        	                                                currentPriority );
        	if( refreshed )
        	    {
                __PRINTS("XAI: prio clean - refresh success");        	    
        	    }
        	else
        	    {
                __PRINTS("XAI: prio clean - refresh failed");        	    
        	    }
        	}
        }
    }
