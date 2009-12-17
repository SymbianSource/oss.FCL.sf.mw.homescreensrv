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
* Description:  contains id and uid
*
*/



#ifndef C_CPLUGINMAP_H
#define C_CPLUGINMAP_H


#include <e32base.h>


namespace HSPluginSettingsIf{

/**
 *  Presents plugin map i.e id and uid
 *  
 *
 *  @code
 *  
 *  @endcode
 *
 *  @lib xnlayoutengine.lib
 *  @since S60 v5.0
 */
class CPluginMap : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPluginMap* NewL();
    IMPORT_C static CPluginMap* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CPluginMap();

    /**
     * Set plugin id
     *
     * @param aPluginId plugin id
     * @return Reference to this object
     */
    IMPORT_C CPluginMap& SetPluginIdL(const TDesC8& aPluginId);
    
    /**
     * Get plugin id
     *
     * @return Reference to plugin id or KNullDesC8
     */
    IMPORT_C const TDesC8& PluginId()const;
    
    /**
     * Set plugin uid
     *
     * @param aPluginId plugin uid
     * @return Reference to this object
     */
    IMPORT_C CPluginMap& SetPluginUidL(const TDesC8& aPluginUid);
    
    /**
     * Get plugin uid
     *
     * @return Reference to plugin uid or KNullDesC8
     */
    IMPORT_C const TDesC8& PluginUid()const;

private:

    CPluginMap();

    void ConstructL();


private: // data

    /**
     * Own. Plugin id.
     */
    HBufC8* iPluginId;
    
    /**
     * Own. Plugin uid.
     */
    HBufC8* iPluginUid;
    };

}

#endif // C_CPLUGINMAP_H
