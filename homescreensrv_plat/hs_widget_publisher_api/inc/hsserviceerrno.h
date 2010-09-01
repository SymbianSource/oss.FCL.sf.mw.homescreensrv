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
* Description:  Updates applications and icons in Operator Tile.
*
*/

#ifndef __HSSERVICEERRNO_H__
#define __HSSERVICEERRNO_H__

#define SERRBASE 1000

enum SapiErrors
        {
        
        SErrNone ,
                
        SErrInvalidServiceArgument  = SERRBASE,
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

#endif /*__HSSERVICEERRNO_H__*/
