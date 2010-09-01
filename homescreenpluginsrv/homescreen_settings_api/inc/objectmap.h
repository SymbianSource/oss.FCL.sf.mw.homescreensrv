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
* Description:  Presents object map as name, path and mediatype data  
*
*/



#ifndef C_COBJECTMAP_H
#define C_COBJECTMAP_H


#include <e32base.h>

namespace HSPluginSettingsIf{


/**
 *  Presents object map as name, path and mediatype data 
 *  
 *
 *  @code
 *  @endcode
 *
 *  @lib xnlayoutengine.lib
 *  @since S60 v5.0
 */
class CObjectMap : public CBase
    {
public:    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CObjectMap* NewL();
    IMPORT_C static CObjectMap* NewLC();
    

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CObjectMap();

    /**
     * Set object name
     *
     * @param aName object name
     * @return Reference to this object
     */
    IMPORT_C CObjectMap& SetNameL( const TDesC8& aName );
    
    /**
     * Get object name
     *
     * @return Reference to name or KNullDesC8
     */
    IMPORT_C const TDesC8& NameL()const;
    
    /**
     * Set object path
     *
     * @param aPath object path
     * @return Reference to this object
     */
    IMPORT_C CObjectMap& SetPathL( const TDesC8& aPath );
    
    /**
     * Get object path
     *
     * @return Reference to path or KNullDesC8
     */
    IMPORT_C const TDesC8& Path()const;
    
    /**
     * Set object media type
     *
     * @param aMediaType object media type
     * @return Reference to this object
     */
    IMPORT_C CObjectMap& SetMediaTypeL( const TDesC8& aMediaType );
    
    /**
     * Get object media type
     *
     * @return Reference to media type or KNullDesC8
     */
    IMPORT_C const TDesC8& MediaType()const;


private:
    CObjectMap();

    void ConstructL();

private: // data

    /**
     * Own. Object name
     */
    HBufC8* iName;
    
    /**
     * Own. Object path
     */
    HBufC8* iPath;
    
    /**
     * Own. Object mediatype
     */
    HBufC8* iMediaType;
    };

}
#endif // C_COBJECTMAP_H
