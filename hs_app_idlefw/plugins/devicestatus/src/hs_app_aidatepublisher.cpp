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
* Description:  Date publisher
*
*/


#include <StringLoader.h>
#include <avkon.rsg>
#include <AknUtils.h>
#include <aidevstaplgres.rsg>
#include <bacntf.h>
#include "hs_app_aidatepublisher.h"
#include "aicontentobserver.h"

const TInt KMaxDateStringLength = 100;

// ======== MEMBER FUNCTIONS ========

CAiDatePublisher::CAiDatePublisher()
    {
    }


void CAiDatePublisher::ConstructL()
    {
  	// Create enviroment notifier
    iEnvNotifier = CEnvironmentChangeNotifier::NewL( 
		CActive::EPriorityStandard, TCallBack( HandleCallBackL, this ) );
    iDateText = NULL;
	// Start listening notifications
    iEnvNotifier->Start(); 
    }


CAiDatePublisher* CAiDatePublisher::NewL()
    {
    CAiDatePublisher* self = new( ELeave ) CAiDatePublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiDatePublisher::~CAiDatePublisher()
    {
    delete iDateText;
    delete iEnvNotifier;
    }


void CAiDatePublisher::ResumeL()
    {
    RefreshDateL();
    }


void CAiDatePublisher::Subscribe( MAiContentObserver& aObserver, 
								    MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiDatePublisher::RefreshL( TBool /*aClean*/ )
    {
    RefreshDateL();
    }


void CAiDatePublisher::RefreshDateL()
    {
    if( !iContentObserver )
        {
        return;
        }

    delete iDateText;
    iDateText = NULL;
    iDateText = ConstructDateStringL();
    if ( iDateText )
        {
        iContentObserver->Publish( *iExtension,
    						EAiDeviceStatusContentDate,
    						*iDateText,
    						0 );
        }
    }


HBufC* CAiDatePublisher::GetDayNameStringLC( TDay aDay, CCoeEnv& aCoeEnv )
    {
	//Create week day string
    TInt wkDayRes = KErrNotFound;

    switch( aDay )
        {
        case EMonday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_MO;
            break;
        case ETuesday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_TU;
            break;
        case EWednesday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_WE;
            break;
        case EThursday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_TH;
            break;
        case EFriday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_FR;
            break;
        case ESaturday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_SA;
            break;
        case ESunday:
            wkDayRes = R_QTN_WEEK_TWO_CHARS_SU;
            break;
        default:
            // invalid weekday fetched
	        User::Leave( KErrNotFound );
        }

    return StringLoader::LoadLC( wkDayRes, &aCoeEnv );
    }


HBufC* CAiDatePublisher::ConstructDateStringL()
    {
    // Construct date string using date format from resource file
    CCoeEnv* coeEnv = CCoeEnv::Static();

    if( !coeEnv )
        {
        User::Leave( KErrNotReady );
        }

    TTime time;
    time.HomeTime();

    HBufC* aiDateString = HBufC::NewLC( KMaxDateStringLength );
    HBufC* aiDateFormatString = StringLoader::LoadLC( R_ACTIVEIDLE_TIME_FORMAT,
    													coeEnv );
    HBufC* dateStringBuf = HBufC::NewLC( KMaxDateStringLength );
    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_SHORT_WITH_ZERO,
    												coeEnv );
    TPtr dateString( dateStringBuf->Des() );
    time.FormatL( dateString, *dateFormatString );
    CleanupStack::PopAndDestroy( dateFormatString );

    //now dateString contains string which is formatted using
    //R_QTN_DATE_USUAL_WITH_ZERO

    // To arabic
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateString );

    TPtr aiDateStringPtr = aiDateString->Des();
    
    TDayNameAbb wkDayAbb = TDayNameAbb();
    wkDayAbb.Set(time.DayNoInWeek());
        
    //add date to string
    StringLoader::Format( aiDateStringPtr, *aiDateFormatString,	1,dateString );

    //reuse dateString
    dateString.Copy( aiDateStringPtr );

    //add weekday to string
    StringLoader::Format( aiDateStringPtr, dateString, 0, wkDayAbb );

    CleanupStack::PopAndDestroy( dateStringBuf );//dateStringBuf, aiDateFormatString
    CleanupStack::PopAndDestroy( aiDateFormatString );

    CleanupStack::Pop( aiDateString );
    return aiDateString;
    }


TBool CAiDatePublisher::RefreshL( TInt aContentId, TBool /*aClean*/ )
	{
	if( aContentId == EAiDeviceStatusContentDate )
	    {
	    RefreshDateL();
	    return ETrue;
	    }
	return EFalse;
	}
	
	
TInt CAiDatePublisher::HandleCallBackL( TAny *aPtr )
	{
	CAiDatePublisher* self = static_cast<CAiDatePublisher*>( aPtr );
	
	if( self )
	    {
        TInt changes( self->iEnvNotifier->Change() );
        
        if ( changes & 
        	( EChangesLocale | EChangesMidnightCrossover | EChangesSystemTime ) )
        	{
        	self->RefreshDateL();
        	}	    
	    }    
    	
	return KErrNone;
	}
