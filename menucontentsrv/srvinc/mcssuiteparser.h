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
* Description:  
*
*/


#ifndef SUITEPARSER_H
#define SUITEPARSER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <xcfwengine.h>

// FORWARD DECLARATIONS
class CXCFWTree;
class CSuiteObject;
class RSuiteObjectArray;
class CSuiteObjectFactory;

// CLASS DECLARATION

/**
 *  CSuiteParser
 * 
 */
NONSHARABLE_CLASS( CSuiteParser ) : public CBase, public MXCFWEngineObserver
	{
public:
    //event enumeration

    enum TSuiteParsingStatus // statuses

        {
        ECompleted = 0, // factory settings completed successfully
        EFailed = -1 // factory settings failed
        };
	
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CSuiteParser();

	/**
	 * Two-phased constructor.
	 * @param aFileName a xml file to parse.
	 */
	static CSuiteParser* NewL( const TDesC& aFileName );

	/**
	 * Two-phased constructor.
	 * @param aFileName a xml file to parse.
	 */
	static CSuiteParser* NewLC( const TDesC& aFileName );

	/**
	 * Gets suites array
	 * @param aSuitesArray array that wil be filled with suites.
	 */
	TInt GetSuitsArrayL( RPointerArray<CSuiteObject>& aSuitesArray );

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CSuiteParser();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 * @param aFileName a xml file to parse.
	 */
	void ConstructL( const TDesC& aFileName );
	
    /**
     * Handles the "entries" node.
     * @param aSuitesArray array that wil be filled with suites.
     */
    void HandleEntriesL( RPointerArray<CSuiteObject>& aSuitesArray );
    
	/**
	 * Creats suite object from node in xml file
	 * @param aNode that contains suite's data.
	 */
    CSuiteObject* CreateSuiteLC( MXCFWNode* aNode );


public:
    // Functions from base classes

    /**
     * Called when Engine parsing / saving state changes
     * User can do desired actions on corresponding events.
     * @param aEvent Engine event.
     */
    virtual void HandleEngineEventL( TXCFWEngineEvent aEvent );

    /**
     * Called when there's an error during parsing / saving
     * @param aErrorCode Error code from engine
     */
    virtual void HandleEngineErrorL( TInt aErrorCode );
    
    /**
     * Called when the file parsing is finished
     * 
     * @param aStatus parsing status.
     */
    void HandleCompletionL( TSuiteParsingStatus aStatus );
	
private:    // data
	
    /**
     * Name of a file to parse. Own.
     */
	RBuf iFileName;
	
    /**
     * Content engine. Own.
     */
	CXCFWEngine* iContentEngine;
	
    /**
     * Content tree. Own.
     */
    CXCFWTree* iContentTree;
	
    /**
     * scheduler for make synchronous restoration
     */
    CActiveSchedulerWait* iActiveScheduler;

    /**
     * Status of parsing
     */
    TSuiteParsingStatus iCompletionStatus;
	
    /**
     * Factory object needed for localised attributes. Own.
     */
    CSuiteObjectFactory* iObjectFactory;
    
	};


#endif // SUITEPARSER_H
