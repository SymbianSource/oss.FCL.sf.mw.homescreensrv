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
* Description:  Device status plugin interface.
*
*/


#ifndef C_AIDEVICESTATUSPLUGIN_H
#define C_AIDEVICESTATUSPLUGIN_H

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aicontentrequest.h>

// Forward declarations
class MAiDeviceStatusPublisher;
class CAiContentItemArrayIterator;
class CAiDeviceStatusPluginEngine;
class CAiMultiContentObserver;
class CAiPublishPrioritizer;
class CAiNetworkInfoListener;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Device status plugin interface.
 *
 *  This class implements required active idle plugin API.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiDeviceStatusPlugin ) : public CHsContentPublisher, 
    public MAiContentRequest                                 
    {
public:
    // constructors and destructor
    static CAiDeviceStatusPlugin* NewL();
    static CAiDeviceStatusPlugin* NewLC();

    virtual ~CAiDeviceStatusPlugin();

protected:
    // from CHsContentPublisher
    
    /**
     * @see CHsContentPublisher
     */
    void Start( CHsContentPublisher::TStartReason aReason );

    /**
     * @see CHsContentPublisher
     */    
    void Stop( CHsContentPublisher::TStopReason aReason );
    
    /**
     * @see CHsContentPublisher
     */    
    void Resume( CHsContentPublisher::TResumeReason aReason );
    
    /**
     * @see CHsContentPublisher
     */    
    void Suspend( CHsContentPublisher::TSuspendReason aReason );

    /**
     * @see CHsContentPublisher
     */    
    void SubscribeL( MAiContentObserver& aObserver );
    
    /**
     * @see CHsContentPublisher
     */    
    void ConfigureL( RAiSettingsItemArray& aSettings );
    
    /**
     * @see CHsContentPublisher
     */    
    TAny* GetProperty( CHsContentPublisher::TProperty aProperty );
    
protected:
    // from MAiContentRequest
    
    /**
     * @see MAiContentRequest
     */
    TBool RefreshContent( TInt aContentId );

    /**
     * @see MAiContentRequest
     */    
    TBool SuspendContent( TInt aContentId );
    
private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CAiDeviceStatusPlugin();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();

private:
    // new functions
    
    void AllocateResourcesL();
    void FreeResources();
    void DoResumeL();
    
    void AddPublisherL( MAiDeviceStatusPublisher* aPublisher );

private: 
    // data

	/** Device Status Plug-in Engine, owned */
	 CAiDeviceStatusPluginEngine* iEngine;
    /** Content observers, owned */
    CAiMultiContentObserver* iContentObservers;
	/** Device Status Plug-in content prioritizer, owned */
	 CAiPublishPrioritizer* iPrioritizer;
    /** Content item array iterator, owned */
    MAiContentItemIterator* iContent;
    /** Content item array iterator for resources, owned */
    MAiContentItemIterator* iResources;
    /** Offset of the loaded resource file */
    TInt iResourceOffset;    
    /** Network info listener, owned */
    CAiNetworkInfoListener* iListener;    
    /** Flag to indicate republish need */
    TBool iRequirePublish;
    };

#endif // C_AIDEVICESTATUSPLUGIN_H

// End of file
