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


#include <aicontentpublisher.h>
#include <aipropertyextension.h>
#include <aicontentrequest.h>

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
class CAiDeviceStatusPlugin : public CAiContentPublisher, 
                                public MAiPropertyExtension, 
                                public MAiContentRequest
    {
public:

    static CAiDeviceStatusPlugin* NewL();
    static CAiDeviceStatusPlugin* NewLC();

    virtual ~CAiDeviceStatusPlugin();

protected:

// from base class CAiContentPublisher
    void Resume( TAiTransitionReason aReason );
    void Suspend( TAiTransitionReason aReason );
    void Stop( TAiTransitionReason aReason );
    void SubscribeL(MAiContentObserver& aObserver);
    TAny* Extension(TUid aUid);
    void ConfigureL(RAiSettingsItemArray& aSettings);

protected:

// from base class MAiPropertyExtension
    TAny* GetPropertyL(TInt aProperty);
    void SetPropertyL(TInt aProperty, TAny* aValue);

// from base class MAiContentRequest
    TBool RefreshContent( TInt aContentId );


private:

    CAiDeviceStatusPlugin();
    void ConstructL();

    void AllocateResourcesL();
    void FreeResources();
    void DoResumeL( TAiTransitionReason aReason );
    TBool IgnoreReason( TAiTransitionReason aReason );

     /**
     * Add device status publisher.
     */
    void AddPublisherL( MAiDeviceStatusPublisher* aPublisher );

private: // data

	/**
	 * Device Status Plug-in Engine.
	 * Own.
	 */
	 CAiDeviceStatusPluginEngine* iEngine;

    /**
     * Content observers.
     * Own.
     */
    CAiMultiContentObserver* iContentObservers;

	/**
	 * Device Status Plug-in content prioritizer.
	 * Own.
	 */
	 CAiPublishPrioritizer* iPrioritizer;

    /**
     * Content item array iterator.
     * Own.
     */
    MAiContentItemIterator* iContent;

    /**
     * Content item array iterator for resources.
     * Own.
     */
    MAiContentItemIterator* iResources;

    /**
     * Publisher info.
     */
    TAiPublisherInfo iInfo;

    /**
     * Offset of the loaded resource file.
     */
    TInt iResourceOffset;
    
    /**
     * Network info listener.
     * Own.
     */
    CAiNetworkInfoListener* iListener;    
    };


#endif // C_AIDEVICESTATUSPLUGIN_H
