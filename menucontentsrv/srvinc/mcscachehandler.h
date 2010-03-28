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
* Description:  The API supports attributes not present in MCS from SAT Api
*
*/


#ifndef __MCSCACHEHANDLER_H__
#define __MCSCACHEHANDLER_H__

#include "menusrvattr.h"
class CMenuEng;
class CMenuSrvEngUtils;

/**
 *  DRM Handler.
 *  @lib mcsdrmhandler.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsCacheHandler ): 
    public CBase
    {
public:
    
    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed object.
    */
    IMPORT_C static CMcsCacheHandler* NewL( 
        CMenuEng& aEng,
        CMenuSrvEngUtils& aUtils );
    
    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    virtual ~CMcsCacheHandler();
    
    void HandleRemoveId( TInt aId );
    void HandleRemoveAttribute( TInt aId, const TDesC& aAttrName );
    
    void EngineEvents( TInt aEvents );
    
private:

    /**
    * Constructor.
    */
    CMcsCacheHandler( 
        CMenuEng& aEng,
        CMenuSrvEngUtils& aUtils );

    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    
    void RemoveNotExistItems();

    void RemoveItemsWithChangedAttributesL();

    
private:    // data
    CMenuEng& iEng ; ///< Engine.
    
    CMenuSrvEngUtils& iUtils;
    
public:
    RMenuSrvAttrArray iAttrCache;
    };

#endif // __MCSDRMHANDLER_H__
