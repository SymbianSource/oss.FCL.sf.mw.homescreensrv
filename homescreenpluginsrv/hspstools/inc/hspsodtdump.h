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
* Description:  Header for ChspsOdtDump.
*
*/


#ifndef HSPS_ODTDUMP_H
#define HSPS_ODTDUMP_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class ChspsDomNode;
class ChspsODT;
class ChspsLogBus;

// CLASS DECLARATION

/**
 * Static helper class for dumping DOM model to a textual logging bus.
 * 
 * @lib hspsTools.lib
 * @since S60 5.0
 * @ingroup group_homescreenpluginservice_tools
 */
class ChspsOdtDump : public CBase
    {
public: // Methods for public usage.
    /**
     * Dumps DOM document and resources contained in given ODT.
     * 
     * @since S60 5.0 
     * @param aOdt          ODT object that contains DOM document and resources.
     * @param aLogBus       Target logging channel to be used as output. 
    */
    IMPORT_C static void Dump( ChspsODT& aOdt, ChspsLogBus& aLogBus );
    
private: // Internal.
    /**
     * Dumps DOM document and resources contained in given ODT.
     * 
     * @param aDomDocument  Document to be dumped.
     * @param aLogBus       Target logging channel to be used as output. 
    */
    static void DumpL( ChspsODT& aOdt, ChspsLogBus& aLogBus );
    
    /**
     * Get depth of node.
     * 
     * @param   aNode       Source node.
     * @return  TInt        Depth of node.
    */
    static TInt GetNodeDepth( ChspsDomNode *aNode );

    /**
     * String helper method.
     * 
     * @param   aText           Source text.
     * @param   aDepth          Depth.
     * @param   aDelim          Delimiter.
     * @param   aShowHighlight  Show highlight.
     * @return  HBufC8*         Resulted text.       
    */
    static HBufC8* GetFixedTextLC( const TDesC8& aText,
                                   const TInt aDepth,
                                   const TDesC8& aDelim,
                                   TBool aShowHighlight );    
    };

#endif // HSPS_DOMDUMP_H
