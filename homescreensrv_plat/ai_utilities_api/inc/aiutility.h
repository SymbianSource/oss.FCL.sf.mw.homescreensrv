/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AIUTILITY_H
#define AIUTILITY_H

#include <e32std.h>
#include <aicontentmodel.h>

class MAiPSPropertyObserver;
class MAiStrParser;
class MAiContentItemIterator;
class MAiPluginSettings;
class MAiPluginTool;

/**
*  Active Idle Utility Library.
*
*  @since S60 3.2
*/
namespace AiUtility
    {
    
	/**
     * Creates a Publish & Subscribe keys observer.
     *
     * @param aCallBack callback method to call when the key value changes.
     * @param aCategory key category.
     * @param aKey key to observe in category.
     * @return pointer to a new publish and subscribe keys observer.
     */
	IMPORT_C MAiPSPropertyObserver* CreatePSPropertyObserverL(
	                                            TCallBack aCallBack,
	                                            TUid aCategory,
	                                            TInt aKey );    
      
    /**
    * Creates a string parser.
    *
    * @return pointer to a new string parser.
    */
    IMPORT_C MAiStrParser* CreateStrParserL();
            
    /**
 	* Creates a new iterator instance from a TAiContentItem array.
 	*
 	* @since S60 3.2
 	* @param aArray content item array.
 	* @param aCount  number of content items in aArray.
 	* @return a new iterator object for aArray.
 	*/
    IMPORT_C MAiContentItemIterator* CreateContentItemArrayIteratorL(
                                                const TAiContentItem* aArray,
                                                TInt aCount );

    /**
     * Helper factory function for creating a MAiContentItemIterator out
     * of a const C array of TAiContentItem objects. This function infers the item
     * count of the array automatically from the array argument passed to it.
     * 
     * @param aArray const C array of TAiContentItem objects.
     * @return MAiContentItemIterator instance for iterating aArray. Client
     *         takes ownership of the returned object.
     */
    template<int N> 
    inline MAiContentItemIterator* CreateContentItemArrayIteratorL
            ( const TAiContentItem (&aArray)[N] )
        {
        return AiUtility::CreateContentItemArrayIteratorL(aArray, N);
        }
    
    /**
     * Creates the plugin settings.
     *
     * @since S60 3.2
     * @return pointer to plugin settings.
     */
    IMPORT_C MAiPluginSettings* CreatePluginSettingsL();

    /**
     * Creates the plugin tool.
     *
     * @since S60 3.2
     * @return pointer to plugin tool.
     */
    IMPORT_C MAiPluginTool* CreatePluginToolL();

    }

#endif      // AIUTILITY_H

