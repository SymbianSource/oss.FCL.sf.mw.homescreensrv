/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CPS Execute parameter 
 *
*/


#ifndef AICPSEXECUTEPARAM_H
#define AICPSEXECUTEPARAM_H

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CLiwGenericParamList;
class CLiwDefaultMap;
// Constants

/**
 * CPS Execute Command Parameter 
 *
 * @since S60 5.2
 */

/**
 * Holds parameters to execute the CPS excute command 
 * 
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CAiCpsExecuteParam ) : public CBase
    {
public:
    // constructors and destructor

    /**
    * Two-phased constructors.
    */
    static CAiCpsExecuteParam* NewL( );
    static CAiCpsExecuteParam* NewLC();

    /**
    * Destructor.
    */
    ~CAiCpsExecuteParam();

private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CAiCpsExecuteParam();

    /**
     * 2nd phase constructor
     */
    void ConstructL( );    

public:
    // new functions
        
    /**
    * Gets plugin id
    * 
    * @since S60 5.2
    * @return plugin id
    */
    const TDesC& PluginId() const;
    
    /**
    * Returns the input parameter map for Execute command
    * it will leave the CLiwDefaultMap object in the stack
    * 
    * @since S60 5.2
    * @return in param map 
    */    
    CLiwDefaultMap* InParamMapLC();
   
    /**
    * Adds a actions to the action list  
    * 
    * @since S60 5.2
    * @param aAction actions to add
    */        
    void AddActionL(const TDesC8& aAction);   
    
    /**
    * Sets the filter values
    * This method always over write the previous filters vlaues.  
    * 
    * @since S60 5.2
    * @param aMap filter map
    */
    void SetFilterL(CLiwDefaultMap* aMap);
    
    /**
    * Sets Registry type
    * This method always over write the previous retgistry type.  
    * 
    * @since S60 5.2
    * @param aRegistryType type of cps registry
    */
    void SetRegistryTypeL(const TDesC& aRegistryType);
    
    /**
    * Sets the plugin id  
    * This method always over write the previous plugin id.
    *  
    * @since S60 5.2
    * @param aPluginId actions to add
    */
    void SetPluginIdL(const TDesC& aPluginId);

private:
    // data
    /** Plugin Id, owned.*/
    HBufC* iPluginId;
    /** Registry type, owned. */
    HBufC* iRegistryType;
    /** Filters, owned.*/
    HBufC* iPublisher;
    HBufC* iContentType;
    HBufC* iContentId;
    
    /** action trigger list, owned.  */
    RPointerArray<HBufC8> iActions;
   
private: 
#ifdef _SAPIDATAPLUGIN_TEST
    friend class MT_SapiDataPlugin;
#endif
    };

#endif /* AICPSEXECUTEPARAM_H */
