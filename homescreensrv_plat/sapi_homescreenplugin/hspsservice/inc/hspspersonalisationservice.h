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
* Description:  Defination of class CHspsPersonalisationService
*
*/


#ifndef C_HSPSPERSONALISATIONSERVICE_H
#define C_HSPSPERSONALISATIONSERVICE_H

// SYSTEM INCLUDES

#include <e32base.h>
#include <hspsclient.h>
#include <hspsthememanagement.h>
#include <hspsodt.h>

#include <hspsdomlist.h>
#include <hspsdomdocument.h>

// USER INCLUDES


// FORWARD DECLARATIONS
class ChspsDomNode;
 
/**
 * @ingroup group_hsps_sapi
 * This class provides HSPS personalisation services.
 * 
 * @since S60 5.0 
*/
class CHspsPersonalisationService : public CBase, public MhspsThemeManagementServiceObserver
    {
    public: // Constructor and destructor

        /**
         * Two-phased constructor.
         * 
         * @since S60 5.0
         */
        IMPORT_C static CHspsPersonalisationService* NewL();
    
        /**
         * Destructor.
         */
        virtual ~CHspsPersonalisationService();

    private: // Constructors

        /**
         * Constructor
        */
        CHspsPersonalisationService();
    
        /**
         * Symbian Constructor
        */
        void ConstructL();


    public: // From base class MhspsThemeManagementServiceObserver

        /**
         * Realisation of MhspsThemeManagementServiceObserver Interface
         * @param aMessage contains a service completion message
         */
        void HandlehspsClientMessage( ThspsServiceCompletedMessage aMessage );

    public: // New methods
       
        /**
         * Get the list of plugins ODT headers.
         * 
         * @since S60 5.0
         * @param aInterface Interface of the requested plugins.
         * @param aType Type of the requested plugins.
         * @param aFamily Requested plugin configuration family
         * @param aCopyLogos Controls whether to copy logos to client's private folder
         * @param aList List of plugins ODT headers.
         */
        IMPORT_C void GetPluginListL( 
            TDesC8& aInterface,
            TDesC8& aType,
            TUint32 aFamily,
            const TBool aCopyLogos,
            CArrayPtrFlat<ChspsODT>& aList
            );

        /**
         * Add a new plugin to the defined configuration.
         * 
         * @since S60 5.0
         * @param aAppUid Uid of modified application configuration.
         * @param aConfId Id of modified configuration.
         * @param aPluginUid Uid of new plugin.
         * @param aPosition Position in configurations plugin list.
         * @param aPluginId Id of new plugin in modified configuration.
         */
        IMPORT_C void AddPluginL(
            TInt aAppUid,
            TDesC8& aConfId,
            TDesC8& aPluginUid,
            TDesC8& aPosition,
            TInt& aPluginId
            );

        /**
         * Remove plugin from the defined configuration.
         * 
         * @since S60 5.0
         * @param aAppUid Uid of modified application configuration.
         * @param aPluginId Id of removed plugin in configuration.
         */
        IMPORT_C void RemovePluginL(
            TInt aAppUid,
            TDesC8& aPluginId
            );

        /**
         * Sets/changed plugin settings. 
         * 
         * @since S60 5.0
         * @param aAppUid Uid of modified application configuration.
         * @param aPluginId Id of plugin whose settings are updated.
         * @param aDom Dom document of the settings which are changed.
         * @param aPluginStoringStatus status to tell whether plugin reference need to be stored.
         */
        IMPORT_C void SetPluginSettingsL(
                TInt aAppUid,
                TDesC8& aPluginId,
                ChspsDomDocument& aDom,
                TBool aPluginStoringStatus);

        /**
         * Updates plugin position in a configuration.
         * 
         * @since S60 5.0 
         * @param aAppUid Uid of modified application configuration.
         * @param aConfId Id of a configuration whose plugins list is updated.
         * @param aPluginIds An array of plugin ids in the new positions.         
         */
        IMPORT_C void MovePluginsL(
            const TInt aAppUid,
            TDesC8& aConfId,
            CArrayFixFlat<TInt>& aPluginIds );

        /**
         * Get a list of ODT headers containing the information of available
         * configurations for the defined application.
         * 
         * @since S60 5.0
         * @param aAppUid application UID.
         * @param aFamily Requested application configuration family
         * @param aList An array of available application configurations.         
         */
        IMPORT_C void GetAppConfListL(
            TInt aAppUid,
            TUint32 aFamily,
            CArrayPtrFlat<ChspsODT>& aList );

        /**
         * Set active application configuration.
         * 
         * @since S60 5.0
         * @param aAppUid application UID.
         * @param aConfUid configuration UID.         
         */
        IMPORT_C void SetActiveAppConfL(
            TInt aAppUid,
            TDesC8& aConfUid );

        /**
         * Gets plugin odt.
         * 
         * @since S60 5.0
         * @param aAppUid - application uid.
         * @param aPluginUid - plugin's uid to be retrieved. 
         * @param aPluginOdt - odt.
         */
        IMPORT_C void GetPluginOdtL(
            TInt aAppUid,
            TDesC8& aPluginUid,
            ChspsODT* aPluginOdt );    

        /**
         * Set configuration state.
         * 
         * @since S60 5.0
         * @param aAppUid - application uid.
         * @param aConfId - configuration id.
         * @param aState - configuration's next state.
         * @param aFilter - configuration selection options
         */
        IMPORT_C void SetConfStateL(
            TInt aAppUid,
            TDesC8& aConfId,
            TDesC8& aState,
            TDesC8& aFilter );    

        /**
         * Restore active application configuration
         * @param aAppUid - application uid
         * @param aConfId - configuration id 
         */
        IMPORT_C void RestoreActiveAppConfL(
            TInt aAppUid,
            TDesC8& aConfUid );

        /**
         * Activates defined plugin in plugin list
         * 
         * @since S60 5.0
         * @param aAppUid Uid of modified application configuration.
         * @param aPluginId Id of activated plugin
         */
        IMPORT_C void SetActivePluginL(
            TInt aAppUid,
            TDesC8& aPluginId
            );
        
        /**
         * Replaces a plugin in active application configuration
         * @since S60 5.1
         * @param aAppUid - application uid
         * @param aPluginId - id of the plugin to be replaced
         * @param aConfUid - uid of the new plugin configuration 
         */
        IMPORT_C void ReplacePluginL(
            const TInt aAppUid,
            const TDesC8& aPluginId,
            const TDesC8& aConfUid );
        
        /**
         * Restores plugin configurations by either removing all plugins
         * from the active view or by removing all extra views.
         * In latter case, the first locked view is emptied. If there are 
         * no locked views then the first view will remain while others
         * are removed.
         * @since S60 5.2
         * @param aAppUid Application uid
         * @param aResetAllViews False if only the active view should be fixed,
         *                       True if also all extra views should removed                
         */
        IMPORT_C void RestoreConfigurationsL(
            const TInt aAppUid, 
            const TBool aResetAllViews );

    private: // Methods
    
    private: // Data        

        // Pointer to HSPS Theme server client object
        ChspsClient* iHspsClient;

    };

#endif // C_HSPSPERSONALISATIONSERVICE_H

// End of File.
