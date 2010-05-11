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
* Description:  AiFw internal publisher info
*
*/


#ifndef _AIFWPUBLISHERINFO_INL
#define _AIFWPUBLISHERINFO_INL

// ---------------------------------------------------------------------------
// TAiFwCallback::TAiFwCallback
//
// ---------------------------------------------------------------------------
//
inline TAiFwCallback::TAiFwCallback()
    : iFunction( NULL ), iPtr( NULL )
    {
    }
    
// ---------------------------------------------------------------------------
// TAiFwCallback::TAiFwCallback
//
// ---------------------------------------------------------------------------
//    	
inline TAiFwCallback::TAiFwCallback( 
    void (*aFunction)( TAny* aPtr, TInt aResult ), TAny* aPtr )
    : iFunction( aFunction ), iPtr( aPtr )
    {
    }

// ---------------------------------------------------------------------------
// TAiFwCallback::operator=
//
// ---------------------------------------------------------------------------
//    	   
inline TAiFwCallback& TAiFwCallback::operator=( const TAiFwCallback& aCb )    
    {
    iPtr = aCb.iPtr;
    iFunction = aCb.iFunction;
    
    return *this;
    }

// ---------------------------------------------------------------------------
// TAiFwCallback::Callback
//
// ---------------------------------------------------------------------------
//    	       
inline void TAiFwCallback::Callback( TInt aResult ) const
    {
    if ( iFunction )
        {                	            
        (*iFunction)( iPtr, aResult );	        
        }
    }
	        
// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::TAiFwPublisherInfo
//
// ---------------------------------------------------------------------------
//    	       
inline TAiFwPublisherInfo::TAiFwPublisherInfo()
    : iInfo( THsPublisherInfo() ), iCb( TAiFwCallback() ), iReason( 0 )    
    {
    }

// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::TAiFwPublisherInfo
//
// ---------------------------------------------------------------------------
//    	                   
inline TAiFwPublisherInfo::TAiFwPublisherInfo( const THsPublisherInfo& aInfo, 
    const TAiFwCallback& aCb, TInt aReason )                
    : iInfo( aInfo ), iCb( aCb ), iReason( aReason )
    {
    }

// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::operator=
//
// ---------------------------------------------------------------------------
//    	                        
inline TAiFwPublisherInfo& TAiFwPublisherInfo::operator=( 
    const TAiFwPublisherInfo& aInfo )
    {
    iInfo = aInfo.iInfo;
    iCb = aInfo.iCb;
    iReason = aInfo.iReason;
    
    return *this;
    }
    
// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::operator==
//
// ---------------------------------------------------------------------------
//    	                        
inline TBool TAiFwPublisherInfo::operator==( 
    const THsPublisherInfo& aInfo ) const
    {
    return iInfo == aInfo;        
    }

// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::Info
//
// ---------------------------------------------------------------------------
//    	                        
inline const THsPublisherInfo& TAiFwPublisherInfo::Info() const
    {
    return iInfo;            
    }        

// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::Reason
//
// ---------------------------------------------------------------------------
//    	                                
inline TInt TAiFwPublisherInfo::Reason() const
    {
    return iReason;
    }                    

// ---------------------------------------------------------------------------
// TAiFwPublisherInfo::Callback
//
// ---------------------------------------------------------------------------
//    	                                     
inline void TAiFwPublisherInfo::Callback( TInt aResult ) const
    {
    iCb.Callback( aResult );
    }
        
#endif // _AIFWPUBLISHERINFO_INL

// End of file
