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

// animgeneralfunctions_stub.h

#ifndef ANIMGENERALFUNCTIONS_STUB_H
#define ANIMGENERALFUNCTIONS_STUB_H

#include <w32adll.h>

class CAnimGeneralFunctionsStub : 
        public CBase, 
        public MAnimGeneralFunctions,
        public MAnimGeneralFunctionsWindowExtension
	{
public:
    static CAnimGeneralFunctionsStub* NewL();
    ~CAnimGeneralFunctionsStub();
    
private:
    CAnimGeneralFunctionsStub();

public: // from MAnimGeneralFunctions
	void Animate(TDateTime *aDateTime);
	void SetSync(TAnimSync aSyncMode);
	void SetInterval(TInt aInterval);
	void SetNextInterval(TInt aInterval);
	TDateTime SystemTime() const;
	TBool FlashStateOn() const;
	TAnimSync Sync() const;
	const CFbsScreenDevice *ScreenDevice();
	CFbsBitmap *DuplicateBitmapL(TInt aHandle);
	CFbsFont *DuplicateFontL(TInt aHandle);
	void CloseFont(CFbsFont *);
	const RThread &Client();
	void ReplyBuf(const TDesC8 &aDes);
	void ReplyBuf(const TDesC16 &aDes);
	void Panic() const;
	void GetRawEvents(TBool aGetEvents) const;
	void PostRawEvent(const TRawEvent &aRawEvent) const;
	void PostKeyEvent(const TKeyEvent &aRawEvent) const;
	const RMessagePtr2* Message();
	TAny* ExtendedInterface(TInt aInterface);
	TInt RegisterForNotifications(TUint32 aNotifications);

public: // from MAnimGeneralFunctionsWindowExtension
	TInt Screens() const;
	TInt FocusScreens() const;
	void SetFocusScreen(TInt aScreenNo);
	TInt WindowGroups(TInt aScreen) const;
	TBool WindowGroupInfo(TWindowGroupInfo& aInfo,TInt aScreen,TInt aFullOrdinalPosition) const;
	TBool WindowGroupName(TPtrC& aWindowName,TInt aScreen,TInt aFullOrdinalPosition) const;
	TInt SetOrdinalPosition(TInt aWindowGroupId,TInt aPos,TInt aOrdinalPriority);
	void WindowConfig(TWindowConfig& aWindowConfig) const;

public:  // Data
    RThread iThread;
    mutable TBool iGetEvents;
    mutable TRawEvent iPostedRawEvent;
    mutable TKeyEvent iPostedKeyEvent;
    TWindowGroupInfo iWgInfo;
    HBufC* iWindowName;
    struct { TInt iWindowGroupId; TInt iPos; TInt iOrdinalPriority; } iSetOrdinalPosition;
	};
	
#endif // ANIMGENERALFUNCTIONS_STUB_H

