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
#include <w32std.h> 
#include <apgcli.h>
#include <WidgetRegistryClient.h>
#include "menuengobject.h"
#include "menueng.h"
#include "menuengfilter.h"

/// Number of characters in UID and in MMC-id strings.
const TInt KUidChars = 10;
const TInt KUidLegacyChars = 8;

class TApaAppInfo;
class CMcsSatHandler;

NONSHARABLE_CLASS( CMenuSrvEngUtils ) : public CBase
    {
public:

    virtual ~CMenuSrvEngUtils();
    static CMenuSrvEngUtils* NewL( CMenuEng& aEng );
    
    static TInt GetAppUid( const CMenuEngObject& aEngObj, TUid& aAppUid );

// Caption support

    TInt GetAppInfo( const CMenuEngObject& aEngObj, TApaAppInfo& aAppInfo ) const;
    TInt GetAppInfoL( TInt aItemId, TApaAppInfo& aAppInfo ) const;

// Icon support

    TInt GetFolderIcon( const CMenuEngObject& aEngObj, 
                     HBufC*& aFullFileName,
                     TInt& aBitmapId,
                     TInt& aMaskId ) const;
    TInt GetFolderIconL( TInt aItemId,  
                     HBufC*& aFullFileName,
                     TInt& aBitmapId,
                     TInt& aMaskId ) const;

// DRM protection info

    enum TDRMProtectionInfo
        {
        EDRMUnknownProtection,
        EDRMNotProtected,
        EDRMProtected,
        EDRMRightsExpired
        };

    TInt GetDrmProtectionL( const CMenuEngObject& aEngObj,
                           TDRMProtectionInfo& aProtectionInfo ) const;
    TInt GetDrmProtectionL( TInt aItemId,
                           TDRMProtectionInfo& aProtectionInfo ) const;
                           
    TInt GetDrmProtectionL( 
        const TUid aAppUid, 
        TDRMProtectionInfo& aProtectionInfo ) const;

  //  TBool GetAppRunningL( TInt aItemId ) ;
    
  //  TBool GetAppRunningL( const CMenuEngObject& aEngObj ) ;
    void SetValueSkinId( TDes& aAttrName, TInt aValue );
  
        
    TInt GetApaAppInfo( 
        const CMenuEngObject& aEngObj, 
        TApaAppInfo& aInfo ) const;
    TInt GetApaAppInfo( TUid aUid, TApaAppInfo& aInfo ) const;
    
    static void GetJavaContentIdL( TUid aUid, TDes& aContentId );
    
    /**
    * Fetches aEngObj native attribute value. 
    * @param aEngObj engine object.
    * @param aAttrExists ETrue if attribute exist.
    * @param aAttrVal attribute value.
    */
    void IsNative( const CMenuEngObject& aEngObj, 
        TBool& aAttrExists, TDes& aAttrVal );
    /**
     * 
    */
    static TBool IsMiddlet( const TApaAppInfo& aInfo );

private:

  //  void GetChildrenRunningAppL( TInt id, TBool& aRunningApp ) ;
    
  //  void CheckAppRunningL( TUid aUid, TBool& aRunningApp ) ;
    
    TInt GetSatAppInfo( TApaAppInfo& aAppInfo ) const;
    
    CMenuSrvEngUtils( CMenuEng& aEng );
    void ConstructL();
    
    /**
     */
    TInt GetDrmProtectionByContentIdL(
			const TDesC& aConstenId,
			TDRMProtectionInfo& aProtectionInfo )const;
    
	/**
	* Checks wether application is native
	* @since S60 v5.0.
	* @param aAppUid applications uid 
	* @param aIsNative ETrue if apppication is native
	* @return KErrNone if successful, otherwise one of the other system-wide error codes 
	*/
    TInt IsNative( TUid aAppUid, TBool& aIsNative );
    
private: // Data

    CMcsSatHandler* iSatHandler;
    
    CMenuEng& iEng; ///< Engine. Not own.
    RApaLsSession iApaLsSession;
    RWidgetRegistryClientSession iWidgetSession;
    RWsSession iWsSession;
    };

/**********************************************
 *  Static utility functions.
 */
NONSHARABLE_CLASS( MenuSrvUtil )
    {

public:

    /**
    */
    static void UidToStringL( TUint aUid, TDes& aResult,
     TBool aLegacy, TRadix aRadix );
    };

/**
* Type filter: match objects by type.
*/
class TMenuSrvTypeFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set type filter.
    * @param aType Type.
    */
    void SetType( const TDesC& aType )
        { iType.Set( aType ); }

private:    // data

    TPtrC iType; ///< Type filter.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvTypeAttrFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    * @param aValue Attribute value.
    */
    void SetAttr( const TDesC& aName, const TDesC& aValue )
        { iAttrName.Set( aName ); iAttrValue.Set( aValue ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.
    TPtrC iAttrValue; ///< Attr value.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvAttrFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    * @param aValue Attribute value.
    */
    void SetAttr( const TDesC& aName, const TDesC& aValue )
        { iAttrName.Set( aName ); iAttrValue.Set( aValue ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.
    TPtrC iAttrValue; ///< Attr value.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvTypeAttrExistsFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    */
    void SetAttr( const TDesC& aName )
        { iAttrName.Set( aName ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvAttrExistsFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    */
    void SetAttr( const TDesC& aName )
        { iAttrName.Set( aName ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.

    };

/**
* Hidden attr filter for application items
*/
class TMenuSrvHiddenAppFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

    };


/*
*
*/
class TRunningAppsAttr
    {
public:     
    /*
    *
    */
    TRunningAppsAttr ( TUid aUid, TBool aFswHidden = EFalse, 
        TBool aHidden = EFalse, TBool aIsRunning = EFalse )
		{
		iUid = aUid;
		iHidden = aHidden;
		iFswHidden = aFswHidden;
		iIsRunning = aIsRunning;
		}
    
    /*
    *
    */
    TUid GetUid() const
		{
		return iUid;
		}	
    
    /*
    *
    */
    TBool IsHidden() const
		{
		return iHidden;
		}
    
    /*
    *
    */
    void SetHidden( TBool aHidden ) 
		{
		iHidden = aHidden;
		}

    /*
    *
    */
    TBool IsFswHidden() const
		{
		return iFswHidden;
		}
    
    /*
    *
    */
    void SetFswHidden( TBool aFswHidden )
		{
		iFswHidden = aFswHidden;
		}


    /*
    *
    */
    TBool IsRunning() const
		{
		return iIsRunning;
		}
    
    /*
    *
    */
    void SetRunning( TBool aIsRunning )
		{
		iIsRunning = aIsRunning;
		}

    /*
    *
    */
    static TBool MatchByUid( const TRunningAppsAttr& aArg1,  
    		const TRunningAppsAttr& aArg2)
    	{
    	return aArg1.GetUid() == aArg2.GetUid();
    	}    
    
private:    // data

    TUid iUid;
	TBool iHidden;
	TBool iFswHidden;
	TBool iIsRunning;
	
    };


#endif // __MENUSRVENGUTILS_H__
