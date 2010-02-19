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
* Description:  Operator logo publisher.
*
*/


#ifndef C_AIOPERATORLOGOPUBLISHER_H
#define C_AIOPERATORLOGOPUBLISHER_H

// System includes
#include <e32base.h>
#include <e32property.h>
#include <cenrepnotifyhandler.h>

// User includes
#include <aiutility.h>
#include "aidevicestatuspublisher.h"
#include "ainetworkinfoobserver.h"

// Forward declarations
class CAiNetworkInfoListener;
class MAiDeviceStatusContentObserver;
class CHsContentPublisher;
class CGulIcon;
class CCenRepNotifyHandler;


/**
 *  @ingroup group_devicestatusplugin
 *
 *  Operator logo publisher.
 *
 *  Listens operator logo changes (OTA and programmable) and publishes
 *  the new logo when it is updated.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiOperatorLogoPublisher ) : public CBase, 
    public MAiDeviceStatusPublisher,
    public MAiNetworkInfoObserver, 
    public MCenRepNotifyHandlerCallback
    {
public:

    static CAiOperatorLogoPublisher* NewL();

    virtual ~CAiOperatorLogoPublisher();

protected:

//from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    CHsContentPublisher& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );
    TBool SuspendL( TInt aContentId, TBool aClean );
    TBool RefreshContentWithPriorityL( TInt aContentId, TInt aPriority );


//from base class MAiNetworkInfoObserver

    /**
     * From MAiNetworkInfoObserver.
     * Called when network info changes.
     */
    void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
    							  const TNWInfo& aInfo, 
    							  const TBool aShowOpInd );


// from base class MCenRepNotifyHandlerCallback

    /** 
     * This callback method is used to notify the client about
     * changes for integer value keys, i.e. key type is EIntKey.
     *
     * @param aId Id of the key that has changed.
     * @param aNewValue The new value of the key.
     */
    void HandleNotifyInt( TUint32 aId, TInt aNewValue );




private:

    CAiOperatorLogoPublisher();

    void ConstructL();

    /**
     * Called when central repository key changes.
     *
     * @since S60 3.2
     * @param aMCC is country code.
     * @param aMNC is network code.
     * @return pointer to loaded logo. Ownership is transferred to caller.
     */
    CFbsBitmap* LoadLogoL( TInt aMCC,
                           TInt aMNC );

    /**
     * Update operator logo.
     * @param aClean ETrue if currently published logo must be cleaned.
     */
    void UpdateOperatorLogoL( TBool aClean );

    /**
     * Returns ETrue if operator logo is allowed to be displayed.
     */
    TBool AllowToShowLogoL() const;
    
    /** 
     * Create monochrome bitmaps (main & mask).
     * 
     * @since S60 3.2
     * @param aBitmap monochrome bitmap.
     * @param aNewLogoMain bitmap, ownership passed to caller.
     * @param aNewLogoMask bitmap mask, ownership passed to caller.
     */
    void CreateMonochromeBitmapsL( 
    	const CFbsBitmap& aBitmap, 
    	CFbsBitmap*& aNewLogoMain,
    	CFbsBitmap*& aNewLogoMask );

    /**
     * Getter method for title pane size.
     * @param aSize Initialized with title pane size in return.
     * @return KErrNotFound if title pane is not found, KErrNone otherwise.
     */
	TInt GetTitlePaneSize( TSize& aSize );
	
	/**
	 * Callback method for changes in operator logo.
	 * Called if operator logo has been changed.
	 */
	static TInt HandleOperatorLogoUpdateL( TAny *aPtr );


private: // data

    /** Network info listener, owned */     
    CAiNetworkInfoListener* iListener;
    /** Operator logo bitmap and mask, owned */     
    CGulIcon* iIcon;
    /** Content observer, not owned */
    MAiContentObserver* iContentObserver;
    /** Property extension, not owned */
    CHsContentPublisher* iExtension;
	/** Content prioritizer, not owned */
	MAiPublishPrioritizer* iPrioritizer;    
	/** Publish broadcaster, not owned */
	MAiPublisherBroadcaster* iBroadcaster;    
    /** Publish-subscribe client, owned */
    MAiPSPropertyObserver* iOperatorLogoObserver;
    /** Central repository client, owned */
    CRepository* iCenRep;
    /** Central repository notifier, owned */
    CCenRepNotifyHandler* iCenRepNotify;
    /** Operator logo priority, can have one of the following values:
     * 
     * 1) EAiOTAOperatorLogo
     * 2) EAiProgOperatorLogo
     * 3) EAiInvalidPriority
     */
    TInt iPriority;
    /** True if publish was successful. */
    TBool iSuccess;    
    /** Flag to indicate if the content is suspended */
     TBool iSuspended;    
    /** Show operator indicator */
    TBool iShowOpInd;
    };

#endif // C_AIOPERATORLOGOPUBLISHER_H

// End of file
