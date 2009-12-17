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
* Description:  Presents plugin info using states uid, interface, type and name
*
*/



#ifndef C_CPLUGININFO_H
#define C_CPLUGININFO_H


#include <e32base.h>

namespace HSPluginSettingsIf{


/**
 *  Presents plugin info using states uid, interface, type and name
 *
 *  @code
 *  @endcode
 *
 *  @lib xnlayoutengine.lib
 *  @since S60 v5.0
 */
class CPluginInfo : public CBase
    {
public:    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPluginInfo* NewL();
    IMPORT_C static CPluginInfo* NewLC();


    /**
    * Destructor.
    */
    IMPORT_C virtual ~CPluginInfo();

    /**
     * Set configuration uid
     *
     * @param aUid configuration uid
     */
    IMPORT_C CPluginInfo& SetUidL(const TDesC8& aUid);
        
    /**
     * Get configuration uid
     *
     * @return configuration uid or KNullDesC8
     */
    IMPORT_C const TDesC8& Uid()const;
        
    /**
     * Set configuration type
     *
     * @param aType configuration type
     */
    IMPORT_C CPluginInfo& SetTypeL(const TDesC8& aType);
        
    /**
     * Get configuration type
     *
     * @return configuration type or KNullDesC8
     */
    IMPORT_C const TDesC8& Type()const;
        
    /**
     * Set configuration interface
     *
     * @param aInterface configuration interface
     */
    IMPORT_C CPluginInfo& SetInterfaceL(const TDesC8& aInterface);
        
    /**
     * Get configuration interface
     *
     * @return configuration interface or KNullDesC8
     */
    IMPORT_C const TDesC8& Interface()const;
        
    /**
     * Set configuration name
     *
     * @param aName configuration name
     */
    IMPORT_C CPluginInfo& SetNameL(const TDesC8& aName);
        
    /**
     * Get configuration name
     *
     * @return configuration name or KNullDesC8
     */
    IMPORT_C const TDesC8& Name()const;

    /**
     * Set configuration state
     *
     * @param aConfigurationState configuration state
     */    
    IMPORT_C CPluginInfo& SetConfigurationStateL(
        const TDesC8& aConfigurationState);

    /**
     * Get configuration state
     *
     * @return configuration state or KNullDesC8
     */
    IMPORT_C const TDesC8& ConfigurationState()const;
    
private:

    CPluginInfo();

    void ConstructL();


private: // data

    /**
     * Own. Plugin uid
     */
    HBufC8* iUid;
    
    /**
     * Own. Plugin interface
     */
    HBufC8* iInterface;
    
    /**
     * Own. Plugin type
     */
    HBufC8* iType;
    
    /**
     * Own. Plugin name
     */
    HBufC8* iName;

    /**
     * Own. Plugin configuration state
     */    
    HBufC8* iConfigurationState;    
    };
}

#endif // C_CPLUGININFO_H
