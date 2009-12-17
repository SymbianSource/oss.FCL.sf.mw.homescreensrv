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
* Description:  Closed user group (CUG) publisher.
*
*/


#ifndef C_AICUGPUBLISHER_H
#define C_AICUGPUBLISHER_H


#include <e32base.h>
#include <RSSSettings.h>
#include <MSSSettingsObserver.h>
#include "aidevicestatuspublisher.h"
#include "aidevicestatuscontentmodel.h"

class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Closed user group (CUG) publisher.
 *
 *  @since S60 3.2
 */
class CAiCUGPublisher : public CBase, public MAiDeviceStatusPublisher,
                        public MSSSettingsObserver
    {
public:

    static CAiCUGPublisher* NewL();

    virtual ~CAiCUGPublisher();


protected:

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    MAiPropertyExtension& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );

// from base class MSSSettingsObserver

    void PhoneSettingChanged( TSSSettingsSetting aSetting, TInt aNewValue );


private:

    CAiCUGPublisher();

    void ConstructL();

    /**
     * Updates CUG indicator if needed.
     */
    void UpdateCUGIndicatorL( TInt aValue );
    

private: // data

    /**
     * Content observer.
     * Not own.
     */
    MAiContentObserver* iContentObserver;

    /**
     * Property extension.
     * Not own.
     */
    MAiPropertyExtension* iExtension;

    /**
     * SS Settings client. Used to observer CUG changes.
     */
    RSSSettings iSSSettings;
    
    /**
     * Published CUG text. Owned.
     */
    HBufC* iCugText;
    
    /**
     * Previous CUG value
     */
    TInt iCugValue;
    };


#endif // C_AICUGPUBLISHER_H
