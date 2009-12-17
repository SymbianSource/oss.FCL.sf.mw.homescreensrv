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

#ifndef CPACTIONHANDLERTHREAD_H_
#define CPACTIONHANDLERTHREAD_H_

#include <e32base.h>
#include <eikenv.h>
#include <eikappui.h>

class MLiwInterface;
class CLiwGenericParamList;
class CCPActionManager;

/**
 * The function to be passed as an argument to RThread::Create.
 * 
 * It just calls CCPActionHandlerThread::ThreadFunction. The only
 * reason this declaration is in the header file is to let
 * CCPActionHandlerThread declare friendship with this funcion.
 * 
 * @param aParam pointer to CCPActionHandlerThread object
 * @return error code
 */
TInt CPExecutionThreadFunction( TAny* aParam );

/**
 * Action handler thread.
 * 
 * Provides easy-to-use interface for executing plugin actions
 * in a separate thread.
 * @since S60 5.0
 */
class CCPActionHandlerThread: public CBase
    {
    friend TInt CPExecutionThreadFunction( TAny* );
    
public:
    
    /**
     * Creates new instance of CCPActionHandlerThread.
     * 
     * @return new instance of CCPActionHandlerThread
     */
    static CCPActionHandlerThread* NewL();
    
    /**
     * Creates new instance of CCPActionHandlerThread.
     * 
     * @return new instance of CCPActionHandlerThread
     */
    static CCPActionHandlerThread* NewLC();
    
    /**
     * Destructor.
     * Stops the action execution thread.
     */
    ~CCPActionHandlerThread();

    /**
     * Executes an action handler action in the action execution thread.
     * 
     * @param aActionParams action parameters
     */
    void ExecuteL( const CLiwGenericParamList& aActionParams );
    
    /**
     * Starts the action execution thread.
     * 
     * Creates a new instance of action execution thread
     * provided it is not running already.
     */
    void StartExecutionThreadIfNeededL();
    
    /**
     * Checks whether action execution thread is alive.
     * 
     * @return ETrue if the thread is alive, EFalse otherwise.
     */
    TBool ExecutionThreadRunning();

private:
    /**
     * Constructor.
     */
    CCPActionHandlerThread();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * This function is run is a separate thread. It creates
     * CServerEikonEnv and CServerAppUi objects, which provide
     * a trap cleanup and an active scheduler for the thread and
     * then starts the active scheduler.
     * 
     * @return error code
     */
    TInt ThreadFunction();
    
private: // internal data types

    /**
     *  EIkonEnv for the server process.
     */
    class CServerEikonEnv : public CEikonEnv
        {
    public:
        void DestroyEnvironment();
        void ConstructL();
        };

    /**
     * EikAppUi for the server process.
     */
    class CServerAppUi : public CEikAppUi
        {
    public:
        ~CServerAppUi();
        void ConstructL();
        };
    
    /**
     * A simple active object for executin actions.
     * Objects of this class are created and destroyed
     * by action execution thread. 
     */
    class CActionExecutorAO: public CActive
        {
    public:
        /**
         * Factory method.
         */
        static CActionExecutorAO* NewL();
        /**
         * Factory method. 
         */
        static CActionExecutorAO* NewLC();
        /**
         * Destructor.
         */
        ~CActionExecutorAO();
        /**
         * The RunL method which is run whever an action needs
         * to be executed in the action execution thread.
         * It just executes the action using the parameters
         * supplied with a call to SetActionParams. If that
         * parameters are NULL, it stops the active scheduler
         * thus terminating action execution thread.
         */
        void RunL();
        /**
         * A dummy cancel method required by Symbian.
         */
        void DoCancel();
        /**
         * Returns a pointer to this active object's iStatus
         * member variable.
         */
        TRequestStatus* RequestStatus();
        /**
         * TODO
         * Sets paramters of the action to be executed in RunL.
         * This method is supposed to be called from the main CPS thread.
         * @param aActionParams action parameters
         */
        void SetActionParams( const CLiwGenericParamList* aActionParams );
    private:
        /**
         * Constructor.
         */
        CActionExecutorAO();
        /**
         * 2nd phase constructor.
         */
        void ConstructL();
    private:
        /**
         * Parameters of the action to be executed in RunL.
         * Not own.
         */
        const CLiwGenericParamList* iActionParams;
        /**
         * Action manager instance.
         * Own.
         */
        CCPActionManager* iActionManager;
        };
    
private: // data
    /**
     * Handle to the action execution thread.
     * Own.
     */
    RThread iActionExecThread;
    
    /**
     * Active object created in the action execution thread.
     * Own.
     */
    CActionExecutorAO *iActionExecutorAO;
    
    /**
     * ETrue if action execution thread was started at least once.
     */
    TBool iActionExecThreadStarted;
    
    /**
     * A counter for naming action execution thread.
     * 
     * Every action execution thread has a number appended to its name.
     * When the thread dies (i.e. as a result of panic in a plugin), a
     * new instance of that thread is started with the number in the name
     * increased by one.
     */
    TUint iThreadNum;
    };

#endif /* CPACTIONHANDLERTHREAD_H_ */
