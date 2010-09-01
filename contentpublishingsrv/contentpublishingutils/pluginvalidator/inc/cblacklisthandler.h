/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef CBLACKLISTHANDLER_H_
#define CBLACKLISTHANDLER_H_

//INCLUDES
#include <e32base.h>

//FORWARD DECLARATIONS
class CRepository;

/*
 * Blacklist Handler 
 */
NONSHARABLE_CLASS( CBlacklistHandler ): public CBase
    {
public:

    /**
     * Two-phased constructor.
     * 
     */     
    static CBlacklistHandler* NewL(  );

    /**
     * Two-phased constructor.
     * 
     */     
    static CBlacklistHandler* NewLC( );
    
    /**
     * Desctructor.
     */     
    ~CBlacklistHandler();

     /*
      * Appends plugin UID to blacklist
      * @param aUid uid of the plugin
      */
     void AppendL( TUid aUid );    

     /*
      * Removes plugin UID from blacklist
      * @param aUid uid of the plugin
      */
     void RemoveL( TUid aUid );    
       
     /*
      * Checks if plugin with provided UID is on the blacklist
      * @param aUid uid of the plugin
      */
     TBool IsPresentL( TUid aUid );    

     /*
      * Copies blacklist unofficial blacklist to official 
      * 	if flag is set to EFalse and official to unofficial if flag = ETrue
      * @param aUnofficialToOfficial flag indicating which blacklist 
      * 	should be copied
      */
     void CopyBlacklistL( TBool aOfficialToUnofficial );
    
private:

    /**
     * C++ default constructor.
     */    
	CBlacklistHandler( );

    /**
     * Perform the second phase construction of a CPluginValidator object.
     */    
    void ConstructL();
     
    /*
     * Gets blacklist array from CPS 
     */
    void GetArrayL( RArray<TUid>& aArray, TBool aOfficial );

    /*
     * Stores blacklist array in CPS
     */
    void StoreArrayL( const RArray<TUid>& aArray, TBool aOfficial );   

private:
    
    /**
     * Provides access to repository
     * Own
     */
    CRepository* iRepository;
	};

#endif /* CBLACKLISTHANDLER_H_ */
