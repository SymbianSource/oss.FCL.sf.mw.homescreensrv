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


#ifndef AIFWTESTPLUGIN2_H
#define AIFWTESTPLUGIN2_H

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
class CAiFwTestContentPlugin2 : public CAiContentPublisher,
							   public MAiPropertyExtension,
							   public MAiEventHandlerExtension
{
public:   // Constructors and destructor
    static CAiFwTestContentPlugin2* NewL();
    ~CAiFwTestContentPlugin2();

public:  // from CAiContentPublisher
    void Resume(TAiTransitionReason aReason);
    void Suspend(TAiTransitionReason aReason);
    void Stop(TAiTransitionReason aReason);
    void SubscribeL(MAiContentObserver& aObserver);
    void  ConfigureL(RAiSettingsItemArray& aSettings);
    TAny* Extension(TUid aUid);

public:  // from MAiPropertyExtension
    virtual TAny* GetPropertyL(TInt aProperty);
    virtual void SetPropertyL(TInt aProperty, TAny* aValue);
    
public:  // from MAiEventHandlerExtension

    void HandleEvent(TInt aEvent, const TDesC& aParam);

private:

    /**
     * C++ default constructor.
     */
    CAiFwTestContentPlugin2();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    static TInt Callback(TAny* aPtr);

private:     // Data

    TBool iFirstExt;

};

#endif      // ?CLASSNAME_H

// End of File.