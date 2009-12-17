/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut definition parser.
*
*/


#ifndef TAISCUTPARSER_H
#define TAISCUTPARSER_H
#include <uri16.h>                  // For TUriParser16
#include <AknsItemID.h> 

// =============================================================================
// ========================= Supported URI schemes =============================
/** URI scheme for local application shortcuts */
_LIT( KScutURISchemeLocalApp, "localapp" );

/** URI sheme for normal web addresses */
_LIT( KScutURISchemeHttp,     "http" );
/**  URI sheme for secure web addresses */
_LIT( KScutURISchemeHttps,    "https" );
// =============================================================================

// =============================================================================
// ============= Application shortcut formatting literals ======================

/** Literal to format an application shortcut without parameters */
_LIT( KScutFormatApplication,                "localapp:0x%x" );

/** Literal to format an application shortcut with parameter name and value */
_LIT( KScutFormatApplicationWithParams,      "localapp:0x%x?%S=%S" );

/** Literal to format an application shortcut with a single parameter string */
_LIT( KScutFormatApplicationWithParamString, "localapp:0x%x?%S" );

// =============================================================================

// =============================================================================
// ============ Shortcut parameter name and value literals =====================

/** Parameter name for view ids */
_LIT( KScutParamNameView,           "view" );

/** Parameter name for remote mailboxes. Specific to messaging shortcuts */
_LIT( KScutParamNameMailbox,        "mailbox" );

/** Parameter name for new message and new email. Specific to messaging shortcuts */
_LIT( KScutParamNameNew,            "new" );

/** Parameter value for new message shortcuts. */
_LIT( KScutParamValueMsg,           "msg" );

/** Parameter value for new email shortcuts */
_LIT( KScutParamValueEmail,         "email" );

/** Parameter value for new syncml mail shortcuts */
_LIT( KScutParamValueSyncMLMail,    "syncmlmail" );

/** Parameter value for new postcard shortcuts */
_LIT( KScutParamValuePostcard,      "postcard" );

/** Parameter value for new audio message shortcuts */
_LIT( KScutParamValueAudioMsg,      "audiomsg" );

/** Parameter value for the connectivity status view shortcut */
const TUid KScutParamValueConnectivityView = { 0x10207250 };

/** Parameter name for enable keylock */
_LIT( KScutParamNameOn,             "on" );

/** Parameter name for missed calls view */
_LIT( KScutParamValueMissedCalls,   "missed" );

/** Parameter name for dialled calls view */
_LIT( KScutParamValueDialledCalls,  "dialled" );

/** Parameter name for received calls view */
_LIT( KScutParamValueReceivedCalls, "received" );

/** Parameter name for logs main view */
_LIT( KScutParamValueMainView,      "counters" );

/** Parameter name for bookmark ids */
_LIT( KScutParamNameBookmark,       "bkm" );

/** Parameter name for icon id in skin
  Format localapp:0xUID?iconid=majorid;minorid;optionalColourGroup */   
_LIT( KScutParamNameIconSkinId,       "iconid" );

/** Parameter name for icon path. Left here for backward
    compatibility.
    Format localapp:0xUID?iconmifpath=mif_file.mif;index */
_LIT( KScutParamNameIconMifPath,       "iconmifpath" );

/** Parameter name for icon path. MBM and MIF supported
    Format localapp:0xUID?iconmifpath=mif_file.mif;index */
_LIT( KScutParamNameIconPath,       "iconpath" );
/** Parameter name for custom URL title.
    Format http://www.url.com?customtitle=Here is my great title */
_LIT( KScutParamNameCustomTitle,       "customtitle" );
/** Parameter name for CBA icon
    Format localapp:0xUID?iconid=majorid;minorid;optionalColourGroup&cba=1 */   
_LIT( KScutParamNameCBAIcon,       "cba" );

/** Parameter name for toolbar icon */
_LIT( KScutParamNameToolbarIcon,       "toolbar" );

/** Shortcut definition parameter for "no effect" */
_LIT( KScutParamNoEffect,           "noeffect" );

// =============================================================================
// =============================================================================
// ============ Shortcut parameter name and value literals =====================

/** Alias for messaging shortcuts */
_LIT( KScutTargetAliasMessaging, "msg" );

/** Alias for keylock shortcuts */
_LIT( KScutTargetAliasKeylock,   "keylock" );

/** Alias for logs shortcuts */
_LIT( KScutTargetAliasLogs,      "logs" );

/** Alias for voice dialer shortcuts */
_LIT( KScutTargetAliasVoiceDial, "voicedial" );

// =============================================================================
/** Maximum length of shortcut definition. Used when composing a definition */
const TInt KMaxDefinitionLength = 100;

/** Shortcut parameter value separator character */
const TText KParamValueSeparator = '=';

/** Shortcut parameter next param separator */
const TText KParamNextSeparator = '&';

_LIT( KScutMIFExtension, ".mif" );  
_LIT( KScutMBMExtension, ".mbm" );  
_LIT( KScutSkinItemSeparator, ";" );

/**
 * Shortcut definition components
 */
enum TScutDefComponent
{
	EScutDefScheme,    
	EScutDefTarget,        
	EScutDefParamName,        
	EScutDefParamValue,    
	EScutDefParamNameAndValue,    
	EScutDefComplete
};

/**
 * Shortcut types
 */
enum TShortcutType
{
	EScutUnknown,
	EScutAnyType = 0,
	EScutNoEffect,
	EScutApplication,
	EScutApplicationView,
	EScutApplicationWithParams,
	EScutNewMessage,
	EScutNewEmail,
	EScutNewSyncMLMail,
	EScutNewPostcard,
	EScutNewAudioMsg,
	EScutNewMsgType,
	EScutMailbox,
	EScutChangeTheme,
	EScutWebAddress,
	EScutBookmark,
	EScutKeylock,
	EScutLogsMissedCallsView,
	EScutLogsDialledCallsView,
	EScutLogsReceivedCallsView,
	EScutLogsMainView,
	EScutConnectivityStatusView,
	EScutApplicationManagerView
};

/**
 * Icon type
 */
enum TShortcutIconType
{
	EScutIconNone,
	EScutIconSkin,
    EScutIconMif,
    EScutIconMbm
};

enum TShortcutIconDestination
{
	EScutDestinationNormal,
	EScutDestinationSoftkey,
	EScutDestinationToolbar
};

class TAiScutIcon
{
	public:
        /**
        * Index of the icon in icon file
        */
        TInt iIconId;

        /**
        * Path to the icon file
        */
		TFileName iPath;
		 
		/**
		 * Skin item id of the icon
		 */
		TAknsItemID iSkinId;
		
		/**
		 * Colour groups id in skin
		 */
		TInt iColourGroup;
		
		/**
		 * Type of the icon. From skin or from mif
		 */
		TShortcutIconType iType;
		
		/**
		 * AppUid that this icon belongs to
		 */
		TUid iAppUid;
		
		/**
		 * Possible view id
		 */
		TUid iViewId;
		
		/**
		 * Type of the shortcut
		 */
		TShortcutType iShortcutType;
		
		/**
		 * Is this CBA specific icon
		 */
		TShortcutIconDestination iDestination;
		
};

/**
 *  Shortcuf definition parser
 *
 *  @since S60 v3.2
 */
class TAiScutParser
{

public:
	TAiScutParser();
	
	/**
	 * Static utility function to parse an uid from the given descriptor
	 *
	 * @since S60 v3.2
	 * @param aString The String to parse
	 * @return Parsed application uid
	 */
	 static TUid ParseUid( const TDesC& aDesC );
	
	/**
	 * Parses a shortcut definition
	 *
	 * @since S60 v3.2
	 * @param aDefinition Shortcut definition
	 * @return System wide error code. KErrCorrupt if not recognized
	 */
	 TInt Parse( const TDesC& aDefinition );
	
	/**
	 * Checks if the shortcut definition was valid
	 *
	 * @since S60 v3.2
	 * @return ETrue if valid, EFalse if not
	 */
	 TBool IsValid() const;
	
	/**
	 * Returns the shortcut target type
	 *
	 * @since S60 v3.2
	 * @return Shortcut target type
	 */
	 TShortcutType Type() const;
	 
	/**
	 * Returns the possible shortcut overriding icon 
	 * that has been defined in the URL either with the format 
	 *  localapp:0xUID?iconid=majorid;minorid;colourgroup
	 * or
	 *  localapp:0xUID?iconmifpath=mif_file.mif;index
	 *
	 * @since S60 v3.2
	 * @return Shortcut icon override
	 */
	 TAiScutIcon Icon() const;
	
	/**
	 * Returns the shortcut target uid. Used for application shortcuts
	 *
	 * @since S60 v3.2
	 * @return Shortcut target uid
	 */
	 TUid Uid() const;
	 
	/**
	 * Returns a shortcut definition component value
	 *
	 * @since S60 v3.2
	 * @param aComponent Shortcut definition component
	 * @return Pointer descriptor to component value
	 */
	 TPtrC Get( TScutDefComponent aComponent ) const;
	 
	/**
	 * Composes a shortcut definition string from given parameters
	 *
	 * @since S60 v3.2
	 * @param aDes On return, the shortcut definition. Transfers ownership
	 * @param aUid Application uid
	 * @param aParamName Parameter name
	 * @param aParamValue Parameter value
	 */
	 void ComposeL( HBufC*& aDes, const TUid aUid,
	 	const TDesC& aParamName, const TDesC& aParamValue );
	
	/**
	 * Composes a shortcut definition string from given parameters
	 *
	 * @since S60 v3.2
	 * @param aDes On return, the shortcut definition. Transfers ownership
	 * @param aUid Application uid
	 * @param aParamString Parameter string
	 */
	 void ComposeL( HBufC*& aDes, const TUid aUid,
	 	const TDesC& aParamString );
	 	
    /**
     * Creates a checksum for the given aDefinition. This is used
     * in URL matching so that no string need to be stored. Checksum is 
     * done checksum = positionInString * charValue
     *
     * @since S60 v3.2 
     * @param aDefinition The definition to calculate the checksum from
     * @return TInt The checksum
     */
    TInt ChecksumForString( const TDesC& aDefinition) const; 

    /**
     * Removes icon definitions from the given string. Icon definitions are
     * KScutParamNameIconSkinId, KScutParamNameIconSkinPath, KScutParamNameCBAIcon
     * KScutParamNameCustomTitle
     * 
     * @since S60 v3.2
     */
    void RemoveExtraDefinitionsL( TDes &aString ) const;
    /**
     * Parses the custom title from the current definition.
     * Returns ETrue on success and places the extracted 
     * custom title to the aTarget
     * 
     * @param aTarget Where to place the custom title. It is callers responsibility
     *  to provide a descriptor with enough room for the custom title.
     * 
     * @return KErrNone if everything is fine, KErrNotFound
     *  if there is no custom title in the definition, KErrNoMemory in
     *  case there is not enough room to place the result (low memory situations
     *  or too small descriptor provided)
     */
    TInt CustomTitle( TDes& aTarget ) const;

protected:
private:

	/**
	 * Checks if an alias was used in shortcut definition and parses an uid from it
	 *
	 * @since S60 v3.2
	 * @return ETrue if alias was found and parsed, EFalse if not
	 */
	 TBool ParseAlias();
	 
	/**
	 * Parses the possible application shortcut parameters
	 *
	 * @since S60 v3.2
	 */
	 void ParseParams();
	 
private:  // data
	/**
	 * URI Parser
	 */
	 TUriParser iUriParser;
	 
	/**
	 * Pointer to the full shortcut definition
	 */
	 TPtrC iDefinition;
	 
	/**
	 * Shortcut target type
	 */
	 TShortcutType iType;
	 
	/**
	 * Shortcut application uid
	 */
	 TUid iUid;
	 
	 /**
	  * Pointer to shortcut parameter name
	  */
	  TPtrC iParamName;
	  
	 /**
	  * Pointer to shortcut parameter value
	  */
	  TPtrC iParamValue;
	 /**
	  * Shortcut icon that has been given with the
	  * URL-string
	  */
	  TAiScutIcon iIcon;
	  };
	  
#endif // TAISCUTPARSER_H

// End of File.