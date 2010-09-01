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
* Description:  Engine for cp active data server
 *
*/


#ifndef C_CONTENTHARVESTERENGINE_H
#define C_CONTENTHARVESTERENGINE_H

#include "contentharvesterplugin.h"

// FORWARD DECLARATIONS
//class CContentHarvesterPlugin;
class MLiwInterface;
class CLiwServiceHandler;
class CPluginValidator;

// CLASS DECLARATION

/**
 * The engine that discovers and manages all installed harvesting plugins.
 *
 *  @lib contentharvesterserver.lib
 *  @since S60 v3.1
 */
class CContentHarvesterEngine : public CBase
    {

public:
    // New methods

    /**
     * Create a CContentHarvesterEngine object using two phase construction,
     * and return a pointer to the created object.
     * @return pointer to new session
     */
    static CContentHarvesterEngine* NewL();

    /**
     * Create a CContentHarvesterEngine object using two phase construction,
     * and return a pointer to the created object.
     * @return pointer to new session
     */
    static CContentHarvesterEngine* NewLC();

    /**
     * Destroy the object and release all memory objects.
     */
    virtual ~CContentHarvesterEngine();

    /**
     * Pass update request to all registered plugins.
     */
    void Update();

private:
    // New methods

    /**
     * Perform the first phase of two phase construction.
     */
    CContentHarvesterEngine();

    /**
     * Perform the second phase construction of a CContentHarvesterEngine object.
     */
    void ConstructL();

private:

    /**
     * handler to CPS
     * Own
     */
    CLiwServiceHandler* iServiceHandler;

    /**
     * interface to CPS
     * Own
     */
    MLiwInterface * iCPSInterface;

    /**
     * Plugins
     * Own
     */
    CPluginValidator* iPluginManager;
    
    };

#endif // C_CONTENTHARVESTERENGINE_H
