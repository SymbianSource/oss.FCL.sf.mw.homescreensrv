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

#ifndef __MENUSRVENGUTILS_H__
#define __MENUSRVENGUTILS_H__

#include <e32base.h>
#include <apgcli.h>
#include <w32std.h>
#include <widgetregistryclient.h>
#include "menuengobject.h"
#include "menueng.h"
#include "menusrvtypefilters.h"
#include "menusrvappattributes.h"


/// Number of characters in UID and in MMC-id strings.
const TInt KUidChars = 10;
const TInt KUidLegacyChars = 8;

class TApaAppInfo;
class CMcsSatHandler;

NONSHARABLE_CLASS( CMenuSrvEngUtils ) : public CBase
    {
public:

    /**
    * Enum indicating application DRM protection.
    */
    enum TDRMProtectionInfo
        {
        EDRMUnknownProtection,
        EDRMNotProtected,
        EDRMProtected,
        EDRMRightsExpired
        };

    /**
    * Two-phased constructor.
    * @param aEng menu engine.
    */
    static CMenuSrvEngUtils* NewL( CMenuEng& aEng );

    /**
    * Destructor.
    */
    virtual ~CMenuSrvEngUtils();

    /**
    * Gets app uid based on engine object.
    * @param aEngObj engine object.
    * @param aAppUid app uid.
    * @return error code.
    */
    static TInt GetAppUid( const CMenuEngObject& aEngObj, TUid& aAppUid );

    /**
    * Gets app info based on engine object.
    * @param aEngObj engine object.
    * @param aAppInfo app uid.
    * @return error code.
    */
    TInt GetAppInfoL( const CMenuEngObject& aEngObj, TApaAppInfo& aAppInfo ) const;

    /**
    * Gets app DRM protection for given item id.
    * @param aItemId item id.
    * @param aProtectionInfo app DRM protection info.
    * @return error code.
    */
    TInt GetDrmProtectionL( TInt aItemId,
                           TDRMProtectionInfo& aProtectionInfo ) const;

    /**
    * Gets Java app content id.
    * @param aUid app uid.
    * @param aContentId content id.
    * @return error code.
    */
    void GetJavaContentIdL( TUid aUid, TDes& aContentId )  const;

    /**
    * Reloads app items.
    */
    void ReloadApaItemsL();

    /**
    * Checks if application is midlet.
    * @param aAppUid app uid.
    * @return ETrue if app is a midlet.
    */
    TBool IsMiddlet( const TUid aAppUid )  const;

    /**
    * Checks wether application is native
    * @param aAppUid applications uid
    * @param aIsNative ETrue if apppication is native
    * @return KErrNone if successful, otherwise one of the other system-wide error codes
    */
    TInt IsNativeL( TUid aAppUid, TBool& aIsNative );

    /**
    * Gets app items list.
    * @return app items list.
    */
    RPointerArray<CMenuSrvAppAttributes>& GetAppItemsL();

private:

    /**
    * Get app info from array
    * @param aAppInfo with application uid to  fetch rest information
    * @return KErrNone if successful,
    *   otherwise one of the other system-wide error codes
    */
    TInt GetAppInfoFromArrayL( TApaAppInfo& aAppInfo ) const;

    /**
    * Get app type from array
    * @param aAppUid with application uid to  fetch rest information
    * @param aType app type
    * @return KErrNone if successful,
    *   otherwise one of the other system-wide error codes
    */
    TInt GetAppTypeFromArrayL( const TUid& aAppUid,
            CMenuEngObject::TAppType& aType  ) const;

    /**
    * Gets app DRM protection for given app uid.
    * @param aAppUid app uid.
    * @param aProtectionInfo app DRM protection info.
    * @return error code.
    */
    TInt GetDrmProtectionL(
        const TUid aAppUid,
        TDRMProtectionInfo& aProtectionInfo ) const;

    /**
    * Gets app DRM protection based on engine object.
    * @param aEngObj engine object.
    * @param aProtectionInfo app DRM protection info.
    * @return error code.
    */
    TInt GetDrmProtectionL( const CMenuEngObject& aEngObj,
                           TDRMProtectionInfo& aProtectionInfo ) const;

    /**
    * Gets SAT app info.
    * @param aAppInfo app info.
    * @return error code.
    */
    TInt GetSatAppInfo( TApaAppInfo& aAppInfo ) const;

    /**
    * Constructor.
    * @param aEng menu engine.
    */
    CMenuSrvEngUtils( CMenuEng& aEng );

    /**
    * Second phased constructor.
    */
    void ConstructL();

    /**
    * Drm protection info.
    * @param aConstenId content id.
    * @param aProtectionInfo DRM protection info.
    * @return error code.
    */
    TInt GetDrmProtectionByContentIdL(
			const TDesC& aConstenId,
			TDRMProtectionInfo& aProtectionInfo )const;

private: // Data

    CMcsSatHandler* iSatHandler; ///< Sat handler. Own.

    CMenuEng& iEng; ///< Engine. Not own.

    RApaLsSession iApaLsSession; ///< ApaLs session. Own.

    RWidgetRegistryClientSession iWidgetSession; ///< Widget session. Own.

    RWsSession iWsSession; ///< Ws session. Own.

    RPointerArray<CMenuSrvAppAttributes> iAppAttributes; ///< App array. Own.

    };

/**********************************************
 *  Static utility functions.
 */
NONSHARABLE_CLASS( MenuSrvUtil )
    {

public:

    /**
    * Converts given uid to string.
    * @param aUid app uid.
    * @param aResult descriptor with result.
    * @param aLegacy legacy format.
    * @param aRadix radix.
    * @return error code.
    */
    static void UidToStringL( TUint aUid,
            TDes& aResult,
            TBool aLegacy,
            TRadix aRadix );
    };

#endif // __MENUSRVENGUTILS_H__
