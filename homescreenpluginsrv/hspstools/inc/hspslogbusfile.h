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
* Description:  Header for ChspsLogBusFile.
*
*/


#ifndef HSPS_LOGBUSFILE_H
#define HSPS_LOGBUSFILE_H

// INCLUDES
#include <hspslogbus.h>
#include <e32std.h>
#include <e32base.h>
#include <flogger.h>

// CLASS DECLARATION

/**
 * ChspsLogBus implementation that logs into RFileLogger.
 * 
 * @lib hspsTools.lib
 * @since S60 5.0
 * @ingroup group_homescreenpluginservice_tools
 */
class ChspsLogBusFile : public ChspsLogBus
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    IMPORT_C ~ChspsLogBusFile();

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param   aLoggingFile            Target file name.
     * @param   aLoggingDirectory       Target folder.
     */
    IMPORT_C static ChspsLogBusFile* NewL( const TDesC& aLoggingFile,
                                           const TDesC& aLoggingDirectory = KNullDesC() );

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param   aLoggingFile            Target file name.
     * @param   aLoggingDirectory       Target folder.
     */
    IMPORT_C static ChspsLogBusFile* NewLC( const TDesC& aLoggingFile,
                                            const TDesC& aLoggingDirectory = KNullDesC() );

    /**
     * Create log file name easily with format "baseline_<timestamp>.log"
     * 
     * @since S60 5.0
     * @param   aBaseline   Baseline text.
     * @return  TFileName   Newly created file name.
     */
    IMPORT_C static TFileName CreateLogFilename( const TDesC& aBaseline );
    
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
    ChspsLogBusFile();

    /**
     * EPOC default constructor for performing 2nd stage construction
     *
     * @param   aLoggingFile          Target file name.
     * @param   aLoggingDirectory     Target folder.  
     */
    void ConstructL( const TDesC& aLoggingFile,
                     const TDesC& aLoggingDirectory );

private: // Data.
    /**
     * Handle to file logger.
     */
    RFileLogger iFileLogger;

    /**
     * Name of logging file.
     */
    HBufC* iLoggingFile;

    /**
     * Name of logging directory.
     */
    HBufC* iLoggingDirectory;
    };

#endif // HSPS_LOGBUSFILE_H
