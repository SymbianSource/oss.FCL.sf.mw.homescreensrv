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


#ifndef C_AH_TEST_PLUGIN_H
#define C_AH_TEST_PLUGIN_H

#include <e32base.h>
#include <ahplugin.h>


// FORWARD DECLARATION
class CLiwMap;
class TLiwVariant;
class CEikonEnv;


/**
 *  Exemplary Action Handler Plugin.
 *  Class extends the CAHPlugin in order to be able to be used
 *  as a plugin for action handler.
 * 
 *  @lib exemplaryactionhandler
 *  @since S60 S60 v5.0
 */
class CActionHandlerTestPlugin : public CAHPlugin
	{

public:

	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CActionHandlerTestPlugin* NewL();
	
	/**
	 * Standard Symbian 2 phase constructor
	 */
	static CActionHandlerTestPlugin* NewLC();

	/**
	 * Standard C++ destructor.
	 */
	~CActionHandlerTestPlugin ();

	/**
	 * Executes provided action
	 *
	 * @return status code
	 */
	TInt ExecuteActionL( const CLiwMap* aMap );
private:
	/**
	 * Standard Symbian 2nd phase constructor.
	 */
	void ConstructL();

private:

	/**
	 * Extract Variant
	 */
	TInt ExtractVariantL( const CLiwMap* aMap,
							TLiwVariant& aVariant,
							const TDesC8& aMapName );
	
	};

#endif /* C_AH_TEST_PLUGIN_H */
