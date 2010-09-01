/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is an interface for controlling keysounds.
*
*/


#if !defined(__AKNSOUNDSYSTEM_H__)
#define __AKNSOUNDSYSTEM_H__

// INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <aknSoundInfo.h>


// CLASS DECLARATION
class CAknSoundPlayer;
class CAknKeySoundStack;

/**
*  This is an interface for controlling keysounds from Avkon.
*  Provides support for handling key sounds.
*  @since Series 60 0.9
*/
NONSHARABLE_CLASS(CAknKeySoundSystem) : public CBase
	{
public:
	/**
	* Base class default constructor.
	* @param aUid Application UID.
	* @return A pointer to a new @c CAknKeySoundSystem object.
	*/
	static CAknKeySoundSystem* NewL(TInt /*aUid*/)
	    {
	    return new(ELeave) CAknKeySoundSystem();
	    };
	
	/**
	* Destructor.
	*/
	~CAknKeySoundSystem()
	    {
	    };
	
	/**
	* Function reads resource data from resource file and push the context
	* to the server session. The most common Avkon resources in this case are
	* @c R_AVKON_DEFAULT_SOUND_LIST, @c R_AVKON_DEFAULT_SKEY_LIST and 
	* @c R_AVKON_SILENT_SKEY_LIST.
	* If a leave occurs the framework generates a Symbian Leave code.
	* @param aResourceId Avkon resource ID.
	*/
	void PushContextL(TInt/* aResourceId*/)
	    {
	    };
	
	/**
	* Pop the context from the server session object.
	*/
	void PopContext()
	    {
	    };
	
	/**
	* This function plays key event sounds.
	* @param aKeyEvent The scan code of the key that caused the event.
	*/
	void PlaySound(const TKeyEvent& /*aKeyEvent*/)
	    {
	    };
		
	/**
	* This function plays sounds by sound ID.
	* @param aSid Sound ID. These SID numbers are specified in Avkon 
	* enum @c TAvkonSystemSID.
	*/
	void PlaySound(TInt /*aSid*/)
	    {
	    };
	
	/**
	* Function reads resource data from resource file. And adds the
	* application sound info list to the server session.
	* If the leave occurs the framework generates a Symbian Leave code.
	* @param aResourceId Avkon resource ID.
	*/
	void AddAppSoundInfoListL(TInt /*aResourceId*/)
	    {
	    };
		
	/**
	* This function brings the sounds back to foreground. This function should
	* be called from @c HandleForegroundEventL(). @c HandleForegroundEventL() is 
	* implemented in @c CAknAppUi and it is reimplemented in @c CAknViewAppUi.
	*/ 
	void BringToForeground()
	    {
	    };

    /**
    * Stops playing the sound with given ID.
    * @param aSid Sound ID. Application defined SID's should be less than 1000.
    */
    void StopSound(TInt /*aSid*/)
        {
        };

    /**
    * Locks context.
    */
    void LockContext()
        {
        };

    /**
    * Releases context.
    */
    void ReleaseContext()
        {
        };

    /**
    * Reads sound information from certain given sound ID.
    * @since Series 60 2.0
    * @param aSid ID for sound which's information is requested.
    * @param aInfo Sound information will be put in this structure.
    * @return TInt Returns @c KErrNone if succesfull otherwise returns 
    * @c KErrNotFound if requested sound ID is not found.
    */
	TInt RequestSoundInfoL(TInt /*aSid*/, CAknSoundInfo& /*aInfo*/)
	    {
	    return KErrNone;
	    };

    /**
    * Returns top context id from the session.
    * 
    * @since 3.1
    * @return The resource id of the context that is at the top of the soundstack. 
    */
    TInt TopContext()
        {
        return KErrNone;
        };

public://private:
	CAknKeySoundSystem()
	    {
	    };
	void ConstructL(TInt /*aUid*/)
	    {
	    };
private:
	//CAknSoundPlayer* iSoundPlayer;
	//TInt iSpare;
	};

#endif // __AKNSOUNDSYSTEM_H__
