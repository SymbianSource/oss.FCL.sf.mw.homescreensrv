/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CPS Client iterable
 *
*/


#ifndef C_CPSERVERITERABLE_H
#define C_CPSERVERITERABLE_H

#include <LiwCommon.h>

/**
 * Implemenation of abstract interface to iterate over data items
 * returned by CPS Server
 *
 * @see CLiwIterable
 *
 */
class CCPClientIterable : public CLiwIterable
    {
public:

    /**
     * Two-phase Constructor
     * @param aList list to be iterated 
     * @return new CIterableCalList object
     */
    static CCPClientIterable* NewL( CLiwGenericParamList* aList );

    /**
     * Resets the iterator. 
     *
     * @return void
     */
    void Reset();

    /**
     * Iterates over the collection entries to fetch the next data element.
     *
     * @param aValue contains the next data element
     *
     * @return false if there are no more data elements to be fetched;
     * true otherwise
     */
    TBool NextL( TLiwVariant& aValue );

    /**
     * Destructor.
     */
    ~CCPClientIterable();

private:

    /**
     * C++ default constructor.
     */     
    CCPClientIterable( CLiwGenericParamList* aList );

    /**
     * Exctracts maps stored inside aMap as binaries and
     * inserts them as CLiwDefaultMap 
     *
     * @param aMap Source and target of the operation
     */
    void ExctractBinariesL( CLiwDefaultMap& aMap );

private:

    /**
     * CLiwGenericParamList  class pointer
     * Own.
     */
    CLiwGenericParamList* iList;

    /**
     * Position on iList
     * Own.
     */
    TInt iPos;

    };

#endif // C_CPSERVERITERABLE_H
// end of file
