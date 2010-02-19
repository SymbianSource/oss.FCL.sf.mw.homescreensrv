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
* Description:  Inline function implementations for hscontentpublisher.h
*
*/


#ifndef _HSCONTENTPUBLISHER_INL
#define _HSCONTENTPUBLISHER_INL

// ---------------------------------------------------------------------------
// CHsContentPublisher::NewL
//
// ---------------------------------------------------------------------------
//
inline CHsContentPublisher* CHsContentPublisher::NewL( 
    const THsPublisherInfo& aPublisherInfo )
    {
    TAny* ptr = REComSession::CreateImplementationL( aPublisherInfo.Uid(),
        _FOFF( CHsContentPublisher, iDestructKey ) );

    CHsContentPublisher* self =  
        reinterpret_cast< CHsContentPublisher*>( ptr );
    
    self->iPublisherInfo = aPublisherInfo;
    
    return self;
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::~CHsContentPublisher
//
// ----------------------------------------------------------------------------
//
inline CHsContentPublisher::~CHsContentPublisher()
    {
    REComSession::DestroyedImplementation( iDestructKey );
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::SetOnline
//
// ----------------------------------------------------------------------------
//
inline void CHsContentPublisher::SetOnline()
    {    
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::SetOffline
//
// ----------------------------------------------------------------------------
//
inline void CHsContentPublisher::SetOffline()
    {    
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::SetProperty
//
// ----------------------------------------------------------------------------
//
inline void CHsContentPublisher::SetProperty( TProperty /*aProperty*/, 
    TAny* /*aAny*/ )
    {
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::GetProperty
//
// ----------------------------------------------------------------------------
//
inline TAny* CHsContentPublisher::GetProperty( 
    TProperty /*aProperty*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::HandleEvent
//
// ----------------------------------------------------------------------------
//
inline void CHsContentPublisher::HandleEvent( TInt /*aEvent*/, 
    const TDesC& /*aParam*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::HandleEvent
//
// ----------------------------------------------------------------------------
//
inline void CHsContentPublisher::HandleEvent( const TDesC& /*aEventName*/, 
    const TDesC& /*aParam*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::HasMenuItem
//
// ----------------------------------------------------------------------------
//
inline TBool CHsContentPublisher::HasMenuItem( const TDesC& /*aMenuItem*/ )
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::Extension
//
// ----------------------------------------------------------------------------
//
inline TAny* CHsContentPublisher::Extension( TUid /*aUid*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CHsContentPublisher::PublisherInfo
//
// ----------------------------------------------------------------------------
//
inline const THsPublisherInfo& CHsContentPublisher::PublisherInfo() const
    {
    return iPublisherInfo;
    }

#endif // _HSCONTENTPUBLISHER_INL

// End of file
