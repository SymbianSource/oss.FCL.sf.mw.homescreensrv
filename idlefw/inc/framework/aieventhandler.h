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
* Description:  Content plugin event handler class for Active idle framework.
*
*/



#ifndef _AIEVENTHANDLER_H
#define _AIEVENTHANDLER_H

// System includes
#include <e32base.h>

// User includes
#include <hscontentpublisher.h>

// Forward declarations
class MAiPluginTool;
class CAiPluginFactory;
class CHsContentPublisher;
class THsPublisherInfo;

// Class declaration
/**
 * @ingroup group_aifw
 * 
 *  Content plugin eventhandler class for Active idle framework.
 *
 *  @lib aifw
 *  @since S60 5.2
 */
NONSHARABLE_CLASS( CAiEventHandler ) : public CBase                                              
	{
public: // Constructor and destructor	        	
		
    static CAiEventHandler* NewL( CAiPluginFactory& aFactory );
                                              
    ~CAiEventHandler();
		    
public: // New functions
    
    /**
     * Forward plugin event to plugins.
     *
     * @since S60 5.2
     * @param aParam event string.
     */
    void HandlePluginEvent( const TDesC& aParam );

    /**
     * Forward plugin event to plugins.
     *
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aParam event string.
     */
    void HandlePluginEventL( 
        const THsPublisherInfo& aPublisherInfo, const TDesC& aParam ); 
            
    /**
     * Queries wheter a plugin has a menuitem or not.
     * 
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aMenuItem menuitem to query. 
     * @return ETrue if exists, EFalse otherwise
     */
    TBool HasMenuItemL( 
        const THsPublisherInfo& aPublisherInfo, const TDesC& aMenuItem );

    /**
     * Requests a Content publisher plug-in to refresh a content item.
     * 
     * @since S60 5.2
     * @param aContentCid Content id to refresh     
     * @return ETrue if succesful, EFalse otherwise      
     */
    TBool RefreshContent( const TDesC& aContentCid );

    /**
     * Requests a Content publisher plug-in to refresh a content item.
     * 
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aContentCid Content id to refresh              
     * @return ETrue if succesful, EFalse otherwise      
     */    
    TBool RefreshContent( 
        const THsPublisherInfo& aPublisherInfo, const TDesC& aContentCid );
        
    /**
     * Requests a Content publisher plug-in to suspend a content item.
     * 
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aContentCid Content id to suspend
     * @return ETrue if succesful, EFalse otherwise      
     */        
    TBool SuspendContent( 
        const THsPublisherInfo& aPublisherInfo, const TDesC& aContentCid );
            
private: 
    // Constructors

    /**
     * C++ default contructor
     */
    CAiEventHandler( CAiPluginFactory& aFactory );
                             
    /**
     * 2nd phase contructor
     */
    void ConstructL();

private: 
    // new functions
                                        
    void GetIdL( CHsContentPublisher& aContentPublisher,
        CHsContentPublisher::TProperty aProperty, const TDesC& aName, TInt& aId );            
                              
    TInt RefreshContentL( const TDesC& aContentCid );

private: 
    // data

    /** Plugin factory, Not owned */
    CAiPluginFactory& iFactory;
    /** Plugin tool from utility lib, Owned */
    MAiPluginTool* iPluginTool;     

private:
    // friend classes
    
#ifdef _AIFW_UNIT_TEST
    class UT_AiEventHandler;
#endif
	};

#endif // _AIEVENTHANDLER_H

// End of File.
