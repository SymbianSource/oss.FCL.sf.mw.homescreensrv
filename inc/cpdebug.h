/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug printing to a log file
 *
*/

#ifndef C_CP_DEBUG_H
#define C_CP_DEBUG_H

// INCLUDES

#include <e32base.h>
#ifdef CONTENT_PUBLISHER_DEBUG
#include <f32file.h>

_LIT(KCPDebugDirName, "contentpublisher" );
_LIT(KCPDebugFileName, "c:\\contentpublisher.txt" );
_LIT(KCPDebugFileName2, "c:\\server.txt" );

/**
 *  Content publisher debug
 *
 *
 *  @lib cpdebug.lib
 *  @since  S60 v3.2
 */
NONSHARABLE_CLASS( CCPDebug ): public CBase
    {
public:

    IMPORT_C static CCPDebug* NewLC( const TDesC& aFile );

    IMPORT_C static CCPDebug* NewL( const TDesC& aFile );

    virtual ~CCPDebug();

    /**
     * Determines whether CCPDebug is exist .		
     */
    IMPORT_C static TBool Enable();

    /**
     * Enables or disables Printf message logging to a file.		
     * @param aEnable Set to true to enable logging, false to disable.
     */
    IMPORT_C static void EnableLogging(TBool aEnable);

    /**
     * Print debug text to the file (c:\\contentpublisher.txt). 
     *
     */
    IMPORT_C static void Printf(TRefByValue<const TDesC8> aFormat, ...);

private:

    CCPDebug();

    void ConstructL( const TDesC& aFile );

    /**
     * Returns a pointer to the thread-local data struct.
     */
    IMPORT_C static struct DebugData* Data();

private: // data

    /**
     * Thread local storage for this DLL, as we cannot declare static
     * global variables in Symbian.
     */
    struct DebugData* iData;
    
    };

#define CP_DEBUG(s)    CCPDebug::Printf(s)

/**
 * Thread local storage space. Writable static data is not supported in
 * Symbian, so static data is stored in this struct.
 */
struct DebugData
    {
    RFile iLogFile;
    RFs iFs;
    TBool iLogEnabled;
    TTime iFirstUpdateTime;
    RBuf iFileName;
    };

#else

#define CP_DEBUG(s) 

#endif

#endif // C_CP_DEBUG_H
