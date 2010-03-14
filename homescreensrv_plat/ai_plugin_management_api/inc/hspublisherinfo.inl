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
* Description: Inline function implementations for hspublisherinfo.h
*
*/


#ifndef _HSPUBLISHERINFO_INL
#define _HSPUBLISHERINFO_INL

// ---------------------------------------------------------------------------
// THsPublisherInfo::THsPublisherInfo
//
// ---------------------------------------------------------------------------
//
inline THsPublisherInfo::THsPublisherInfo()
    : iUid( TUid::Null() ), iName( KNullDesC ), iNamespace( KNullDesC8 )             
    {
    }

// ---------------------------------------------------------------------------
// THsPublisherInfo::THsPublisherInfo
//
// ---------------------------------------------------------------------------
//
inline THsPublisherInfo::THsPublisherInfo( const TUid& aUid, 
    const TDesC& aName, const TDesC8& aNamespace )
    {    
    iUid = TUid::Uid( aUid.iUid );
    iName.Copy( aName );
    iNamespace.Copy( aNamespace );    
    }
    
// ---------------------------------------------------------------------------
// THsPublisherInfo::operator=
//
// ---------------------------------------------------------------------------
//
inline THsPublisherInfo& THsPublisherInfo::operator= ( 
    const THsPublisherInfo& aInfo )
    {
    iUid = TUid::Uid( aInfo.iUid.iUid );
    iName.Copy( aInfo.iName );
    iNamespace.Copy( aInfo.iNamespace );   

    return *this;  
    }

// ---------------------------------------------------------------------------
// THsPublisherInfo::operator==
//
// ---------------------------------------------------------------------------
//
inline TBool THsPublisherInfo::operator== ( 
    const THsPublisherInfo& aInfo ) const
    {
    if( iUid == aInfo.iUid && 
        iName == aInfo.iName &&
        iNamespace == aInfo.iNamespace )
        {
        return ETrue;
        }
        
    return EFalse;                                       
    }

// ---------------------------------------------------------------------------
// THsPublisherInfo::Uid
//
// ---------------------------------------------------------------------------
//
inline TUid THsPublisherInfo::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// THsPublisherInfo::Name
//
// ---------------------------------------------------------------------------
//
inline const TDesC& THsPublisherInfo::Name() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// THsPublisherInfo::Namespace
//
// ---------------------------------------------------------------------------
//
inline const TDesC8& THsPublisherInfo::Namespace() const
    {
    return iNamespace;
    }

#endif  // _HSPUBLISHERINFO_INL

// End of file
