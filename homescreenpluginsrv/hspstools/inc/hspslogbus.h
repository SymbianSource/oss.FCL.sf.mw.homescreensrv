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
* Description:  Header for Base class for log busses.
*
*/


#ifndef HSPS_LOGBUS_H
#define HSPS_LOGBUS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 * Base class for log busses. Not instanciable.
 * 
 * @lib hspsTools.lib
 * @since S60 5.0
 * @ingroup group_homescreenpluginservice_tools
 */
class ChspsLogBus : public CBase, TDes16Overflow, TDes8Overflow
    {
public:
    /**
     * Destructor.
     */
    IMPORT_C ~ChspsLogBus();

    /**
     * Log formattable 16bit string.
     * 
     * @since S60 5.0
     * @param aFmt  String to be logged. May contain format specifiers, but
     *              those must match with additional arguments given.
     *        ...   Data to be inserted to formatted string.
     */    
    IMPORT_C void LogText( TRefByValue<const TDesC16> aFmt, ... );

    /**
     * Log formattable 8bit string.
     * 
     * @since S60 5.0
     * @param aFmt  String to be logged. May contain format specifiers, but
     *              those must match with additional arguments given.
     *        ...   Data to be inserted to formatted string.
     */    
    IMPORT_C void LogText( TRefByValue<const TDesC8> aFmt, ... );    
    
protected:
    /**
     * This method must be overwritten in inherited class (implementation).
     * Implementation must log given string using logging channel of
     * it's choice.
     * 
     * @param aMessage Message to be logged.
     */    
    virtual void _LogText( const TDesC& aMessage ) = 0;

    /**
     * This method is used to query max. line length from inheriting object.
     * Default implementation returns 128.
     * This value is used to crop descriptors before _LogText is called.
     * 
     * @return  TInt    Max line length supported by this log bus.
     */    
    virtual TInt MaxLineLen() const;    
    
    /**
     * Constructor for performing 1st stage construction.
     */
    IMPORT_C ChspsLogBus();    
    
private: // Methods.
    /**
     * Create a heap descriptor using given arguments.
     * 
     * @param   aFmt    Descriptor
     * @param   aList   Variable length argument list.
     * @return  HBufC*  Pointer to created heap descriptor. OWNERSHIP TRANSFERRED
     *                  TO CALLER.
     */    
    HBufC* FormatMessage( TRefByValue<const TDesC16> aFmt, VA_LIST aList );

    /**
     * From TDes16Overflow.
     * 
     * Handles the overflow.
    
     * This function is called when the TDes16::AppendFormat() 
     * variant that takes an overflow handler argument, fails.    
     * 
     * @param aDes  The 16-bit modifiable descriptor whose overflow results in the 
     *              call to this overflow handler.
     * 
     */    
    void Overflow( TDes16& aDes );    

    /**
     * Create a heap descriptor using given arguments.
     * 
     * @param   aFmt    Descriptor
     * @param   aList   Variable length argument list.
     * @return  HBufC*  Pointer to created heap descriptor. OWNERSHIP TRANSFERRED
     *                  TO CALLER.
     */    
    HBufC* FormatMessage( TRefByValue<const TDesC8> aFmt, VA_LIST aList );

    /**
     * From TDes8Overflow.
     * 
     * Handles the overflow.
    
     * This function is called when the TDes16::AppendFormat() 
     * variant that takes an overflow handler argument, fails.    
     * 
     * @param aDes  The 16-bit modifiable descriptor whose overflow results in the 
     *              call to this overflow handler.
     * 
     */    
    void Overflow( TDes8& aDes );     

    /**
     * Crops text according to max line and then calls
     * inherited class' _LogText. 
     * 
     * @param   aText   Message to be logged.
     */
    void CropAndLogText( const TDesC& aText );
    
private: // Data.    
    /**
     * Internal flag to indicate possible overflow.
     */
    TBool iOverflow;
    };

#endif // HSPS_LOGBUS_H
