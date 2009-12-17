/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Locale parsing and applying languages
*
*/



// INCLUDE FILES
#include    "hspslocaleparser.h"
#include    "hspsdtdentity.h"
#include    "hspsdomdocument.h"
#include    "hspsdomnode.h"
#include    "hspsdomattribute.h"
#include    "hspsdomdepthiterator.h"
#include    "hspsdomlist.h"

#include    <s32file.h>


// LOCAL CONSTANTS AND MACROS


_LIT8(KEntityStart, "<!ENTITY" );
_LIT8(KhspsCommonWSChars, " \n\t\r");

// Predefined entities
_LIT8(KLtLitRef,"&lt;");
_LIT8(KLtCharRef,"&#60;");
_LIT8(KLtLit,"<");

_LIT8(KGtLitRef,"&gt;");
_LIT8(KGtCharRef,"&#62;");
_LIT8(KGtLit,">");

_LIT8(KAmpersandLitRef,"&amp;");
_LIT8(KAmpersandCharRef, "&#38;");
_LIT8(KAmpersandLit, "&");

_LIT8(KUnderscore, "_");

_LIT8(KQuotaLitRef,"&quot;");
_LIT8(KQuotaCharRef,"&#34;");
_LIT8(KQuotaLit,"\"");

_LIT8(KAposLitRef,"&apos;");
_LIT8(KAposCharRef,"&#39;");
_LIT8(KAposLit,"\'");


_LIT8(KPercentRef, "&#37;");
_LIT8(KPercentLit, "%");


// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// SeekWSSequence() Seek end or start of white space sequence.
// Returns: Offset.
// -----------------------------------------------------------------------------
//
static TInt SeekWSSequence(TPtr8 aPtr, TBool aStart)
    {
    TInt wsIndex( 0 );
    TInt len( aPtr.Length() );
    for ( ; wsIndex<len ; wsIndex++ )
        {
        TInt ret = KhspsCommonWSChars().LocateF( aPtr[wsIndex] );
        if ( aStart )
            {
            if ( ret != KErrNotFound )
                {
                break;       
                }
            }
        else
            {
            if ( ret == KErrNotFound )
                {
                break;    
                }
            }    
        
        }
    return wsIndex;
    }

// -----------------------------------------------------------------------------
// ReplaceCharacterReferences Replace predefined character references with 
// literals.
// Returns: On return buffer has literals instead of character references.
// -----------------------------------------------------------------------------
//
static void ReplaceCharacterReferences( TPtr8& aPtr )
    {
    
    TInt pos( KErrNotFound );
    //Replace all lt
    while ( (pos = aPtr.Find( KLtLitRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KLtLitRef().Length(), KLtLit );    
        }
    
    while ( (pos = aPtr.Find( KLtCharRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KLtCharRef().Length(), KLtLit );    
        }
    //Replace all gt
    while ( (pos = aPtr.Find( KGtLitRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KGtLitRef().Length(), KGtLit );    
        }
    
    while ( (pos = aPtr.Find( KGtCharRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KGtCharRef().Length(), KGtLit );    
        }
    //Replace all ampersands
    while ( (pos = aPtr.Find( KAmpersandLitRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KAmpersandLitRef().Length(), KAmpersandLit );    
        }
    while ( (pos = aPtr.Find( KAmpersandCharRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KAmpersandCharRef().Length(), KAmpersandLit );    
        }        
    
    //Replace all quotas
    while ( (pos = aPtr.Find( KQuotaLitRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KQuotaLitRef().Length(), KQuotaLit );    
        }
    while ( (pos = aPtr.Find( KQuotaCharRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KQuotaCharRef().Length(), KQuotaLit );    
        }    
    
     //Replace all apos
    while ( (pos = aPtr.Find( KAposLitRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KAposLitRef().Length(), KAposLit );    
        }
    while ( (pos = aPtr.Find( KAposCharRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KAposCharRef().Length(), KAposLit );    
        }   
    //Replace all percents
    while ( (pos = aPtr.Find( KPercentRef ) ) > KErrNotFound )
        {
        aPtr.Replace( pos,KPercentRef().Length(), KPercentLit );    
        }
    
    
    }
// -----------------------------------------------------------------------------
// ParseL Parses the DTD file.
// 
// Returns: On return aEntityReferences contains parsed entityreferences with 
//          values.
// -----------------------------------------------------------------------------
//
static void ParseL(  
    RFile& aFile, 
    RPointerArray<ChspsDtdEntity>& aEntityReferences  )
    {
    const TChar KQuotation = '"';
    const TChar KSingleQuotation = '\'';
    
    TInt fileSize( 0 );
    aFile.Size( fileSize );
    if ( !fileSize )
        {
        return;    
        }
    HBufC8* buf = HBufC8::NewLC( fileSize );
    TPtr8 p( buf->Des() );
    
    if ( aFile.Read( p ) == KErrNone )
        {
        // Now we have read file into buffer
        // syntax is <!ENTITY menu_first_item "File">
        // Do until end of file
        // Find first token <!ENTITY
        ChspsDtdEntity* entity = NULL;
        TInt pos( p.FindF(KEntityStart) );
        while( pos != KErrNotFound && p.Length() )
            {
            p.Delete(0, pos + KEntityStart().Length() );   
            // Consume following white spaces
            TInt consume = SeekWSSequence(p,EFalse);
            p.Delete(0,consume);
            // Read until next white space sequence starts
            TInt use = SeekWSSequence(p,ETrue);
            // Now we have reference name
            entity = ChspsDtdEntity::NewL();
            CleanupStack::PushL( entity );
            entity->SetEntityL( p.Left(use) );
            // Consume reference name
            p.Delete(0,use);
            // Consume following white spaces
            consume = SeekWSSequence(p,EFalse);
            p.Delete(0,consume);
            // Read everythig between "" or ''
            TBool quatation( KQuotation == p[0] );
            TBool singleQuatation( KSingleQuotation == p[0] );
            if ( quatation || singleQuatation  )
                {
                p.Delete(0,1);
                TInt len( p.Length() );
                TInt i(0);
                for(; i<len; i++ )
                    {
                    if ( (quatation?KQuotation:KSingleQuotation) == p[i] )
                        {
                        break;    
                        }
                    }
                HBufC8* tmp = p.Left(i).AllocLC();    
                TPtr8 tmpP( tmp->Des() );
                ReplaceCharacterReferences( tmpP );    
                entity->SetValueL( tmpP );
                CleanupStack::PopAndDestroy( tmp );
                aEntityReferences.AppendL( entity );
                CleanupStack::Pop( entity );                   
                p.Delete(0,i);
                }
            else
                {
                CleanupStack::PopAndDestroy( entity );    
                }    
            pos = p.FindF(KEntityStart);
            }
        
        }
    CleanupStack::PopAndDestroy(buf);
    
    }

// -----------------------------------------------------------------------------
// FindAndReplaceEntitiesL Finds entity reference from the input string 
// and replaces found entity references with entity values. 
// Entity reference starts with &-mark and ends with semicolon ';'
// e.g &menu_app_str;
// 
// Returns: HBufC8: Caller has the ownership. Length is 0, if there are no 
//                  enties replaced
// -----------------------------------------------------------------------------
//

static HBufC8* FindAndReplaceEntitiesL( 
    const TDesC8& aValue,
    RPointerArray<ChspsDtdEntity>& aEntityReferences  ) //Array holding parsed entities
    {
    TChar entityRef('&');
    TChar semicolon(';');
    
    HBufC8* replaced = KNullDesC8().AllocL();
    
    TInt localeCount( aEntityReferences.Count() );
   
    if ( aValue.Length() <= 0 )
        {
        return replaced;
        }
    
    TLex8 lex( aValue );
    while ( lex.Peek() )
        {
        TChar ch( lex.Get() );
        if ( ch == entityRef ) //Entity token start found
            {
            lex.Mark();
            lex.SkipCharacters();
            TPtrC8 p( lex.MarkedToken() );
            if ( p.LocateReverse( semicolon ) != KErrNotFound ) //Token ends with semicolon
                {
                // We found a reference in skeleton, lets find the same ref in the list 
                TBool found( EFalse );
                for ( TInt j=0; j<localeCount && !found; j++ )
                    {
                    //Value is now in form 'qtn_app_menu;' thus strip trailing semicolon
                    const TDesC8& entityRef = p.Left( p.Length()-1 );
                    ChspsDtdEntity* dtdEntity = aEntityReferences[j];
                    const TDesC8& entity = dtdEntity->Entity();
                    const TDesC8& entityVal = dtdEntity->Value(); 
                           
                    if( entity.CompareC( entityRef ) == 0 )
                        {
                        // We have the ref-value pair 
                        CleanupStack::PushL( replaced );
                        replaced = 
                            replaced->ReAllocL(replaced->Length() + entityVal.Length() );
                        replaced->Des().Append( entityVal );
                        CleanupStack::Pop(1);
                        found = ETrue;
                        }
                    }//End for
                }// End entity token end
            }//End entity token start found
        else
            {
            lex.UnGet();
            lex.Mark();   
            lex.SkipCharacters(); //Token must start with '&'
            lex.SkipSpace();
            TPtrC8 token( lex.MarkedToken() );
            CleanupStack::PushL( replaced );
            replaced = replaced->ReAllocL(replaced->Length() + token.Length() );
            replaced->Des().Append( token );
            CleanupStack::Pop(1);
            }    
        }//End while
           
    return replaced;    
    }

static void AddAttributeDescL(
        ChspsDomNode& aNode,
        const TDesC8& aAttrName, 
        const TDesC8& aValue )
    {
    ChspsDomList& attrList = aNode.AttributeList();
    ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(aAttrName) );
    if ( attr )
        {               
        // Replace value of the attribute
        attr->SetValueL( aValue );
        }
    else
        {              
        // Add an attribute
        ChspsDomAttribute* attr = ChspsDomAttribute::NewL( aAttrName, aNode.StringPool() );    
        CleanupStack::PushL( attr );            
        attr->SetValueL( aValue );
        ChspsDomList& attrList = aNode.AttributeList();
        attrList.AddItemL( attr );    //takes ownership
        CleanupStack::Pop( attr );
        }        
    }
    
// -----------------------------------------------------------------------------
// ApplyListToDomL Seek if node has any entity references. Replace entity 
// with value if there are any.
// 
// Returns: On return node containing localized strings.
// -----------------------------------------------------------------------------
//
static void ApplyListToNodeL( 
    ChspsDomNode& aNode,  //Node to match
    RPointerArray<ChspsDtdEntity>& aEntityReferences  ) //Array holding parsed entities
    {
    HBufC8* replaced = NULL;
    ChspsDomAttribute* attr = NULL;
    ChspsDomList& attrList = aNode.AttributeList();
    TInt count( attrList.Length() );
    
    const TDesC8& name = aNode.Name();
    
    // Loop node's attributes
    for ( TInt i=0; i<count; i++ )
        {
        attr = static_cast<ChspsDomAttribute*>( attrList.Item(i) );
        
        const TDesC8& value = attr->Value();
        
        // Check whether the attribute value matches an entity  
        replaced  = FindAndReplaceEntitiesL( value, aEntityReferences );
        CleanupStack::PushL( replaced );
        if( replaced->Length() )
            {            
            TPtrC8 entityName = attr->Name();
            const TInt indicatorLength = KUnderscore().Length();
            if ( entityName.Length() > indicatorLength && entityName.Left(indicatorLength) == KUnderscore )
            	{
            	// Name of the variant attribute is formed by removal of the 1st "_" character
            	TPtrC8 variantName = entityName.Mid(indicatorLength);
            	
            	// Add the variant attribute or update it's exisitng value
            	AddAttributeDescL(
        			aNode,
        			variantName,
        			*replaced );            		
            	}                     
            }
        CleanupStack::PopAndDestroy( replaced );
        replaced = NULL;    
        }

    const TDesC8& pcData = aNode.PCData();
    replaced = FindAndReplaceEntitiesL( pcData, aEntityReferences );
    CleanupStack::PushL( replaced );
    if( replaced->Length() )
        {
        aNode.SetPCDataL( *replaced );
        }
    CleanupStack::PopAndDestroy( replaced );
    replaced = NULL;     
    }
    
// -----------------------------------------------------------------------------
// ApplyListToDomL Traverse the DOM and apply entity reference values to node.
// 
// Returns: On return DOM containing localized strings.
// -----------------------------------------------------------------------------
//
static void ApplyListToDomL( 
    ChspsDomDocument& aLocalizedDom, //DOM to be localized
    RPointerArray<ChspsDtdEntity>& aEntityReferences )//Array holding parsed entities
    {
    ChspsDomNode* root = aLocalizedDom.RootNode();
    
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *root );
    CleanupStack::PushL( iter );
    ChspsDomNode* node = iter->First();
    while ( node )
        {
        ApplyListToNodeL( *node, aEntityReferences );
        node = iter->NextL();
        }
   
    CleanupStack::PopAndDestroy( iter );
    }
        
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsLocaleParser::ChspsLocaleParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsLocaleParser::ChspsLocaleParser()
    {
    }

// -----------------------------------------------------------------------------
// - ChspsLocaleParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsLocaleParser::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// ChspsLocaleParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsLocaleParser* ChspsLocaleParser::NewL()
    {
    ChspsLocaleParser* self = new( ELeave ) ChspsLocaleParser;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
ChspsLocaleParser::~ChspsLocaleParser()
    {
    iEntityReferences.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// ChspsLocaleParser::ApplyDtdToDomL
// Applies locale references to a skeleton dom
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsLocaleParser::ApplyDtdToDomL(
    RFs& aFsSession, 
    const TDesC& aFileName,
    ChspsDomDocument& aLocalizedDom )
    {
    RFile infile;
    User::LeaveIfError( infile.Open(aFsSession, aFileName, EFileRead) );
    CleanupClosePushL( infile );
    
    ParseL( infile, iEntityReferences );
    if( iEntityReferences.Count() )
        {
        ApplyListToDomL( aLocalizedDom, iEntityReferences );        
        iEntityReferences.ResetAndDestroy();
        }
    
    CleanupStack::PopAndDestroy(&infile);
    }

//  End of File  
