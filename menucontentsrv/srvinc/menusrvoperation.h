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

#ifndef __MENUSRVOPERATION_H__
#define __MENUSRVOPERATION_H__

//  INCLUDES

#include "menusrvobject.h"
#include "menuengoperation.h"
#include "mcsdef.h"
#include "menuitemattr.h"

// FORWARD DECLARATION

class RReadStream;
class CMenuEng;
class CMenuEngObject;

// CLASS DECLARATION

/**
* Server side operation object base.
*/
NONSHARABLE_CLASS( CMenuSrvOperation )
: public CMenuSrvObject, public MMenuEngOperation
    {

protected:  // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuSrvOperation( CMenuEng& aEng );

public:     // new methods

    /**
    * Start operation.
    * @param aMessage Message pending on this operation.
    * "Completion responsibility" taken: will be completed by this object.
    * -> No leaving after calling this!
    */
    void StartL( const RMessage2& aMessage );

public:     // from CMenuSrvObject

    /**
    * Cancel operation.
    */
    void Cancel();

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu.
    */
    virtual void RunMenuEngOperationL() = 0;

    /**
    * Operation complete (changes saved).
    * @param aErr Error code.
    */
    virtual void CompletedMenuEngOperation( TInt aErr );

protected:  // data

    CMenuEng& iEng; ///< Engine.

    };

/**
* Menu Server Remove Operation.
*/
NONSHARABLE_CLASS( CMenuSrvRemoveOperation ): public CMenuSrvOperation
    {

public:     // construction

    /**
    * Constructor.
    * @param aEng Engine.
    * @param aId Remove this item.
    */
    CMenuSrvRemoveOperation( CMenuEng& aEng, TInt aId );

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu: delete the item.
    */
    virtual void RunMenuEngOperationL();

protected:  // data

    TInt iId; ///< Item ID.

    };

/**
* Menu Server MoveToFolder Operation.
*/
NONSHARABLE_CLASS( CMenuSrvMoveToFolderOperation ): public CMenuSrvOperation
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuSrvMoveToFolderOperation();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @param aItems Item array streamed.
    * @param aFolder Target folder.
    * @param aMoveBefore Insertion point.
    * @return The created operation.
    */
    static CMenuSrvMoveToFolderOperation* NewL(
        CMenuEng& aEng,
        RReadStream& aItems,
        TInt aFolder,
        TInt aMoveBefore );

protected:   // construction

    /**
    * Constructor.
    * @param aEng Engine.
    * @param aFolder Target folder.
    * @param aMoveBefore Insertion point.
    */
    CMenuSrvMoveToFolderOperation(
        CMenuEng& aEng,
        TInt aFolder,
        TInt aMoveBefore );

    /**
    * Second phase constructor.
    * @param aItems Item array streamed.
    */
    void ConstructL( RReadStream& aItems );

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu: move the items.
    */
    virtual void RunMenuEngOperationL();

protected:  // data

    TInt iFolder; ///< Target folder.
    TInt iMoveBefore; ///< Insertion point.
    RArray<TInt> iItems; ///< Items to be moced. Own.
    };

/**
* Menu Server Reorder Operation.
*/
NONSHARABLE_CLASS( CMenuSrvReorderOperation ): public CMenuSrvOperation
    {

public:     // construction

    /**
    * Constructor.
    * @param aEng Engine.
    * @param aId Item ID.
    * @param aMoveBefore Insertion point.
    */
    CMenuSrvReorderOperation( CMenuEng& aEng, TInt aId, TInt aMoveBefore );

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu: reorder the item.
    */
    virtual void RunMenuEngOperationL();

protected:  // data

    TInt iId; ///< Item ID.
    TInt iMoveBefore; ///< Insertion point.

    };

/**
* Menu Server Add Item Operation.
*/
NONSHARABLE_CLASS( CMenuSrvAddOperation ): public CMenuSrvOperation
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuSrvAddOperation();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @param aType Type.
    * @param aChanges Streamed data.
    * @return The created operation.
    */
    static CMenuSrvAddOperation* NewL(
        CMenuEng& aEng,
        const TDesC& aType,
        RReadStream& aChanges );

protected:   // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuSrvAddOperation( CMenuEng& aEng );

    /**
    * Second phase constructor.
    * @param aType Type.
    * @param aChanges Streamed data.
    */
    void ConstructL( const TDesC& aType, RReadStream& aChanges );

public:     // new methods

    /**
    * Get object ID. The ID is generated, but the object is not yet added
    * to the engine!
    * @return Object ID.
    */
    TInt ObjectId();

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu: add the item.
    */
    virtual void RunMenuEngOperationL();

protected:  // data

    CMenuEngObject* iObject; ///< Object to be added. Own.
    TInt iParent; ///< Parent folder.
    TInt iInsertBefore; ///< Insertion point.

    };

/**
* Menu Server Update Item Operation.
*/
NONSHARABLE_CLASS( CMenuSrvUpdateOperation ): public CMenuSrvOperation
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuSrvUpdateOperation();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @param aId Item ID.
    * @param aChanges Streamed data.
    * @return The created operation.
    */
    static CMenuSrvUpdateOperation* NewL(
        CMenuEng& aEng,
        TInt aId,
        RReadStream& aChanges );

protected:   // construction

    /**
    * Constructor.
    * @param aEng Engine.
    * @param aId Item ID.
    */
    CMenuSrvUpdateOperation( CMenuEng& aEng, TInt aId );

    /**
    * Second phase constructor.
    * @param aChanges Streamed data.
    */
    void ConstructL( RReadStream& aChanges );

private:    // from MMenuEngOperation

    /**
    * Perform operations on the menu: update the item.
    */
    virtual void RunMenuEngOperationL();

protected:  // data

    TInt iId; ///< Item ID.
    TUint32 iFlags; ///< Flags.
    TUint32 iChangedFlags; ///< Flags change mask.
    RMenuItemAttrArray iAttrChanges; ///< Attribute changes. Own.

    };

#endif // __MENUSRVOPERATION_H__
            
// End of File
