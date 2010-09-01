/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: EUnit test for homescreen settings API, main suite
*
*/

#ifndef __CHomescreenSettingsObserver_H__
#define __CHomescreenSettingsObserver_H__

//  External includes
#include <e32def.h>

//  Internal includes
#include <MHomescreenSettingsObserver.h>

//  Forward declarations

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition
/**
 *
 * Homescreen settings API test class. 
 *
 */
NONSHARABLE_CLASS( CHomescreenSettingsObserver )
    : public HSPluginSettingsIf::MHomeScreenSettingsObserver
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static CHomescreenSettingsObserver* NewL();

    /**
     * Two phase construction
     */
    static CHomescreenSettingsObserver* NewLC();

    /**
     * Destructor
     */
    ~CHomescreenSettingsObserver();

private:

    /**
     * Private constructor
     */
    CHomescreenSettingsObserver();

    /**
     * Leaving constructor
     */
    void ConstructL();

public:

private:

    /**
     * Interface method SettingsChangedL
     */
    void SettingsChangedL( const TDesC8& aEvent, const TDesC8& aPluginName,
        const TDesC8& aPluginUid, const TDesC8& aPluginId );                                  

private:

    };

#endif      //  __CHomescreenSettingsObserver_H__
