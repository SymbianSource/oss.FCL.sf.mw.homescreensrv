/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements application shell SAT interface for external 
*                applications.
*
*/


// INCLUDE FILES
#include <MenuSatInterface.h>

#include <e32property.h>

#include "../../cenrep/menu2privatecrkeys.h"

// CONSTANTS
const TInt KMaxFolderNameLength = 248;

// -----------------------------------------------------------------------------
// CMenuSATInterface::CMenuSATInterface
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CMenuSATInterface::CMenuSATInterface()
    {
    RProperty::Define( KCRUidMenu, KMenuShowSatUI, RProperty::EInt );
    RProperty::Define( KCRUidMenu, KMenuSatUIName, RProperty::EText );
    RProperty::Define( KCRUidMenu, KMenuSatUIIconId, RProperty::EInt );

    }

// -----------------------------------------------------------------------------
// CMenuSATInterface::MakeSatUiVisible
// Change visibility and name properties in central repository.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMenuSATInterface::MakeSatUiVisible( TBool aVisible,
                                                  const TDesC& aName )
    {
    return SetSatUiVisibilityData( aVisible, aName, -1 );
    }

// -----------------------------------------------------------------------------
// CMenuSATInterface::MakeSatUiVisible
// Change visibility, name and icon properties in central repository.
// -----------------------------------------------------------------------------
EXPORT_C TInt CMenuSATInterface::MakeSatUiVisible( TBool aVisible,
                                                  const TDesC& aName,
                                                  TUint8 aIconInfoId )
    {
    return SetSatUiVisibilityData( aVisible, aName, (TInt)aIconInfoId );
    }

// -----------------------------------------------------------------------------
// CMenuSATInterface::SetSatUiVisibilityData
// Do the actual hard work of setting visibility, name and icon properties 
// in central repository.
// -----------------------------------------------------------------------------
TInt CMenuSATInterface::SetSatUiVisibilityData( TBool aVisible,
     const TDesC& aName,
     TInt8 aIconInfoId )
    {

    // Append only the KMaxFolderNameLength leftmost characters
    TBuf<KMaxFolderNameLength> satName;
    satName.Append( aName.Left( KMaxFolderNameLength ) );

    // Passes the make sat ui command to AppShell using the central repository
 
    // Visibility
    TInt err = RProperty::Set( KCRUidMenu, KMenuShowSatUI, aVisible );;

    // Name
    if( aName.Length() )
        {
        err |= RProperty::Set( KCRUidMenu, KMenuSatUIName, satName );
        }

    // Icon id
    if( aIconInfoId != -1 )
        {
        // set the Sat UI icon ID
        err |= RProperty::Set( KCRUidMenu, KMenuSatUIIconId, aIconInfoId );
        }

    return err;
    }

// End of file
