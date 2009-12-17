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
* Description:  factory settings functionality for content harvester
 *
*/


#ifndef C_FACTORYSETTINGSPARSER_H
#define C_FACTORYSETTINGSPARSER_H

//  INCLUDES
#include <e32base.h>
#include <xcfwengine.h>
// CONSTANTS

// FORWARD DECLARATIONS
class CXCFWTree;
class MXCFWNode;
class CGECODefaultObject;
class CLiwDefaultMap;
class CLiwContainer;
class CLiwDefaultList;
class MLiwInterface;

// CLASS DECLARATION
/**
 * Factory Settings Observer.
 *
 * @lib XCFW.lib
 * @since Series 60 3.1
 */
class MADatFactorySettingsObserver
    {
public:
    //event enumeration

    enum TADatFactorySettingsStatus // statuses

        {
        ECompleted = 0, // factory settings completed successfully
        EFailed = -1 // factory settings failed
        };

public:
    /**
     * Called when the factory settings finish the restoriation process
     * 
     * @param aEvent Restoration status.
     */
    virtual void HandleCompletionL( TADatFactorySettingsStatus aStatus ) = 0;

    };

/**
 *  Factory settings for 
 *
 *  @since Series Series60 3.1
 */
class CCHFactorySettingsParser : public CBase, public MXCFWEngineObserver,
    public MADatFactorySettingsObserver

    {

    enum TTypes
        {
        EUndefined,
        EInteger,
        EUInteger,
        EDescriptor8,
        EDescriptor16,
        EMap,
        EList,
        EBool,
        EReal,
        EUid
        };

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aStorage instance of AS storage DB
     */
    static CCHFactorySettingsParser* NewL( MLiwInterface* aInterface );
    static CCHFactorySettingsParser* NewLC( MLiwInterface* aInterface );

    /**
     * Destructor.
     */
    virtual ~CCHFactorySettingsParser();

public:
    // New functions


    /**
     * Restores all factory settings
     *
     * @since Series 60 3.1
     * @param aStorage instance of AS storage DB
     */
    TInt RestoreL( const TDesC& aStr, TBool aUnInstall=EFalse );

public:
    // Functions from base classes

    /**
     * Called when Engine parsing / saving state changes
     * User can do desired actions on corresponding events.
     * @param aEvent Engine event.
     */
    virtual void HandleEngineEventL( TXCFWEngineEvent aEvent );

    /**
     * Called when there's an error during parsing / saving
     * @param aErrorCode Error code from engine
     */
    virtual void HandleEngineErrorL( TInt aErrorCode );

public:
    // function from MASpaFactorySettingsObserver
    /**
     * Called when the factory settings finish the restoriation process
     * 
     * @param aEvent Restoration status.
     */
    void HandleCompletionL( TADatFactorySettingsStatus aStatus );

private:
    // New functions

    /**
     * Handles the "entries" node.
     * @since Series 60 3.1
     * @param aUnInstall Uninstall bool.
     */
    void HandleEntriesL( TBool aUnInstall );

    /**
     * Adds an item do the database.
     * @since Series 60 3.1
     * @param aItem An "item" node.
     * @param aContainer container with item.
     * @param aType type of item.
     */
    void AddItemL( MXCFWNode* aItem, CLiwContainer* aContainer, TTypes aType );
    
    /**
     * Removes an item from the database.
     * @since Series 60 3.2
     * @param aItem An "item" node.
     * @param aMap CLiwDefaultMap with item.
     */
    void RemoveItemL( MXCFWNode* aItem, CLiwDefaultMap* aMap );

    /**
     * Extracts deleted attributes from "item" node.
     * @since Series 60 3.2
     * @param aItem An "item" node.
     * @param aMap CLiwDefaultMap with item.
     */    
    void ExtractDeleteAttributesL( MXCFWNode* aItem, CLiwDefaultMap* aMap );
    
    /**
     * Extracts and adds attributes from "item" node to map.
     * @since Series 60 3.2
     * @param aItem An "item" node.
     * @param aMap CLiwDefaultMap with item.
     */ 
    void ExtractAttributesAndAddL( MXCFWNode* aItem, CLiwDefaultMap* aMap );
    
    /**
     * Extracts and adds attributes from "item" node to list.
     * @since Series 60 3.2
     * @param aItem An "item" node.
     * @param aMap CLiwDefaultList with item.
     */
    void ExtractAttributesAndAddL( MXCFWNode* aItem, CLiwDefaultList* aMap );

    /**
     * Converts type from TDesC to TTypes.
     * @since Series 60 3.2
     * @param aType type key as attributes.
     */
    TTypes ConvertToType( const TDesC& aType );

private:

    /**
     * C++ default constructor.
     */
    CCHFactorySettingsParser();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( MLiwInterface* aInterface );

private:
    // Data    
    /**
     * Content tree
     */
    CXCFWTree* iContentTree;
    
    /**
     * Content parsing engine
     */
    CXCFWEngine* iContentEngine;
    
    /**
     * completion observer 
     * Not own.
     */
    MADatFactorySettingsObserver* iCompletionObserver; 
    
    /**
     * scheduler for make synchronous restoration
     */
    CActiveSchedulerWait* iActiveScheduler;
    
    /**
     * status about completion of restoration
     */
    TADatFactorySettingsStatus iCompletionStatus;
    
    /**
     * CPS interface
     * Not own
     */
    MLiwInterface* iCPSInterface;
    };

#endif      // C_FACTORYSETTINGSPARSER_H   
// End of File
