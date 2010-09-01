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

#ifndef _AICONTENTOBSERVER_INL
#define _AICONTENTOBSERVER_INL

// ---------------------------------------------------------------------------
// MAiContentObserver::PublishPtr
//
// ---------------------------------------------------------------------------
//
inline TInt MAiContentObserver::PublishPtr( CHsContentPublisher& aPlugin, 
    TInt aContent, TAny* aPtr, TInt aIndex )        
    {
    // Package the pointer to a buffer and delegate to buffer publish method
    return this->Publish( aPlugin, aContent, TPckgC<TAny*>( aPtr ), aIndex );
    }

// ---------------------------------------------------------------------------
// MAiContentObserver::UnpackPtr
//
// ---------------------------------------------------------------------------
//
template< class PtrT > inline PtrT* MAiContentObserver::UnpackPtr( 
    const TDesC8& aBuf )
    {
    TAny* result( NULL );
    
    if ( aBuf.Size() == sizeof( TAny* ) )
        {
        // Effectively writes aBuf contents to result
        TPckg< TAny* >( result ).Copy( aBuf ); 
        }
        
    return static_cast< PtrT* >( result );
    }

#endif // _AICONTENTOBSERVER_INL

// End of file    
