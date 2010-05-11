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
* Description:  Cps command buffer
*
*/


#ifndef C_CAICPSCOMMANDBUFFER_H
#define C_CAICPSCOMMANDBUFFER_H

// System includes
#include <e32base.h>
 
// User includes
#include <aicpscommandbuffer.h>

// Forward declarations
class CLiwGenericParamList;
class CLiwServiceHandler;
class CLiwCriteriaItem;
class CAiCpsExecuteParam;
class MLiwInterface;

/**
 * AI Cps command buffer
 *
 * @ingroup group_aifw
 * @lib aifw  
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( CAiCpsCommandBuffer ) : public CBase,
    public MAiCpsCommandBuffer
    {
public:
    // constructors and destructor

    /**
    * Two-phased constructors.
    */
    static CAiCpsCommandBuffer* NewL();
    static CAiCpsCommandBuffer* NewLC();

    /**
    * Destructor.
    */
    ~CAiCpsCommandBuffer();

private:
    // constructors
    
    /**
     * C++ default constructor
     */
    CAiCpsCommandBuffer();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

public:
    // new function
    
    /**
     * Flushes command buffer
     * 
     * @since S60 v5.2
     */
    void Flush();
    
    /**
    * Gets the CPS interface 
    *
    * @since S60 5.2
    */
    void GetCPSInterfaceL();
    
private:
    // from MAiCpsCommandBuffer

    /**
     * @see MAiCpsCommandBuffer
     */    
    void AddCommand( const TDesC& aPluginId, const TDesC& aType, 
        CLiwDefaultMap* aFilter, const TDesC8& aAction);

    /**
     * @see MAiCpsCommandBuffer
     */        
    CLiwServiceHandler* ServiceHandler() const;            
    
    /**
     * @see MAiCpsCommandBuffer
     */        
    MLiwInterface* CpsInterface() const;
    
private:
    // new functions
    
    /**
    * Detach the CPS interface 
    *
    * @since S60 5.2
    */    
    void DetachL();
    
    /**
    * Adds a CPS command execute commnad for a spcific Plugin
    * Note: aType and Filter will overwrite the previous value
    *
    * @since S60 5.2
    * @param aPluginId plugin id.
    * @param aType type of the cps registry.
    * @param aFilter filter values.
    * @param aAction action trigger.
    */
    void DoAddCommandL(const TDesC& aPluginId,const TDesC& aType, 
            CLiwDefaultMap* aFilter, const TDesC8& aAction );
    
    /**
    * Flush all the CPS execute commands..
    *
    * @since S60 5.2
    */
    void DoFlushL();
    
private: 
    // data
    /**
    * SAPI service handler.
    * Owned.
    */
    CLiwServiceHandler* iServiceHandler;
    
    /**
    * CPS SAPI service.
    * Owned.
    */
    CLiwCriteriaItem* iCpsService;
    
    /**
    * Provides hsps services.
    * Owned.
    */
    MLiwInterface* iCpsInterface;

    /**
    * Plugins execute parameter array 
    * Owned.
    */
    RPointerArray<CAiCpsExecuteParam> iPlugins;

private: 
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiCpsCommandBuffer;
#endif
    };

#endif // C_CAICPSCOMMANDBUFFER_H

// End of file

