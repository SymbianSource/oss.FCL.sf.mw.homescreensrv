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
* Description:  HSPS Service utility class
*
*/


#include "hspsserviceutilities.h"
#include "hspsliwvocabulary.hrh"

// ======== STATIC FUNCTIONS ===================================================

//----------------------------------------------------------------------------
// HspsServiceUtilities::HexString2Int
// ----------------------------------------------------------------------------
//
TInt HspsServiceUtilities::HexString2IntL(
    const TDesC8& aStr )
    {
    TLex8 lex( aStr );
    TUint value;
    lex.Mark();
    
    if( lex.Peek() == '0' )
        {
        lex.Inc();
        }
    if( lex.Peek() == 'x' )
        {
        lex.Inc();
        }
    lex.Mark();
    while ( lex.Peek().IsHexDigit() )
        {
        lex.Inc();
        }
    TPtrC8 uidToken = lex.MarkedToken();
    TLex8 uidLex( uidToken );
    User::LeaveIfError( uidLex.Val( value, EHex ) );        
    return value;
    }

//----------------------------------------------------------------------------
// HspsServiceUtilities::DecString2Int
// ----------------------------------------------------------------------------
//
TInt HspsServiceUtilities::DecString2Int(
    const TDesC8& aStr )
    {
    TLex8 lex( aStr );
    TInt value(-1);
    lex.Mark();
    
    while ( lex.Peek().IsDigit() || lex.Peek()== '-' )
        {
        lex.Inc();
        }
    TPtrC8 uidToken = lex.MarkedToken();
    TLex8 uidLex( uidToken );
    TInt err = uidLex.Val( value );
    if ( err )
        {
        value = -1;
        }
    
    return value;
    }

//----------------------------------------------------------------------------
// HspsServiceUtilities::GetConfigurationTypeL
// ----------------------------------------------------------------------------
//
void HspsServiceUtilities::GetConfigurationTypeL(
    const TDesC8& aTypeStr,
    TUint& aType )
    {
    if( aTypeStr.CompareF( KHspsLiwApplicationConf ) == 0 )
        {
        aType = EhspsAppConfiguration;
        }
    else if ( aTypeStr.CompareF( KHspsLiwViewConf ) == 0 )
        {
        aType = EhspsViewConfiguration;
        }
    else if ( aTypeStr.CompareF( KHspsLiwWidgetConf ) == 0 )
        {
        aType = EhspsWidgetConfiguration;
        }
    else if ( aTypeStr.CompareF( KHspsLiwTemplateConf ) == 0 )
        {
        aType = EhspsTemplateConfiguration;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

//----------------------------------------------------------------------------
// HspsServiceUtilities::GetConfigurationStateL
// ----------------------------------------------------------------------------
//
void HspsServiceUtilities::GetConfigurationStateL(
    const TDesC8& aStateStr,
    ThspsConfigurationState& aState )
    {
    if( aStateStr.CompareF( KHspsLiwConfNotConfirmed ) == 0 )
        {
        aState = EhspsConfStateNotConfirmed;
        }
    else if ( aStateStr.CompareF( KHspsLiwConfWaitForConfirmation ) == 0 )
        {
        aState = EhspsConfStateWaitForConfirmation;
        }
    else if ( aStateStr.CompareF( KHspsLiwConfConfirmed ) == 0 )
        {
        aState = EhspsConfStateConfirmed;
        }
    else if ( aStateStr.CompareF( KHspsLiwConfError ) == 0 )
        {
        aState = EhspsConfStateError;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

//----------------------------------------------------------------------------
// HspsServiceUtilities::GetConfigurationStateFilterL
// ----------------------------------------------------------------------------
//
void HspsServiceUtilities::GetConfigurationStateFilterL(
    const TDesC8& aFilterStr,
    ThspsConfStateChangeFilter& aFilter )
    {
    if( aFilterStr.CompareF( KHspsLiwConfFilterPlugins ) == 0 )
        {
        aFilter = EhspsConfStateChangePlugins;
        }
    else if ( aFilterStr.Length() == 0 )
        {
        aFilter = EhspsConfStateChangeNoFilter;
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// End of file

