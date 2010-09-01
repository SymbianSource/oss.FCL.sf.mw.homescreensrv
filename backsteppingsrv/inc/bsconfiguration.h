/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class keeps BS configuration
 *
*/


#ifndef BSCONFIGURATION_H
#define BSCONFIGURATION_H

#include <xcfwengine.h>

class CXCFWTree;
class MXCFWNode;
class CGECODefaultObject;

/**
 *  Class keeps configuration of BS.
 *  Configuration is stored in XML in private folder of Back Stepping engine.
 *  This class is responsible for reading the XML
 *  @since S60 3.2
 */
class CBSConfiguration : public CBase, public MXCFWEngineObserver
    {
public:

    static CBSConfiguration* NewL();
    static CBSConfiguration* NewLC();

    /**
     * Destructor.
     */
    ~CBSConfiguration();

    // from base class MXCFWEngineObserver

    /**
     * From MXCFWEngineObserver.
     * Called when Engine parsing / saving state changes
     * User can do desired actions on corresponding events.
     *
     * @param aEvent Engine event.
     */
    virtual void HandleEngineEventL( TXCFWEngineEvent aEvent );

    /**
     * From MXCFWEngineObserver.
     * Called when there's an error during parsing / saving
     *
     * @param aErrorCode Error code from engine
     */
    virtual void HandleEngineErrorL( TInt aErrorCode );

    /**
     * Check if application with given UID is in reset list.
     * Application from that the list should reset BS stack
     *
     * @since S60 3.2
     * @param aApp
     * @return ETrue if application is in the list, otherwise EFalse
     */
    TBool IsReset( TUid aApp );

    /**
     * Check if application with given UID is in ignored list.
     * Application from that the list should be ignored in BS
     *
     * @since S60 3.2
     * @param aApp
     * @return ETrue if application is in the list, otherwise EFalse
     */
    TBool IsIgnored( TUid aApp );

    /**
     * Check if application with given UID is in additional ignored list.
     * Application that is started from applicaton from the list 
     * should ignore BS stack
     *
     * @since S60 3.2
     * @param aApp
     * @return ETrue if application is in the list, otherwise EFalse
     */
    TBool IsIgnoredIfStartedFrom( TUid aApp );

    /**
     * Returns list of application that should reset BS stack, 
     * if any application was stated thru them (i.e. FastSwap)
     *
     * @since S60 3.2     
     * @return list of UIDs
     */
    RArray<TInt>& ResetIfThruList();

private:

    CBSConfiguration();

    void ConstructL();

    /**
     * Convert descriptor to integer.
     * It detects if format is hexadecimal
     *
     * @since S60 3.2
     * @param aStr descriptor to convert
     * @return converted intereger, 
     * if there will be problem wiht convertion return is KErrArgument
     */
    TInt Str2Int( const TDesC& aStr );

    /**
     * Loads configuration from XML file
     * Read file from private folder, parse the file and 
     * fill in internal structures.
     *
     * @since S60 3.2
     */
    void LoadConfigurationL();

private:
    // data

    /**
     * List of application that should reset BS stack
     */
    RArray<TInt> iResetApps;

    /**
     * List of application that should be ignoresd from BS stack
     */
    RArray<TInt> iIgnoredApps;

    /**
     * List of application that should deactive BS 
     * if application was started/activated from them
     */
    RArray<TInt> iIgnoredIfStartedFrom;

    /**
     * List of application that should reset BS 
     * if any application was started/activated thu them
     */
    RArray<TInt> iResetIfThru;

    /**
     * Content tree
     * Own. 
     */
    CXCFWTree* iContentTree;

    /**
     * Content parsing engine
     * Own. 
     */
    CXCFWEngine* iContentEngine;

    };

#endif // BSCONFIGURATION_H
