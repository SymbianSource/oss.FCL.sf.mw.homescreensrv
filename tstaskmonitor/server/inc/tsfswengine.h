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
* Description:  Task monitor engine
 *
*/


#ifndef TSFSWENGINE_H
#define TSFSWENGINE_H

#include <e32base.h>
#include "tsfswentry.h"
#include "tsdataobserver.h"

class CTsFswDataList;


/**
 * Engine for fast swap server.
 * Performs task monitoring, notifications about wg changes are
 * received from the appui.
 */
NONSHARABLE_CLASS( CTsFswEngine ) : public CBase,
                                    public MHsDataObserver

    {
    
public:
    /**
     * Creates a new instance.
     * @param   observer   ref to observer
     */
    static CTsFswEngine* NewL( MHsDataObserver& observer );

    /**
     * @copydoc NewL
     */
    static CTsFswEngine* NewLC( MHsDataObserver& observer );

    /**
     * Destructor.
     */
    ~CTsFswEngine();

    /**
     * Returns a reference to the current content.
     * @return  ref to content array
     */
    const RTsFswArray& FswDataL();

    /**
     * Set screenshot 
     */
    void SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority, TInt wgId);
    
    /**
     * Removes screenshot 
     */    
    void RemoveScreenshotL(TInt wgId);


private:

    // from MTsFswTaskListObserver
    void DataChanged();



private:
    /**
     * Constructor.
     */
    CTsFswEngine( MHsDataObserver& observer );

    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();

    /**
     * Gets the window group list and reconstructs the fsw content.
     * @return   TBool   ETrue if the list has been modified
     */
    TBool CollectTasksL();

private:
    MHsDataObserver& mObserver;
    CTsFswDataList* mDataList; //own

    };

#endif
