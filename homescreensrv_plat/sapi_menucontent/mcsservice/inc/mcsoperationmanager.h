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


#ifndef __MCSOPERATIONMANAGER_H_
#define __MCSOPERATIONMANAGER_H_

#include <e32base.h>

class RMcsOperationManager
    {
public:

		/** 
	    * Default constructor.
		*/
	    RMcsOperationManager( );
		
		/**
	           * Destructor.
		*/
	    virtual ~RMcsOperationManager();


		/**
		* @param aOperation pointer to active object to be added to 
		*       iOperations array. AddL transfers AO ownership to 
		*       RMcsOperationManager object
		*/
		void AddL( CActive* aOperation );

		/**
		* @param aOperation pointer to active object to be removed
		*       from the iOperations array 
		*
		*/
		void Remove( CActive* aOperation );
		
		/**
		*/
		void Close();
		
	
private:
    RPointerArray< CActive > iOperations;
    };

#endif /*__MCSOPERATIONMANAGER_H_*/
