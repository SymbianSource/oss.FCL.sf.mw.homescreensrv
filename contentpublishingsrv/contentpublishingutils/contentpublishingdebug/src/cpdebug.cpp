/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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


#include "cpdebug.h"

const TUint KMemDataSize( 100 );
const TUint KBufMaxSize( 512 );
const TUint KThousand( 1000 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDebug::ConstructL( const TDesC& aFile )
    {
    iData = new (ELeave) DebugData();
    iData->iLogEnabled = EFalse;
    iData->iFirstUpdateTime.UniversalTime( );
    iData->iFileName.Create( aFile.Length( ) );
    iData->iFileName = aFile;
    Dll::SetTls( iData );
    User::LeaveIfError( iData->iFs.Connect( ) );
    EnableLogging( EFalse );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CCPDebug* CCPDebug::NewL( const TDesC& aFile )
    {
    CCPDebug* self = NewLC( aFile );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CCPDebug* CCPDebug::NewLC( const TDesC& aFile )
    {
    CCPDebug* self = new (ELeave) CCPDebug();
    CleanupStack::PushL(self);
    self->ConstructL( aFile );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCPDebug::CCPDebug()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
CCPDebug::~CCPDebug()
    {
    if ( iData )
        {
        iData->iFileName.Close( );
        iData->iLogFile.Close( );
        iData->iFs.Close( );
        }
    Dll::FreeTls( );
    delete iData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCPDebug::Enable()
    {
    DebugData* data = Data();
    if(!data)
        {
        return false;
        }
    else
        {
        return true;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCPDebug::EnableLogging(TBool aEnable)
    {
    DebugData* data = Data();
    if(aEnable && !data->iLogEnabled)
        {
        if( data->iLogFile.Replace(data->iFs, data->iFileName, EFileWrite) == KErrNone )
            {
            data->iLogEnabled = ETrue;
            Printf(_L8("CCPDebug::EnableLogging()"));
            }
        }
    else if(!aEnable && data->iLogEnabled)
        {
        data->iLogFile.Close();
        data->iLogEnabled = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCPDebug::Printf(TRefByValue<const TDesC8> aFormat, ...)
    {
    DebugData* data = Data();
    if(!data || !data->iLogEnabled)
        {
        return;
        }
    TTime now;
    now.UniversalTime();
    TInt32 elapsed = (TInt32)(now.MicroSecondsFrom(data->iFirstUpdateTime).Int64() / KThousand);

    TBuf8<KMemDataSize> memData;
    memData.Format(_L8("% 2d,%03d "), elapsed / KThousand, elapsed % KThousand);

    TBuf8<KBufMaxSize> buf;
    VA_LIST list;
    VA_START(list, aFormat);
    buf.FormatList(aFormat, list);
    VA_END(list);
    buf.Insert(0, memData);
    buf.Append(_L8("\n"));

    data->iLogFile.Write(buf);
    data->iLogFile.Flush();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCPDebug::ExtendedPrint( const char* aStringParam,
                                       const CLiwGenericParamList& aInParamList )
    {
    RDebug::Printf( "CPS Client::Request %s Parameters: \n", aStringParam );
    for ( TInt i = 0; i < aInParamList.Count( ); i++ )
        {
        TLiwGenericParam tempParam;
        tempParam.PushL();
        TRAP_IGNORE( aInParamList.AtL(i ,tempParam) );
        Dump( tempParam.Value() );
        CleanupStack::Pop(&tempParam);
        tempParam.Reset();
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C DebugData* CCPDebug::Data()
    {
    return static_cast<DebugData*>(Dll::Tls());
    }

// End of File
