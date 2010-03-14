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


#ifndef CMCSPLUGIN_H
#define CMCSPLUGIN_H

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aicontentmodel.h>

// Forward declarations
class MAiContentObserver;
class MAiContentItemIterator;
class CMCSPluginEngine;
class TMCSData;

/**
 * @ingroup group_mcsplugin
 *
 * MCS Plug-in main class
 *
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CMCSPlugin ) : public CHsContentPublisher    
    {
public:
    // constructor and destructor

    static CMCSPlugin* NewL();

    ~CMCSPlugin();

private:
    // constructors
    
    /**
    * C++ default constructor
    */
    CMCSPlugin();

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
    void HandleEvent( const TDesC& aEventName, const TDesC& aParam );
    
public:
    // new functions

    /**
    * Publishes data
    */
    void PublishL();
    
private:
    // new functions
    
    /**
     * CompareItems
     */
    static TInt CompareItems( const MAiPluginSettings& aFirst,
        const MAiPluginSettings& aSecond );
          
    /**
     * Publishes content for one menu item
     */
    void PublishLItemL( MAiContentObserver& aObserver, TMCSData& aDataItem, TInt aIndex );
        
    /**
    * Delete content model
    */  
    void DeleteContentModel();    
    
    /**
     * Determines if dirty items exists
     */
    TBool PublishRequired() const; 
    
private: 
    // data

    /** Iterator for plugin content, owned */    
    MAiContentItemIterator* iContent;
    /** Number of data in the content model */
    TInt iDataCount;
    /** Dynamic content model, owned */    
    TAiContentItem* iContentModel;
    /** Plugin engine, owned */    
    CMCSPluginEngine* iEngine;
    /** Array of content observers, not owned */    
    RPointerArray<MAiContentObserver> iObservers;
    };

#endif // CMCSPLUGIN_H

// End of file
