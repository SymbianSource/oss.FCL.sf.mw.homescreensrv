/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 	Top level service provider error codes.
 *	%created_by:	smatasah %
 *	%date_created:	Fri Jan 22 17:24:14 2010 %
 *
*/
/**********************************************************************
 *
 *	Header %name:	serviceerrno.h %
 *	Instance:	bh1sapi_1
 *	Description:	Top level service provider error codes.
 *	%created_by:	smatasah %
 *	%date_created:	Fri Jan 22 17:24:14 2010 %
 *
 **********************************************************************/
#ifndef SERVICEERRNO_H
#define SERVICEERRNO_H

#define SERRBASE 1000

enum SapiErrors
    {

    SErrNone ,

    SErrInvalidServiceArgument = SERRBASE,
    SErrUnknownArgumentName ,
    SErrBadArgumentType ,
    SErrMissingArgument ,
    SErrServiceNotSupported ,
    SErrServiceInUse ,
    SErrServiceNotReady ,
    SErrNoMemory ,
    SErrHardwareNotAvailable ,
    SErrServerBusy ,
    SErrEntryExists ,
    SErrAccessDenied ,
    SErrNotFound ,
    SErrUnknownFormat ,
    SErrGeneralError ,
    SErrCancelSuccess,
    SErrServiceTimedOut

    };

#endif
