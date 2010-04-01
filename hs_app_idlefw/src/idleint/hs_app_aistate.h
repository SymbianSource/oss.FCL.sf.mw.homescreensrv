/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_ACTIVEIDLESTATE_H
#define C_ACTIVEIDLESTATE_H

#include <e32base.h>

/**
* @ingroup group_aiidleint
*
*  Update idle state flag in system agent
*
*  @since Series 60 2.8
*/
NONSHARABLE_CLASS(CActiveIdleState) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aSystemAgentProxy System Agent object.
        * @return Idle state monitor object.
        */
        static CActiveIdleState* NewL();

        /**
        * Destructor.
        */
        ~CActiveIdleState();
       
    public: // new function

        /**
        * Update the Idle state 
        */
        void SetIsIdleForeground( TBool aIsForeground );

    private:

        /**
        * C++ default constructor.
        */
        CActiveIdleState( );

        /**
        * Symbian OS constructor.
        */
        void ConstructL( );

        /**
        * Update setting asynchronously.
        */
        void UpdateStateAsync();

        /**
        * Update settings in System Agent.
        */
        void UpdateActiveIdleState();

        /**
        * Callback method, updates system agent setting.
        */
        static TInt DoUpdateState( TAny* aAny );
        
    private:    // Data
        // ETrue if system is in Idle state
        TBool iIsIdleForeground;

        // Owned idle callback to update.
        CIdle* iUpdater;

    };

#endif      // C_ACTIVEIDLESTATE_H

