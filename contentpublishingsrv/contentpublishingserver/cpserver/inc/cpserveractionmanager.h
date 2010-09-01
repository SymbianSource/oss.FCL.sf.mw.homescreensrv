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
* Description:  Used by sessions for handling execute action requests
 *
*/


#ifndef C_CPACTION_MANAGER_H
#define C_CPACTION_MANAGER_H

#include <e32base.h>
// FORWARD DECLARATIONS
#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif
class CLiwServiceHandler;
class MLiwInterface;
class CLiwMap;

/**
 *  Used to execute action using Action Handler Service
 *
 *  @since S60 5.0
 */
class CCPActionManager : public CBase
    {
public:

    /**
     * C++ destructor
     */
    ~CCPActionManager();

    /**
     * Two-phased constructor.
     */
    static CCPActionManager* NewL();

    /**
     * Two-phased constructor.
     */
    static CCPActionManager* NewLC();

    /**
     * Executes action
     * @param Action to be executed
     */
    void ExecuteL( const CLiwGenericParamList& aList );

private:
    // methods

    /**
     * Standard C++ constructor.
     */
    CCPActionManager();

    /**
     * Standard 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Loads interface
     * @param interface loaded from service
     */
    void PrepareServiceL( MLiwInterface*& aInterface );

    /**
     * Prepares data to execute command
     * @param paramList from server
     * @param target list to execute action
     */
    void PrepareInputListL( const CLiwGenericParamList& aList,
        CLiwGenericParamList& aTarget );

    /**
     * Extracts and adds attributes from aMap to aTarget.
     * @since Series 60 3.2
     * @param aMap CLiwMap with action.
     * @param aTarget CLiwGenericParamList with Uid and Map.
     */
    void ExtractUidAndMapL( const CLiwMap& aMap,
        CLiwGenericParamList& aTarget );

private:
    // data

    /*
     * Own.
     * Handler to service
     */
    CLiwServiceHandler* iServiceHandler;

    /*
     * Own.
     * Interface to CPS service
     */
    MLiwInterface* iInterface;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif
    };

#endif // C_CPACTION_MANAGER_H
// end of file
