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
* Description:  Contact metching class
*
*/


#ifndef HGCONTACTMATCHER_H
#define HGCONTACTMATCHER_H

// System includes
#include <e32base.h>
#include <e32std.h>
#include <bamdesca.h>
#include <badesca.h>

#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkFieldType.h>
#include <CVPbkPhoneNumberMatchStrategy.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <MVPbkContactViewObserver.h>

#include <finditemengine.h>

// Classes referenced
class RFs;
class CActiveSchedulerWait;
class CVPbkContactManager;
class MVPbkContactLink;
class CVPbkContactLinkArray;
class CVPbkContactStoreUriArray;
class MVPbkContactOperationBase;
class MVPbkFieldType;
class MVPbkFieldTypeList;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CVPbkFieldTypeRefsList;
class MVPbkStoreContactFieldCollection;
class MVPbkContactFieldSelector;

class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
class CFbsBitmap;

/**
 * Observer interface for contact added/changed/deleted notifications.
 */
class MHgCtxContactObserver
    {
public:
    /**
     * Called when a contact is added, changed, or deleted.
     */
    virtual void HandleContactEventL() = 0;
    };
    
/**
* CHgCtxContactMatcher class is an API for contact matching.
*
* Before a method starts executing a Virtual Phonebook operation,
* it checks if there's an older asynchronous operation already in execution.
* If there is, synchronous methods leave with error code KErrInUse.
* Asynchronous methods complete the request with same error code.
*
* If phone backup/restore has been detected, using methods which
* require Virtual Phonebook is not possible normally. 
* Instead those methods leave with error code KErrAccessDenied.
* Once EStoreBackupRestoreCompleted event has been received,
* methods can be called normally.
*
* @lib hgcontextservicesutils.lib
*/

NONSHARABLE_CLASS( CHgCtxContactMatcher ) : public CBase,
	public MVPbkSingleContactOperationObserver,
	public MVPbkContactStoreListObserver,
	public MVPbkContactFindObserver, 
	public MVPbkContactFindFromStoresObserver,
	public MVPbkContactViewObserver
    {
public: // Construct & destruct

    /**
    * Two-phase constructor for CHgCtxContactMatcher class.
    *
    * @param  aFsSession File server session.
    *
    * @return CHgCtxContactMatcher* Pointer to newly created instance.
    */
    IMPORT_C static CHgCtxContactMatcher* NewL( RFs* aFsSession = 0 );

    /**
    * Two-phase constructor for CHgCtxContactMatcher class.
    * Like NewL(), but also places instance on cleanup stack.
    *
    * @return CHgCtxContactMatcher* Pointer to newly created instance.
    */
    IMPORT_C static CHgCtxContactMatcher* NewLC( RFs* aFsSession = 0 );

    /**
    * C++ destructor.
    */
    IMPORT_C ~CHgCtxContactMatcher();

private: // Internal construct

    /**
    * C++ Constructor.
    */
    CHgCtxContactMatcher( RFs* aFsSession );

    /**
    * Second phase constructor
    */
    void ConstructL();

public: // API methods

    /**
    * Opens one or more contact stores for future match operations.
    *
    * @param aUriArray  Array of contact store URIs to be opened.
    *
    * @exception KErrNotSupported when none of the stores opens
    * @exception KErrGeneral if some stores are already open
    */
    IMPORT_C void OpenStoreL( const CVPbkContactStoreUriArray& aUriArray );

    /**
    * Opens one or more contact stores for future match operations.
    * Asynchronous version.
    *
    * @param aUriArray  Array of contact store URIs to be opened.
    * @param aStatus  Completion status of the request.
    *                 KErrNone - at least one store opened successfully
    *                 KErrNotSupported - all stores failed to open
    *                 KErrGeneral - if some stores are already open
    */		    
    IMPORT_C void OpenStoreL( const CVPbkContactStoreUriArray& aUriArray,
        TRequestStatus& aStatus );			    

    /**
    * Opens all contact stores for future match operations.
    * NOTE: does not open OwnNumber store.
    *
    * @exception KErrNotSupported when none of the stores opens
    * @exception KErrGeneral if some stores are already open
    */
    IMPORT_C void OpenAllStoresL();

    /**
    * Opens all contact stores for future match operations.
    * Asynchronous version.
    * NOTE: does not open OwnNumber store.
    *
    * @param aStatus  Completion status of the request.
    *                 KErrNone - at least one store opened successfully
    *                 KErrNotSupported - all stores failed to open
    *                 KErrGeneral - if some stores are already open
    */
    IMPORT_C void OpenAllStoresL( TRequestStatus& aStatus );			

    /**
    * Opens default contact stores for future match operations.
    * Used to find local aliases to phone numbers and email addresses
    * NOTE: does not open OwnNumber store or fixed dialing store
    * Opens the stores in the default preference order
    *
    * @exception KErrNotSupported when none of the stores opens
    * @exception KErrGeneral if some stores are already open
    */
    IMPORT_C void OpenDefaultMatchStoresL();

    /**
    * Opens default contact stores for future match operations.
    * Used to find local aliases to phone numbers and email addresses
    * Asynchronous version.
    * NOTE: does not open OwnNumber store or fixed dialing store
    * Opens the stores in the default preference order
    *
    * @param aStatus  Completion status of the request.
    *                 KErrNone - at least one store opened successfully
    *                 KErrNotSupported - all stores failed to open
    *                 KErrGeneral - if some stores are already open
    */
    IMPORT_C void OpenDefaultMatchStoresL( TRequestStatus& aStatus );

    /**
    * Opens all OwnNumber stores for future match operations.
    *
    * @exception KErrNotSupported when none of the stores opens
    * @exception KErrGeneral if some stores are already open
    */
    IMPORT_C void OpenOwnNumberStoresL();

    /**
    * Opens all OwnNumber stores for future match operations.       
    *
    * @param aStatus  Completion status of the request.
    *                 KErrNone - at least one store opened successfully
    *                 KErrNotSupported - all stores failed to open
    *                 KErrGeneral - if some stores are already open
    */	
    IMPORT_C void OpenOwnNumberStoresL( TRequestStatus& aStatus );	

    /**
    * Closes all currently open contact stores
    * including OwnNumber stores.
    *
    * @return Leaves on error.
    */
    IMPORT_C void CloseStoresL();

    /**
    * Determines whether a phone number is OwnNumber. Synchronous version.
    * The method searches for the number from currently open stores.
    * In order to use SIM's ISDN store, it should be the only open store.
    * Can be opened with OpenOwnNumberStoresL().
    *
    * @param aSearch  Phone number to search for
    * @param aResult  Boolean result.
    */
    IMPORT_C void IsOwnNumberL( const TDesC& aSearch, TBool& aResult );

    /**
    * Determines whether a phone number is OwnNumber. Asynchronous version.
    * The method searches for the number from currently open stores.
    * In order to use SIM's ISDN store, it should be the only open store.
    * Can be opened with OpenOwnNumberStoresL().
    *
    * @param aSearch  Phone number to search for
    * @param aStatus  Request status containing the result and 
    *                 possible error code.
    *                 KErrNone     = TRUE,
    *                 KErrNotFound = FALSE,
    *                 other value  = system wide error code.        
    */
    IMPORT_C void IsOwnNumberL( const TDesC& aSearch, TRequestStatus& aStatus );

    /**
    * Finds contacts with phone numbers that match the search string.
    * Synchronous version.
    * Contacts are searched from all currently open contact stores.
    * Matching is done from the end of the phone number.
    * Note: If the search string is shorter than 7 digits, matching from
    * Phone memory contact store works ONLY if the phone number is
    * exactly the same as the search string.
    * Example:
    * Search string "567" would find phone number "567", but not "1234567".
    * From SIM stores both numbers would be found.
    *
    * @param aSearch  Search string
    * @param aDigits  Number of digits to match from the end of phone number.
    * @param aFlags   Search all or stop at first match.
    *                 See enum values from CVPbkPhoneNumberMatchStrategy.h
    * @param aLinkArray  Links to matching contacts are returned in this
    *                 array that method caller provides.
    *
    * @return KErrNone = success, otherwise an error code. ,,list of codes
    */
    IMPORT_C void MatchPhoneNumberL( const TDesC& aSearch, TInt aDigits,
        CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags,
        CVPbkContactLinkArray& aLinkArray );

    /**
    * Finds contacts with phone numbers that match the search string.
    * Asynchronous version.
    * Searching is done according to parameters like in the synchronous version,
    * but status code is returned in aStatus.
    *
    * @param aStatus  Completion status of the request.
    *                 Values: KErrNone = success, otherwise an error code.
    */
    IMPORT_C void MatchPhoneNumberL( const TDesC& aSearch, TInt aDigits,
        CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags,
        CVPbkContactLinkArray& aLinkArray, TRequestStatus& aStatus );

    /**
    * Returns the global list of possible field types.
    *
    * @return List of field types
    */
    IMPORT_C const MVPbkFieldTypeList& FieldTypes() const;
                
    /**
    * Gets a store contact from a contact link.
    * @param aContactLink  The link from which store contact should be returned.
    * @param aStoreContact Pointer to store contact.
    *                      Client must take the ownership immediately.
    */
    IMPORT_C void GetStoreContactL( const MVPbkContactLink& aContactLink,
        MVPbkStoreContact** aStoreContact );
        
    /**
    * Gets a store contact from a contact link.
    * Asynchronous version.
    * Parameters like in the synchronous version,
    * but status code is returned in aStatus.
    *
    * @param aStatus  Completion status of the request.
    *                 Values: KErrNone = success, otherwise an error code.
    */
    IMPORT_C void GetStoreContactL( const MVPbkContactLink& aContactLink,
        MVPbkStoreContact** aStoreContact, TRequestStatus& aStatus );

    /**
    * Returns a pointer to contact's field data of given field type.
    * If the field type isn't found from contact, return value is KNullDesC.
    * NOTE: this works only for field types of storage type 
    * EVPbkFieldStorageTypeText.
    * For other types, leaves with error code KErrArgument.
    *
    * @param  aStoreContact The contact from which field data should be returned.
    * @param  aFieldType    Field's type.
    * @return TPtrC pointer to field's data.
    */
    IMPORT_C TPtrC GetFieldDataTextL( const MVPbkStoreContact& aStoreContact,
        const MVPbkFieldType& aFieldType ) const; 

    /**
    * Returns a pointer to contact's field data of given field type.
    * If the field type isn't found from contact, return value is
    * "1.1.1111". (d.m.yyyy).
    * NOTE: this works only for field types of storage type 
    * EVPbkFieldStorageTypeDateTime.
    * For other types, leaves with error code KErrArgument.
    *
    * @param  aStoreContact The contact from which field data should be returned.
    * @param  aFieldType    Field's type.
    * @return TTime pointer to field's data.
    */
    IMPORT_C TTime GetFieldDataDateTimeL( const MVPbkStoreContact& aStoreContact,
        const MVPbkFieldType& aFieldType ) const; 

    /**
    * Returns a pointer to contact's field data of given field type.
    * If the field type isn't found from contact, return value is KNullDesC8.
    * NOTE: this works only for field types of storage type 
    * EVPbkFieldStorageTypeBinary.
    * For other types, leaves with error code KErrArgument.
    *
    * @param  aStoreContact The contact from which field data should be returned.
    * @param  aFieldType    Field's type.
    * @return TPtrC8 pointer to field's data.
    */
    IMPORT_C TPtrC8 GetFieldDataBinaryL(  const MVPbkStoreContact& aStoreContact,
        const MVPbkFieldType& aFieldType ) const; 		    		    

    /**
    * Cancels asynchronous operation.
    */	
    IMPORT_C void CancelOperation();

    /**
    * Returns the used contact store list, needed for e.g. aiw fetch
    * @return The used contact store list
    */
    IMPORT_C MVPbkContactStoreList& GetContactStoresL();

    /**
    * Returns the name of the contact in the same format as MPbk2ContactNameFormatter
    * @param  aFieldCollection The fieldcollection of the contact
    * @return HBufC* the name of the contact or null
    */
    IMPORT_C HBufC* GetNameL(
        MVPbkStoreContactFieldCollection& aFieldCollection );

    /**
    * Returns reference to the contactmanager
    * @return reference to the contact manager
    */
    IMPORT_C CVPbkContactManager& GetContactManager();
    
    /**
    * Finds contacts with field data that match the search string.
    * Contacts are searched from all currently open contact stores.
    * Matching is done from field types given by the method caller.
    * The list of all possible field types can be fetched from the wrapper
    * using FieldTypes().
    *
    * @param aSearch  Search string
    * @param aFieldTypes  List of field types included in matching.
    * @param aLinkArray  Links to matching contacts are returned 
    *                    in this array that method caller provides.
    *
    * @return KErrNone = success, otherwise an error code. ,,list of codes
    */
    IMPORT_C void MatchDataL( const TDesC& aSearch, const MVPbkFieldTypeList& aFieldTypes, 		
        CVPbkContactLinkArray& aLinkArray);

    /**
    * Finds contacts with field data that match the search string.
    * Contacts are searched from all currently open contact stores.
    * Matching is done from field types given by the method caller.
    * The list of all possible field types can be fetched from the wrapper
    * using FieldTypes().
    *
    * @param aSearchStrings  Search strings
    * @param aFieldTypes  List of field types included in matching.
    * @param aLinkArray  Links to matching contacts are returned
    *                    in this array that method caller provides.
    * @param aWordParserCallBack is the callback function to the parser
    *
    * @return KErrNone = success, otherwise an error code.
    */
    IMPORT_C void MatchDataL( const MDesC16Array& aSearchStrings, 
        const MVPbkFieldTypeList& aFieldTypes,
        CVPbkContactLinkArray& aLinkArray,
        const TCallBack& aWordParserCallBack );		   

    /**
    * Finds contacts with field data that match the search string.
    * Asynchronous version.
    * Searching is done according to parameters like in the synchronous version,
    * but status code is returned in aStatus.
    *
    * @param aStatus  Completion status of the request.
    *                 Values: KErrNone = success, otherwise an error code.
    */
    IMPORT_C void MatchDataL( const TDesC& aSearch, const MVPbkFieldTypeList& aFieldTypes, 		
        CVPbkContactLinkArray& aLinkArray, TRequestStatus& aStatus );
        
     /**
    * Returns reference to the contact name formatter. Object is created if not used earlier.
    * @return reference to the contact name formatter
    */
    IMPORT_C MPbk2ContactNameFormatter& ContactNameFormatterL();
    
    /**
     * Splits the input to words and then performs a MatchDataL using the same
     * splitter callback function.
     *
     * LookupL and this are the preferred functions to do text-based lookups from Hg code.
     *
     * @see MatchDataL
     * @see LookupL
     */
    IMPORT_C void SplitAndMatchL( const TDesC& aData,
        const MVPbkFieldTypeList& aFieldTypes,
        CVPbkContactLinkArray& aLinkArray );
        
    /**
     * Registers for getting notifications when a contact is added, changed, or deleted.
     * Does nothing if already added.
     * @param   aObserver   ref to observer
     */
    IMPORT_C void RegisterContactObserverL( MHgCtxContactObserver& aObserver );
    
    /**
     * Unregisters the given observer.
     * Does nothing if the given observer has not been registered before.
     * @param   aObserver   ref to observer
     */
    IMPORT_C void UnregisterContactObserver( MHgCtxContactObserver& aObserver );
    
    /**
     * Looks up a contact based on
     *   - name, or
     *   - phone number, or
     *   - email address, or
     *   - service id
     *
     * Also handles the "name <number>" or "name <email>" format.
     *
     * For service IDs the full uri must be given, e.g. if a contact
     * has an Ovi ID in phonebook set to "somebody" then the search will
     * only succeed if aData contains "Ovi:somebody" (unless of course
     * the string "somebody" matches some name fields).
     *
     * @param   aData           name or phone number of email address
     * @param   aLinkArray      array to which the result links are added,
     *                          no items are appended if not found
     */
    IMPORT_C void LookupL( const TDesC& aData, CVPbkContactLinkArray& aLinkArray );

    /**
    * Returns the phone numbers of the contact.
    * @param  aFieldCollection The fieldcollection of the contact
    * @param    aArray         Phone numbers are appended to this. (not resetted)
    */
    IMPORT_C void GetNumbersL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray );
 
    /**
    * Returns the email addresses of the contact.
    * @param  aFieldCollection The fieldcollection of the contact
    * @param    aArray          Email addresses are appended to this. (not resetted)
    */
    IMPORT_C void GetEmailAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray );

    /**
    * Constructs the address (e.g. street, city, country) for the contact.
    * The elements are combined into one string, using space as separator.
    * aArray will have 0-3 items appended: nothing, or general and/or work and/or home address.
    * @param  aFieldCollection The fieldcollection of the contact
    * @param  aArray           Addresses are appended to this. (not resetted)
    */
    IMPORT_C void GetAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray );

    enum TWebAddressesType
        {
        EWebAddresses, //for general web adress
        EWebAddressesHome, //for home web adress
        EWebAddressesWork //for work web adress
        };
    
    /**
    * Gets the prefered web address for the contact.
    * 
    * aArray can have 0 or more items appended.
    * 
    * @param  aFieldCollection The fieldcollection of the contact
    * @param  aArray           Web addresses are appended to this. (not resetted)
    * @param  aType            Determinates which web adress return, @see TWebAddressesType
    */
    IMPORT_C void GetWebAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray,
        TWebAddressesType aType );   
      
    
    /**
    * Gets the thumbnail for a contact.
    * @param  aFieldCollection The fieldcollection of the contact
    * @return bitmap or null (null if not found)
    */
    IMPORT_C CFbsBitmap* GetThumbnailL(
        MVPbkStoreContactFieldCollection& aFieldCollection );

    /**
     * Checks if the given string is a phone number.
     */
    IMPORT_C static TBool IsPhoneNumberL( const TDesC& aData );
    
    /**
     * Checks if the given string is an email address.
     */
    IMPORT_C static TBool IsEmailAddressL( const TDesC& aData );
    
    /**
     * Appends content of name fields to the given array.
     * Uses less fields than LookupL, here only family and given name
     * are taken and returned.
     *
     * @param   aFieldCollection        fields to check
     * @param   aArray                  array to which strings are appended
     */
    IMPORT_C void GetNamesForFindL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray );
        
    /**
     * Gets content of name fields.
     * Overload for getting results in one string instead of an array.
     * @param   aFieldCollection    fields to check
     * @return  content of name fields in no particular order separated by a space
     */
    IMPORT_C HBufC* GetNamesForFindL(
        MVPbkStoreContactFieldCollection& aFieldCollection );
        
    /**
     * Splits a string into parts.
     * Space, comma, and semicolon are treated as separators.
     */
    IMPORT_C static CDesCArray* SplitFindStringL(const TDesC& aFindString);
    
    /**
     * Splits a "name1 name2 ... <something>" into two parts:
     * "name1 name2 ..." and "something".
     * < and > characters are removed.
     * The resulting strings are appended to the given arrray.
     * Such format is used by messaging components and this function
     * is useful if a search has to be made based on both the name and
     * the phone number / email address.
     * @param   aString     input string
     * @param   aArray      array to append to
     */
    IMPORT_C static void SplitMsgContactL( const TDesC& aString, CDesCArray& aArray );

    /**
     * Appends the values from all matching text fields to a given array.
     * @param   aFieldCollection    fields to scan through
     * @param   aArray              destination array
     * @param   aVersitName         versit property name
     * @param   aVersitParam        versit property parameter
     */
    IMPORT_C void GetCustomFieldL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray,
        TVPbkFieldTypeName aVersitName,
        TVPbkFieldTypeParameter aVersitParam );

    /**
     * Returns a matching field type for the given versit name/parameter.
     * The returned list is empty if no matching field types were found.
     * @param   aVersitName         versit property name
     * @param   aVersitParam        versit property parameter
     * @return  field type list, also on the cleanup stack
     */
    IMPORT_C CVPbkFieldTypeRefsList* GetCustomFieldTypeLC(
        TVPbkFieldTypeName aVersitName,
        TVPbkFieldTypeParameter aVersitParam );

    /**
     * Finds impp field data.
     * @param   aFieldCollection    fields to scan
     * @param   aSchemeOnlyAray     if non-null then scheme parts are appended to here
     * @param   aUriOnlyArray       if non-null then uri parts are appended to here
     * @param   aFullArray          if non-null then full field contents are appended to here
     */
    IMPORT_C void GetImppFieldL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray* aSchemeOnlyArray,
        CDesCArray* aUriOnlyArray,
        CDesCArray* aFullArray );

    /**
     * Finds all contacts that have a birthday on the given month/day.
     * Other components from aDate - like year, hours, etc. - are ignored.
     * @param   aDate       date to look for
     * @param   aLinkArray  links are appended to this array
     * If no matching contacts are found then aLinkArray is left untouched.
     */
    IMPORT_C void FindContactWithBirthdayL( const TTime& aDate,
        CVPbkContactLinkArray& aLinkArray );
        
private: // from MVPbkContactStoreListObserver, MVPbkContactStoreObserver

    /**
    * Called when the opening process is complete, ie. all stores have
    * been reported either failed or successfully opened.
    */
    void OpenComplete();

    /**
    * Called when a contact store is ready for use.
    */
    void StoreReady( MVPbkContactStore& aContactStore );

    /**
    * Called when a contact store becomes unavailable.
    * Client may inspect the reason of the unavailability and decide whether or not
    * it will keep the store opened (ie. listen to the store events).
    * @param aContactStore The store that became unavailable.
    * @param aReason The reason why the store is unavailable.
    *                This is one of the system wide error codes.
    */
    void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );

    /**
    * Called when changes occur in the contact store.
    * @see TVPbkContactStoreEvent, MVPbkContactStoreObserver.h
    *
    * @param aContactStore
    * @param aStoreEvent Event that has occured.
    */
    void HandleStoreEventL( MVPbkContactStore& aContactStore,
        TVPbkContactStoreEvent aStoreEvent );

private: // from MVPbkContactFindFromStoresObserver
    /**
    * Called when find is complete on a single store. Callee takes
    * ownership of the results. In case of an error during find,
    * the aResultsFromStore may contain only partial results of the find.
    *
    * @param aStore is the store from which the contacts were searched from
    *
    * @param aResultsFromStore Array of contact links that matched the find.
    *                 Callee must take ownership of this object in
    *                 the end of the function, ie. in case the function
    *                 does not leave.
    */
    void FindFromStoreSucceededL( MVPbkContactStore& aStore,
        MVPbkContactLinkArray* aResultsFromStore );
                        
    /**
    * This function is called if/when there were errors while searching
    * from a store.
    * @param aStore is the store from which the search was done.
    * @param aError is the error code.
    */
    void FindFromStoreFailed( MVPbkContactStore& aStore, TInt aError );

    /**
    * Called when find is complete.
    */
    void FindFromStoresOperationComplete();

private: // from MVPbkContactFindObserver

    /**
    * Called when find is complete. Callee takes ownership of the results.
    * In case of an error during find, the aResults may contain only
    * partial results of the find.
    *
    * @param aResults Array of contact links that matched the find.
    *                 Callee must take ownership of this object in
    *                 the end of the function, ie. in case the function
    *                 does not leave.
    */
    void FindCompleteL( MVPbkContactLinkArray* aResults );

    /**
    * Called in case the find fails for some reason.
    * 
    * @param aError One of the system wide error codes.
    */
    void FindFailed( TInt aError );

private: // from MVPbkSingleContactOperationObserver

    /**
    * Called when operation is completed.
    *
    * @param aOperation the completed operation.
    * @param aContact  the contact returned by the operation.
    *                  Client must take the ownership immediately.
    */
    void VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation, MVPbkStoreContact* aContact );

    /**
    * Called if the operation fails.
    *
    * @param aOperation    the failed operation.
    * @param aError        error code of the failure.
    */
    void VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, TInt aError );        

private: // from MVPbkContactViewObserver
    void ContactViewReady(
            MVPbkContactViewBase& aView );
    void ContactViewUnavailable(
            MVPbkContactViewBase& aView );
    void ContactAddedToView(
            MVPbkContactViewBase& aView, 
            TInt aIndex, 
            const MVPbkContactLink& aContactLink );
    void ContactRemovedFromView(
            MVPbkContactViewBase& aView, 
            TInt aIndex, 
            const MVPbkContactLink& aContactLink );
    void ContactViewError(
            MVPbkContactViewBase& aView, 
            TInt aError, 
            TBool aErrorNotified );

private: // Constants

    // Wrapper method IDs for calls that have an asynchronous version
    enum TMethodId
        {
        ENoMethod = 0,
        EMatchPhoneNumber,
        EMatchData,
        EGetStoreContact,
        EOpenStore, 
        ECloseStores 
        };

    // Wrapper method processing status.
    enum TMethodStatus
        {
        EIdle = 0,
        EExecuting,
        EFinished
        };

private: // Methods
    const MVPbkStoreContactField* FindField( const MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFType ) const;
    static CVPbkContactStoreUriArray* GetStoreArrayLC(
        const TDesC& (* const aFuncPtrs[])() );
    void OpenStoreL( const TDesC& (* const aFuncPtrs[])() );
    void OpenStoreL( const TDesC& (* const aFuncPtrs[])(),
        TRequestStatus&  aStatus );
    void FreeOldOperation();

    // Copies the entries to the existing recipient array
    void CopyFindResultsL( MVPbkContactLinkArray* aResults );

    // Open store, code common to sync/async versions.
    void OpenStoreCommonL( const CVPbkContactStoreUriArray& aUriArray );

    // Match phonenumber, code common to sync/async versions.
    void MatchPhoneNumberCommonL( const TDesC& aData, TInt aDigits,
        CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags );

    void InitOperationL( TMethodId aApiMethod );
    void InitOperationL( TMethodId aApiMethod, TRequestStatus* aStatus );
    void InitOperation( TRequestStatus* aStatus );
    void OperationComplete( TInt ErrorCode = KErrNone );
    void OperationFailed( TInt aError );
    void CleanupNumberMatch();
    void RemoveSimilarEmailAddressesL( const TDesC& aData, CVPbkContactLinkArray& aLinkArray, const MVPbkFieldTypeList& aFieldTypes );

    void TryTextLookupL( const TDesC& aName, CVPbkContactLinkArray& aLinkArray );
    void TryNumberLookupL( const TDesC& aName, CVPbkContactLinkArray& aLinkArray );

    void PreCreateNameFieldTypesL();
    void PreCreateEmailFieldTypesL();
    void PreCreateXspIdFieldTypesL();
    void PreCreateNumberFieldTypesL();
    void PreCreateAddressFieldTypesL();
    void PreCreateWebAddressFieldTypesL();
    void PreCreateWebAddressHomeFieldTypesL();
    void PreCreateWebAddressWorkFieldTypesL();
    void PreCreateWorkAddressFieldTypesL();
    void PreCreateHomeAddressFieldTypesL();
    
    void GetTextFieldsL( const CVPbkFieldTypeRefsList& aList,
        const MVPbkStoreContactFieldCollection& aFieldCollection, CDesCArray& aArray );

private: // Data

    // Used members

    RFs* iFsSession;
    TBool iFsSessionOwned;
    MVPbkStoreContact** iResultStoreContact; // result of GetStoreContact
    CVPbkContactLinkArray* iResultContactLinkArray; // result of matching operations
    TInt iResultContactLinkCnt; // number of links found in matching operations
    TRequestStatus* iClientStatus; // request status used in asynch calls

    // Own members
    CVPbkContactManager* iContactManager;
    MVPbkContactOperationBase* iOperation;  // CM operation being processed
    CActiveSchedulerWait iASchedulerWait;   // used in asynch calls
    CVPbkContactStoreUriArray* iStoreUris;  // used in matching
    CVPbkPhoneNumberMatchStrategy* iMatchStrategy; // used in matching
    CVPbkPhoneNumberMatchStrategy::TConfig* iStratConfig;  // used in matching
    TBool iSync;  // is this wrapper call Synchronous (1) or Asynchronous (0) 
    TInt iError;  // error code used while processing VPbk-operations
    TBool iBackup;// contact store backup/restore in progress
    // API method ID. Needed for cleanup after method finishes.
    TMethodId iApiMethod;
    // API method status. Needed to know processing has finished.
    TMethodStatus iApiMethodStatus;

    CPbk2SortOrderManager* iSortOrderManager;
    MPbk2ContactNameFormatter* iNameFormatter;
    
    RPointerArray<MHgCtxContactObserver> iContactObservers; // ptrs not owned
    CVPbkFieldTypeRefsList* iNameFieldTypes;
    CVPbkFieldTypeRefsList* iEmailFieldTypes;
    CVPbkFieldTypeRefsList* iXspIdFieldTypes;
    CVPbkFieldTypeRefsList* iNumberFieldTypes;
    CVPbkFieldTypeRefsList* iAddressFieldTypes;
    CVPbkFieldTypeRefsList* iWebAddressFieldTypes;
    CVPbkFieldTypeRefsList* iWebAddressHomeFieldTypes;
    CVPbkFieldTypeRefsList* iWebAddressWorkFieldTypes;
    CVPbkFieldTypeRefsList* iWorkAddressFieldTypes;
    CVPbkFieldTypeRefsList* iHomeAddressFieldTypes;
    };

/**
* Panic codes used in CHgCtxContactMatcher.
*
* @since 3.1u
*/
class HgContactMatcherPanics
    {
public: // API
        
    /**
    * Panic codes
    */
    enum TPanic
        {
        EPanNullPointer = 0,
        EPanInvalidParam,
        EPanInvalidOp
        };

public:

    /**
    * Panic
    */
    static void Panic( TPanic aPanic );
    };

#endif

// End of File
