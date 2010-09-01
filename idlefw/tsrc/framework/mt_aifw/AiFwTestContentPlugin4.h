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


#ifndef AIFWTESTPLUGIN4_H
#define AIFWTESTPLUGIN4_H

//  INCLUDES
#include <e32base.h>
#include "aicontentpublisher.h"
#include "aicontentmodel.h"
#include "aipropertyextension.h"
#include "aieventhandlerextension.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAiContentObserver;

// CLASS DECLARATION

/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 *
 *  @lib ?library
 *  @since Series ?XX ?SeriesXX_version
 */
class CAiFwTestContentPlugin4 : public CAiContentPublisher,
							   public MAiPropertyExtension,
							   public MAiEventHandlerExtension
{
public:   // Constructors and destructor
    static CAiFwTestContentPlugin4* NewL();
    ~CAiFwTestContentPlugin4();

public:  // from CAiContentPublisher
    void Resume(TAiTransitionReason aReason);
    void Suspend(TAiTransitionReason aReason);
    void Stop(TAiTransitionReason aReason);
    void SubscribeL(MAiContentObserver& aObserver);
    void ConfigureL(RAiSettingsItemArray& aSettings);
    TAny* Extension(TUid aUid);

public:  // MAiPropertyExtension
    TAny* GetPropertyL(TInt aProperty);
    void SetPropertyL(TInt aProperty, TAny* aValue);
    
public:  // MAiEventHandlerExtension
    void HandleEvent(TInt aEvent, const TDesC& aParam);

private:
    CAiFwTestContentPlugin4();
    void ConstructL();
    static TInt Callback(TAny* aPtr);

private:     // Data

    TAiPublisherInfo*                   iInfo;

};

#endif      // ?CLASSNAME_H

// End of File.
