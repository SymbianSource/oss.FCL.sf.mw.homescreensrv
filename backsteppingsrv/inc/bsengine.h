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
* Description:  Engine for BS Engine
 *
*/


#ifndef C_CBSENGINE_H
#define C_CBSENGINE_H

#include <e32base.h>

class CBSConfiguration;
class CBSApplicationInfo;
class CEikonEnv;

/**
 * Engine for BS solution
 * It is the place when whole BS logic is implemented.
 *
 *  @since S60 v3.2
 */
class CBSEngine : public CBase
    {
public:

    static CBSEngine* NewL();
    static CBSEngine* NewLC();

    /**
     * Destructor
     */
    virtual ~CBSEngine();

    /*
     * This method is notified when new application gain focus
     * 
     * @since S60 3.2
     * @param aApp application UID
     */
    void HandleFocusChangeL( const TUid& aApp );

    /**
     * Invoked when activation event appeared
     * appliction state changed or focus state changed
     *
     * @since S60 3.2
     * @param aApp application UID
     * @param aState state of application (like view ids)
     * @param aFocused indicate if it was item/exit state
     * @return EBSEngineCommandWasConsumed if event was consumed,
     *  else EBSEngineCommandWasNotConsumed
     */
    TInt HandleActivationEventL( const TUid& aApp, const TDesC8& aState,
        TBool aFocused );

    /**
     * Invoked when back event appeared
     *
     * @since S60 3.2
     * @param aApp application UID
     * @param aState state of application (like view ids)
     * @param aCheckOnly indicates if only checking should be done (no actual action)
     * @return EBSEngineCommandWasConsumed if event was consumed,
     *  else EBSEngineCommandWasNotConsumed
     */
    TInt HandleBackEventL( const TUid& aApp, const TDesC8& aState,
        TBool aCheckOnly );

    /**
     * Remove information about given application from BS stack
     */
    void RemoveAppRecord( const TUid& aApp );

private:

    CBSEngine();

    void ConstructL();

    /**
     * Returns previous application in focus stack for given application.
     *
     * @since S60 3.2
     * @param aApp application UID
     * @return UID of previous application, 
     * if information is missing returns TUid::Null
     */
    TUid FindPreviousApp( const TUid& aApp );

    /**
     * Sends given appliction to background
     * @since S60 3.2
     * @param aApp application UID
     */
    void SendToBackground( const TUid& aApp );
    /**
     * Resets BS history
     */
    void ResetHistory();

private:
    // data

    /**
     * list of application in BS stact
     */
    RPointerArray<CBSApplicationInfo> iAppsStack;

    /**
     * focus stack
     */
    RArray<TInt> iFocusHistory;

    /**
     * flag for handling fast swap events
     */    
    TBool iSwap;

    /**
     * Configuration of BS
     * Own.  
     */
    CBSConfiguration* iConfiguration;

    /**
     * Eikon environment
     * Not own.  
     */
    CEikonEnv* iEnv;
    };

#endif // C_CBSENGINE_H
