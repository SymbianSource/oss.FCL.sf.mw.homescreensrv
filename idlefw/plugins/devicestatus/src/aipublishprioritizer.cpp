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


#include "aipublishprioritizer.h"
#include "ainwidpriorities.h"
#include <aidevicestatuscontentmodel.h>
#include <aicontentrequest.h>

#include "aipublisherbroadcaster.h"
#include "debug.h"


CAiPublishPrioritizer::CAiPublishPrioritizer( 
                                        MAiContentObserver& aContentObserver,
                                        CHsContentPublisher& aPropertyExtension )
    : iContentObserver( aContentObserver ),
        iPropertyExtension( aPropertyExtension ),
        iPriority( EAiInvalidPriority )
    {
    }


CAiPublishPrioritizer* CAiPublishPrioritizer::NewL( 
                                        MAiContentObserver& aContentObserver,
                                        CHsContentPublisher& aPropertyExtension )
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
    __PRINT(__DBG_FORMAT("XAI: TryToPublishL - content %d, aResource %d, aPriority %d, iPriority %d"), aContent, aResource, aPriority, iPriority );
    if( aPriority >= iPriority )
        {
        __PRINTS("XAI: prio publish - priority check ok");
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aResource, 
                                            0 );
        }
    __PRINTS("XAI: prio publish - done");
    }


void CAiPublishPrioritizer::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                            TInt aContent, 
                                            const TDesC16& aText, 
                                            TInt aPriority )
    {
    __PRINTS("XAI: CAiPublishPrioritizer::TryToPublishL");
    __PRINT(__DBG_FORMAT("XAI: prio publish - content %d, aPriority %d, iPriority %d, text %S"), aContent, aPriority, iPriority, &aText );
    
    if( aPriority >= iPriority )
        {
        __PRINTS("XAI: prio publish - priority check ok");
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aText, 
                                            0 );
        }
    else
        {
        __PRINTS("XAI: prio publish - aPriority < iPriority !!!! ");
        }
        
        
    __PRINTS("XAI: prio publish - done");
    }


void CAiPublishPrioritizer::TryToPublishL( MAiPublisherBroadcaster& /*aBroadcaster*/, 
                                            TInt aContent, 
                                            const TDesC8& aBuf, 
                                            TInt aPriority )
    {
    __PRINTS("XAI: CAiPublishPrioritizer::TryToPublishL - TDesC8" );    
    if( aPriority >= iPriority )
        {
        __PRINTS("XAI: prio publish - priority check ok");
        iPriority = aPriority;
        (void)iContentObserver.Publish( iPropertyExtension, 
                                            aContent, 
                                            aBuf, 
                                            0 );
        }
    else
        {
        __PRINTS("XAI: prio publish - aPriority < iPriority !!!! ");
        }
        
        
    __PRINTS("XAI: prio publish - done");
    }
                            

void CAiPublishPrioritizer::TryToCleanL( MAiPublisherBroadcaster& aBroadcaster, 
                                        TInt aContent, 
                                        TInt aPriority )
    {
    __PRINT(__DBG_FORMAT("XAI: CAiPublishPrioritizer::TryToCleanL - content %d, aPriority %d, iPriority %d"), aContent, aPriority, iPriority );
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
    else
        {
        __PRINT(__DBG_FORMAT("XAI: prio clean - CLEAN failed - iPriority %d"), iPriority );
        }        
    }
