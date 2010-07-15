/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef INC_AIFWTESTCONTROLLER_H
#define INC_AIFWTESTCONTROLLER_H

// ========== INCLUDE FILES ===================================================

#include <f32file.h>
#include "aicontentobserver.h"
#include "aipropertyextension.h"
#include "AiUiController.h"
#include <e32base.h>
#include <e32std.h>
#include <coemain.h>

// ========== CONSTANTS =======================================================

// ========== MACROS ==========================================================

// ========== DATA TYPES ======================================================

// ========== FUNCTION PROTOTYPES =============================================

// ========== FORWARD DECLARATIONS ============================================

class MAiContentItemIterator;
class TAiPublisherInfo;

// ========== CLASS DECLARATION ===============================================

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 3.1
*/
namespace AiTestUiController
{
class CAiFwTestControllerPlugin : public CAiUiController,
								  public MAiContentObserver,
								  public MAiMainUiController
								  /*public MAiSecondaryUiController*/
	{
	public:   // Constructors and destructor
		
		static CAiFwTestControllerPlugin* NewL();
		
        IMPORT_C static CAiFwTestControllerPlugin* Instance();
		
		~CAiFwTestControllerPlugin();
		
	private:   // Constructors
		
		CAiFwTestControllerPlugin();

		void ConstructL();
		
	public:  // Functions from MAiContentObserver
		
		TInt StartTransaction( TInt aTxId );

		TInt Commit( TInt aTxId );

		TInt CancelTransaction( TInt aTxId );
		
		TBool CanPublish( MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex );
		
		TInt Publish( MAiPropertyExtension& aPlugin, TInt aContent, TInt aResource, TInt aIndex );
		
		TInt Publish( MAiPropertyExtension& aPlugin, TInt aContent, const TDesC16& aBuf, TInt aIndex );

		TInt Publish( MAiPropertyExtension& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex );
		
		TInt Publish( MAiPropertyExtension& aPlugin, TInt aContent, RFile& aFile, TInt aIndex );
		
		TInt Clean( MAiPropertyExtension& aPlugin, TInt aContent, TInt aIndex );
		
		TAny* Extension( TUid aUid );
		
		TBool RequiresSubscription( const TAiPublisherInfo& aPublisherInfo ) const;
		
	public:  // Functions from base class CAiUiController
		
        virtual void LoadUIDefinitionL();

        virtual void GetSettingsL(const TAiPublisherInfo& aPubInfo, RAiSettingsItemArray& aSettings);

        virtual void GetPluginsL( RAiPublisherInfoArray& aPlugins );

		virtual void ActivateUI();
		
        virtual void SetEventHandler(MAiFwEventHandler& aEventHandler);
    
		virtual MAiContentObserver& GetContentObserver();
		
		virtual MAiMainUiController* MainInterface()
		    {
		    return this;
		    };

        virtual MAiSecondaryUiController* SecondaryInterface()
            {
            return NULL;
            };
            
        virtual void HandleLoadedPlugins( const RAiPublisherInfoArray& aPlugins );
		
    virtual void RunApplicationL();
    
    virtual CCoeEnv& CoeEnv()
        {
        return *CCoeEnv::Static();
        };
    
     virtual void Exit()
        {
        };

	virtual TBool IsMenuOpen()
		{
			return ETrue;
		};
	
    virtual void SetUiFrameworkObserver( MAiUiFrameworkObserver& aObserver )
        {
        iFwObserver = &aObserver;
        }; 
        
    virtual void RemovePluginFromUI( MAiPropertyExtension& aPlugin )
        {
        }

	public:  // Test functions
	
	    IMPORT_C TBool UiActivated();
	    
	    IMPORT_C TBool RunApplicationCalled();
	    
	    IMPORT_C TBool DefinitionLoaded();
	    
	    IMPORT_C void SendReadyEventL();

	    IMPORT_C void SendTestEventL();

        IMPORT_C void SetResourceChangedL( TInt aRes );
    
        IMPORT_C void SetForegroundL( TBool aFore );

        IMPORT_C void SetAppEnvReadyL();

	protected:   // New functions
		
		void SetText(const TDesC8& aId, TPtrC8& aBuf);
		const TDesC* GetText(const TDesC8& aId);
		
	private:     // Data
		
		TBool                               iUiActivated;
		TBool                               iRunApplicationCalled;
		TBool                               iDefinitionLoaded;
		
        MAiContentItemIterator*             iContent;
        MAiContentItemIterator*             iResources;
        MAiContentItemIterator*             iEvents;
        MAiFwEventHandler*                  iFwEventHandler;
        
        MAiUiFrameworkObserver*             iFwObserver;
		// Reserved pointer for future extension
		//TAny* iReserved;
};

} // namespace AiTestUiController

#endif

// End of File.
