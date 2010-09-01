/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug helpers
*
*/


#ifndef __AI2_DEBUG_H__
#define __AI2_DEBUG_H__

#include <e32def.h>

// Uncomment to turn on RD logging
//#define AI_ENABLE_RD_LOGGING

#ifdef __WINS__
    // On the emulator log to debug output
    #define AI_RD_LOG_TO_DEBUG_OUTPUT
#endif

#ifndef AI_ENABLE_RD_LOGGING
// RD logging is off

// Empty definitions of logging macros when RD logging is turned off
#define __TIME_MARK(_var)
#define __TIME_ENDMARK(_msg, _var)
#define __TIME(_msg, _code) _code;
#define __TICK(_msg)
#define __HEAP(_msg)
#define __DBG_FORMAT(_fmt) TAiEmptyDebugFormat()
#define __PRINT __impl_print_empty
#define __DBG_FORMAT8(_fmt) TAiEmptyDebugFormat()
#define __PRINT8 __impl_print_empty
#define __PRINTS(_str)
#define __PRINT_IF_ERROR(_stmt, _fmt) (void)_stmt;
#define __TRAP_AND_PRINT_IF_ERROR(_stmt, _fmt) TRAP_IGNORE(_stmt);

// Unnamed namespace to keep debug functions out of global namespace
namespace {

/**
 * Zero-overhead definition for an empty debug formatting string.
 * @see macro __DBG_FORMAT
 */
NONSHARABLE_STRUCT(TAiEmptyDebugFormat)
    { 
    };

/// Zero-overhead RD log print implementation when RD logging is turned off
inline void __impl_print_empty(const TAiEmptyDebugFormat&) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2, class T3> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2,T3) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2, class T3, class T4> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2,T3,T4) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2, class T3, class T4, class T5> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2,T3,T4,T5) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2, class T3, class T4, class T5, class T6> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2,T3,T4,T5,T6) { }

/// Zero-overhead RD log print implementation when RD logging is turned off
template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7> inline 
void __impl_print_empty(const TAiEmptyDebugFormat&,T0,T1,T2,T3,T4,T5,T6,T7) { }

}  // namespace

#else
// RD logging is on

#include <e32std.h>
#include <e32debug.h>
#include <flogger.h>

/**
 * Stores the current time as microsecond value.
 *
 * @param _var A name, which will be declared as a TInt, and will receive the 
 *             current time value. After the macro, _var remains in scope until 
 *             the end of its enclosing block.
 * @see __TIME_ENDMARK
 */
#define __TIME_MARK(_var) TTime _var;_var.UniversalTime();

/**
 * Logs the microsecond difference between current value and stored earlier 
 * with __TIME_MARK macro.
 *
 * @param _msg textual message to write to the RD log file.
 * @param _var  the start time, the value is set by __TIME_MARK macro.
 * @see __TIME_MARK
 */
#define __TIME_ENDMARK(_msg, _var) __impl_time_endmark(_L(_msg), _var);

/**
 * Measures the execution time of a code statement
 *
 * @param _msg textual message to write to the RD log file.
 * @param _code code statements to be measured.
 */
#define __TIME(_msg, _code) {TTime _time;_time.UniversalTime();_code;__impl_time_endmark(_L(_msg),_time);};

/*
 * Prints a message and current time as ms to the RD log file.
 *
 * @param _msg textual message to write to the log file.
 */
#define __TICK(_msg) __impl_tick(_L(_msg));

/*
 * Prints a message and current heap memory status to the log file.
 *
 * @param _msg textual message to write to the log file.
 */
#define __HEAP(_msg) __impl_heap(_L(_msg));

/**
 * Defines an UNICODE RD logging format string. Used with __PRINT macro.
 * @see __PRINT
 */
#define __DBG_FORMAT(_fmt) _L(_fmt)

#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    /**
     * Prints a printf-style formatted message to the debug output.
     * Format string needs to be defined with the __DBG_FORMAT macro.
     */
    #define __PRINT RDebug::Print
#else
    /**
     * Prints a printf-style formatted message to the RD log file.
     * Format string needs to be defined with the __DBG_FORMAT macro.
     */
    #define __PRINT __impl_print
#endif

#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    /**
     * Defines a 8-bit RD logging format string. Used with __PRINT8 macro.
     * @see __PRINT8
     */
    #define __DBG_FORMAT8(_fmt) _fmt

    /**
     * Prints a printf-style formatted message to the debug output.
     * The format string (first parameter) needs to be a 8-bit literal
     * C string.
     */
    #define __PRINT8 RDebug::Printf
#else
    /**
     * Defines a 8-bit RD logging format string. Used with __PRINT8 macro.
     * @see __PRINT8
     */
    #define __DBG_FORMAT8(_fmt) _L8(_fmt)

    /**
     * Prints a printf-style formatted message to the debug output.
     * The format string (first parameter) needs to be a 8-bit literal
     * C string.
     */
    #define __PRINT8 __impl_print8
#endif 

/**
 * Prints a string to the RD log.
 * 
 * @param _msg textual message to write to the log file.
 */
#define __PRINTS(_msg) __impl_prints(_L(_msg));

/**
 * Prints a message to the RD log if a statement returns an error code other
 * than KErrNone.
 * 
 * @param _stmt statment to check for error return code.
 * @param _fmt textual message to write to the log file. If the message string
 *             contains %d directive it is replaced with the error code that
 *             _stmt returned.
 */
#define __PRINT_IF_ERROR(_stmt, _fmt) { TInt _error = _stmt; if (_error != KErrNone) __PRINT(_L(_fmt), _error); };

/**
 * Prints a message to the RD log if a statement leaves with an error code.
 * 
 * @param _stmt statment to check for leave.
 * @param _fmt textual message to write to the log file. If the message string
 *             contains %d directive it is replaced with the error code that
 *             _stmt leaved with.
 */
#define __TRAP_AND_PRINT_IF_ERROR(_stmt, _fmt) { TRAPD(_error, _stmt); if (_error != KErrNone) __PRINT(_L(_fmt), _error); };

/**
 * Directory under C:\Logs in the device filesystem where the ActiveIdle2 RD 
 * log file is stored.
 */
#define D_LOG_DIR _L("ActiveIdle2")

/**
 * Name of the ActiveIdle2 RD log file under D_LOG_DIR.
 */
#define D_LOG_FILE _L("debug.log")

// Unnamed namespace to keep debug functions out of global namespace
namespace {

// ----------------------------------------------------------------------------
//
// Performance logging operation, give results in microseconds 10^-6
// The metricts are based on the measurment current value of microseconds in 
// the begining of operation and in the end. The time difference is actual result
//
// The logging operations take about 5ms, which should be considered if inner
// tag are used.
//
// Examples:
//
// __TIME_MARK(t)
// DoOperation(...);
// __TIME_ENDMARK(_L("Message 1"), t);
//
// __TIME(_L("Message 1"), DoOperation(...))
//
// ----------------------------------------------------------------------------
//

//
// Actual implementation of 
// Logs the microsecond difference between current value and stored earlier
// aMsg - textual message stored to the log file
// aStart - the start time, the value is set by previous macros
//
inline void __impl_time_endmark(const TDesC& aMsg, TTime aStart)
{
	TTime now;
	now.UniversalTime();
	TTimeIntervalMicroSeconds delta = now.MicroSecondsFrom(aStart);
#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    RDebug::Print( _L("\t[T]\t%S\tTime:\t%Ld"), &aMsg, delta.Int64() );
#else
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[T]\t%S\tTime:\t%Ld"), &aMsg, delta.Int64());	
#endif
}

//
// Prints the message and current ms value to the log file
//
inline void __impl_tick(const TDesC& aMsg)
{
	//Gets the nanokernel tick count. This is the current value of the machine's millisecond tick counter.
    TUint32 nTick = User::NTickCount();
#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    RDebug::Print( _L("\t[T]\t%S\tMicro Sec:\t%d000"), &aMsg, nTick );
#else
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[T]\t%S\tMicro Sec:\t%d000"), &aMsg, nTick);	
#endif
}

// ----------------------------------------------------------------------------
//
// Logs the amout of used heap to log file. Notes that the Heap value equals
// to the result provided by Goofy tool.
//
// ----------------------------------------------------------------------------
//
inline void __impl_heap(const TDesC& aMsg)
    {
    TInt size;
    User::Heap().AllocSize(size);
#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    RDebug::Print( _L("\t[M]\t%S\tAlloc:\t%d\tHeap:\t%d"), &aMsg, size, User::Heap().Size() );
#else
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[M]\t%S\tAlloc:\t%d\tHeap:\t%d"), &aMsg, size, User::Heap().Size());
#endif
    }

inline void __impl_print(TRefByValue<const TDesC16> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, aFmt, list);
    VA_END(list);
    }

inline void __impl_print8(TRefByValue<const TDesC8> aFmt, ...)
    {
	VA_LIST list;
	VA_START(list,aFmt);
    RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, aFmt, list);
    VA_END(list);
    }

inline void __impl_prints(const TDesC& aMsg)
    {
#ifdef AI_RD_LOG_TO_DEBUG_OUTPUT
    RDebug::Print( aMsg );
#else
	RFileLogger::WriteFormat(D_LOG_DIR, D_LOG_FILE, EFileLoggingModeAppend, _L("\t[I]\t%S"), &aMsg);
#endif
    }
    
}  // namespace

#endif // AI_ENABLE_DEBUG_LOGGING

#endif // !__AI2_DEBUG_H__
