/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Define the API for a external standby screen
*
*/



#ifndef EXTRSTANDBYSCR_H
#define EXTRSTANDBYSCR_H

// INCLUDES
#include <e32std.h>
#include <ecom/ecom.h>
#include <coecntrl.h>

#include "ExtrStandbyScrConst.h"

const TUid KCExtrStandbyScrInterfaceUid = {KExtrStandbyScrInterfaceUid} ;
	
class CExtrStandbyScr : public CCoeControl 
    {
public:
	
	/**
    * Creates new instance of CExtrStandbyScr based class.
    * @param aImplementationUid implementation UID .
    * @param aDataStore 
    * @return CExtrStandbyScr* Newly created instance.
    */
    inline static CExtrStandbyScr* NewL(TUid aImplementationUid,CCoeControl *aContainer);

	/**
    * Destructor.
    */
    inline virtual ~CExtrStandbyScr();

public:

	/**
	* LoadEngineL
	* Instructs the plugin that is has been selected to run and to now load its corresponding engine
	*/
    virtual void LoadEngineL()=0;

public: 	/* Layout */

	/**
	* GetPreferedSize
	* Returns the preferred size of the plugin
	*/
	inline virtual TSize GetPreferedSize();

	/**
	* GetNextSizeDecrement
	* The difference in size from the current size to the next smaller size this plugin can support
	*/
	inline virtual TSize GetNextSizeDecrement() ;        

	/**
	* ResetToDefaultState
	* Set the plugin to the default state. This is called when the plugin start and with the red key 
	*/
	inline virtual void ResetToDefaultState();

public: 	/* Functions to read static settings from plugin */

	/**
	* GetMinSize
	* Returns the minimum size this plugin can render its content in
	*/
	virtual TSize GetMinSize() = 0 ;

	/**
	* GetMaxSize
	* Returns the maximum size this plugin can render its content in
	*/
	virtual TSize GetMaxSize() = 0 ;

protected:

	virtual void Draw(const TRect& aRect) const;

private:
	// ECOM framework requires this ID in object destructor
	TUid iDtor_ID_Key;
    };

#include "ExtrStandbyScr.inl"

#ifdef __ACTIVE_IDLE
#include "extrstandbyscrutil.inl"
#endif

#endif // EXTRSTANDBYSCR_H
