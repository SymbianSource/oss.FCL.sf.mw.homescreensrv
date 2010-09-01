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
* Description:  Implementation of ChspsLogBusFile.
*
*/


#include "hspslogbusfile.h"
#include "e32debug.h"
#include "f32file.h"
// Constants

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
_LIT( KDefaultLoggingDirectory, "hsps" );
_LIT( KTimeFormatAndExtension, "%F%D_%M_%Y_%H_%T_%C.log"); // F=FORCE, dd_mm_yyyy_hh_mm_mmmmmm 
const TInt KTimeFormatAndExtensionMaxLength = ( 2 + 1 ) +   // 'dd_'
                                              ( 2 + 1 ) +   // 'mm_'
                                              ( 4 + 1 ) +   // 'yyyy_'
                                              ( 2 + 1 ) +   // 'hh_'
                                              ( 2 + 1 ) +   // 'mm_'
                                              ( 6 + 1 ) +   // 'mmmmmm_'
                                              ( 4 ) +       // '.log'
                                              ( 16 ) ;       // overflow protection
#endif

// Methods

//----------------------------------------------------------------------------
// ChspsLogBusFile::~ChspsLogBusFile
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C ChspsLogBusFile::~ChspsLogBusFile()
    {
    iFileLogger.CloseLog();
    iFileLogger.Close();
    
    delete iLoggingFile; iLoggingFile = NULL;
    delete iLoggingDirectory; iLoggingDirectory = NULL;
    }
#else
EXPORT_C ChspsLogBusFile::~ChspsLogBusFile()
    {
    }
#endif
//----------------------------------------------------------------------------
// ChspsLogBusFile::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBusFile* ChspsLogBusFile::NewL( const TDesC& aLoggingFile,
                                                 const TDesC& aLoggingDirectory )
    {
    ChspsLogBusFile* self = ChspsLogBusFile::NewLC( aLoggingFile, aLoggingDirectory );
    CleanupStack::Pop(); // self;
    return self;
    }

//----------------------------------------------------------------------------
// ChspsLogBusFile::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBusFile* ChspsLogBusFile::NewLC(  const TDesC& aLoggingFile,
                                                   const TDesC& aLoggingDirectory )
    {
    ChspsLogBusFile* self = new (ELeave)ChspsLogBusFile();
    CleanupStack::PushL(self);
    self->ConstructL( aLoggingFile, aLoggingDirectory );
    return self;
    }

//----------------------------------------------------------------------------
// ChspsLogBusFile::CreateLogFilename
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C TFileName ChspsLogBusFile::CreateLogFilename( const TDesC& aBaseline )
    {
    RFs fs;
    if ( KErrNone == fs.Connect() )
        {
        fs.MkDirAll(_L("c:\\logs\\hsps\\"));
        fs.Close();
        }
    
    TFileName fileName;
    
    // Append baseline and trailing '_'.
    fileName.Append( aBaseline );
    fileName.Append( TChar('_') );
    
    // Append Timestamp (formatting string contains extension).
    TTime time;
    time.HomeTime();   
    TBuf<KTimeFormatAndExtensionMaxLength> timestampAndExtension;
    TRAPD( err, time.FormatL( timestampAndExtension, KTimeFormatAndExtension() ) );
    if( err == KErrNone &&
        ( fileName.MaxLength() - fileName.Length() ) > timestampAndExtension.Length() )
        {
        fileName.Append( timestampAndExtension );
        }
    
    // Return created descriptor.
    return fileName;
    }
#else
EXPORT_C TFileName ChspsLogBusFile::CreateLogFilename( const TDesC& /*aBaseline*/ )
    {    
    return KNullDesC();
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBusFile::_LogText
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLogBusFile::_LogText( const TDesC& aMessage)
    {
    iFileLogger.Write( aMessage );
    }
#else
void ChspsLogBusFile::_LogText( const TDesC& /*aMessage*/ )
    {
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBusFile::ChspsLogBusFile
// ----------------------------------------------------------------------------
//
ChspsLogBusFile::ChspsLogBusFile()
    {
    iLoggingFile = NULL;
    iLoggingDirectory = NULL;
    }

//----------------------------------------------------------------------------
// ChspsLogBusFile::ConstructL
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLogBusFile::ConstructL( const TDesC& aLoggingFile,
                                  const TDesC& aLoggingDirectory )
    {
    iLoggingFile = aLoggingFile.AllocL();
    if( aLoggingDirectory != KNullDesC() )
        {
        iLoggingDirectory = aLoggingDirectory.AllocL();
        }
    else
        {
        iLoggingDirectory = KDefaultLoggingDirectory().AllocL();
        }

    User::LeaveIfError( iFileLogger.Connect() );
    iFileLogger.CreateLog( *iLoggingDirectory,
                           *iLoggingFile,
                           EFileLoggingModeOverwrite );
    iFileLogger.SetDateAndTime( EFalse, ETrue );       
    }
#else
void ChspsLogBusFile::ConstructL( const TDesC& /*aLoggingFile*/,
                                  const TDesC& /*aLoggingDirectory*/ )
    {       
    }
#endif
