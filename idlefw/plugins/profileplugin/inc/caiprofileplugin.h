/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in main class
*
*/


#ifndef CAIPROFILEPLUGIN_H
#define CAIPROFILEPLUGIN_H

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aicontentmodel.h>
#include "maiprofilepluginnotifier.h"

// Forward declarations
class MAiContentObserver;
class MAiContentItemIterator;
class CAiProfileEngine;

/**
 *  @ingroup group_profileplugin
 *
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAiProfilePlugin ) : public CHsContentPublisher,
    public MAiProfilePluginNotifier                     	                     
    {
public:
    // constructors and destructor
    
    /**
    * Part of the two phased constuction
    *
    * @param none
    * @return none
    */
    static CAiProfilePlugin* NewL();
        
    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CAiProfilePlugin();

private:
    // constructors

    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CAiProfilePlugin();
    
    /**
    * 2nd phase constructor
    */
    void ConstructL();
    
public:    
    // from CHsContentPublisher
  
    /**
     * @see CHsContentPublisher
     */
    void Start( TStartReason aReason );

    /**
     * @see CHsContentPublisher
     */
    void Stop( TStopReason aReason );

    /**
     * @see CHsContentPublisher
     */
    void Resume( TResumeReason aReason );

    /**
     * @see CHsContentPublisher
     */
    void Suspend( TSuspendReason aReason );

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
    TAny* GetProperty( TProperty aProperty );

    /**
     * @see CHsContentPublisher
     */
    void HandleEvent( TInt aEvent, const TDesC& aParam );

private:    
    // from MAiProfilePluginNotifier  
  
    /**
    * @see MAiProfilePluginNotifier
    */
	void NotifyContentUpdate();
	
public:
    // new functions
    
    /**
    * Publishes profile names
    *
    * @param void
    * @return void
    */
    void PublishL();
	
private:
    // new functions
    
    /**
    * Resume the plug-in.
    *
    * @param void
    * @return void
    */    
    void DoResumeL();
    
    /**
    * Clean profile names from published content
    *
    * @param void
    * @return void
    */  
    void CleanPublishedProfileNames();

private: 
    // data

    /** Iterator for plugin content, owned */    
    MAiContentItemIterator* iContent;
    /** Iterator for plugin events, owned */    
    MAiContentItemIterator* iEvents;    
    /** Iterator for plug-in resources, owned */    
    MAiContentItemIterator* iResources;    
    /** Plugin engine, owned */    
    CAiProfileEngine* iEngine;
    /** Array of content observers */    
    RPointerArray<MAiContentObserver> iObservers;    
    /** Current profile count */
    TInt iCurrentCount;
    /** Profile count that is used as reference when profile list is updated */
    TInt iPreviousCount;			
	/** Active profile, owned */
    HBufC* iActiveProfileAndChar;
	/** Previous profile, owned */
	HBufC* iPreviousProfileNameAndChar;
    };

#endif // CAIPROFILEPLUGIN_H

// End of file


