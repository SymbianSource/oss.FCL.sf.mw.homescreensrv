/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class definition for CGSPluginLoader
*
*/



#ifndef  GSPLUGINLOADER_H
#define  GSPLUGINLOADER_H

// INCLUDES
#include    <e32base.h>
#include    <ecom/implementationinformation.h>

// CONSTANTS

// Value for a to b comparison result when logically a == b.
const TInt KGSComparisonEqual     = 0;

// Value for a to b comparison result when logically a < b.
const TInt KGSComparisonBefore    = -1;

// Value for a to b comparison result when logically a > b.
const TInt KGSComparisonAfter     = 1;

// CLASS DEFINITIONS
class CGSPluginInterface;
class MGSPluginLoadObserver;
class CAknViewAppUi;
class REComSession;
class CAknColumnListBox;
class CAknInfoPopupNoteController;
class CGSBaseDocument;
class MGSWatchDog;

/**
* CGSPluginLoader. Mechanism used to load plugins asynchronously. Uses
* MGSPluginLoadObserver as a callback.
*
* @lib GSFramework.lib
* @since Series60_30.1
*/
class CGSPluginLoader : public CActive
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        *
        * @param aAppUi Pointer to application UI. Does not take ownership.
        */
        IMPORT_C static CGSPluginLoader* NewL( CAknViewAppUi* aAppUi );

        /**
        * Destructor
        */
        IMPORT_C ~CGSPluginLoader();

    private: // Internal construction

        /**
        * Default C++ contructor
        */
        CGSPluginLoader();

        /**
        * Symbian OS default constructor
        * @return void
        */
        void ConstructL( CAknViewAppUi* aAppUi );

    public: // API

        /**
        * Starts loading GS plug-ins asynchronously. Will call
        * MGSPluginLoadObserver::HandlePluginLoaded() each time a plug-in is
        * loaded and when all plugins are loaded.
        *
        * CGSPluginLoader transfers the ownership of each loaded plugin view to
        * CAknViewAppUi. It is client's responsibility to remove the views from
        * CAknViewAppUi and delete the plugins if necessary.
        *
        * @param aInterfaceUid Uid ofthe interfaces to be loaded.
        * @param aParentUid Uid of the parent. Only children of this parent
        *        will be loaded.
        * @param aPluginArray An array for the loaded GS plug-ins.
        *        CGSPluginLoader does not take the ownership of this array.
        */
        IMPORT_C void LoadAsyncL(
            TUid aInterfaceUid,
            TUid aParentUid,
            CArrayPtrFlat<CGSPluginInterface>* aPluginArray );

        /**
        * Load a specific plugin instance synchronously.
        *
        * @param aInterfaceUid Uid ofthe interfaces to be loaded.
        * @param aImplementationUid Uid of the implementation to load
        */
        CGSPluginInterface& LoadSyncL( TUid aInterfaceUid, TUid aImplementationUid );

        /**
        * Sets observer for this loader. Only one observer per loader in one
        * time is possible.
        */
        IMPORT_C void SetObserver(MGSPluginLoadObserver* aObserver);

        /**
        * Aborts asynchronous loading of the GS plug-ins.
        */
        IMPORT_C void AbortAsyncLoad();

        /**
        * Sorts the plugin array.
        *
        * Sorting criterias:
        *
        * 1. Order number if provider category is Internal.
        * 2. Provider category. Precedence as follows:
        *       1. KGSPluginProviderInternal
        *       2. EGSPluginProviderOEM
        *       3. EGSPluginProviderOperator
        *       4. EGSPluginProvider3rdParty
        * 3. Alphabetical
        *
        * @param aPlugins The array which will be sorted.
        */
        IMPORT_C void SortPluginsL( CArrayPtrFlat<CGSPluginInterface>* aPlugins );


        /**
        * Attempts to change CActive priority on next RunL cycle. Use this if
        * needed to change plugin loader priority. Normally when parent plugin
        * view is opened, it is advisable to increase loader priority to get
        * child plugins loaded faster.
        *
        * @param aPriority Desired priority.
        */
        IMPORT_C void RequestPriority( CActive::TPriority aPriority );

    private: // Internal methods

        /**
        * Starts loading next plugin.
        */
        void LoadNextPluginL();

        /**
        * Creates a plugin instance from given UID. Ownership is transferred.
        */
        CGSPluginInterface& CreatePluginInstanceL(
                const CImplementationInformation& aImpInfo );

        /**
        * Notifies MGSPluginLoadObserver.
        */
        void NotifyProgress();

        /**
        * Notifies MGSPluginLoadObserver.
        */
        void NotifyFinished();

        /**
        * Wait for the next round of CActive execution.
        */
        void CompleteOwnRequest();

        // Insertion function used by sorting:

        /**
        * Inserts plugin in the correct position in the array using sorting
        * criterias. Assumes aPlugins is ordered.
        *
        * @param aPlugin The plugin to be inserted.
        * @param aPlugins Array in which the plugin is inserted into the
        *        corresponding location.
        */
        void InsertPluginInOrderL(
            CGSPluginInterface* aPlugin,
            CArrayPtrFlat<CGSPluginInterface>* aPlugins );

        // Comparison functions:

        /**
        * Compares plugins according to comparison criterias.
        *
        * Note: GS internal comparison constants such as KGSComparisonEqual are
        * different from the ones outputted by this function. This is because
        * this function is also usable by RArray sort -functionality but BC
        * cannot be broken in GS.
        *
        * @return Negative value: If aFirst before aSecond.
        *                      0: If equal.
        *         Positive value: If aSecond before aFirst.
        */
        static TInt Compare( const CGSPluginInterface& aFirst,
                             const CGSPluginInterface& aSecond );

        /**
        * Precedence is as follows:
        * 1. KGSPluginProviderInternal
        * 2. EGSPluginProviderOEM
        * 3. EGSPluginProviderOperator
        * 4. EGSPluginProvider3rdParty
        *
        * @return   KGSComparisonEqual  = equal categories
        *           KGSComparisonBefore = aFirst is before aSecond
        *           KGSComparisonAfter  = aFirst is after aSecond
        */
        static TInt CompareCategory(
                const CGSPluginInterface& aFirst,
                const CGSPluginInterface& aSecond );
        /**
        *
        * @return   KGSComparisonEqual  = equal captions
        *           KGSComparisonBefore = aFirst is before aSecond
        *           KGSComparisonAfter  = aFirst is after aSecond
        */
        static TInt CompareCaption(
                const CGSPluginInterface& aFirst,
                const CGSPluginInterface& aSecond );
        /**
        *
        * @return   KGSComparisonEqual  = equal indexes
        *           KGSComparisonBefore = aFirst is before aSecond
        *           KGSComparisonAfter  = aFirst is after aSecond
        */
        static TInt CompareIndex(
                const CGSPluginInterface& aFirst,
                const CGSPluginInterface& aSecond );

    private: // Utility methods

        /**
        * Parses descriptor to UID.
        */
        static TInt ParseToUid( const TDesC8& aSource, TUid& aTarget );

        /**
        * Parsers plugin's order number
        */
        static TInt ParseOrderNumber( const TDesC8& aSource, TInt& aOrderNumber );

        /**
        * Print debug information.
        */
        static void PrintInfoDebugL( const CImplementationInformation& aInfo,
                                    TInt aIterator,
                                    TInt aPluginCount );
        /**
         * Print ordering debug data.
         */
        void CGSPluginLoader::PrintOrderTracesL(
                CArrayPtrFlat<CGSPluginInterface>* aPlugins );

        /**
         * Display loading error popup message.
         */
        void DisplayErrorPopupL( TInt aError,
                                 const CImplementationInformation* aInfo );

    private: // From CActive

        /**
        * See base class.
        */
        void RunL();

        /**
        * See base class.
        */
        TInt RunError( TInt aError );

        /**
        * See base class.
        */
        void DoCancel();

    private: // Data


        // Pluginloader goes through this array and loads the plugins into
        // iPluginArray if they fulfill the criterias.
        RImplInfoPtrArray iImplInfoArray;

        // Used as an iterator to maintain location in iImplInfoArray.
        TInt iImplInfoArrayIterator;

        // Array of loaded plugins.Plugins are owned by iAppUi. Only the array
        // pointers are owned by this class and therefore only reset array.
        CArrayPtrFlat<CGSPluginInterface>* iPluginArray;

        // UID for plugin parent.
        TUid iParentUid;

        // Pointer to app UI. Not owned.
        CAknViewAppUi* iAppUi;

        // Pointer to observer. Not owned.
        MGSPluginLoadObserver* iObserver;

        // Number of RunL calls.
        TInt iRunLDebugCount;

        // For debugging: shows a note in case of a crashing plugin
        CAknInfoPopupNoteController* iErrorPopup;

        // Reference to GS application owned watchdog. Not onwed.
        MGSWatchDog* iWatchDog;

        // Reference to document. Not owned.
        CGSBaseDocument* iDocument;

        // Desired oactive object priority for loading child plugins.
        CActive::TPriority iRequestedPriority;
    };


/**
* Interface for GSPluginLoader observer. MGSPluginLoadObserver gets
* notifications when plugins are loaded.
*
* @lib GSFramework.lib
* @since Series60_30.1
*/
class MGSPluginLoadObserver
    {
    public: // Enums
        enum KGSPluginLoaderStatus
            {
            // One plugin loaded successfully, continue to next.
            EGSSuccess,
            // Loading one plugin failed, contiue to next.
            EGSFail,
            // Client called AbortAsyncLoad(), finished loading.
            EGSAborted,
            // All plugins loaded successfully, finished loading.
            EGSFinished,
            // Severe error with loader, finished loading.
            EGSError
            };

    public: // New

        /**
        * CGSPluginLoader calls this function when each plug-in is loaded or
        * loading is finished..
        */
        IMPORT_C virtual void HandlePluginLoaded(
            KGSPluginLoaderStatus aStatus ) = 0;
    };


#endif // GSPLUGINLOADER_H
// End of File
