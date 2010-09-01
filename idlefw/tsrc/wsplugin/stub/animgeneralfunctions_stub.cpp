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

// animgeneralfunctions_stub.cpp

#include "animgeneralfunctions_stub.h"

CAnimGeneralFunctionsStub* CAnimGeneralFunctionsStub::NewL()
    {
    return new (ELeave) CAnimGeneralFunctionsStub;
    }
    
CAnimGeneralFunctionsStub::~CAnimGeneralFunctionsStub()
    {
    delete iWindowName;
    }

CAnimGeneralFunctionsStub::CAnimGeneralFunctionsStub()
    {
    }

void CAnimGeneralFunctionsStub::Animate(TDateTime* /*aDateTime*/)
    {
    }
    
void CAnimGeneralFunctionsStub::SetSync(TAnimSync /*aSyncMode*/)
    {
    }
    
void CAnimGeneralFunctionsStub::SetInterval(TInt /*aInterval*/)
    {
    }
    
void CAnimGeneralFunctionsStub::SetNextInterval(TInt /*aInterval*/)
    {
    }
    
TDateTime CAnimGeneralFunctionsStub::SystemTime() const
    {
    return TDateTime();
    }
    
TBool CAnimGeneralFunctionsStub::FlashStateOn() const
    {
    return EFalse;
    }
    
MAnimGeneralFunctions::TAnimSync CAnimGeneralFunctionsStub::Sync() const
    {
    return ESyncNone;
    }
    
const CFbsScreenDevice* CAnimGeneralFunctionsStub::ScreenDevice()
    {
    return NULL;
    }
    
CFbsBitmap* CAnimGeneralFunctionsStub::DuplicateBitmapL(TInt /*aHandle*/)
    {
    return NULL;
    }
    
CFbsFont* CAnimGeneralFunctionsStub::DuplicateFontL(TInt /*aHandle*/)
    {
    return NULL;
    }
    
void CAnimGeneralFunctionsStub::CloseFont(CFbsFont *)
    {
    }
    
const RThread& CAnimGeneralFunctionsStub::Client()
    {
    return iThread;
    }
    
void CAnimGeneralFunctionsStub::ReplyBuf(const TDesC8& /*aDes*/)
    {
    }
    
void CAnimGeneralFunctionsStub::ReplyBuf(const TDesC16& /*aDes*/)
    {
    }
    
void CAnimGeneralFunctionsStub::Panic() const
    {
    }
    
void CAnimGeneralFunctionsStub::GetRawEvents(TBool aGetEvents) const
    {
    iGetEvents = aGetEvents;
    }
    
void CAnimGeneralFunctionsStub::PostRawEvent(const TRawEvent &aRawEvent) const
    {
    iPostedRawEvent = aRawEvent;
    }
    
void CAnimGeneralFunctionsStub::PostKeyEvent(const TKeyEvent &aRawEvent) const
    {
    iPostedKeyEvent = aRawEvent;
    }
    
const RMessagePtr2* CAnimGeneralFunctionsStub::Message()
    {
    return NULL;
    }
    
TAny* CAnimGeneralFunctionsStub::ExtendedInterface(TInt aInterface)
    {
    TAny* result = NULL;
    
    switch( aInterface )
        {
        case MAnimGeneralFunctions::EWindowExtensionInterface:
            result = static_cast< MAnimGeneralFunctionsWindowExtension* >( this );
            break;
        default:
            break;
        }
    
    return result;
    }
    
TInt CAnimGeneralFunctionsStub::RegisterForNotifications(TUint32 /*aNotifications*/)
    {
    return KErrNone;
    }
	
void MAnimGeneralFunctions::Reserved1() const
    {
    }
    
void MAnimGeneralFunctions::Reserved2() const
    {
    }
    
void MAnimGeneralFunctions::Reserved3() const
    {
    }

TInt CAnimGeneralFunctionsStub::Screens() const
    {
    return 1;
    }
    
TInt CAnimGeneralFunctionsStub::FocusScreens() const
    {
    return 1;
    }
    
void CAnimGeneralFunctionsStub::SetFocusScreen(TInt /*aScreenNo*/)
    {
    }
    
TInt CAnimGeneralFunctionsStub::WindowGroups(TInt /*aScreen*/) const
    {
    return 1;
    }
    
TBool CAnimGeneralFunctionsStub::WindowGroupInfo(TWindowGroupInfo& aInfo,TInt aScreen, TInt aFullOrdinalPosition) const
    {
    if ( aScreen == 0 && aFullOrdinalPosition == 0 )
        {
        aInfo = iWgInfo;
        return ETrue;
        }
    return EFalse;
    }
    
TBool CAnimGeneralFunctionsStub::WindowGroupName(TPtrC& aWindowName, TInt aScreen, TInt aFullOrdinalPosition) const
    {
    if ( aScreen == 0 && aFullOrdinalPosition == 0 && iWindowName )
        {
        aWindowName.Set( *iWindowName );
        return ETrue;
        }
    return EFalse;
    }
    
TInt CAnimGeneralFunctionsStub::SetOrdinalPosition(TInt aWindowGroupId,TInt aPos,TInt aOrdinalPriority)
    {
    iSetOrdinalPosition.iWindowGroupId = aWindowGroupId;
    iSetOrdinalPosition.iPos = aPos;
    iSetOrdinalPosition.iOrdinalPriority = aOrdinalPriority;
    return KErrNone;
    }
    
void CAnimGeneralFunctionsStub::WindowConfig(TWindowConfig& /*aWindowConfig*/) const
	{
	
	}
    
void MAnimGeneralFunctionsWindowExtension::Reserved1() const
    {
    }
    
void MAnimGeneralFunctionsWindowExtension::Reserved2() const
    {
    }
    
void MAnimGeneralFunctionsWindowExtension::Reserved3() const
    {
    }


