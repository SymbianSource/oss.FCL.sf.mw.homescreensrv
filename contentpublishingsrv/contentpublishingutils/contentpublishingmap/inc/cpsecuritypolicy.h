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
* Description:  Class used by session to handle access control list
 *  Version     : %version: 5 % << Don't touch! Updated by Synergy at check-out.
 *
*/


#ifndef C_CPSECURITYPOLICY_H
#define C_CPSECURITYPOLICY_H

#include <e32base.h>

class CLiwDefaultMap;

/**
 *  Internal class for Security related activities
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCPSecurityPolicy ): public CBase
    {
public:
    
    enum TACLPolicy 
        {
        ERead,
        EWrite,
        EDelete    
        };
    
   /**
     * Two-phased constructor.
     */
    static CCPSecurityPolicy* NewL( const RMessage2& aMessage );

    /**
     * Two-phased constructor.
     */
    static CCPSecurityPolicy* NewLC( const RMessage2& aMessage );

    /**
     * Destructor.
     */
    virtual ~CCPSecurityPolicy();

    /**
     * Sets default ACL map and serializes it
     *
     * @param aPolicy target for ACL binary
     */
    void GetDefaultSecurityL( RBuf8& aResult ) const ;
    
    /**
     * Checks if access for client (iMessage) 
     * should be granted or not
     *
     * @param aBinary serialized ACL map
     * @param aPolicy secururity policy to check
     * @return ETrue if security check is ok, 
     * 	EFalse if not 
     */
    TBool CheckSecurityL( TPtrC8& aBinary , TACLPolicy aPolicy ) const;
    
    /**
     * Performs actuall security check
     * should be granted or not
     *
     * @param aMap ACL map
     * @return ETrue if security check is ok, 
     * 	EFalse if not 
     */
    TInt DoCheckSecurityL( const CLiwMap* aMap ) const;

    /**
     * Performs security check based on client UIDs
     *
     * @param aList list of uids which have access
     *
     * @return access granted or not
     */
    TBool CheckUids( const TDesC& aList ) const ;
     
    /**
     * Performs security check based on client Capabilities
     *
     * @param aList list of capabilities which client must have
     *
     * @return access granted or not
     */
    TBool CheckCapabilities( const TDesC& aList ) const;
        
    /**
     * Exctracts from list next token( UID or Capability )
     *
     * @param aList list as desciptor
     * @param aResult single token extracted from list
     * @param aPos position of the reuired token
     */
    void ExctractNextToken( const TDesC& aList, 
                            TPtrC& aResult , TInt& aPos ) const;
    /**
     * Converts capability given as descriptor to enum
     *
     * @param aCapability descriptor to convert
     *
     * @return Capablity as enum
     */
    TCapability GetCapability( const TDesC& aCapability ) const; 
    
private:
    
    /**
     * C++ default constructor.
     */
    CCPSecurityPolicy( const RMessage2& aMessage );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    // data

    /*
     * Not own.
     * Message from client
     */
    const RMessage2& iMessage;

    };

#endif // C_CPSECURITYPOLICY_H
