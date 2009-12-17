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
*/


#ifndef __MCSDRMOBSERVERMANAGER_H_
#define __MCSDRMOBSERVERMANAGER_H_

#include <e32base.h>
#include "mcsdrmobserver.h"


class CMcsDrmObserver;
class CMenuSrvEngUtils;


class RMcsDrmObserverManager
    {
public:

		/** 
	    * Default constructor.
		*/
	    RMcsDrmObserverManager( CMenuSrvEngUtils& aUtils );
		
		/**
	           * Destructor.
		*/
	    virtual ~RMcsDrmObserverManager();


        void RemoveMissingObservers( 
        		const RArray<TUid>& aDrmProtectedArray );
        
        void CreateObserversL( 
            const RArray<TUid>& aDrmArray,
            TDRMEventType aMask,
            MMcsDrmObserver& aObserver );

        void CreateObserversL( 
        	const RArray<TUid>& aDrmProtectedArray, 
        	MMcsDrmObserver& aObserver );

        void RefreshObserverL( 
        		const RArray<TUid>& aDrmProtectedArray,         		
        		TUid aUid, 
        		MMcsDrmObserver& aObserver );
        
		/**
		*/
		void AddL( CMcsDrmObserver* aObserver );

		/**
		*
		*/
		void Remove( CMcsDrmObserver* aObserver );
		
		/**
		*/
		void Close();

        TInt FindUid( TUid aUid );		
	
private:
    RPointerArray< CMcsDrmObserver > iObservers;
    
    CMenuSrvEngUtils& iUtils;
    };

#endif /*__MCSDRMOBSERVERMANAGER_H_*/
