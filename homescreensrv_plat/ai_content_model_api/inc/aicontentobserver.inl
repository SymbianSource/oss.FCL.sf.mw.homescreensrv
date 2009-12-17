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
* Description:  Inline function implementations for aicontentobserver.h
*
*/


inline TInt MAiContentObserver::PublishPtr
        (MAiPropertyExtension& aPlugin, TInt aContent, TAny* aPtr, TInt aIndex)
    {
    // Package the pointer to a buffer and delegate to buffer publish method
    return this->Publish(aPlugin, aContent, TPckgC<TAny*>(aPtr), aIndex);
    }

template<class PtrT> inline 
PtrT* MAiContentObserver::UnpackPtr(const TDesC8& aBuf)
    {
    TAny* result = NULL;
    if ( aBuf.Size() == sizeof( TAny* ) )
        {
        TPckg<TAny*>(result).Copy(aBuf); // Effectively writes aBuf contents to result
        }
    return static_cast<PtrT*>(result);
    }

// End of file    