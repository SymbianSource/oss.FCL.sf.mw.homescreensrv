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
* Description:  Definition of menu change notifier
*
*/


#ifndef __MCSMENUNOTIFIER_H__
#define __MCSMENUNOTIFIER_H__

#include <e32base.h>

class RMenu;

/**
 *  Menu change notifier.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuNotifier ): public RSubSessionBase
    {

public:

    enum TEvent ///< Menu events.
        {
		EItemsNone = 0,///< Nothing changed.
        EItemsAddedRemoved = 0x1, ///< Items added and/or removed in folder.
        EItemsReordered = 0x2, ///< Items reordered in folder.
        EItemAttributeChanged = 0x4, ///< Item attribute and/or flag changed.
        };

public:
	
	    /**
    * Constructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
	RMenuNotifier(){ iData = NULL; };

    /**
    * Close the notifier. Safe to call if not open.
    * Outstanding request, if any, is cancelled.
    * @since S60 v5.0
    * @capability ECapabilityReadDeviceData.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void Close();

    /**
    * Open the notifier.
    * @since S60 v5.0
    * @param aMenu Menu.
    * @return Error code.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C TInt Open( RMenu& aMenu );

    /**
    * Request notification of menu events occurring in specified folder.
    * @since S60 v5.0
    * @param aFolder Folder to observe. Pass 0 to observe all folders
    * (the whole menu).
    * @param aEvents Events to be notified about. A combination of TEvent bits.
    * @param aStatus Completes when menu events occur in the specified folder,
    * or on error. Completion value is error code (if negative), or a combination
    * of events specified in aEvents (if positive). Note, more than one event
    * bit can be set.
    * @capability ECapabilityReadDeviceData.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void Notify( TInt aFolder, TInt aEvents, TRequestStatus &aStatus );

    /**
    * Cancel notification request.
    * @since S60 v5.0
    * @capability ECapabilityReadDeviceData.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void Cancel();

private:    // data
    
    class TData;
    TData* iData; ///< Private data. Own.

    };

#endif // __MCSMENUNOTIFIER_H__
