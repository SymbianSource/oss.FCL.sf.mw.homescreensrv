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
* Description:  Class used by session to handle asynchronous requests
 *
*/


#ifndef C_CPNOTIFICATIONHANDLER_H
#define C_CPNOTIFICATIONHANDLER_H

#include <e32base.h>
#include <e32hashtab.h> 

class CLiwGenericParamList;
class CLiwDefaultList;
class CLiwDefaultMap;
class CCPLiwMap;

/**
 *  Internal class for handling asynchronous request
 *  for notifications. Used by server session object.
 *
 *  @since S60 v5.0
 */
class CCPNotificationHandler : public CBase
    {
public:

   /**
     * Two-phased constructor.
     */
    static CCPNotificationHandler* NewL( RPointerArray<CLiwDefaultList>&
        aNotifications );

    /**
     * Two-phased constructor.
     */
    static CCPNotificationHandler* NewLC( RPointerArray<CLiwDefaultList>&
        aNotifications );

    /**
     * Destructor.
     */
    virtual ~CCPNotificationHandler();

    /**
     * Save message from client and initialize needed members
     */
    void SaveMessageL( const RMessage2& aMessage );

    /**
     * Adds observer
     */
    void AddObserverL( const RMessage2& aMessage );    

    /**
     * Removes observer
     */
    void RemoveObserverL( const RMessage2& aMessage );      
    
    /**
     * Invoked in order to send notification
     * @param List of changes to send
     */
    void SendNotificationL( CLiwDefaultList* aListOfMaps );

    /**
     * Getter
     * @return pointer to current notification
     */
    const CLiwGenericParamList* GetPointerToChangeInfoList();

    /**  
     * Resets internal stare
     */
    void Reset();

    /**
     * Completes iMessage with provided error
     * @param Error Code
     */
    void ErrorComplete( TInt aErrCode );

private:
    
    /**
     * C++ default constructor.
     */
    CCPNotificationHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( RPointerArray<CLiwDefaultList>& aNotifications );

    /**
     * Invoked in order to send notification
     * @param List of changes to send
     * @since Series 60 5.0
     */
    TBool SendChangeInfoListL( const CLiwDefaultList* aListOfMaps );

    /**
     * Compares argument to stored filter
     * @param aMap Information about change
     * @param aFilter filter to compare with
     * @return ETrue if argument matches filter
     */
    TBool IsProperForFilterL( const CLiwMap& aMap, 
    		const CCPLiwMap& aFilter );

    /**
     * Extracts parameter from LIW Map 
     *
     * @param aMap Source Map
     * @param aProperty Key of parameter to extract
     * @param aResult Target for value
     */
    void GetPropertyL( const CLiwMap& aMap, const TDesC8& aProperty,
        RBuf& aResult );
    
    /**
     * Checks if IDs are the same
     *
     * @param aMap Map with notifications
     * @param aFilter filter to compare with
     * @return ETrue if ID are identical
     */
    TBool CheckIdL( const CLiwMap& aMap, const CCPLiwMap& aFilter );
    
    /**
     * Checks if Properties like publisher,content type 
     * and content id  are the same
     *
     * @param aMap Map with notifications
     * @param aFilter filter to compare with
     * @return ETrue if Parameters are identical
     */
    TBool CheckPropertiesL( const CLiwMap& aMap, 
    		const CCPLiwMap& aFilter );
    
    /**
     * Checks if operation types are the same
     *
     * @param aMap Map with notifications
     * @param aFilter filter to compare with
     * @return ETrue if operation types are the same
     */
    TBool CheckOperationTypeL( const CLiwMap& aMap, 
    		const CCPLiwMap& aFilter );
    
    /**
     * Checks if registry types are the same
     *
     * @param aMap Map with notifications
     * @param aFilter filter to compare with
     * @return ETrue if registry types are the same
     */ 
    TBool CheckRegistryTypeL( const CLiwMap& aMap,
    		const CCPLiwMap& aFilter );

private:
    // data

    /*
     * Own.
     * ETrue if message is ready to send notification
     */
    TBool iIsReadyToSend;

    /*
     * Own.
     * List of waiting notifications.
     */
    RPointerArray<CLiwDefaultList> iNotifications;

    /*
     * Not own.
     * Message from client
     */
    RMessage2 iMessage;

    /*
     * Own.
     * Notification to send.
     */
    CLiwGenericParamList* iChangeInfoList;

    /*
     * Own.
     * Filters
     */
    RHashMap< TInt32, CCPLiwMap*> iFilters;
    };

#endif // C_CPNOTIFICATIONHANDLER_H
