/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef TSUTILS_H
#define TSUTILS_H
#include <e32base.h>
namespace TaskSwitcher {
    template <class T>
    void CArrayPtrCleanupMethod(TAny *aPtr)
    {
        CArrayPtr<T> *ptr(static_cast< CArrayPtr<T>* >(aPtr));
        ptr->ResetAndDestroy();
        delete ptr;
    }
    
    template <class T>
    void CleanupResetAndDestroyPushL(CArrayPtr<T> *array) {
        CleanupStack::PushL(TCleanupItem(&CArrayPtrCleanupMethod<T>, array));
    }

    template <class T>
    void RPointerArrayCleanupMethod(TAny *aPtr)
    {
        static_cast< RPointerArray<T>* >(aPtr)->ResetAndDestroy();
    }

    template <class T>
    void CleanupResetAndDestroyPushL(RPointerArray<T> &array) {
        CleanupStack::PushL(TCleanupItem(&RPointerArrayCleanupMethod<T>, &array));            
    }
}
#endif//TSUTILS_H
