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

#include <f32file.h>
#include "testutils.h"

_LIT(KMcsFolderC, "C:\\private\\200113dd\\content\\");
_LIT(KMcsFolderImportC, "C:\\private\\200113dd\\import\\content\\");
_LIT(KTestdataMCSFolder, "C:\\testing\\data\\MCSAPI\\");
_LIT(KTestdataMCSFolderImport, "C:\\testing\\data\\MCSAPI\\import\\");

_LIT(KMcsFolderAllFilesC, "C:\\private\\200113dd\\content\\*.*");

//-----------------------------------------------------------------------
// CONSTRUCTION
// ---------------------------------------------------------------------------
// 
CTestUtils* CTestUtils::NewL()
    {
    CTestUtils* self = CTestUtils::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CTestUtils* CTestUtils::NewLC()
    {
    CTestUtils* self = new( ELeave ) CTestUtils();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
CTestUtils::~CTestUtils()
    {
    delete iFileManager;
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CTestUtils::CTestUtils()
    {    
    
    }

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void CTestUtils::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    iFileManager = CFileMan::NewL( iFs );
    iFs.MkDirAll(KMcsFolderC);
    iFs.MkDirAll(KMcsFolderImportC);
    }


//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CTestUtils::Copy( const TDesC& aFileName, TInt aSuffix )
    {
    _LIT(KXml, ".xml");
    TBuf<150> source(KTestdataMCSFolder);
    TBuf<150> dest(KMcsFolderC);
    source.Append( aFileName );
    source.Append( KXml );
    
    dest.Append( aFileName );
    dest.AppendNum( aSuffix );
    dest.Append( KXml );
    
    
    return iFileManager->Copy(source, dest);  
    }

//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CTestUtils::CopyImport( const TDesC& aFileName, TInt aSuffix )
    {
    _LIT(KXml, ".xml");
    TBuf<150> source(KTestdataMCSFolderImport);
    TBuf<150> dest(KMcsFolderImportC);
    source.Append( aFileName );
    source.Append( KXml );
    
    dest.Append( aFileName );
    dest.AppendNum( aSuffix );
    dest.Append( KXml );
    
    
    return iFileManager->Copy(source, dest);  
    }


//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CTestUtils::Copy( const TDesC& aSource, const TDesC& aDest )
    {
    return iFileManager->Copy(aSource, aDest);  
    }    
    
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
TInt CTestUtils::Delete( /*const TDesC& aFilePath*/ )
    {
    return iFileManager->Delete( KMcsFolderAllFilesC );  
    }


//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void CTestUtils::InstallSisL( const TDesC& aName )
    {
    
    }
    
    
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void CTestUtils::UninstallSisL( const TDesC& aName )
    {
    
    }    
