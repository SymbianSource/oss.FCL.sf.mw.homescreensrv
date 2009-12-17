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
* Description:  
*
*  HSPS ChspsPluginIdList-class defines all information what is needed to maintain
*  resources of Application Theme. Every ChspsPluginIdList-objects defines just one 
*  resource source, however, the same resource source could be referenced
*  multiple times inside a theme - resources are usually shared. 
*  HSPS Application Theme Management system supports theme resourcing by 
*  offering resource conversations and storing services, and offering Rendering
*  Engines the following services:
*  1) the way to match a resource quoted in xml- or css-module to the 
*  corresponding resource source, and 2) a secure way to access a resource 
*  through the Symbian Platform Security's process separation wall.
*
*
*/


#ifndef hspsPLUGINIDLIST_H
#define hspsPLUGINIDLIST_H

#include <e32base.h>
#include <s32mem.h> // streams

/**
*  ChspsPluginIdList is HSPS-utility class for passing IPC data 
*
*  @lib hspsResource.lib
*  @since S60 5.0
*  @ingroup group_hspsresult
*/
class ChspsPluginIdList : public CArrayFixFlat<TInt>
    {
    public:  // Constructors and destructor
                            
    	/**
        * C++ default constructor.
        * 
        * @since S60 5.0
        * @param aGranularity List granularity.
        */
    	IMPORT_C ChspsPluginIdList( TInt aGranularity );
    	    	
        /**
        * Destructor.
        */
    	IMPORT_C virtual ~ChspsPluginIdList();

    public: // New functions
                        
        /**
        * Externalizes ChspsResource-object's member data.
        * 
        * @since S60 5.0
        * @param aStream is a write stream given.
        */
        IMPORT_C void ExternalizeL( RDesWriteStream& aStream ) const;
        
        /**
        * Internalizes ChspsResource-object's member data.
        * 
        * @since S60 5.0
        * @param aStream is data read stream given.
        */
        IMPORT_C void InternalizeL( RDesReadStream& aStream );
        
        /**
        * Calculates data length of the ChspsPluginIdList object.
        * 
        * @since S60 5.0
        * @return Data length.
        */
        IMPORT_C TInt GetDataLength();
                                   
    private:     // Data
    	
    };

#endif      // hspsPLUGINIDLIST_H

// End of File
