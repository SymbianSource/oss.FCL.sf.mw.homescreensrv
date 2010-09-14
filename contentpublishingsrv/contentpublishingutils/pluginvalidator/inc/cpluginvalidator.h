/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPLUGINVALIDATOR_H
#define CPLUGINVALIDATOR_H

// INCLUDES
#include <e32base.h>
#include <e32hashtab.h>
#include <ecom/implementationinformation.h>

// FORWARD DECLARATIONS
class REComSession;

/**
 *  Class containing information regarding a plugin
 *
 *
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
NONSHARABLE_CLASS( TPluginInfo )
    {
public:
    TAny* iPlugin; // not own
    TUid iImplementationUid;
    TInt iVersion;
    TUid iDtor_ID_Key;
    };

/**
 *  Class used to maintain required ECOM plugins implementations
 *
 *
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
NONSHARABLE_CLASS( CPluginValidator ): public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     */
    IMPORT_C static CPluginValidator* NewL( TUid aUid,
                                            TAny *aParameter = NULL );

    /**
     * Two-phased constructor.
     *
     */
    IMPORT_C static CPluginValidator* NewLC( TUid aUid,
                                             TAny *aParameter = NULL );

    /**
     * Gets plugin with provided UID
     *
     * @param aUid required plugin
     * @return pointer to plugin implementation
     */
    IMPORT_C TAny* GetImplementation( TUid aUid );

    /**
     * Gets plugin with provided index
     *
     * @param aIndex index of plugin
     * @return pointer to plugin implementation
     */
    IMPORT_C TAny* GetImplementation( TInt aIndex );

    /**
     * Gets number of plugins
     *
     * @return number of plugins
     */
    IMPORT_C TInt GetCount();

    /**
     * Desctructor.
     */
    virtual ~CPluginValidator();

protected:

    /**
     * From CActive, RunL.
     * Handles the active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, DoCancel.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive, RunError.
     * Method called when leave occured in RunL
     */
    TInt RunError( TInt aError );

protected:

    /**
     * C++ default constructor.
     */
    CPluginValidator( TUid aUid, TAny *aParameter );

    /**
     * Perform the second phase construction of a CPluginValidator object.
     */
    void ConstructL();

    /*
     * Checks if plugin is present in an array
     */
    TBool PresentInArrayL( TPluginInfo aPluginInfo,
        const RImplInfoPtrArray& aInfoArray );

    /*
     *
     */
    void DestroyPlugin( TInt aIndex );

    /**
     * Cleans plugins table;
     */
    void CleanPluginsTable();

    /**
     * Loads or destroys plugins
     */
    virtual void ManagePluginsL();

    /**
     * Loads ECOM plugins
     */
    virtual void LoadPluginL( TPluginInfo& aPluginInfo );

protected:

    /**
     * ECOM handler
     * Own.
     */
    REComSession* iSession;

    /**
     * Array containing plugins
     * Own.
     */
    RArray<TPluginInfo> iPluginArray;

    /**
     * UID of the interface
     */
    const TUid iUid;

    /**
     * Parameter to pass to the object creation method.
     */
    TAny* iParameter;
     };

// ==================== LOCAL FUNCTIONS ====================

/**
* Identity function to search in an array of TPluginInfo.
* @param aLeft Search term.
* @param aRight Array item.
* @return ETrue if ID-s match.
*/
LOCAL_C TBool UidMatch( const TPluginInfo& aLeft, const TPluginInfo& aRight )
    {
    return aLeft.iImplementationUid == aRight.iImplementationUid;
    }

#endif // CPLUGINVALIDATOR_H
