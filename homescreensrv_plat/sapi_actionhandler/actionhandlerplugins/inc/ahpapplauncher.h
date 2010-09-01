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
* Description:  ECOM Component
 *
*/


#ifndef C_ACTION_HANDLER_APPLAUNCHER_H
#define C_ACTION_HANDLER_APPLAUNCHER_H
#include <apadef.h>
#include "ahplugin.h"
// FORWARD DECLARATION
class CEikonEnv;
class CLiwMap;
class TLiwVariant;
class TVwsViewId;

/**
 *  Application Launcher Plugin
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAHAppLauncher : public CAHPlugin
    {

public:

    /**
     * Two phase contruction
     */
    static CAHAppLauncher* NewL();

    /**
     * Two phase contruction
     */
    static CAHAppLauncher* NewLC();

    /**
     * Destructor
     */
    ~CAHAppLauncher();

    /**
     * Executes provided action
     *
     * @return status code
     */
    TInt ExecuteActionL( const CLiwMap* aMap );
private:

    TInt ExecuteTypeLaunchL( const CLiwMap* aMap );
    TInt ExecuteCommmandLineL( const CLiwMap* aMap );
    TInt ExecuteApaMessageL( const CLiwMap* aMap );
    TInt ExecuteActivateViewL( const CLiwMap* aMap );
    TInt StartDocumentL( const CLiwMap* aMap );
    TInt StartAppL( const CLiwMap* aMap );

    TInt ExtractDesL( const CLiwMap* aMap, RBuf& aString,
        const TDesC8& aMapName );
        
    TInt ExtractDes8L( const CLiwMap* aMap, RBuf8& aString,
        const TDesC8& aMapName );    

    TInt ExtractUidL( const CLiwMap* aMap, TUid& aUid,
        const TDesC8& aMapName );

    TInt ExtractViewIdL( const CLiwMap* aMap, TVwsViewId& aViewId );

    TApaCommand GetCommandL( const CLiwMap* aMap );
    
    /**
     * 2nd phase contructor
     */
    void ConstructL();

private:
    //data

    /**
     * A static CEikonEnv
     */
    CEikonEnv* iEnv;
    };

#endif // C_ACTION_HANDLER_APPLAUNCHER_H
