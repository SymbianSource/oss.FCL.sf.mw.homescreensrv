/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Updates applications and icons in Operator Tile.
*
*/

 
#ifndef __HSWIDGETPUBLISHERIMPL_H__
#define __HSWIDGETPUBLISHERIMPL_H__

#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <liwcommon.h>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "hsdataobserver.h"

namespace Hs {

class HsWidget;
class HsWidgetItem;

typedef std::map<std::string, std::wstring> WidgetContentIdMapType;

class HsWidgetPublisherImpl :
    public MLiwNotifyCallback
    {
public://MLiwNotifyCallback

    /**
    * Handles notifications caused by an asynchronous Execute*CmdL call
    * or an event.
    *
    * @param aCmdId The service command associated to the event.
    * @param aEventId Occured event, see LiwCommon.hrh.
    * @param aEventParamList Event parameters, if any, as defined per
    *        each event.
    * @param aInParamList Input parameters, if any, given in the
    *        related HandleCommmandL.
    * @return Error code for the callback.
    */
    virtual TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& aInParamList);

public:
    /**
     */
	HsWidgetPublisherImpl();
    
    /**
     */
    virtual ~HsWidgetPublisherImpl();

    /**
     */
    HsWidget& CreateWidget( std::string& aTemplateName, 
        std::string& aWidgetName,
        std::string& aIdentifier,
        std::string& aDescription,
        std::string& aIconLocation );

    /**
     */
    void RemoveWidget( std::string& aTemplateName, 
        std::string& aWidgetName,
        std::string& aIdentifier );
    
    /**
     */
    void SetDataObserver( IHsDataObserver* aDataObserver );
    
    /**
     */
    HsWidget& GetWidget( std::string& aTemplateName, 
        std::string& aWidgetName,
        std::string& aIdentifier );
    
    /**
     */
    void PublishWidget( HsWidget& aWidget );
    
    
private:
    /**
     */
    MLiwInterface* GetServiceInterfaceL();
    
    /**
     */
    void RegisterNotificationL( HsWidget& aWidget );
    
    /**
     */
    void UnegisterNotificationL();
    
    /**
     */
    void PublishWidgetActionsL( HsWidget& aWidget );
    
    /**
     */
    void PublishWidgetDataL( HsWidget& aWidget );
    
    /**
     */    
    void RemoveWidgetL( HsWidget& aWidget );
    
    /**
     */
    void RemoveFromCpsL( CLiwDefaultMap* aCpDataMap, const TDesC&  aType );
    
    /**
     */
    void TranslateObserverAction( const TDesC8& aActionDes,
        IHsDataObserver::EEvent& aAction );

    /**
     */
    HsWidget* GetWidgetL( const CLiwMap* aMap );

    /**
     */
    void HandleWidgetActionL( const TDesC8& aActionDes,
            const TDesC& aContentIdDes, HsWidget& aWidget );

    /**
     */
    int HandleWidgetItemActionL( const TDesC8& aActionDes,
    		HsWidget& aWidget );

    /**
     */
    bool CheckIfWidgetExist( std::string& aTemplateName, 
        std::string& aWidgetName,
        std::string& aIdentifier,
        TBool aAdjustToIdentifier );
    
    /**
     */
    std::string ToStringL( const TDesC& aText );

    /**
     */
    std::wstring ToWideStringL( const TDesC& aText ) const;
    
    /**
     */
    void InsertWidgetIdentifiersL( HsWidget& aWidget, 
		CLiwDefaultMap* aDataMap );

    /**
     */
    void InsertWidgetDataIdentifiersL( HsWidget& aWidget, 
		CLiwDefaultMap* aDataMap );
    
    /**
     */
    void InsertWidgetDataIdentifiersL( HsWidget& aWidget, 
            CLiwDefaultMap* aDataMap, const TDesC& aContentType );
    
    /**
     */
    void InsertWidgetInfoL( HsWidget& aWidget, 
		CLiwDefaultMap* aDataMap );
    
    /**
     */
    void InsertWidgetItemsL( HsWidget& aWidget, 
		CLiwDefaultMap* aDataMap );

    /**
     */
    TInt ObtainErrorCode( const CLiwGenericParamList& aOutParamList );

    /**
     */
    TInt TranslateServiceError( TInt32 aServiceErrorCode );
    
    /**
     */    
    void AddItemsActionsL( HsWidget& aWidgetItem, TInt aItemIndex  );

    /**
     */     
    void InsertWidgetACLL( CLiwDefaultMap* aDataMap );
    
    /**
     * Returns publisher name that should be used with a certain widget
     * when registering the widget, writing/deleting widget data or
     * requesting event notification.
     * 
     * @param aWidget A valid widget object.
     * @return Publisher name.
     */
    std::string GetPublisherNameL( HsWidget& aWidget );
    
    /**
     * Parses publisher name in order to determine widget identifier.
     * Will leave with KErrNotFound if publisher name is invalid.
     * 
     * @param aPublisherName Publisher name
     * @return Widget identifier.
     */
    std::string GetWidgetIdentifierFromPublisherNameL(
            const TDesC& aPublisherName );
    
    /**
     * Wraps CnvUtfConverter::ConvertToUnicodeFromUtf8.
     * 
     * @param aSrcString 8-bit utf8-encoded string.
     * @return Converted unicode-encoded (16 bit) string.
     */
    HBufC* Utf8ToUnicodeLC( const TDesC8& aSrcString ) const;
    
    /**
     * A convenient std::string to HBufC converter.
     *
     * @param aSrcString utf8-encoded std::string.
     * @return Converted unicode-encoded (16 bit) string.
     */
    HBufC* StdStringToUnicodeLC( const std::string& aSrcString ) const;
    
    /**
     * Convert template name if needed
     *
     * @param aSrcString utf8-encoded std::string.
     * @return Converted utf8-encoded  std::string.
     */
    std::string TemplateName(const std::string &aSrcTemplate);
    
private:
    
	std::vector<HsWidget*>	mWidgets;
	WidgetContentIdMapType  mWidgetContentIds;
    CLiwServiceHandler*		mServiceHandler;
    MLiwInterface*          mServiceInterface;
    IHsDataObserver*       	mHsDataObserver;
    TInt32                    iWidgetUid;
    };
}

#endif /*__HSWIDGETPUBLISHERIMPL_H__*/
