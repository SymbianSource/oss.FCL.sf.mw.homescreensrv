/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CHARVESTERPLUGINVALIDATOR_H
#define CHARVESTERPLUGINVALIDATOR_H

// INCLUDES
#include <e32base.h>
#include <e32hashtab.h>
#include "cpluginvalidator.h" 

// FORWARD DECLARATIONS
class CBlacklistHandler;

//CONSTS
//PubSub Category AI plug-in registry API
// changed from 0x102750F0 to 0xEDFF45C4 for standalone app
const TUid KPSUidActiveIdle2 =  {0xEDFF45C4}; // ActiveIdle2 SID
// Indicates that all the CPS Harvester plugins have been updated
const TUint KActiveIdleCpsPluginsUpdated = 0x0000000C;
enum EPSActiveIdleCpsPluginsUpdated
    {
    EPSAiPluginsNotUpdated = 0,
    EPSAiPluginsUpdated
    };

/**
 *  Class used to maintain required ECOM plugins implementations
 *
 * 
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
NONSHARABLE_CLASS( CHarvesterPluginValidator ): public CPluginValidator
    {
public:

    /**
     * Two-phased constructor.
     * 
     */     
    IMPORT_C static CHarvesterPluginValidator* NewL( TUid aUid, 
                                            TAny *aParameter = NULL );

    /**
     * Two-phased constructor.
     * 
     */     
    IMPORT_C static CHarvesterPluginValidator* NewLC( TUid aUid,
                                             TAny *aParameter = NULL );
    
    /**
     * Desctructor.
     */     
    ~CHarvesterPluginValidator();


private:

    /**
     * C++ default constructor.
     */    
	CHarvesterPluginValidator( TUid aUid, TAny *aParameter );

    /**
     * Perform the second phase construction of a CPluginValidator object.
     */    
    void ConstructL();

    /**
     * Loads or destroys plugins
     */    
    void ManagePluginsL();
    
    /**
     * Loads ECOM plugins
     */    
    void LoadPluginL( TPluginInfo& aPluginInfo );


private:
     
    /**
     * Blacklist Handler
     * Own
     */
    CBlacklistHandler* iBlacklist;
    
    /*
     * Startup flag
     * 
     */
    TBool iStartup;
    
    /*
     * Property indicating the status of load operation
     * Own
     */
    RProperty iInProgressProperty;
    };

#endif // CHARVESTERPLUGINVALIDATOR_H
