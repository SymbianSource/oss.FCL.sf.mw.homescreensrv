/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOGOTOOLS_H
#define LOGOTOOLS_H

#include <rphcltserver.h>
//#include <phcltext.h>
/*#include <CPhCltImageHandler.h>
#include <CPhCltImageParams.h>
#include <cphcltextbaseimageparams.h>*/
#include <cphcltimagehandler.h>
#include <cphcltimageparams.h>
#include <cphcltbaseimageparams.h>
#include <fbs.h>

// Name of the Phone Client extension library.
_LIT( KActiveIdlePhCltExtLibrary, "PhoneClientExt.dll" );

// Ordinal of Phone Client extension factory method.
const TInt KActiveIdlePhCltExtFactoryOrdinal = 1;



LOCAL_C void StoreOTALogoL( TInt aMCC, TInt aMNC )
    {
    RPhCltServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );

    RLibrary phCltExtLib;
    User::LeaveIfError( phCltExtLib.Load( KActiveIdlePhCltExtLibrary ) );
    CleanupClosePushL( phCltExtLib );

    CPhCltImageHandler* imageHandler = CPhCltImageHandler::NewL();
    CleanupStack::PushL( imageHandler );

    CPhCltImageParams* params =
        imageHandler->CPhCltBaseImageParamsL( EPhCltTypeOperatorLogo );
    CleanupStack::PushL( params );

    // opLogoParams not owned.
    CPhCltExtOperatorLogoParams* opLogoParams =
        static_cast<CPhCltExtOperatorLogoParams*>( params );

    CFbsBitmap* bitmap = new( ELeave )CFbsBitmap;
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( TSize( 97, 25 ), EColor256 ) );

    opLogoParams->SetCodesL( aMCC, aMNC, EPhCltLogoTypeOTA );
    opLogoParams->AddImageL( bitmap->Handle() );

    User::LeaveIfError( imageHandler->SaveImages( *params ) );


    CleanupStack::PopAndDestroy( bitmap );

    CleanupStack::PopAndDestroy( params );
    CleanupStack::PopAndDestroy( imageHandler );
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    }
LOCAL_C void DeleteOTALogoL( TInt aMCC, TInt aMNC )
    {
    RPhCltServer server;
    User::LeaveIfError( server.Connect() );
    CleanupClosePushL( server );

    RLibrary phCltExtLib;
    User::LeaveIfError( phCltExtLib.Load( KActiveIdlePhCltExtLibrary ) );
    CleanupClosePushL( phCltExtLib );

    CPhCltImageHandler* imageHandler = CPhCltImageHandler::NewL();
    CleanupStack::PushL( imageHandler );

    CPhCltImageParams* params =
        imageHandler->CPhCltBaseImageParamsL( EPhCltTypeOperatorLogo );
    CleanupStack::PushL( params );

    // opLogoParams not owned.
    CPhCltExtOperatorLogoParams* opLogoParams =
        static_cast<CPhCltExtOperatorLogoParams*>( params );

    opLogoParams->SetCodesL( aMCC, aMNC, EPhCltLogoTypeOTA );
    opLogoParams->AddImageL( KPhCltDeleteOperatorLogo );

    User::LeaveIfError( imageHandler->SaveImages( *params ) );

    CleanupStack::PopAndDestroy( params );
    CleanupStack::PopAndDestroy( imageHandler );
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    }




#endif      //  LOGOTOOLS_H

// End of file
