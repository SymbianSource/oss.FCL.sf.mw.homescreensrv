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
* Description:  Central Repository about DB state change information observer
 *
*/


#ifndef C_CHDBSTATEHANDLER_H
#define C_CHDBSTATEHANDLER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <f32file.h>


class CRepository;
class CCHFactorySettings;

class CCHDbStateHandler :
	public CActive
	{
public:
	/*
	 * Destructor.
	 */
	~CCHDbStateHandler();

	/**
	 * Two-phased constructor.
	 */
	static CCHDbStateHandler* NewL( CCHFactorySettings* aCallback );

private:
	/**
	* Simply constructor.
	*/
	CCHDbStateHandler( CCHFactorySettings* aCallback );

	/**
	* 2nd phase constructor.
	*/
	void ConstructL();

private:
	/**
	* From CActive.
	* Invoke when asynchronous request will be completed.
	*
	* @since S60 S60 v3.1
	*/
	void RunL();

	/**
	* From CActive.
	* Invoke for canceling asynchronous request.
	*
	* @since S60 S60 v3.1
	*/
	void DoCancel();

	/**
	 * Override to handle leaves from RunL(). Default implementation causes
	 * the active scheduler to panic.
	 *
	 * @since S60 S60 v3.1
	 */
	TInt RunError(TInt aError);

private:
	/**
	 */
	void ConstructFactorySettingsPathL();

	/**
	 * Removes parsed Factory Settings files
	 */
	void RemoveFactorySettingsParsedFilesL();

	/**
	 * Handle repository db state.
	 * If KSQLDBStateRestored state match then calls HandleRestoredDbStateL
	 * remove repository db state KSQLDBStateRestored, add KSQLDBStateNormal.
	 */
	void HandleDbStateL();

	/**
	 * Handle restored db state.
	 * Calls RemoveFactorySettingsParsedFilesL and call UpdateL on iCallback
	 * member.
	 */
	void HandleRestoredDbStateL();

private:

	/**
	 * Pointer to repository.
	 * Own.
	 */
    CRepository* iRepository;

    /**
     * Interface for notifying need of update after DB restore.
     * Not own.
     */
    CCHFactorySettings* iCallback;

    /**
     * File server session.
     */
    RFs iFs;

    /**
     * Path of parsed Factory Settings
     */
    TFileName iFSPath;
	};

#endif // C_CHDBSTATEHANDLER_H
