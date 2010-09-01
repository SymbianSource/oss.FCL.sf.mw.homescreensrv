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
* Description:  The API supports item information that is not supported by
*                the MCS server itself
*
*/

#ifndef CMENUSRVAPPATTRIBUTES_H_
#define CMENUSRVAPPATTRIBUTES_H_

#include <e32base.h>
#include "menuengobject.h"


NONSHARABLE_CLASS( CMenuSrvAppAttributes ) : public CBase
    {

public:

    /**
    * Two-phased constructor.
    * @param aCapability application capability buffer.
    * @param aAppInfo apparc application info.
    */
    static CMenuSrvAppAttributes* NewL( TApaAppCapabilityBuf& aCapability,
            TApaAppInfo& aAppInfo );

    /**
    * Two-phased constructor.
    * @param aCapability application capability buffer.
    * @param aAppInfo apparc application info.
    */
    static CMenuSrvAppAttributes* NewLC( TApaAppCapabilityBuf& aCapability,
            TApaAppInfo& aAppInfo );

    /**
    * Destructor.
    */
    ~CMenuSrvAppAttributes();

    /**
    * Gets application uid.
    * @return application uid.
    */
    TUid GetUid() const;

    /**
    * Gets hidden attribute of application.
    * @return ETrue if application is hidden.
    */
    TBool IsHidden() const;

    /**
    * Sets hidden attribute of application.
    * @param application hidden attribute.
    */
    void SetHidden( TBool aAppIsHidden );

    /**
    * Sets application type.
    * @param application type.
    */
    void SetAppType( CMenuEngObject::TAppType aAppType );

    /**
    * Gets application type.
    * @return application type.
    */
    CMenuEngObject::TAppType GetAppType() const;

    /**
    * Gets application group name.
    * @return application group name.
    */
    const TPtrC GetGroupName() const;

    /**
    * Gets application full name.
    * @return application full name.
    */
    const TPtrC GetFullName() const;

    /**
    * Gets application caption.
    * @return application caption.
    */
    const TPtrC GetCaption() const;

    /**
    * Gets application short caption.
    * @return application short caption.
    */
    const TPtrC GetShortCaption() const;

    /**
    * Match items function used for finding in array.
    * @return ETrue if items match.
    */
    static TBool MatchItems(const CMenuSrvAppAttributes& aItem1,
            const CMenuSrvAppAttributes& aItem2);

private:

    /**
    * Second phased constructor.
    * @param aCapability application capability buffer.
    * @param aAppInfo apparc application info.
    */
    void ConstructL( TApaAppCapabilityBuf& aCapability,
            TApaAppInfo& aAppInfo );

    /**
    * Constructor.
    */
    CMenuSrvAppAttributes();

private:    // data

    RBuf iGroupName; // Series 60 extension to allow apps
    //to be categorized according a logical group name, e.g. 'games'

    /** The full path name of the application DLL. */
    RBuf iFullName;

    /** The caption for the application. */
    RBuf iCaption;

    /** The short caption for the application. */
    RBuf iShortCaption;

    /** The application specific UID. */
    TUid iUid;

    /** The application type see TAppType. */
    CMenuEngObject::TAppType iAppType;

    TBool iAppIsHidden; // not shown in the Shell etc.
    };

#endif /* CMENUSRVAPPATTRIBUTES_H_ */
