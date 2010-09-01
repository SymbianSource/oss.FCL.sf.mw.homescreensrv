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
* Description:  Implementation of ChspsLogBus.
*
*/


#include "hspslogbus.h"

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
const TInt KMessageGranularity = 256; // initial size and grow-by factor. 
const TInt KMessageSanityLimit = 16384; // 16kb.
_LIT( KLogError, "Warning: Logging failed." );
const TInt KDefaultMaxLineLen = 128;
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::~ChspsLogBus
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBus::~ChspsLogBus()
    {
    }

//----------------------------------------------------------------------------
// ChspsLogBus::LogText
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C void ChspsLogBus::LogText( TRefByValue<const TDesC16> aFmt, ... )
    {    
    VA_LIST list;
    VA_START( list, aFmt );
       
    HBufC* formatted = FormatMessage( aFmt, list );    
    if( formatted )
        {
        CropAndLogText( *formatted );
        delete formatted;
        formatted = NULL;
        }
    else
        {
        _LogText( KLogError() );
        }
    
    VA_END( list );    
    }
#else
EXPORT_C void ChspsLogBus::LogText( TRefByValue<const TDesC16> /*aFmt*/, ... )
    {
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::LogText
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C void ChspsLogBus::LogText( TRefByValue<const TDesC8> aFmt, ... )
    {    
    VA_LIST list;
    VA_START( list, aFmt );
       
    HBufC* formatted = FormatMessage( aFmt, list );    
    if( formatted )
        {
        CropAndLogText( *formatted );
        delete formatted;
        formatted = NULL;
        }
    else
        {
        _LogText( KLogError() );
        }
    
    VA_END( list );    
    }
#else
EXPORT_C void ChspsLogBus::LogText( TRefByValue<const TDesC8> /*aFmt*/, ... )
    {
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::ChspsLogBus
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
TInt ChspsLogBus::MaxLineLen() const
    {    
    return KDefaultMaxLineLen;
    }
#else
TInt ChspsLogBus::MaxLineLen() const
    {
    return KErrNotSupported;
    }    
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::ChspsLogBus
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBus::ChspsLogBus()
    {
    // No implementation required
    }

//----------------------------------------------------------------------------
// ChspsLogBus::FormatMessage
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC* ChspsLogBus::FormatMessage( TRefByValue<const TDesC16> aFmt, VA_LIST aList )
    {
    HBufC* message = NULL;
    
    TInt messageSize = KMessageGranularity;    
    
    while( ETrue )
        {        
        TRAPD( err, message = HBufC::NewL( messageSize ) );        
        if( err != KErrNone )
            {
            delete message;
            message = NULL;

            break;
            }

        iOverflow = EFalse;
        message->Des().AppendFormatList( aFmt, aList, this );        
        if( !iOverflow )
            {
            // All ok.
            break;
            }
        else
            {
            messageSize *= 2;
            
            delete message;
            message = NULL;

            // Sanity check.
            if( messageSize > KMessageSanityLimit )
                {
                break;
                }            
            }        
        }
    
    return message; 
    }
#else
HBufC* ChspsLogBus::FormatMessage( TRefByValue<const TDesC16> /*aFmt*/, VA_LIST /*aList*/ )
    {
    return NULL;
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::Overflow
// ----------------------------------------------------------------------------
//
void ChspsLogBus::Overflow( TDes16& /*aDes*/ )
    {
    iOverflow = ETrue;
    }

//----------------------------------------------------------------------------
// ChspsLogBus::FormatMessage
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC* ChspsLogBus::FormatMessage( TRefByValue<const TDesC8> aFmt, VA_LIST aList )
    {
    HBufC8* message8bit = NULL;
    
    TInt messageSize = KMessageGranularity;    
    
    while( ETrue )
        {        
        TRAPD( err, message8bit = HBufC8::NewL( messageSize ) );        
        if( err != KErrNone )
            {
            delete message8bit;
            message8bit = NULL;

            break;
            }

        iOverflow = EFalse;
        message8bit->Des().AppendFormatList( aFmt, aList, this );        
        if( !iOverflow )
            {
            // All ok.
            break;
            }
        else
            {
            messageSize *= 2;
            
            delete message8bit;
            message8bit = NULL;

            // Sanity check.
            if( messageSize >= KMessageSanityLimit )
                {
                break;
                }            
            }        
        }
    
    // Convert 8bit to 16bit for logging.    
    HBufC* message16bit = NULL;
    if( message8bit && message8bit->Length() > 0 )
        {
        TRAPD( err, message16bit = HBufC::NewL( message8bit->Length() ));
        if( err == KErrNone )
            {
            message16bit->Des().Copy( *message8bit );
            }
        else
            {
            delete message16bit;
            message16bit = NULL;
            }
        }
    else if( message8bit && message8bit->Length() == 0 )
        {
        // Source is empty.
        TRAP_IGNORE( message16bit = KNullDesC().AllocL() );
        }
    
    delete message8bit;
    message8bit = NULL;
    
    return message16bit; 
    }
#else
HBufC* ChspsLogBus::FormatMessage( TRefByValue<const TDesC8> /*aFmt*/, VA_LIST /*aList*/ )
    {
    return NULL;
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBus::Overflow
// ----------------------------------------------------------------------------
//
void ChspsLogBus::Overflow( TDes8& /*aDes*/ )
    {
    iOverflow = ETrue;
    }

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLogBus::CropAndLogText( const TDesC& aText )
    {
    const TInt KLineMax = MaxLineLen();
    
    // Fits to one line.
    if( aText.Length() <= KLineMax )
        {
        _LogText( aText );
        return;
        }
    
    // Does not fit. Log cropped.    
    TPtrC ptr = aText;    
    while( ETrue )
        {                
        const TInt KRemainingLen = ptr.Length();
        if( KRemainingLen == 0 )
            {
            break;
            }
        
        if( KRemainingLen <= KLineMax )
            {
            _LogText( ptr );
            break;
            }
        else
            {
            _LogText( ptr.Left( KLineMax ) );
            ptr.Set( ptr.Right( ptr.Length() - KLineMax ) );
            }
        }
    }
#else
void ChspsLogBus::CropAndLogText( const TDesC& /*aText*/ )
    {
    }
#endif
