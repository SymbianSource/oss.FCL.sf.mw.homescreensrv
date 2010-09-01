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
* Description:  Definition of static utility functions
*
*/


#ifndef __MENUUTIL_H__
#define __MENUUTIL_H__

#include "mcsmenuitem.h"
#include <e32base.h>

class RWriteStream;
class RReadStream;

/**
 *  Static utility functions.
 */
NONSHARABLE_CLASS( MenuUtil )
    {

public:

    /**
    * Externalize integer array into a stream.
    * @param aArray Array to externalize.
    * @param aStream The stream to externalize to.
    */
    static void ExternalizeL
        ( const RArray<TInt>& aArray, RWriteStream& aStream );

    /**
    * Internalize integer array from a stream. Existing data is kept,
    * new ones appended.
    * @param aArray Array to internalize.
    * @param aStream The stream to externalize from.
    */
    static void InternalizeL
        ( RArray<TInt>& aArray, RReadStream& aStream );

    /**
    * Externalize uid array into a stream.
    * @param aArray Array to externalize.
    * @param aStream The stream to externalize to.
    */
    static void ExternalizeL
        ( const RArray<TUid>& aArray, RWriteStream& aStream );
    
    /**
    * Internalize uid array from a stream. Existing data is kept,
    * new ones appended.
    * @param aArray Array to internalize.
    * @param aStream The stream to externalize from.
    */
    static void InternalizeL
    	( RArray<TUid>& aArray, RReadStream& aStream );
    
    /**
    * Externalize header array into a stream.
    * @param aArray Array to externalize.
    * @param aStream The stream to externalize to.
    */
    static void ExternalizeL
        ( const RArray<TMenuItem>& aArray, RWriteStream& aStream );

    /**
    * Internalize header array from a stream. Existing data is kept,
    * new ones appended.
    * @param aArray Array to internalize.
    * @param aStream The stream to externalize from.
    */
    static void InternalizeL
        ( RArray<TMenuItem>& aArray, RReadStream& aStream );

    /**
    * Check if backup operation is in progress.
    * @return ETrue if backup is in progress.
    */
    static TBool BackupInProgressL();

    };

#endif // __MENUUTIL_H__
