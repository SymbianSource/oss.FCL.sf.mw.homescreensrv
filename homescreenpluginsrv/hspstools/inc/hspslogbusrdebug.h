/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef HSPS_LOGBUSRDEBUG_H
#define HSPS_LOGBUSRDEBUG_H

// INCLUDES
#include <hspslogbus.h>
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 * ChspsLogBus implementation that logs into RDebug.
 * 
 * @lib hspsTools.lib
 * @since S60 5.0
 * @ingroup group_homescreenpluginservice_tools
 */
class ChspsLogBusRDebug : public ChspsLogBus
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    IMPORT_C ~ChspsLogBusRDebug();

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C static ChspsLogBusRDebug* NewL();

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     */
    IMPORT_C static ChspsLogBusRDebug* NewLC();

private: // Methods.
    /**
     * From ChspsLogBus.
     * 
     * This method must be overwritten in inherited class (implementation).
     * Implementation must log given string using logging channel of
     * it's choice.
     * 
     * @param aMessage Message to be logged.
     */
    virtual void _LogText( const TDesC& aMessage); 
    
    /**
     * Constructor for performing 1st stage construction
     */
    ChspsLogBusRDebug();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
    };

#endif // HSPS_LOGBUSRDEBUG_H
