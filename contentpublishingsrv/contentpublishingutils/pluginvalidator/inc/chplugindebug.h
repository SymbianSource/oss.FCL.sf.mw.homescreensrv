/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CPSDEBUG_H
#define CPSDEBUG_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>

// Maximum formatted size resulting from a single DEBUG* call
#ifndef MAX_DEBUG_STRING_LENGTH
#define MAX_DEBUG_STRING_LENGTH 4096
#endif

#ifdef _DEBUG
// ----------------------------------------------------------
// Debug file - debug output is disabled if the parent dir does not exist
_LIT(KPerformanceLogDir, "cps");
_LIT(KPerformanceLogFile, "pluginvalidator_perf.txt");

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
static void DoOutputPerf(TDesC8& aData)
    {
    RFileLogger::Write( KPerformanceLogDir,
                       KPerformanceLogFile,
                       EFileLoggingModeAppend,
                       aData );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
static void DebugStringNarrowPerfL(const char* aFmt, ...)
    {
    VA_LIST args;
    VA_START(args, aFmt);

    TPtrC8 fmt(reinterpret_cast<const unsigned char *>(aFmt));
    HBufC8* buf = HBufC8::NewLC(MAX_DEBUG_STRING_LENGTH);
    buf->Des().FormatList(fmt, args);
    buf->Des().Append('\n');

    TTime timestamp;
    timestamp.HomeTime();

    TBuf<64> dateTimeString;
    TBuf8<64> dateTimeString8;

    _LIT(KTimeString, "%:0%J%:1%T%:2%S%.%*C4%:3");
    timestamp.FormatL(dateTimeString, KTimeString);
    dateTimeString8.Copy( dateTimeString );
    buf->Des().Insert( 0, _L8(" -> ") );
    buf->Des().Insert( 0, dateTimeString8 );

    DoOutputPerf(*buf);
    CleanupStack::PopAndDestroy(buf);

    VA_END(args);
    }

#define CPSPERF(x) TRAP_IGNORE( DebugStringNarrowPerfL x )
#else
#define CPSPERF(x)
#endif

#endif // CPSDEBUG_H