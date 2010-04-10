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


#ifndef __MCSGETLISTCREATORINTERFACE_H__
#define __MCSGETLISTCREATORINTERFACE_H__

#include <mcsmenuitem.h>

//forward declarations
class CLiwDefaultList;
class CLiwDefaultMap;
class CMenuSrvEng;


/**
 * CMcsGetAttrStrategy
 * Abstract class for sttribute getting strategy
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsGetAttrStrategy ): public CBase
	{
public:

	/**
	 * Constructor.
	 */
	CMcsGetAttrStrategy( CMenuSrvEng& aEng );

	/**
	 * Destructor.
	 */
	virtual ~CMcsGetAttrStrategy();


	/**
	 * TODO: add comment
	 */
	virtual void PrepareL(TInt aId) = 0;

	/**
	 * Get attribute value.
	 * @since S60 v5.0
	 * @param aAttrName Attribute name.
	 * @param aAttrExists Will be set to indicate whether attribute exists
	 * or not.
	 * @return Attribute value. Empty string if attribute does not exist.
	 */
	virtual void GetAttributeL(const TDesC& aAttrName,
	        TBool& aAttrExists, TDes& aAttrVal ) = 0;

protected:
	/**
	 * Engine. not own
	 */
	CMenuSrvEng& iEng;
	};


/**
 *  Interface for creating list containg result for getlist operation
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( CMcsGetListCreatorInterface ) : public CBase
    {
public:
    /**
    *   Destructor.
    */
    ~CMcsGetListCreatorInterface();

	/*
	* Create getlist output list
	* @param aIdsArray array containing ids of items to be added to output list
	* @param aRequiredAttributes required attributes
	* @param aIgnoredAttributes ignored attributes
	* @result output list
	*/
    virtual CLiwDefaultList* CreateLC(
    		RArray<TMenuItem>& aIdsArray,
    		CDesC16Array* aRequiredAttributes,
    		CDesC16Array* aIgnoredAttributes) = 0;

protected:
	/**
	*   Constructor.
	*/
	CMcsGetListCreatorInterface( CMenuSrvEng& aEng );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

	/**
	 * Builds map with attributes
	 * @param aMenuItem an item
	 * @param aRequiredAttributes required attributes
	 * @param aIgnoredAttributes ignored attributes
	 * @param aMap map to store result
	 */
	void BuildMapL(
			const TMenuItem& aMenuItem,
			CDesC16Array* aRequiredAttributes,
			CDesC16Array* aIgnoredAttributes,
			CLiwDefaultMap* aMap );

	/**
	 * Adds fundamental attributes for an item
	 * @param aMenuItem an item
	 * @param aMap map to store result
	 */
	void AddFundamentalAttributesL(
			const TMenuItem& aMenuItem,
			CLiwDefaultMap* aMap );

	/**
	 * Adds required attributes
	 * @param aMenuItem an item
	 * @param aRequiredAttributes required attributes
	 * @param aMap map to store results
	 */
	void AddRequiredAttributesL(
			const TMenuItem& aMenuItem,
			CDesC16Array* aRequiredAttributes,
			CLiwDefaultMap* aMap );

	/**
	 * Adds all attributes without ignored
	 * @param aMenuItem an item
	 * @param aIgnoredAttributes ignored attributes
	 * @param aMap map to store results
	 */
	void AddAttributesWithoutIgnoredL(
			const TMenuItem& aMenuItem,
			CDesC16Array* aIgnoredAttributes,
			CLiwDefaultMap* aMap );

	/*
	 * Adds required attributes without ignored
	 * @param aMenuItem an item
	 * @param aRequiredAttributes required attributes
	 * @param aIgnoredAttributes ignored attributes
	 * @param aMap map to store results
	 */
	void AddRequiredAttributesWithoutIgnoredL(
			const TMenuItem& aMenuItem,
			CDesC16Array* aRequiredAttributes,
			CDesC16Array* aIgnoredAttributes,
			CLiwDefaultMap* aMap );

	/**
	 * Adds all attributes
	 * @param aMenuItem an item
	 * @param aMap map to store results
	 */
	void AddAllAttributesL(
			const TMenuItem& aMenuItem,
			CLiwDefaultMap* aMap );

	/*
	 * Adds icon related attributes to a map
	 * @param aMenuItem an item
	 * @param aMap map to store results
	 */
	void FillIconAttributesMapL(
			const TMenuItem& aMenuItem,
			CLiwDefaultMap* aMap );

	/*
	 * Appends attribute to a map
	 * @param aAttrName an attribute name
	 * @param aMap map to store results
	 */
	void AppendAttrL(
			const TDesC& aAttrName,
			CLiwDefaultMap* aMap );

	/*
	 * Sets actual attribute getting strategy for give item id.
	 */
	void SetGetterStrategyL(TInt aId);

protected:
	/**
	 * Engine. not own
	 */
	CMenuSrvEng& iEng ;
	CMcsGetAttrStrategy* iActualGetter; ///< Actual strategy for item.
	CMcsGetAttrStrategy* iSuiteGetter;  ///< Get strategy for suite. Own.
	CMcsGetAttrStrategy* iNormalGetter; ///< Get strategy for not suite. Own.
    };
#endif // __MCSGETLISTCREATORINTERFACE_H__
