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
* Description:  Implementation of ChspsOdtDump.
*
*/


#include "hspsodtdump.h"
#include "hspslogbus.h"
#include <hspsodt.h>
#include <hspsdomdocument.h>
#include <hspsdomlist.h>
#include <hspsdomdepthiterator.h>
#include <hspsdomnode.h>
#include <hspsdomattribute.h>
#include <hspsresource.h>
#include <utf.h>

//----------------------------------------------------------------------------
// ChspsOdtDump::Dump()
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C void ChspsOdtDump::Dump( ChspsODT& aOdt, ChspsLogBus& aLogBus )
    {    
    TRAP_IGNORE( DumpL( aOdt, aLogBus ) );
    }
#else
EXPORT_C void ChspsOdtDump::Dump( ChspsODT& /*aOdt*/, ChspsLogBus& /*aLogBus*/ )
    {    
    }
#endif

//----------------------------------------------------------------------------
// ChspsOdtDump::DumpL()
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsOdtDump::DumpL( ChspsODT& aOdt, ChspsLogBus& aLogBus )
    {            
    __UHEAP_MARK;
        ChspsDomDocument& domDocument = aOdt.DomDocument();
    
        aLogBus.LogText( _L( "### DOM DUMP START ###" ) );
    
        TInt attrCount(0);
        // TInt propCount(0);
        
        ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *domDocument.RootNode() );
        CleanupStack::PushL( iter );
        
        _LIT8(KDelim, "\t" );
        ChspsDomNode* nodeParent( NULL );
        ChspsDomNode* node = iter->First();
        while ( node )    
            {
            const TInt depth = GetNodeDepth( node );
                            
            HBufC8 *nameBuf = GetFixedTextLC( node->Name(), depth, KDelim, ETrue );
            CleanupStack::PushL( nameBuf );
            const TDesC8& name = nameBuf->Des();
            
            aLogBus.LogText( _L8("%S\n"), &name );              
            
            ChspsDomList& attrList = node->AttributeList();
            TInt length( attrList.Length() );
            attrCount += length;
            //_LOGTFRM1(_L8(" Attribute count: %d\n"), length );
            for( TInt i=0; i<length; i++ )
                {
                ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.Item(i) );
                
                HBufC8 *attrBuf = GetFixedTextLC( attr->Name(), depth,  KDelim, EFalse );
                CleanupStack::PushL( attrBuf );
                const TDesC8& attrName = attrBuf->Des();
                
                const TDesC8& attrValue = attr->Value();
                                        
                aLogBus.LogText( _L8("%S=%S\n"), &attrName, &attrValue );
                
                CleanupStack::PopAndDestroy( attrBuf );
                }
            
            if ( node->Parent() )
                {
                nodeParent = node;
                }
            if ( node != nodeParent )
                {
                
                }
            node = iter->NextL();
            
            CleanupStack::PopAndDestroy( nameBuf );
            }
        CleanupStack::PopAndDestroy( iter );

       aLogBus.LogText( _L( "### DOM DUMP END ###" ) );        

       aLogBus.LogText( _L8("**** Resources: ") );
       
       for( TInt index=0; index < aOdt.ResourceCount(); index++ )
           {
           ChspsResource* r = &aOdt.ResourceL(index);     
           TBuf8<512> name;
           CnvUtfConverter::ConvertFromUnicodeToUtf8( name, r->FileName() );               
           aLogBus.LogText( _L8( "%D) %S\n" ), index, &name );
           }
       
       aLogBus.LogText( _L8("**** Resources END ") );        
       
    __UHEAP_MARKEND;    
    }
#else
void ChspsOdtDump::DumpL( ChspsODT& /*aOdt*/, ChspsLogBus& /*aLogBus*/ )
    {    
    }
#endif

//----------------------------------------------------------------------------
// ChspsOdtDump::GetNodeDepth()
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
TInt ChspsOdtDump::GetNodeDepth( ChspsDomNode *aNode )
    {
    TInt depth = 0;
    ChspsDomNode* node( aNode );
    while( node )
        {
        node = node->Parent();
        if ( node )
            {
            depth++;
            }
        }
    return depth;
    }
#else
TInt ChspsOdtDump::GetNodeDepth( ChspsDomNode* /*aNode*/ )
    {
    return 0;
    }
#endif

//----------------------------------------------------------------------------
// ChspsOdtDump::GetFixedTextLC()
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC8* ChspsOdtDump::GetFixedTextLC( const TDesC8& aText,
                                      const TInt aDepth,
                                      const TDesC8& aDelim,
                                      TBool aShowHighlight )
    {
    HBufC8 *buf = HBufC8::NewL( aDepth * aDelim.Length() + aText.Length() + 1 + 1 );    
    TInt i = 0;
    for( ; i < aDepth; i++ )
        {
        buf->Des().Append( aDelim );
        }
    if ( !aShowHighlight )
        {
        buf->Des().Append( _L("*") );
        }
    buf->Des().Append( aText );
    return buf;
    }
#else
HBufC8* ChspsOdtDump::GetFixedTextLC( const TDesC8& /*aText*/,
                                      const TInt /*aDepth*/,
                                      const TDesC8& /*aDelim*/,
                                      TBool /*aShowHighlight*/ )
    {
    return NULL;
    }
#endif
