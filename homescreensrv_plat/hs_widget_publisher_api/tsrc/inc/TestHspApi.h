/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#ifndef TESTHSPAPI_H
#define TESTHSPAPI_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <liwcommon.h>
#include "hsdataobserver.h"
#include <string>
#include "hswidgetpublisher.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KTestHspApiLogPath, "\\logs\\testframework\\TestHspApi\\" ); 
// Log file
_LIT( KTestHspApiLogFile, "TestHspApi.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTestHspApi;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTestHspApi test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestHspApi) : public CScriptBase, public Hs::IHsDataObserver, public MLiwNotifyCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestHspApi* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Destructor.
        */
        virtual ~CTestHspApi();

    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CTestHspApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
    
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        /**
        * Test methods are listed below. 
        */
        
        TInt TestHspApiConstrL( CStifItemParser& aItem );
        TInt TestHspWidgetAddWidgetItem1L( CStifItemParser& aItem );
        TInt TestHspWidgetAddWidgetItem2L( CStifItemParser& aItem );
		TInt TestHspWidgetAddWidgetItem3L( CStifItemParser& aItem );
        TInt TestHspApiDeleteWidgetL( CStifItemParser& aItem );
        TInt TestHspApiSetDataObserver1L( CStifItemParser& aItem );
		TInt TestHspApiSetDataObserver2L( CStifItemParser& aItem );
        TInt TestHspApiGetWidgetL( CStifItemParser& aItem );
        TInt TestHspApiWidgetRemoveItem1L( CStifItemParser& aItem );
		TInt TestHspApiWidgetRemoveItem2L( CStifItemParser& aItem );
        TInt TestHspApiExceptionL( CStifItemParser& aItem );

    	// from IHSPDataObserver
    	void handleItemEvent( std::string aWidgetName, std::string aTemplateItemName, Hs::IHsDataObserver::EItemEvent aAction);
    	void handleEvent( std::string aWidgetName, Hs::IHsDataObserver::EEvent aAction );
    	
    	MLiwInterface* GetServiceInterfaceL();
    	
    	CLiwGenericParamList* GetListL( std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );
    	
    	CLiwGenericParamList* ExecuteActionL( std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );

    	CLiwGenericParamList* ExecuteActionSelectedL( std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );

		TInt CTestHspApi::ExtractItemCountL(
    	      const CLiwGenericParamList& aInParamList );

    	TInt ExtractItemL( const CLiwGenericParamList& aInParamList,
    			const TDesC8& aItemName, RBuf& aItemValue );
    	
    	void AddTemplate( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );
    	
    	void SendFakeActivateEvent( std::string& aIdentifier );
    	
    	void AddExistingTemplate( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );
    	
    	void AddAndRemoveTemplateItem1( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );
    	
    	void AddAndRemoveTemplateItem2( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
    			std::string& aIdentifier, std::string& aWidgetName );
				
    	void WaitL(TInt aMicroSec);
    	
    	// from MLiwNotifyCallback
    	TInt HandleNotifyL( TInt /*aCmdId*/, TInt /*aEventId*/, 
    			CLiwGenericParamList& aEventParamList,
    			const CLiwGenericParamList& /*aInParamList*/ );
    	
    	void ThrowHspException();
    	
    private:    // Data
    	
    	CLiwServiceHandler* mServiceHandler;
    	MLiwInterface* mServiceInterface;
    	TInt iItemActionStatus;
    	TInt iActionStatus;

    };

#endif      // TESTHSPAPI_H
            
// End of File
