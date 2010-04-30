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
#ifndef ACTIVITYQUERIES_H
#define ACTIVITYQUERIES_H


#include <e32base.h>

_LIT( KSelectRow, "SELECT * FROM Activities WHERE ApplicationId=%S AND ActivityName='%S'");
_LIT( KSelectRows, "SELECT Data FROM Activities");
_LIT( KSelectAppRows, "SELECT Data FROM Activities WHERE ApplicationId=%S");
_LIT( KDeleteRow, "DELETE FROM Activities WHERE ApplicationId=%S AND ActivityName='%S'");
_LIT( KDeleteRows, "DELETE FROM Activities WHERE ApplicationId=%S");

_LIT(KApplicationColumnName, "ApplicationId");
_LIT(KActivityColumnName, "ActivityName");
_LIT(KDataColumnName, "Data");

_LIT(KActivityTableName, "Activities");
_LIT(KActivityIndexName, "ActivitiesKey");
#endif  // ACTIVITYQUERIES_H

