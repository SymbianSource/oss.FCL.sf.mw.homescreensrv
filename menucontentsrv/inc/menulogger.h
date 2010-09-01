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
* Description:  Logging macros
*
*/


#ifndef __SSYNC_LOGGER_H__
#define __SSYNC_LOGGER_H__

#include <e32std.h>
#include <e32def.h>

#include <flogger.h>

#undef __LOGGING__

/// Logging directory.
_LIT( KLogDir,                   "mcs" );
/// Log file name.
_LIT( KLogFile,                  "mcs.txt" );
/// Format string: enter function.
_LIT( KLogEnterFn,               "-> %S" );
/// Format string: leave function.
_LIT( KLogLeaveFn,               "<- %S" );
/// Format string: time.
_LIT( KLogTimeFormatString,      "%H:%T:%S:%*C2" );
/// Format string: timestamp.
_LIT( KLogTimeStampFormatString, "%S %S" );

#ifdef __LOGGING__

#pragma message("Logging is enabled")

#include <flogger.h>

/// Write log: enter function.
#define CLOG_ENTERFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KLogDir,                    \
        KLogFile,                   \
        EFileLoggingModeAppend,     \
        KLogEnterFn,                \
        &temp                       \
        );                          \
    }

/// Write log: leave function.
#define CLOG_LEAVEFN( a )           \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::WriteFormat        \
        (                           \
        KLogDir,                    \
        KLogFile,                   \
        EFileLoggingModeAppend,     \
        KLogLeaveFn,                \
        &temp                       \
        );                          \
    }

/// Write log: string 'a'.
#define CLOG_WRITE( a )             \
    {                               \
    _LIT( temp, a );                \
    RFileLogger::Write              \
        (                           \
        KLogDir,                    \
        KLogFile,                   \
        EFileLoggingModeAppend,     \
        temp                        \
        );                          \
    }

/// Write log: formatted.
#define CLOG_WRITE_FORMAT( a, b... )   \
    {                                  \
    _LIT( temp, a );                   \
    RFileLogger::WriteFormat           \
        (                              \
        KLogDir,                       \
        KLogFile,                      \
        EFileLoggingModeAppend,        \
        temp,                          \
        b                              \
        );                             \
    }

#define CLOG_WRITE_FORMAT8( a, b... )   \
    {                                   \
    _LIT8( temp, a );                   \
    RFileLogger::WriteFormat            \
        (                               \
        KLogDir,                        \
        KLogFile,                       \
        EFileLoggingModeAppend,         \
        temp,                           \
        b                               \
        );                              \
    }

#else /* not defined __LOGGING__ */

/// Empty definition (disable log).
#define CLOG_ENTERFN( a... )

/// Empty definition (disable log).
#define CLOG_LEAVEFN( a... )

/// Empty definition (disable log).
#define CLOG_WRITE( a... )

/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT( a... )

/// Empty definition (disable log).
#define CLOG_WRITE_FORMAT8( a... )

#endif /* def __LOGGING__ */

#endif // __SSYNC_LOGGER_H__

