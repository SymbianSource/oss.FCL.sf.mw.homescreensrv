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


#ifndef C_AIPUBLISHPRIORITIZER_H
#define C_AIPUBLISHPRIORITIZER_H

// System includes
#include <e32def.h>

// User includes
#include "ainwidpriorities.h"
#include "aiprioritizer.h"

// Forward declarations
class CHsContentPublisher;


/**
 *  @ingroup group_devicestatusplugin
 *
 *  Network identity related prioritizer.
 *
 *  This class is used as proxy between publishers which publish data that
 *  needs to be priorised and content observers. It keeps record of published
 *  data and decides if new published data has high enough priority to go 
 *  through to content observer.
 *
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiPublishPrioritizer ) : public CBase, 
    public MAiPublishPrioritizer
    {
public:

    static CAiPublishPrioritizer* NewL( MAiContentObserver& aContentObserver,
                                        CHsContentPublisher& aPropertyExtension );

    virtual ~CAiPublishPrioritizer();


    // from MAiPublishPrioritizer
    
    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                        TInt aContent, 
                        TInt aResource, 
                        TInt aPriority );

    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                        TInt aContent, 
                        const TDesC16& aText, 
                        TInt aPriority );

    void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                        TInt aContent, 
                        const TDesC8& aBuf, 
                        TInt aPriority );
                                
    void TryToCleanL( MAiPublisherBroadcaster& aBroadcaster, 
                        TInt aContent, 
                        TInt aPriority );
                        
    TInt NextPriority() const;

    
private:

    CAiPublishPrioritizer( MAiContentObserver& aContentObserver,
                            CHsContentPublisher& aPropertyExtension );


private: // data

    /**
     * Content observer.
     */
    MAiContentObserver& iContentObserver;
    
    /**
     * Property extension.
     */
    CHsContentPublisher& iPropertyExtension;

    /// Current priority
    TInt iPriority;
    };


#endif // C_AIPUBLISHPRIORITIZER_H
