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
*
*/


#ifndef hspsSECURITYENFORCER_H
#define hspsSECURITYENFORCER_H

#include <e32base.h>

#include "hsps_builds_cfg.hrh"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

class ChspsODT;
class ChspsDefinitionRepository;
class CRepository;

/**
*  ChspsSecurityEnforcer performs all security related functions in HSPS.
*  These functions are the following:
*  - control user access on service request, and
*  - adjust user access in queries.
*
*  Function are explained below:
*
*  CheckAccessRightsL()
*  --------------------
*  CheckAccessRightsL() function is called by CPolicyServer when hspsThemeServer 
*  receives user request. 
*  Access rights are hard-coded here for S60 5.0, however, in later versions 
*  support for dynamic configuration of access rights must atken care. 
*  This would be appropriate to solve together TARM-policy implementation.
*
*  CheckQueryValidityL()
*  -------------------
*  CheckQueryValidityL() function is called by hsps Theme Server's service handlers 
*  (ChspsInstallionHandler, ChspsMaintenanceHandler, and ChspsClientRequestHandler) for adjusting 
*  user request to match the access rights level that user actually have.
*  This function is to be called immediately when actual ODT is known. 
*  In the istallation cases, ODT is known after manifest-file parsing.
*  In the maintenanace cases, ODT is known immediately on query.
*  In the theme usage cases, ODT is known when it application theme has retrieved from 
*  UI Definition Repository.  
*  For instance, if user is requesting the theme listing (message hspsGetListHeaders) with
*  application UID set to 0 in query meaning that the query concerns all themes in storage.
*  However, if user is not S60 Personalisation Application, the query must be restricted to
*  the themes which belongs to the this particular client only. 
*  This solution follows the recilient server approach; when function is called, the user
*  access rights are already checked by CheckUserAccessRightsL() function. In this sense, 
*  offering recilient service by adjusting user access insteads of denying it is reasonable 
*  resolution.
*  @ingroup group_hspsserver
*  @lib hspsThemeServer.exe
*  @since S60 5.0
*/
class ChspsSecurityEnforcer: public CBase, public MhspsSecurityService
    {
   public:  // Destructor
        
        static ChspsSecurityEnforcer* NewL( ChspsDefinitionRepository& aDefRep, CRepository& aCenRep );
        
        /**
        * Destructor.
        */
        virtual ~ChspsSecurityEnforcer();
        
   public: // Functions from base classes
                
        /**
        * From MhspsSecurityService 
        * Checks if Licensee Default theme exists. This method is used if a theme that
        * is either User or Operator Default is installed.
        * @since S60 5.0
        * @param aODT ODT of the installed theme 
        * @return void
        */
        void CheckIfLicenseeDefaultExistsL( const ChspsODT& aOdt );
        
        /** 
        * From MhspsSecurityService CheckAccessRightsL
        * @since S60 5.0
        * @param aMessage is the RMessage2 containing the client request data. 
        *        Client's access rights in requesting a service of hsps Theme Server
        *        is to be checked.
        * @return ETrue if request has passed the access rights check, otherwise returns EFalse.
        */
        TBool CheckAccessRightsL( const RMessage2& aMessage );
      
        TBool CheckThemeLockingL( const ChspsODT& aOdt );

#ifdef HSPS_LOG_ACTIVE        
        /** 
        * Set log bus.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif        
        
   private:
        /**
        * ChspsSecurityEnforcer
        * C++ default constructor
        * @since S60 5.0
        */
        ChspsSecurityEnforcer( ChspsDefinitionRepository& aDefRep, CRepository& aCenRep );
        
        /**
        * ConstructL.
        * By default Symbian 2nd phase constructor is private.
        * @since S60 5.0
        */
        void ConstructL(); 
        
        /**
         * HandleAccessControlListL
         * Parse UIDs from descriptor buffer aStrBuf to array of intergers.
         */
        void HandleAccessControlListL(const TDesC8& aStrBuf, RArray<TInt>& aArray);
        
        /**
         * CheckAccessControlListL
         * Checks whether aUid found in access control list.
         * returns ETrue if uid is found
         */
        TBool CheckAccessControlListL( TInt aUid );
        
   private:
        // Reference to definition repository
        ChspsDefinitionRepository& iDefRep;
        
        // Reference to central repository
        CRepository& iCentralRepository;
        
        // Access control list
        RArray<TInt> iAccessControlList;
#ifdef HSPS_LOG_ACTIVE        
        // Log bus. Not owned.     
        ChspsLogBus* iLogBus;
#endif        
    };

#endif      // hspsSECURITYENFORCER_H  
            
// End of File
