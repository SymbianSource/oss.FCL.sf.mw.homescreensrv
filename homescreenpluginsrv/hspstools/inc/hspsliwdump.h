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
* Description:  Header for ChspsLiwDump.
*
*/


#ifndef HSPS_LIWDUMP_H
#define HSPS_LIWDUMP_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <liwservicehandler.h>

// FORWARD DECLARATIONS
class ChspsLogBus;
class ChspsByteBuffer;

// CLASS DECLARATION

/**
 * Helper class for dumping LIW to logging bus.
 * 
 * @lib hspsTools.lib
 * @since S60 5.0
 * @ingroup group_homescreenpluginservice_tools
 */
class ChspsLiwDump : public CBase
    {
public:
    /**
     * Destructor.
     */
    IMPORT_C ~ChspsLiwDump();

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aLogBus   ChspsLogBus to be used as output channel.
     */
    IMPORT_C static ChspsLiwDump* NewL( ChspsLogBus& aLogBus );

    /**
     * Two-phased constructor.
     * 
     * @since S60 5.0
     * @param aLogBus ChspsLogBus to be used as output channel.
     */
    IMPORT_C static ChspsLiwDump* NewLC( ChspsLogBus& aLogBus );

    /**
     * Write liw list as binary to specified log bus.
     * 
     * @since S60 5.0
     * @param aLiwList A list to be printed.
     */
    IMPORT_C void LogLiwListAsText( CLiwGenericParamList& aLiwList );

    /**
     * Write liw list as binary to specified log bus.
     * 
     * @since S60 5.0
     * @param aLiwList A list to be printed.
     */
    IMPORT_C void LogLiwListAsBinary( CLiwGenericParamList& aLiwList );    
    
private:

    /**
     * Constructor for performing 1st stage construction
     * 
     * @param aLogBus ChspsLogBus to be used as output channel.
     */
    ChspsLiwDump( ChspsLogBus& aLogBus );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Write liw list as binary to specified log bus.
     * 
     * @param aLiwList A list to be printed.
     */
    void LogLiwListAsTextL( CLiwGenericParamList& aLiwList );

    /**
     * Write liw list as binary to specified log bus.
     * 
     * @param aLiwList      A list to be printed.
     */
    void LogLiwListAsBinaryL( CLiwGenericParamList& aLiwList );    
    
    /**
     * Parses the variant type.
     * 
     * @param value Variant to be printed.
     */
    void PrintVariantL( TLiwVariant value );
    
    /**
     * Prints liw map.
     * 
     *  @param value Map variant to be printed.
     */
    void PrintMapL( TLiwVariant value );
    
    /**
     * Prints liw string variant.
     *
     * @param value String variant to be printed.
     */
    void PrintStringL( TLiwVariant value );
    
    /**
     * Prints liw string variant. 8 bit variant.
     *
     * @param value String variant to be printed.
     */
    void PrintString8L( TLiwVariant value );
    
    /**
     * Prints liw list variant.
     * 
     * @param value List variant to be printed.
     */
    void PrintListL( TLiwVariant value );
    
    /**
     * Appends indentation (delimiters) to the line of text. 8 bit variant.
     * 
     * @param aText text to be printed.
     * @param aDepth intendation depth.
     * @param aDelim delimiter character or string.
     */
    HBufC8* GetFixedText8LC( const TDesC8& aText, 
                             const TInt aDepth, 
                             const TDesC8& aDelim );
    
    /**
     * Appends indentation (delimiters) to the line of text.
     * 
     * @param aText text to be printed.
     * @param aDepth intendation depth.
     * @param aDelim delimiter character or string.
     */
    HBufC* GetFixedTextLC( const TDesC& aText, 
                           const TInt aDepth, 
                           const TDesC& aDelim );
    
    /**
     * Appends indentation (delimiters) to the line of text.
     * 
     * @param   aBuffer     ChspsByteBuffer instance containing
     *                      source data for byte buffer.
     * @return  HBufC*      Created c++ array.
     */
    HBufC* CreateCPlusPlusDumpL( ChspsByteBuffer& aBuffer ) const;    

private:
    /**
     * Log bus to be used.
     */    
    ChspsLogBus& iLogBus;
    
    /**
     * Depth level of indentation.
     */
    TInt iDepth;    
    };

#endif // HSPS_LIWDUMP_H
