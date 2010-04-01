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

#include <e32def.h>
#include "hs_app_ainwidpriorities.h"
#include "hs_app_aiprioritizer.h"
#include "aipropertyextension.h"

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
 
class CAiPublishPrioritizer : public CBase, public MAiPublishPrioritizer
    {

public:

    static CAiPublishPrioritizer* NewL( MAiContentObserver& aContentObserver,
                                        MAiPropertyExtension& aPropertyExtension );

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
                            MAiPropertyExtension& aPropertyExtension );


private: // data

    /**
     * Content observer.
     */
    MAiContentObserver& iContentObserver;
    
    /**
     * Property extension.
     */
    MAiPropertyExtension& iPropertyExtension;

    /// Current priority
    TInt iPriority;
    };


#endif // C_AIPUBLISHPRIORITIZER_H
