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

#ifndef __MENUSRVENG_H__
#define __MENUSRVENG_H__

//  INCLUDES

#include <e32base.h>
#include "menuengobserver.h"
#include "menuengobject.h"
#include "menusrvengutils.h"
#include "menusrvattr.h"
// FORWARD DECLARATION

class CMenuEng;
class CMenuSrv;
class CMenuSrvSession;
class CTimeout;
class CMenuSrvAppScanner;
class CMenuSrvFolderCrNotifier;
class CMcsSatMonitor;
class CMcsDrmHandler;
class CMcsCacheHandler;
class CMcsChildrenHandler;
class CMcsRunningAppsHandler;
class CMcsGetlistHandler;
/**
* In release code the macro must be undefined and application
* scanning must be enabled!
* Define the macro only for testing purposes!
*/
#undef __TEST_DISABLE_APPSCAN

// GLOBAL FUNCTIONS

/**
* Menu engine instance.
* Shared by the sessions using the same content file.
* It is also "shared" by its own iTimedClose timer. The timer keeps this
* object alive without any sessions, for a short time.
* It is a reference counting object, but that is only used for sanity
* checking: this->CObject::iAccessCount == iSessions.Count() + 1.
*/
NONSHARABLE_CLASS( CMenuSrvEng ) : public CObject, public MMenuEngObserver
    {
public:
    enum TExtendedAttributes
        {
        EMenuAttrTitleName,
        EMenuAttrShortName,
        EMenuAttrLongName,
        EMenuAttrIconFile,
        EMenuAttrIconId,
        EMenuAttrMaskId
        };

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuSrvEng();

    /**
    * Two-phased constructor. Leaves on failure.
    * @param aMenuSrv Menu Server.
    * @param aContentName Content file name.
    * @return The constructed object.
    */
    static CMenuSrvEng* NewL( CMenuSrv& aMenuSrv, const TDesC& aContentName );

private:    // construction

    /**
    * Constructor.
    * @param aMenuSrv Menu Server.
    */
    CMenuSrvEng( CMenuSrv& aMenuSrv );

    /**
    * Second-phase constructor.
    * @param aContentName Content file name.
    */
    void ConstructL( const TDesC& aContentName );

public:     // new methods

    /**
    * Get content file name (without extension).
    * @return Content file name.
    */
    const TDesC& ContentName() const { return iContentName; }

    /**
    * Get the Engine.
    * @return Engine.
    */
    CMenuEng& Engine() { return *iEng; }

    /**
    * Add a session. Ownership not taken, this object only keeps a list of
    * sessions that are dependent on its engine.
    * @param aSession Session.
    */
    void AddSessionL( CMenuSrvSession* aSession );

    /**
    * Remove a session. Safe to call if not added.
    * When no more sessions remain dependent on this object, timed
    * self-deletion is scheduled.
    */
    void RemoveSession( CMenuSrvSession* aSession );


    /**
    * Get attribute name list.
    * @since S60 v5.0
    * @param aList Attribute name list.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    *
    */
    void GetAttributeListL(
  		  TInt aId,
  		  RArray<TPtrC>& aList );

    /**
    * Get attribute value.
    * @since S60 v5.0
    * @param aAttrName Attribute name.
    * @param aAttrExists Will be set to indicate whether attribute exists
    * or not.
    * @return Attribute value. Empty string if attribute does not exist.
    */
    TPtrC GetAttributeL( TInt aId, const TDesC& aAttrName, TBool& aAttrExists );

    void GetAttributeL( TInt aId, const TDesC& aAttrName,
        TBool& aAttrExists, TDes& aAttrVal );

     /**
     * Get array of running apps
     *
     */
    void GetRunningAppsL(  RArray<TUid>& aArray );


    /**
     * Fetches children count for a folder
     */
    TUint GetChildrenCountL( TInt aId );

    /**
    * Builds output list for GetList operation and returns result
    * @param aSerializedInput serialized list with input parameters
    * 	for GetList operation
    * @return size of output list
    */
    TInt GetListSizeL( const TDesC8& aSerializedInput );

    /**
    * Returns serialized list containing result of GetList oepration
    * @return serialized output list
    */
    TPtrC8 GetListDataL( );

    /**
    * Closes output buffer.
    */
    void CloseOutputBuffer( );

    /**
    * Cleans attribute cache. Used in case of no memory.
    */
    void CleanAttributeCache( );

private:    // from MMenuEngObserver


    void GetExtendedAttributesL( TInt aId, const TDesC& aAttrName,
        TBool& aAttrExists, TDes& aAttrVal );

    void AddToCacheL( TInt aId, const TDesC& aAttrName, TDes& aAttrVal );


    /**
    * Fetches an application native attribute value.
    * @param aId item id.
    * @param aAttrExists ETrue if attribute exist.
    * @param aAttrVal attribute value.
    */
    void ApplicationNativeAttributeL(
    		TInt aId,
    		TBool & aAttrExists,
    		TDes & aAttrVal );

    /**
    * Engine event. Dispatch events to all dependent sessions.
    * @param aFolder Events relate to this folder. May be 0.
    * @param aEvents Events.
    */
    void EngineEvents( TInt aFolder, TInt aEvents );

    /**
    * Unrecoverable engine error. Dispatch to all dependent sessions.
    * @param aErr Error code.
    */
    void EngineError( TInt aErr );

    /**
     * Engine tree reload event.
     * Run appscanner again.
     */
    void EngineTreeReloaded();
    /**
    * Fetches an attribute value from the caption info attributes
    */
    void CaptionInfoL( TInt aId, const TDesC& aAttrName,
        TBool& aExists, TDes& aAttrVal );


    /**
    * Fetches an attribute value from the skin icon info attributes
    */
    void SkinInfoL( TInt aId, TInt aSelect, TBool& aExists, TDes& aAttrVal  );

    /**
    * Fetches the DRM protection attribute value
    */
    void AppDrmProtectionL( TInt aId, TBool& aExists, TDes& aAttrVal );

    /**
    * Fetches an attribute value from the caption info attributes for application
    */
    TPtrC AppCaptionInfoL( const CMenuEngObject& aEngObj,
        const TDesC& aAttrName, TBool& aExists );

    /**
     * Fetches the children_count attribute value (for folders)
     */
    void FolderChildrenCountL( TInt aId, TBool& aExists, TDes& aAttrVal );

    /**
    * Fetches the running status for application and folder
    */
    void GetAppRunningL( TInt aId, TBool& aExists, TDes& aAttrVal );

    /**
    * Appends extended attributes to attributes list
    */
    void AppendExtendedAttrributesL(
    		const TDesC& aType,
    		RArray<TPtrC>& aList );


private:    // Data

    CMenuSrv& iMenuSrv; ///< Menu Server.
    RBuf iContentName; ///< Content name. Own.
    RPointerArray<CMenuSrvSession> iSessions; ///< Sessions using this engine.
    CMenuEng* iEng; ///< Engine. Own.
    CTimeout* iTimedClose; ///< Closing timer. Closes this object.
    CMenuSrvAppScanner* iAppScanner; ///< App scanner. Own.
    CMenuSrvFolderCrNotifier* iFolderNotifier; ///< Folder scanner. Own.
    CMenuSrvEngUtils* iCMenuSrvEngUtils;///CMenuSrvEngUtils. Own.
    CMcsSatMonitor* iMcsSatMonitor;
    CMcsDrmHandler* iDrmHander;
    CMcsRunningAppsHandler* iRunningAppsHandler;//own
    CMcsCacheHandler* iCacheHandler;
    CMcsChildrenHandler* iChildrenHandler;
    CMcsGetlistHandler* iGetlistHandler;
    };

#ifdef __TEST_DISABLE_APPSCAN
#pragma message("Undefine __TEST_DISABLE_APPSCAN !")
#endif

#endif // __MENUSRVENG_H__

// End of File
