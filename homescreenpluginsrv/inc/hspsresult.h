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
* Description:  Class ChspsResult is a Xuikon utility class that defines Additional 
*                Return Code for Xuikon Application Theme Management Services on service
*                request return.
*                It is quaranteed that ChspsResult-object is always accessible after
*                client request whether the result was successful or not.
*                ChspsResult-class has attributes that informs the result as follows:
*                - iSystemError - Symbian OS returned error code
*                - iXuikonError - Xuikon defined error code in Xuikon error space
*                - iIntValue1   - additional information relevant the result. 
*                - iIntValue2   - additional information relevant the result.
* 
*  For more information about Xuikon errors, see hspsUIManagerErrorCodes.h.  
*
*
*/


#ifndef hspsRESULT_H
#define hspsRESULT_H

#include <e32base.h>
 
const TInt KMaxResultDataLength8 = 4*sizeof(TUint32);

class RDesReadStream;
class RDesWriteStream;

/**
 * ChspsResult is utility class serving in error handling.
 *
 * @lib hspsResult.lib
 * @since S60 5.0
 * @ingroup group_hspsresult
 */
class ChspsResult : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsResult* NewL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsResult();

    public: // New functions
              
        /**
        * ChspsResult::ExternalizeL() writes ChspsResult object to a given stream.
        * 
        * @since S60 5.0
        * @param aStream Stream to write to.
        */
        IMPORT_C void ExternalizeL( RDesWriteStream& aStream ) const;
        
        /**
        * ChspsResult::InternalizeL() reads ChspsResult from given stream.
        * 
        * @since S60 5.0
        * @param aStream Stream to read from.
        */
        IMPORT_C void InternalizeL( RDesReadStream& aStream );
        
        /**
        * ChspsResult::GetDataLenght() calculates data length of ChspsResult object.
        * 
        * @since S60 5.0
        * @return Length of the data in ChspsResult object.
        */
        IMPORT_C TInt GetDataLength() const;
        
        /**
        * Resets ChspsResult data members.
        * 
        * @since S60 5.0
        */
        IMPORT_C void ResetData();
               
    private:

        /**
        * C++ default constructor.
        */
        ChspsResult();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        // Symbian OS returned error code
        TInt iSystemError;
        // Xuikon defined error code in Xuikon error space
        TInt iXuikonError;
        // Additional information relevant the result.
        TInt iIntValue1;
        // Additional information relevant the result.
        TInt iIntValue2;
           
    private:     // Data

        // Reserved pointer for future extension
        TAny* iReserved;
    };

#endif      // hspsRESULT_H   
            
// End of File
