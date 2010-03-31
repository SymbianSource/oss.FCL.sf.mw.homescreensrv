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
* Description:
*
*/
#ifndef MPOPUPFSMACTIONS_H
#define MPOPUPFSMACTIONS_H

//  EXTERNAL INCLUDES
#include <e32def.h>

//  CLASS DEFINITION
/**
 * MPopupFSMActions actions.
 * Note: This file has been generated automatically. Do not edit!
 */
class MPopupFSMActions
    {
    public:     // Abstract methods
        virtual void StartLongTimer() = 0;
        virtual void IssueCleanPopup() = 0;
        virtual void IssuePublishPopup() = 0;
        virtual void IssuePublishCaption() = 0;
        virtual void IssueCleanCaption() = 0;
        virtual void StartShortTimer() = 0;
        virtual void CompleteSelf() = 0;
        virtual void CancelRequest() = 0;
    };

#endif      //  MPOPUPFSMACTIONS_H

// End of file
