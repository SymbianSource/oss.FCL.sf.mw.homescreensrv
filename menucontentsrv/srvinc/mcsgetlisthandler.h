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
* Description:  This class is a handler for getlist operation. It manages fetching and 
*   providing getlist information
*
*
*/


#ifndef __MCSGETLISTHANDLER_H__
#define __MCSGETLISTHANDLER_H__

#include <e32base.h>
#include <e32property.h>

#include "mcsmenuitem.h"

//Forward declatations
class CMenuSrvEng;
class CLiwGenericParamList;
class CMenuFilter;
class CLiwMap;

/*
 *  GetList Handler.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsGetlistHandler ): public CBase
    {
    public:
    typedef TUint TBasicParameters;	
    enum 
        {
        EGetListRecursive = 1,
        EGetListFlat = 2,
        EGetListParentOnly = 4,
        EGetListIncludeParent = 8
        };

	/**
	*   Factory method.
	*/
    static CMcsGetlistHandler* NewL( CMenuSrvEng& aEng );

    /**
    *   Destructor.
    */
    ~CMcsGetlistHandler();
    
    /**
    *   Do getlist operation. Stores serialized result in 
    *   iSerializedOutput descriptor
    *   @param aSerializedInput serialized input parameters for getlist
    *   @result size of serialized result descriptor
    */
    TInt GetListSizeL( const TDesC8& aSerializedInput );
    
    /*
    * Returns result of GetList operation
    * @return serialized output of GetList operation 
    **/
    TPtrC8 GetListDataL( );
    
    /*
    * Destroys the output buffer.
    **/
    void CloseOutputBuffer( );
    
private:
	/**
	*   Constructor.
	*/
	CMcsGetlistHandler( CMenuSrvEng& aEng );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

private:
	
    /**
    * Fetches basic input parameters for GetList operation
    * @param aInputList input list for GetList
    * @param aFolderId to store folder id
    * @param aBasicParameters to store basic parameters
    */
    void FetchBasicParametersL( const CLiwGenericParamList* aInputList, 
    		TInt& aFolderId, TBasicParameters& aBasicParameters );    
    
    /**
    * Fetches required and ignored attributes
    * @param aInputList input list for GetList
    * @param aRequiredAttributes to store required attributes
    * @param aIgnoredAttributes to store ignored attributes
    */    
    void FetchRequiredAndIgnoredAttributesL(
    		const CLiwGenericParamList* aInputList,
    		CDesC16Array* aRequiredAttributes,
    		CDesC16Array* aIgnoredAttributes );
    
    /**
    * Fetches filter
    * @param aInputList input list for GetList
    * @param aFilter to store filter 
    */    
    TBool FetchFilterL( const CLiwGenericParamList* aInputList, 
    		CMenuFilter* aFilter );
    
    /**
    * Fetches items ids
    * @param aItemArray items ids
    * @param aFolderId folder id
    * @param aFilter filter
    * @param aBasicParameters basic input parameters for getlist
    */    
    void FetchItemsIdsL( RArray<TMenuItem>& aItemArray, TInt aFolderId,
    		 CMenuFilter* aFilter, TBasicParameters& aBasicParameters );
    
    /**
    * Gets attributes from descriptor
    * @param aList descriptor containing attributes
    * @param aArray array to store attributes 
    */    
    void GetAttributesL( const TDesC& aList, CDesC16Array* aArray );

    /**
     * Set flags in filter
     * @param inputMap Input map containint flags
     * @param aFilter filter
    */
    void SetFlagsL( const CLiwMap* inputMap, CMenuFilter* aFilter );
    
    /**
     * Set attribute string to input map 
     * @param inputMap input map
     * @param aAttrName input attribute name
     * @param aFilter filter
    */
    void SetAttributeDesL( const CLiwMap* inputMap,
                    const TDesC& aAttrName, CMenuFilter* aFilter );
    
    /**
     * Set attribute hex to input map 
     * @param inputMap input map
     * @param aAttrName input attribute name
     * @param aFilter filter
    */
    void SetAttributeHexFormatL( const CLiwMap* inputMap,
                      const TDesC& aAttrName, CMenuFilter* aFilter );
    
    /**
     * Set attribute num to input map 
     * @param inputMap input map
     * @param aAttrName input attribute name
     * @param aFilter filter
    */        
    void SetAttributeNumL( const CLiwMap* inputMap,
                       const TDesC& aAttrName, CMenuFilter* aFilter );
    
    /**
     * Set attribute bool to input map 
     * @param inputMap input map
     * @param aAttrName input attribute name
     * @param aFilter filter
    */ 
    void SetAttributeBoolL( const CLiwMap* inputMap,
                      const TDesC& aAttrName, CMenuFilter* aFilter );
    
    
private:    
	/**
	 * Engine. not own
	 */
	CMenuSrvEng& iEng ; 
	
	/**
	 * Buffer for serialized result.own
	 */
    RBuf8 iOutputBuffer;
    };
	
#endif // __MCSGETLISTHANDLER_H__
