/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef AIFWTESTPLUGIN_H
#define AIFWTESTPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include "aicontentpublisher.h"
#include "aicontentmodel.h"
#include "aipropertyextension.h"
#include "aieventhandlerextension.h"
#include <aicontentrequest.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAiContentObserver;
class MAiContentItemIterator;

// CLASS DECLARATION

/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 *
 *  @lib ?library
 *  @since Series ?XX ?SeriesXX_version
 */
class CAiFwTestContentPlugin : public CAiContentPublisher,
							   public MAiPropertyExtension,
							   public MAiEventHandlerExtension,
							   public MAiContentRequest
{
public:   // Constructors and destructor

    static CAiFwTestContentPlugin* NewL();

    IMPORT_C static CAiFwTestContentPlugin* Instance();

    virtual ~CAiFwTestContentPlugin();

public:  // from CAiContentPublisher

    void Resume(TAiTransitionReason aReason);
    void Suspend(TAiTransitionReason aReason);
    void Stop(TAiTransitionReason aReason);
    void SubscribeL(MAiContentObserver& aObserver);
    void ConfigureL(RAiSettingsItemArray& aSettings);
    TAny* Extension(TUid aUid);

// from base class MAiContentRequest
    TBool RefreshContent( TInt aContentId );

public:  // Test functions

    IMPORT_C RPointerArray<MAiContentObserver>& Observers();

    IMPORT_C TBool WasLoaded();

    IMPORT_C TBool WasResumed();

    IMPORT_C TBool WasConfigured();

    IMPORT_C TBool EventReceived();

    IMPORT_C TBool ContentRefreshed();

public:  // from MAiPropertyExtension
    TAny* GetPropertyL(TInt aProperty);
    void SetPropertyL(TInt aProperty, TAny* aValue);
    
public:  // from MAiEventHandlerExtension
    void HandleEvent(TInt aEvent, const TDesC& aParam);

private:
    CAiFwTestContentPlugin();
    void ConstructL();
    void LoadEngineL(TAiTransitionReason aReason);
    static TInt Callback(TAny* aPtr);

private:     // Data

    RPointerArray<MAiContentObserver>   iObservers;

    TAiPublisherInfo*                   iInfo;
    MAiContentItemIterator*             iContent;
    MAiContentItemIterator*             iResources;
    MAiContentItemIterator*             iEvents;

    CPeriodic*                          iTicker;

    TInt                                iCounter;
    HBufC8*                             iText;
    
    TBool                               iContentRefreshed;
    
    TBool                               iPluginLoaded;

    TBool                               iPluginResumed;

    TBool                               iSettingFound;
    
    TBool                               iEventReceived;
    TBool                               i2ndEvent;

    // Reserved pointer for future extension
    //TAny* iReserved;

};

#endif      // ?CLASSNAME_H

// End of File.
