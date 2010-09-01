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


#ifndef INC_AIFWTESTCONTROLLER2_H
#define INC_AIFWTESTCONTROLLER2_H

// ========== INCLUDE FILES ===================================================

#include <f32file.h>
#include "aicontentobserver.h"
#include "aipropertyextension.h"
#include "AiUiController.h"
#include <e32base.h>
#include <e32std.h>

// ========== CONSTANTS =======================================================

// ========== MACROS ==========================================================

// ========== DATA TYPES ======================================================

// ========== FUNCTION PROTOTYPES =============================================

// ========== FORWARD DECLARATIONS ============================================

// ========== CLASS DECLARATION ===============================================

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series 60 3.1
*/
namespace AiTestUiController2
{
class CAiFwTestControllerPlugin2 : public MAiSecondaryUiController,
								  public MAiContentObserver
	{
	public:   // Constructors and destructor
		
		static CAiFwTestControllerPlugin2* NewL();
		
        IMPORT_C static CAiFwTestControllerPlugin2* Instance();
		
		virtual ~CAiFwTestControllerPlugin2();
		
	private:   // Constructors
		
		CAiFwTestControllerPlugin2();
		
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
		
	public:  // Functions from base classes CAiUiController
		
        void LoadUIDefinitionL();

        void GetSettingsL(const TAiPublisherInfo& aPubInfo, RAiSettingsItemArray& aSettings);

        void GetPluginsL( RAiPublisherInfoArray& aPlugins );

	    void RunApplicationL();
		
		virtual void ActivateUI();
		
        virtual void SetEventHandler(MAiFwEventHandler& aEventHandler);
    
        void RemovePluginFromUI( MAiPropertyExtension& aPlugin )
            {
            }
            
		virtual MAiContentObserver& GetContentObserver();
		
		MAiMainUiController* MainInterface()
		    {
		    return NULL;
		    };
		
        MAiSecondaryUiController* SecondaryInterface()
            {
            return this;
            };
		
    virtual void SetCoeEnv( CCoeEnv& /*aCoeEnv*/ )
        {
        };

    virtual MAiUiFrameworkObserver* UiFrameworkObserver()
        {
        return NULL;
        };

	protected:   // New functions
		
		void SetText(const TDesC8& aId, TPtrC8& aBuf);
		
		const TDesC* GetText(const TDesC8& aId);
		
	private:     // Data
		
		TBool                               iUiActivated;
		
		TBool                               iDefinitionLoaded;
		
        MAiFwEventHandler*                  iFwEventHandler;
		// Reserved pointer for future extension
		//TAny* iReserved;
};

} // namespace AiTestUiController

#endif

// End of File.
