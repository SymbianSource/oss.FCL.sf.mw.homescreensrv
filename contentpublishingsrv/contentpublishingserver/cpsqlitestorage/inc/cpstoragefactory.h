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
* Description:  
 *
*/


#ifndef C_CPSTORAGE_FACTORY_H
#define C_CPSTORAGE_FACTORY_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CCpStorage;
class TLiwVariant;

// CLASS DECLARATION
/**
 *  Class with one static method used by CPS Server to 
 *  instantiate storage database.  
 *
 *  @since S60 v5.0
 */
class StorageFactory
    {
public:
    IMPORT_C static CCpStorage* NewDatabaseL(TLiwVariant& aDataMapCache);
    };

#endif //C_CPSTORAGE_FACTORY_H
