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


// INCLUDES

// ---------------------------------------------------------
// CExtrStandbyScr::NewL
// ---------------------------------------------------------
//
inline CExtrStandbyScr* CExtrStandbyScr::NewL(TUid aImplementationUid,CCoeControl *aContainer)
	{
	return REINTERPRET_CAST(CExtrStandbyScr*,
		                    REComSession::CreateImplementationL (aImplementationUid,
															     _FOFF (CExtrStandbyScr, iDtor_ID_Key),
																 aContainer)) ;
	}

// ---------------------------------------------------------
// CExtrStandbyScr::~CExtrStandbyScr
// ---------------------------------------------------------
//
inline CExtrStandbyScr::~CExtrStandbyScr()
	{
    // Destroy any instance variables and then
    // inform the framework that this specific
    // instance of the interface has been destroyed.
	REComSession::DestroyedImplementation (iDtor_ID_Key);
	}

// ---------------------------------------------------------
// CExtrStandbyScr::GetNextSizeDecrement
// ---------------------------------------------------------
//
inline TSize CExtrStandbyScr::GetNextSizeDecrement()
	{
	return TSize(0,0);
	}

// ---------------------------------------------------------
// CExtrStandbyScr::ResetToDefaultState
// ---------------------------------------------------------
//
inline void CExtrStandbyScr::ResetToDefaultState()
	{
	}

// ---------------------------------------------------------
// CExtrStandbyScr::SetLayoutL
// ---------------------------------------------------------
//
inline TSize CExtrStandbyScr::GetPreferedSize()
	{
	return GetMaxSize();
	}

#ifndef __ACTIVE_IDLE
// -----------------------------------------------------------------------------
// CExtrStandbyScr::Draw
// -----------------------------------------------------------------------------
//
inline void CExtrStandbyScr::Draw(const TRect& /*aRect*/) const
     {
     }
#endif