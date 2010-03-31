/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CAISCUTTARGET_H
#define CAISCUTTARGET_H

#include <e32base.h>
#include <coemain.h>

class TApaTask;
class CPeriodic; 

/**
 *  Base class for shortcut targets
 *
 *  @since S60 v3.2
 */
class CAiScutTargetShutter : public CActive
{

public:
	
	static CAiScutTargetShutter* NewL( CCoeEnv* aEnv, TUid aAppUid, TBool aIsRunning, TBool aIsDialog );

    virtual ~CAiScutTargetShutter();
    
     void StartL();
 
private:

	CAiScutTargetShutter( CCoeEnv* aEnv, TUid aAppUid );
    
    void ConstructL( TBool aIsRunning, TBool aIsDialog );

    // from base class CActive

    /**
     * From CActive
     *
     * @since S60 v3.2
     */
    void DoCancel();

    /**
     * From CActive
     *
     * @since S60 v3.2
     */
    void RunL();

    /**
     * From CActive
     * Handles an error situation
     *
     * @since S60 v3.2
     * @param aError Error code received from system
     * @return Error code after error handling
     */
    TInt RunError( TInt aError );
    
    static TInt TaskExistsCallback( TAny* aPtr );
    
    static TInt TaskNotExistsCallback( TAny* aPtr );
    
    void Run();
    
private: //data

	/**
     * Pointer to the control environment
     * Not own.
     */
    CCoeEnv*	iEnv;
	
	/**
     * Target application uid
     */
    TUid		iAppUid;
    
    /**
     * Window Server session
     */
    RWsSession 	iWsSession;
    
    /**
     * Task exsit or not
     */
    TBool iTaskExists;
    
    /**
     * Periodic timer
     * Own
     */
    CPeriodic* iPeriodic;
    
    /**
     * Count retry times
     */
    TInt iCounter;
    
    /**
     * Target application is running
     */
    TBool iIsRunning;
    
    /**
     * Target app is dialog
     */
    TBool iIsDialog;
    
    
    /**
     * 
     */
    TBool iTaskKilled;
};

#endif // CAISCUTTARGET_H

// End of File.
