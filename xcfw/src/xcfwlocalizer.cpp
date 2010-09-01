/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for XCFWLocalizer
*
*/



// INCLUDE FILES
#include    "xcfwlocalizer.h"
#include    <bautils.h>
#include    <e32std.h>
#include    <badesca.h>
#include    <s32file.h>
#include    <s32strm.h>
#include    <utf.h>
#include    <pathinfo.h>    
// CONSTANTS
_LIT8( KEntityStart, "!ENTITY " );
_LIT( KDrivePathChars, ":\\" );
const TInt KDefaultGranularity = 10;
//Maximum entity declaration length is rather long because
//in RND builds the DTD files contain a lot of comments 
//and parsing the file requires long enough buffer.
const TInt KMaxEntityDeclLen = 1024; 
const TInt KPathStartLoc = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXCFWLocalizer::CXCFWLocalizer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXCFWLocalizer::CXCFWLocalizer()
    {
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXCFWLocalizer::ConstructL()
    {
    iReferences = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    iLocStrings = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::NewL
// Two-phased constructor with Node Data initializer.
// -----------------------------------------------------------------------------
//
CXCFWLocalizer* CXCFWLocalizer::NewL()
    {
    CXCFWLocalizer* self = new( ELeave ) CXCFWLocalizer;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

   
// Destructor
CXCFWLocalizer::~CXCFWLocalizer()
    {
    delete iReferences;
    delete iLocStrings;
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::LoadDTDL
// Load entity references from DTD file
// Function searches the DTD file first from session path (which is where
// the loaded XML file is located). If it is not found there, it is sought
// from ROM drive at the same location. 
// If function can't find loc file for current language, it will search the
// possible replacement using language downgrade path.
// -----------------------------------------------------------------------------
//
void CXCFWLocalizer::LoadDTDL(
    const TDesC& aDTD,
    RFs& aFileSystem,
    TRequestStatus* aStatus )
    {
    const TInt KMaxLangCodeLen = 5;
    const TInt KMinLangCodeLen = 2;

    TBuf<KMaxLangCodeLen> lcstring;

    delete iLocStrings;
    iLocStrings = NULL;
    delete iReferences;
    iLocStrings = NULL;
    iReferences = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    iLocStrings = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    
    iStatus = aStatus;
    

    //First get filepath without drive character
    TPtrC filepath;
    if ( aDTD.Find( KDrivePathChars ) != KErrNotFound )
        {
        filepath.Set( aDTD.Mid( KPathStartLoc ) );             
        }
    else
        {
        filepath.Set( aDTD );
        }

    // format filepath with current language data
    TLanguage langcode = User::Language(); //current language
    lcstring.Num( (TInt64)langcode ); //to get decimal langcode length in characters
    //use always at least 2 digits.
    TInt codelen = lcstring.Length() > 1 ? lcstring.Length() : KMinLangCodeLen; 

    HBufC* locfile = HBufC::NewL( aDTD.Length() + KMaxLangCodeLen );
    CleanupStack::PushL( locfile );
    locfile->Des().Format( filepath, codelen, langcode );

    //Check for file, if not found, check language downgrade path
    if( !BaflUtils::FileExists( aFileSystem, locfile->Des() ) )
        {
        RArray<TLanguage> langs;
        BaflUtils::GetDowngradePathL( aFileSystem, langcode, langs );
        //add current language to array for ROM fetching
        langs.Append( langcode );
        TInt current = langs.Count() - 1;
        do 
            {
            lcstring.Num( (TInt64)langs[current] );
            codelen = lcstring.Length()>1?lcstring.Length():KMinLangCodeLen;
            locfile->Des().Format( filepath, codelen, langs[current] );
            current--;
            } 
        while ( current >= 0 && 
                !BaflUtils::FileExists( aFileSystem, locfile->Des() ) );
        
        //If still not found, check from ROM
        if ( !BaflUtils::FileExists( aFileSystem, locfile->Des() ) )
            {
            current = langs.Count() - 1;
            do {
                lcstring.Num( (TInt64)langs[current] );
                codelen = lcstring.Length()>1?lcstring.Length():KMinLangCodeLen;
                locfile->Des().Format( filepath, codelen, langs[current] );
                locfile->Des().Insert( 0, PathInfo::RomRootPath().Left( KPathStartLoc ) );
                current--;
                } while ( current >= 0 && 
                    !BaflUtils::FileExists( aFileSystem, locfile->Des() ) );
            }
        langs.Reset();
        langs.Close();
        }


    if ( BaflUtils::FileExists( aFileSystem, locfile->Des() ) )
        {
        RFile infile;
        User::LeaveIfError( infile.Open(aFileSystem, locfile->Des(), EFileRead) );
        CleanupClosePushL( infile );
        RFileReadStream readStream( infile,0 );
        CleanupClosePushL( readStream );
       
        ParseDTDL( readStream );
       
        CleanupStack::Pop( &readStream ); //readStream
        CleanupStack::Pop( &infile ); //infile
        }
    

    CleanupStack::PopAndDestroy( locfile );

    *aStatus = KRequestPending;
    User::RequestComplete( aStatus, KErrNone );
    
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::LoadDTDL
// Parse DTD file. Function assumes that the DTD contains ENTITY declarations
// only.
// -----------------------------------------------------------------------------
//
void CXCFWLocalizer::ParseDTDL( RFileReadStream& aStream )
    {
    
    TChar space = ' ';
    TChar quote = '"';
    TChar lessthan = '<';
    TChar morethan = '>';

    HBufC8* buf = HBufC8::NewLC( KMaxEntityDeclLen );
    TPtr8 p( buf->Des() );

    TInt streamError = KErrNone;

    do 
        {
        // Read until declaration start "<"
        TRAP( streamError, aStream.ReadL( p , lessthan)); 
        // This might be EOF, or other problem, we exit 
        if( streamError != KErrNone )   
            {
            aStream.Close();
            }
        else
            {
            aStream.ReadL( p , space); // Read until space " "
            
            // entity declaration handled, others are ignored
            if ( p.CompareC( KEntityStart ) == 0 ) 
                {
                p.Zero();
                aStream.ReadL( p , space); // Read the reference name
                p.TrimRight();  // Delete space from the end
                
                HBufC* ucsbuf = HBufC::NewLC( p.Length() );
                TPtr ucsp( ucsbuf->Des() );
                CnvUtfConverter::ConvertToUnicodeFromUtf8( ucsp, p );
                iReferences->AppendL( ucsp );
                CleanupStack::PopAndDestroy( ucsbuf );
                
                p.Zero();
                aStream.ReadL( p , quote);
                p.Zero();
                aStream.ReadL( p , quote); 
                p.SetLength(p.Length() - 1); // Delete quotation from the end

                //Seek for percentage char symbol and do necessary conversion
                const TInt KUCEntityLen = 8;
                _LIT8( KUniCodeChar, "&#x0025;");
                _LIT8( KPctage, "%" );
                TInt pct = p.Find( KUniCodeChar );
                while ( pct != KErrNotFound )
                    {
                    HBufC8* tmp = HBufC8::NewLC( KMaxEntityDeclLen );
                    tmp->Des().Copy ( p.Left( pct ) );
                    tmp->Des().Append( KPctage );
                    tmp->Des().Append( p.Mid( pct + KUCEntityLen ) );
                    p.Copy ( tmp->Des() );
                    CleanupStack::PopAndDestroy( tmp );
                    pct = p.Find( KUniCodeChar );
                    }


                ucsbuf = HBufC::NewLC( p.Length() );
                ucsp.Set ( ucsbuf->Des() );
                CnvUtfConverter::ConvertToUnicodeFromUtf8( ucsp, p );
                iLocStrings->AppendL( ucsp );
                CleanupStack::PopAndDestroy( ucsbuf );

                
                p.Zero();
                aStream.ReadL( p , morethan); // Read until declaration ends ">"           
                }
            }
        
        } while ( streamError == KErrNone );
    
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::EntityRefToText
// Gets localized text value for given entity reference
// -----------------------------------------------------------------------------
//
TInt CXCFWLocalizer::EntityRefToText(
    TDesC& aRef, 
    TPtrC& aText)
    {
    TInt count = iReferences->Count();
    TInt ret = KErrNotFound;
    for ( TInt i = 0; i < count && ret == KErrNotFound; i++ )
        {
        if ( iReferences->MdcaPoint( i ).Compare( 
            aRef.Mid(1, aRef.Length()-2) ) == 0 )
            {
            ret = KErrNone;
            aText.Set( iLocStrings->MdcaPoint( i ) );
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::TextToEntityRef
// Gets entity reference for given text 
// -----------------------------------------------------------------------------
//
TInt CXCFWLocalizer::TextToEntityRef(
    TDesC& aText, 
    TPtrC& aRef )
    {
    TInt count = iLocStrings->Count();
    TInt ret = KErrNotFound;
    for ( TInt i = 0; i < count && ret == KErrNotFound; i++ )
        {
        if ( iLocStrings->MdcaPoint( i ).Compare( aText ) == 0 )
            {
            ret = KErrNone;
            aRef.Set( iReferences->MdcaPoint( i ) );
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CXCFWLocalizer::LastError
// Returns error code from the parser.
// -----------------------------------------------------------------------------
//
TInt CXCFWLocalizer::LastError()
    {
    TInt ret = KErrNone;

    if ( !iReferences || !iLocStrings )
        {
        ret = KErrNotReady;            
        }
    else
        {
        if ( iReferences->Count() != iLocStrings->Count() )
            {
            ret = KErrCorrupt;             
            }
        }

    return ret;        

    }



//  End of File  
