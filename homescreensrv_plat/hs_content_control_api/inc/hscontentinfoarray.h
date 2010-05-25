/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HSCONTENTINFOARRAY_H
#define HSCONTENTINFOARRAY_H

// System includes
#include <e32base.h>
#include <s32strm.h>

// User includes

// Forward declarations
class CHsContentInfo;

/**
 *  Content data
 *
 *  @lib hscontentinfo.lib
 *  @since S60 v5.2
 */
class CHsContentInfoArray : public CBase
    {
    public:
    /**
     * Two-phased constructor.
     *
     * @since S60 v5.2
     */
    IMPORT_C static CHsContentInfoArray* NewL();
    
    /**
     * Two-phased constructor.
     *
     * @since S60 v5.2
     */
    IMPORT_C static CHsContentInfoArray* NewL( RReadStream& aStream );    

    /**
     * Destructor.
     *
     * @since S60 v5.2
     */
    virtual ~CHsContentInfoArray();
    
    /**
     * Externalizes the array into the stream.
     *
     * @since S60 v5.2
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream );
    
    /**
     * Returns reference to content info array
     *
     * @since S60 v5.2
     * @return Content info array
     */
    IMPORT_C RPointerArray< CHsContentInfo >& Array();

    /**
     * Internalizes the array from the stream.
     *
     * @since S60 v5.2
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );
    
    /**
     * Returns size of a descriptor needed to externalize the class
     *
     * @since S60 v5.2
     */
    IMPORT_C TInt Size();

    /**
     * Marshals Content Info array to a descriptor
     * 
     * @since S60 5.2
     * @return Descriptor containing the externalized Content Info array data
     */
    IMPORT_C HBufC8* MarshalL();

private:
    /**
     * Constructor.
     */
    CHsContentInfoArray();

    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private: // Data
 		RPointerArray< CHsContentInfo > iArray;
    };

#endif // HSCONTENTINFOARRAY_H

// End of file
