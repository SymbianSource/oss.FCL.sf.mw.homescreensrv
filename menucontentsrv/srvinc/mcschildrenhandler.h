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

#ifndef MCSCHILDRENHANDLER_H_
#define MCSCHILDRENHANDLER_H_

#include "mcschildrenscanner.h"

class CMenuSrvEng;
class CMcsCacheHandler;

/**
 *  FolderChildrenCount Handler.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsChildrenHandler ): 
    public CBase,
    public MMcsChildrenScanner
    {
public:
    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed object.
    */
    
    static CMcsChildrenHandler* NewL( 
        CMenuSrvEng& aSrvEng,
        CMcsCacheHandler& aCacheHandler );

    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    virtual ~CMcsChildrenHandler();
    
    
    void EngineEvents( TInt aFolder, TInt aEvents );
    void HandleChildrenEvent(TInt aId, TUint aOldCount, TUint aNewCount);
    
private:

    /**
    * Constructor.
    */
	CMcsChildrenHandler( 
        CMenuSrvEng& aSrvEng,
        CMcsCacheHandler& aCacheHandler  );
	
    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    void ScanL(TInt aFolder);
	
	
private:    // data
    
    
    CMenuSrvEng& iSrvEng;
    
    CMcsCacheHandler& iCacheHandler;
    
    CMcsChildrenScanner* iScanner; /// < Own
    
    };

#endif /*MCSCHILDRENHANDLER_H_*/
