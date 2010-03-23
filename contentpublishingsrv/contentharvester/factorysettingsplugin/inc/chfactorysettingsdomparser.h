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

#ifndef CHFACTORYSETTINGSDOMPARSER_H_
#define CHFACTORYSETTINGSDOMPARSER_H_

//  INCLUDES
#include <e32base.h>

#include <xmlengdomimplementation.h>
#include <xmlengelement.h>
#include <badesca.h>
#include <f32file.h>

class MLiwInterface;
class CLiwDefaultMap;
class CLiwContainer;
class CLiwDefaultList;

class CCHFactorySettingsDOMParser : public CBase
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
     * @param aInterface Liw Interface.
     * @param aFs file session.
     */
    static CCHFactorySettingsDOMParser* NewL( MLiwInterface* aInterface,
            RFs& aFs );

    /**
     * Two-phased constructor.
     * @param aInterface Liw Interface.
     * @param aFs file session.
     */
    static CCHFactorySettingsDOMParser* NewLC( MLiwInterface* aInterface,
            RFs& aFs );

    /**
     * Destructor.
     */
    virtual ~CCHFactorySettingsDOMParser();

public: // new functions

    /**
     * Restores all factory settings
     *
     * @param aStr Path to file.
     * @param aUnInstall mark indicating add or remove option.
     */
    TInt Restore( const TDesC& aStr, TBool aUnInstall=EFalse );

private:

    /**
     * C++ default constructor.
     * @param aFs file session.
     */
	CCHFactorySettingsDOMParser( RFs& aFs );

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aInterface Liw Interface.
     */
    void ConstructL( MLiwInterface* aInterface );

private: // new functions

    /**
     * Parse xml files.
     * @param aFilePath Path to file.
     * @param aUnInstall mark indicating add or remove option.
     */
    void ParseFactorySettingsFileL( const TDesC& aFilePath,
            TBool aUnInstall );

    /**
     * Converts type from TDesC to TTypes.
     * @param aType type key as attributes.
     */
    TTypes ConvertToType( const TDesC& aType );

    /**
     * Removes an item from the database.
     * @param aNode A node.
     * @param aMap CLiwDefaultMap with item.
     */
    void RemoveItemL(
    		const TXmlEngElement& aNode,
    		CLiwDefaultMap* aMap );
    /**
     * Adds an item do the database.
     * @param aNode An "item" node.
     * @param aContainer container with item.
     * @param aType type of item.
     */
    void AddItemL( const TXmlEngElement& aNode,
            CLiwContainer* aContainer,
            TTypes aType );

    /**
     * Extracts deleted attributes from "item" node.
     * @param aNode An "item" node.
     * @param aMap CLiwDefaultMap with item.
     */
    void ExtractDeleteAttributesL(
    		const TXmlEngElement& aNode,
    		CLiwDefaultMap* aMap );

    /**
     * Extracts and adds attributes from "item" node to map.
     * @param aNode An "item" node.
     * @param aMap CLiwDefaultMap with item.
     */
    void ExtractAttributesAndAddL(
    		const TXmlEngElement& aNode,
    		CLiwDefaultMap* aMap );

    /**
     * Extracts and adds attributes from "item" node to list.
     * @param aItem An "item" node.
     * @param aList CLiwDefaultList with item.
     */
    void ExtractAttributesAndAddL(
            const TXmlEngElement& aNode,
            CLiwDefaultList* aList );

private:
    // Data

    /**
     * CPS interface
     * Not own
     */
    MLiwInterface* iCPSInterface;

    /*
     * File server session.Not Own
     */
    RFs iFs;

    };

#endif /* CHFACTORYSETTINGSDOMPARSER_H_ */
