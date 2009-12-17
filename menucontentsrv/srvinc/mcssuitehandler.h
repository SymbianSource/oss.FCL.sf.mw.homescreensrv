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
* Description:  
*
*/

#ifndef MCSSUITEHANDLER_H_
#define MCSSUITEHANDLER_H_

#include <e32base.h>
#include <f32file.h>


#include "menusvrsuite.h"
#include "mcsinstallnotifier.h"
#include "menuengoperation.h"
#include "mcsmmcobserver.h"
#include "mcsfreespaceobserver.h"

class CMenuEng;
class TMenuItem;

/**
 *  Suite Handler.
 *  @lib mcsdrmhandler.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsSuiteHandler ): 
    public CBase, MMcsInstallListener, MMenuEngOperation, MMcsMmcObserver,
        MMcsFreeSpaceObserver
    {
public:
    
    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed object.
    */
    static CMcsSuiteHandler* NewL( 
        CMenuEng& aEng, const TDesC& aDirName );
    
    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    virtual ~CMcsSuiteHandler();
    
    /**
     * Check if suite of given name is present
     * in handler.
     */
    TBool HaveSuite(const TDesC& aSuiteName);
    
    /**
     * Get attribute from suite handler.
     */
    void GetAttribute( const TDesC& aSuiteName, const TDesC& aAttrName, 
               TBool& aAttrExists, TDes& aAttrVal );
    
private:

    /**
    * Execute engine operation:
    * Do the application scan and update menu accordingly.
    */
    void RunMenuEngOperationL();
    void CompletedMenuEngOperation( TInt aErr );
    /**
    * Constructor.
    */
	CMcsSuiteHandler( CMenuEng& aEng );

    /**
    * 2nd phase constructor.
    */
    void ConstructL(const TDesC& aDirName);
    
    /**
     * Handle Install Event
     */
    void HandleInstallNotifyL(TInt aEvent);
    
    /**
     * Handle Mmc Event
     */
    void HandleMmcEventL(TInt aEvent);
    
    /**
     * Handle Free Space Event.
     */
    void HandleFreeSpaceEventL();
    /**
     * Lists new suits
     */
    void CheckSuiteDirL();
    
    /**
     * Add new suits
     */
    void AddNew();
    
    /**
     * Remove old suits 
     */
    void RemoveOld();
    
	/**
	* Removes not installed suites from content xml file(basically during restore) 
	* @since S60 v5.0.
	*/
    void ScanSuitesL();
    
    
    /**
     * Check if file is installed
     */
    TBool FileInstalled(const TDesC& aFileName);
    
    /**
     * Check if file was removed from installed file list
     */
    TBool FileUninstalled(const TDesC& aFileName);
    
    /**
     * Check if file need Update
     * @param aTimestamp - actual timestamp of file 
     */
    TBool FileNeedUpdate(const TDesC& aFileName, const TTime& aTimestamp);

    /**
     * Validates file extansion
     */
    TBool ValidExtansion(const TDesC& aFileName);
    
    /**
     * Install file with suites
     */
    void InstallFileL(const TDesC& aFileName, const TTime& aTimestam);
    
    /**
     * Removes file with suites
     */
    void UninstallFileL(const TDesC& aFileName);
    
    /**
     * Update file with suites
     */
    void UpdateFileL(const TDesC& aFileName, const TTime& aTimestam);
    
    /**
     * Get suites from a file
     */
    void GetSuitesL(const TDesC& aFileName, RPointerArray<CSuiteObject>& aSuiteArray);
    
    /*
     * Reopen older suite. Used when deleteing suite that was replaced by newer one.
     */
    void ReopenSuiteL(TInt aSuiteArrayPos);

	/**
	* Append suite items to array
	* @since S60 v5.0.
	* @param aItemArray array to append suite items 
	* @param aAttrName attribute name
	* @param aAttrValue attribute value
	*/
    void GetMenuSuitesL( 
    		RArray<TMenuItem>& aItemArray, 
    		const TDesC& aAttrName, 
    		const TDesC& aAttrValue );

    
    /**
     * Addes or updates suite
     */
    void AddSuiteL( const TPtrC& aSuiteName );
  
    /**
     * Removes suite
     */
    void RemoveSuiteFromXmlL( const TPtrC& aSuiteName );
    
	/**
	* Updates suites in content xml file
	* @since S60 v5.0.
	* @param aMcsItems suite items
	* @param aIndex index of suite item to update
	*/
    void UpdateSuiteInXmlL( 
    		RArray<TMenuItem> & aMcsItems, 
    		TInt aIndex );
    
    /**
     * Addes suite to main xml file
     */
    void AddSuiteToXmlL( const TPtrC& aSuiteName );
    
    
    void UpdateItemsL( const RArray<TMenuItem>& aItemArray );
    
private:    // data
    CMenuEng& iEng ; ///< Engine. Not Own.
    RFs iFs; ///< File Server. Own.
    RBuf iDirPath; ///< Path to observerd dir. Own.
    CDesCArrayFlat* iFileNewList; ///< List of new files in dir. Own.
    CDesCArrayFlat* iInstalledFileList; ///< List of installed suites. Own.
    RArray<TTime> iNewTimestamps; ///< List of current files' timestamps. Own.
    RArray<TTime> iTimestamps; ///< List of files' timestamps when isntalled. Own.
    CMcsInstallNotifier* iInstallNotifier; ///< Own
    CMcsMmcObserver* iMmcObserver;
    CMcsFreeSpaceObserver* iFreeSpaceObserver; /// Free space observer. Own.
    RMenuSrvSuiteArray iSuites; ///< Suite array. Own.
    TInt iEvent;/// < last event. Own
    };

#endif /* MCSSUITEHANDLER_H_ */
