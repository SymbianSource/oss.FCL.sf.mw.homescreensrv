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
* Description:  Action Handler test Plugin
 *
*/


#ifndef C_CPS_TEST_PLUGIN_H
#define C_CPS_TEST_PLUGIN_H

#include <e32base.h>
#include <contentharvesterplugin.h>


// FORWARD DECLARATION

/**
 *  Exemplary CPS Plugin.
 * 
 *  @lib exemplaryactionhandler
 *  @since S60 S60 v5.0
 */
class CCPSTestPlugin : public CContentHarvesterPlugin
	{

public:

	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CCPSTestPlugin* NewL();
	
	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CCPSTestPlugin* NewLC();

	/**
	 * Standard C++ destructor.
	 */
	~CCPSTestPlugin ();

	/**
	 * Executes provided action
	 *
	 * @return status code
	 */
	void UpdateL();
private:
	/**
	 * Standard Symbian 2nd phase constructor.
	 */
	void ConstructL();
    
public:

    TInt iInt;    

	
	};

#endif /* C_CPS_TEST_PLUGIN_H */
