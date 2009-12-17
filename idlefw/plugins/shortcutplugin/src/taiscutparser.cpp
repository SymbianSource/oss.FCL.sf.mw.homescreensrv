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
* Description:  Shortcut definition parser
*
*/


#include "taiscutparser.h"
#include "aiscutdefs.h"

#include "debug.h"


// ======== LOCAL FUNCTIONS ========
/**
 * Extract a value with the given name from a URI query string.
 * For example a query string of ?view=1234&iconid=3;5&foo=bar
 * and we wanted iconid from that string. Function places the 3;5 
 * into aValue and if needed deletes the "iconid=3;5&" string from the
 * query string.
 *
 * @param aQueryString The querystring
 * @param aParameterName The name of the parameter to find
 * @param aValue Where to place the value
 * @param aRemoveNameAndValue ETrue to remove the name=value from the querystring
 * @return KErrNone on succesful extraction. KErrNotFound if the parameter was not found
*/
TInt ExtractValueFromQueryString( TDes &aQueryString, const TDesC &aParameterName,
    TDes &aValue, TBool aRemoveNameAndValue )
    {
    TInt err = KErrNone;
    HBufC *tempBuffer = aQueryString.Alloc();
    if ( !tempBuffer )
        {
        return KErrNoMemory;
        }
    
    TPtr temp = tempBuffer->Des();
    
    TInt foundStartPos = 0;
    TInt foundStopPos = 0;
    foundStartPos =  aQueryString.FindC(aParameterName);
    if ( foundStartPos != KErrNotFound )
        {
        // remove the beginning of the string from temp, so no additional &-marks are found
        // at the start of string
        temp.Delete(0,foundStartPos);

        foundStopPos = temp.Locate(KParamNextSeparator);
        // stop either at the eos or at the next & mark
        foundStopPos = (foundStopPos != KErrNotFound ) ? (foundStopPos): (temp.Length() );
        // start after the = separator and stop either on eos or at & mark
        TInt from = (aParameterName.Length() + 1);
        TInt length = foundStopPos - from;

        // Get just the value part
        if ( aValue.MaxLength() >= length )
            {
            aValue = temp.Mid( from, length );
            }
        else // Can't place the value to aValue string
            {            
            err = KErrNoMemory;
            }

        if ( err == KErrNone && aRemoveNameAndValue )
            {
            // Delete the aParameterName=aValue string from the querystring
            // If eos reached then we need to delete the & before us also
            // Don't try to delete if this is an only parameter
            if ( foundStopPos == temp.Length() && foundStartPos > 0 )
                {
                aQueryString.Delete(foundStartPos - 1, (foundStopPos + 1));
                }
            else
                {
                aQueryString.Delete(foundStartPos, (foundStopPos + 1));
                }
            }        
        }
    else
        {
        err = KErrNotFound;
        }
    
    delete tempBuffer;
    return err;
    }

/**
 * Tests if string ends with given pattern
 * 
 * @param aString input string
 * @param aPattern test pattern
 * @return ETrue if string ends with given pattern.
 */
TBool EndsWith( const TDesC& aString, const TDesC& aPattern )
    {
    TBuf<10> temp(aString.Right(aPattern.Length()));
    return ( aString.Right( aPattern.Length() ) == aPattern );
    }    
    
/**
 * Resolves skin item id from pattern majorId;minorId;colourGroupId.
 * The colourGroupId in the syntax is optional, and if no value found then
 * aColourValue will be -1
 *
 * @param aPath   skin item id string     
 * @param aItemId skin item id to fill
 * @param aColourValue colour value to fill. 
 * 
 * @return ETrue if id was succesfully parsed.
 */
TBool ResolveSkinItemId( const TDesC& aPath, TAknsItemID& aItemId, TInt& aColourValue )
    {
    // Syntax: major;minor;colourgroup    
    aColourValue = -1;
  
    // Initialize lexer
    TLex lex( aPath );
    lex.SkipSpace();

    TInt majorId( 0 );        
    TInt minorId( 0 );
    
    // Resolve major id        
    TInt error = lex.Val( majorId );
    
    // Resolve minor id
    if ( lex.Eos())
        return KErrNotFound;
    
    lex.Inc();
    error |= lex.Val( minorId );
    
    // initilize skin item id object
    aItemId.Set( majorId, minorId );
    
    if ( lex.Eos())
        return KErrNotFound;
    lex.Inc();

    TInt colorError = lex.Val( aColourValue );
    if ( colorError != KErrNone || aColourValue < 0)
        {
        aColourValue = -1;
        }
                
    // Check error
    return ( error == KErrNone );

    }
    
/**
* Resolves filename and id from syntax
* filename.ext;id. If the syntax is incorrect
* aId is -1 and filename zeroed and EFalse is returned
* MIF and MBM supported.
*
* @param aPath The path to extract the data from
* @param aId    Id to fill
* @param aFilename Filename to fill
* @return ETrue if id and path was succesfully parsed.
*/
TBool ResolveFileIdAndPath( const TDesC& aPath, TInt& aId, TDes& aFilename )
{
    // Syntax: filename.ext;index
    // Supported: MIF, MBM
    TInt pos = aPath.FindF( KScutSkinItemSeparator );
    aFilename.Zero();
    if( pos != KErrNotFound )
        {
        aFilename = (aPath.Left(pos));
        
        if ( ( !EndsWith(aFilename, KScutMIFExtension ) ) &&
             ( !EndsWith(aFilename, KScutMBMExtension ) )   )
        	{
        	aFilename.Zero();
        	return EFalse;
        	}
        
        TLex lex(aPath.Mid(pos+1));
        TInt error = lex.Val(aId);
        if ( error != KErrNone )
        	{
        	aId = -1;
        	return EFalse;
        	}
        return ETrue;
        }
    return EFalse;
}     

TInt CreateChecksumFromString( const TDesC& aString )
    {
    TInt checksum = 0;
    
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        checksum += aString[i] * ( i + 1);
        }
    return checksum;       
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TAiScutParser::TAiScutParser() : iType(EScutUnknown), iUid(KNullUid)
{
}


// -----------------------------------------------------------------------------
// Static utility function to parse an uid from the given descriptor.
// -----------------------------------------------------------------------------
//
TUid TAiScutParser::ParseUid(const TDesC& aDesC)
{
    TRadix radix(EDecimal);

    // Check if the number is in hexadecimal format.
    _LIT(KHexPrefix, "0x");
    const TInt prefixLen = 2;
    TPtrC ptr(aDesC);

    if (ptr.Left(prefixLen).CompareC(KHexPrefix) == 0)
    {
        // Strip the '0x' prefix.
        ptr.Set(ptr.Right(ptr.Length() - prefixLen));

        radix = EHex;
    }

    // Do the actual parsing.
    TUint uint;
    TUid uid(KNullUid);
    TLex lexer(ptr);
    TInt err = lexer.Val(uint, radix);
    if (err == KErrNone)
    {
        uid.iUid = uint;
    }

    return uid;
}


// ---------------------------------------------------------------------------
// Parses a shortcut definition.
// ---------------------------------------------------------------------------
//
TInt TAiScutParser::Parse(const TDesC& aDefinition)
{
    iType = EScutUnknown;
    iDefinition.Set(aDefinition);
    
    // Default values for the icon
    iIcon.iIconId = KErrNotFound;
    iIcon.iPath.Zero();
    iIcon.iSkinId.iMajor = -1;
    iIcon.iSkinId.iMinor = -1;
    iIcon.iColourGroup = -1;
    
    iIcon.iType = EScutIconNone;
    iIcon.iDestination = EScutDestinationNormal;
    iIcon.iShortcutType = EScutUnknown;
    iIcon.iAppUid = TUid::Uid(0);
    iIcon.iViewId = TUid::Uid(0);
    
    TInt err = iUriParser.Parse(aDefinition);
    if (err != KErrNone)
    {
        return err;
    }

    TPtrC scheme(iUriParser.Extract(EUriScheme));

    if (scheme.Length() == 0 ||
        scheme.Compare(KScutURISchemeHttp) == 0 ||
        scheme.Compare(KScutURISchemeHttps) == 0)
    {
        ParseParams();
        iType = EScutWebAddress;
        
        if ( iIcon.iType != EScutIconNone )
            {
            // create a checksum for unique identifying
            TInt checksum = CreateChecksumFromString( aDefinition );
            iIcon.iViewId = TUid::Uid( checksum );
            iIcon.iShortcutType = iType;
            iIcon.iAppUid = KScutBrowserUid;           
            }
        err = KErrNone;
    }
    else if (scheme.Compare(KScutURISchemeLocalApp) == 0)
    {
        iType = EScutApplication;

        if (!ParseAlias())
        {
            iUid = ParseUid(iUriParser.Extract(EUriPath));
        }

        if (iUid == KScutAppShellUid)
        {
            // appshell shortcut is always of type app view.
            iType = EScutApplicationView;
        }

        // ParseParams() parses params from an URL. If it encounters
        // iconid, iconmifpath or cba parameter from the URL, then it
        // places the values to iIcon and removes the parameters from
        // the URL. It also extract the additional viewid if needed.
        // For example bookmarks and apps with views use them. 

        ParseParams();
        // Icon found so apply the appuid and type to icon. 
        if ( iIcon.iType != EScutIconNone )
            {
            iIcon.iShortcutType = iType;
            
            // If we are dealing with messaging icons then the 
            // appuid needs to be changed in order to match it against
            // the shortcuts appuid
            switch( iType )
                { 
                case EScutNewMessage:
                    iIcon.iAppUid.iUid = KScutUnifiedEditorUidValue;
                    break;

                case EScutNewEmail:
                    iIcon.iAppUid.iUid = KScutEmailEditorUidValue;
                    break;

            #ifdef __SYNCML_DS_EMAIL
                case EScutNewSyncMLMail:
                    iIcon.iAppUid.iUid = KScutEmailEditorUidValue; 
                    iIcon.iViewId.iUid = KScutSyncMlEmailUidValue;
                    break;
            #endif

                case EScutNewPostcard:
                    iIcon.iAppUid.iUid = KScutPostcardEditorUidValue;
                    break;

                case EScutNewAudioMsg:
                    iIcon.iAppUid.iUid = KScutAmsEditorUidValue;
                    break;

                case EScutNewMsgType:
                    iIcon.iAppUid.iUid = KScutMessagingCenterUidValue;
                    iIcon.iViewId.iUid = KScutMessagingCenterUidValue;
                    break;
                    
                default:
                    iIcon.iAppUid = iUid;       
                    break;
               
                }
                 
            }
        err = KErrNone;
    }
    else
    {
        err = KErrCorrupt;
    }

	__PRINTS( "XAI: TAiScutParser::Parse");
	__PRINT( __DBG_FORMAT( "XAI:   type = %d, definition = '%S', err = %d"), iType, &aDefinition, err);
    return err;
}


// ---------------------------------------------------------------------------
// Checks if the shortcut definition was valid.
// ---------------------------------------------------------------------------
//
TBool TAiScutParser::IsValid() const
{
    return iType != EScutUnknown;
}


// -----------------------------------------------------------------------------
// Returns the shortcut target type.
// -----------------------------------------------------------------------------
//
TShortcutType TAiScutParser::Type() const
{
    return iType;
}


// -----------------------------------------------------------------------------
// Returns the shortcut target uid. Used for application shortcuts.
// -----------------------------------------------------------------------------
//
TUid TAiScutParser::Uid() const
{
    return iUid;
}


TAiScutIcon TAiScutParser::Icon() const
    {
    return iIcon;
    }
// -----------------------------------------------------------------------------
// Returns a shortcut definition component value.
// -----------------------------------------------------------------------------
//
TPtrC TAiScutParser::Get(TScutDefComponent aComponent) const
{
    TPtrC componentValue;

    switch (aComponent)
    {
    case EScutDefScheme:
        componentValue.Set(iUriParser.Extract(EUriScheme));
        break;

    case EScutDefTarget:
        componentValue.Set(iUriParser.Extract(EUriPath));
        break;

    case EScutDefParamName:
        componentValue.Set(iParamName);
        break;

    case EScutDefParamValue:
        componentValue.Set(iParamValue);
        break;

    case EScutDefParamNameAndValue:
        componentValue.Set(iUriParser.Extract(EUriQuery));
        break;

    case EScutDefComplete:
        componentValue.Set(iDefinition);
        break;

    default:
        break;
    }

    return componentValue;
}


// ---------------------------------------------------------------------------
// Composes a shortcut definition string from given parameters.
// ---------------------------------------------------------------------------
//
void TAiScutParser::ComposeL(HBufC*& aDes, const TUid aUid,
    const TDesC& aParamName, const TDesC& aParamValue)
{
    HBufC* temp = HBufC::NewLC(KMaxDefinitionLength);
    TPtr ptr = temp->Des();

    if (aParamName.Length() && aParamValue.Length())
    {
        ptr.Format(KScutFormatApplicationWithParams, aUid.iUid, &aParamName, &aParamValue);
    }
    else
    {
        ptr.Format(KScutFormatApplication, aUid.iUid);
    }

    aDes = temp->AllocL();
    CleanupStack::PopAndDestroy(temp);
}


// ---------------------------------------------------------------------------
// Composes a shortcut definition string from given parameters.
// ---------------------------------------------------------------------------
//
void TAiScutParser::ComposeL(HBufC*& aDes, const TUid aUid,
    const TDesC& aParamString)
{
    HBufC* temp = HBufC::NewLC(KMaxDefinitionLength);
    TPtr ptr = temp->Des();

    if (aParamString.Length())
    {
        ptr.Format(KScutFormatApplicationWithParamString, aUid.iUid, &aParamString);
    }
    else
    {
        ptr.Format(KScutFormatApplication, aUid.iUid);
    }

    aDes = temp->AllocL();
    CleanupStack::PopAndDestroy(temp);
}


// ---------------------------------------------------------------------------
// Checks if an alias was used in shortcut definition and parses an uid from it.
// ---------------------------------------------------------------------------
//
TBool TAiScutParser::ParseAlias()
{
    TPtrC ptr(iUriParser.Extract(EUriPath));

    // "localapp:msg?..." is an alias for messaging application.
    if (ptr.CompareC(KScutTargetAliasMessaging) == 0)
    {
        iUid = KScutMessagingUid;
        return ETrue;
    }
    // "localapp:keylock?..." is an alias for keylock
    else if (ptr.CompareC(KScutTargetAliasKeylock) == 0)
    {
        iUid = KScutKeyLockUid;
        return ETrue;
    }

    // "localapp:voicedial..." is an alias for voicedial
    else if (ptr.CompareC(KScutTargetAliasVoiceDial) == 0)
    {
        iUid = KScutVoiceDialUid;
        return ETrue;
    }

    // "localapp:logs?..." is an alias for logs
    else if (ptr.CompareC(KScutTargetAliasLogs) == 0)
    {
        iUid = KScutLogsUid;
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}


// ---------------------------------------------------------------------------
// Parses the possible application shortcut parameters.
// ---------------------------------------------------------------------------
//
void TAiScutParser::ParseParams()
{
    TPtrC params(iUriParser.Extract(EUriQuery));
    
    if (params.Length() > 0)
    {        
        HBufC *tempParams = params.Alloc();
        // value can't be longer than the params
        // but in some cases it can be equally long
        HBufC *value = HBufC::New(params.Length());
        
        // low memory or similar situation so cannot do anything
        if ( !value || !tempParams )
            {
            return;
            }
        
        TPtr valuePtr = value->Des();
        TPtr tempParamsPtr = tempParams->Des();
        
        TBool addonFound = EFalse;
        TInt err = KErrNone;
        
        // First extract the CBA
        err = ExtractValueFromQueryString(tempParamsPtr,KScutParamNameCBAIcon,valuePtr, ETrue);               
        if ( err == KErrNone )
            {
            iIcon.iDestination = EScutDestinationSoftkey;
            }
        // Then the toolbar

        err = ExtractValueFromQueryString(tempParamsPtr,KScutParamNameToolbarIcon,valuePtr, ETrue);               

        if ( err == KErrNone )
            {
            iIcon.iDestination = EScutDestinationToolbar;
            }
        
        // then extract the iconskinid
        err = ExtractValueFromQueryString(tempParamsPtr,
                KScutParamNameIconSkinId,valuePtr, ETrue);
        if ( err == KErrNone &&
                ResolveSkinItemId(valuePtr,iIcon.iSkinId,iIcon.iColourGroup))
            {
            iIcon.iType = EScutIconSkin;
            addonFound = ETrue;
            }
        // Then extract the iconmifpath
        // Iconmifpath extraction left here for backward compatibility
        valuePtr.Zero();
        err = ExtractValueFromQueryString(tempParamsPtr,
                KScutParamNameIconMifPath,valuePtr, ETrue);
        if ( err == KErrNone &&
                ResolveFileIdAndPath(valuePtr,iIcon.iIconId,iIcon.iPath) )
            {
            iIcon.iType = EScutIconMif;
            addonFound = ETrue;
            }        

        // Then extract the iconpath.
        valuePtr.Zero();
        err = ExtractValueFromQueryString(tempParamsPtr,
                KScutParamNameIconPath,valuePtr, ETrue);
        if ( err == KErrNone &&
                ResolveFileIdAndPath(valuePtr,iIcon.iIconId,iIcon.iPath) )
            {
            if ( EndsWith(iIcon.iPath, KScutMIFExtension ))
                {
                iIcon.iType = EScutIconMif;                
                }
            else if ( EndsWith(iIcon.iPath, KScutMBMExtension ))
                {
                iIcon.iType = EScutIconMbm;
                } 
            addonFound = ETrue;
            } 
                   
        // Use the new params string where the addons 
        // have been removed
        if( addonFound )
            {
            params.Set(tempParamsPtr);   
            // no need to process anything because there are no
            // parameters left after our addons have been taken out
            if ( params.Length() <= 0)
                {
                delete value;
                delete tempParams;
                return;
                }
            }
        
        delete value;
        
        iType = EScutApplicationWithParams;
        
        const TInt valueSeparatorPos = params.Locate(KParamValueSeparator);

        if (valueSeparatorPos >= 0)
        {
            iParamName.Set(params.Left(valueSeparatorPos));
        }
        if (valueSeparatorPos >= 0)
        {
            iParamValue.Set(params.Mid(valueSeparatorPos + 1));
        }
        if (valueSeparatorPos == -1)
        {
            iParamName.Set(params);
        }

        if (iParamName.CompareC(KScutParamNameView) == 0)
        {
            iType = EScutApplicationView;

            if (iUid == KScutPersonalisationUid)
            {
                TUid uid = ParseUid(iParamValue);
                if (uid == KScutChangeThemeViewId)
                {
                    iType = EScutChangeTheme;
                }
                iIcon.iViewId = uid;
            }

            if (iUid == KScutLogsUid)
            {
                if (iParamValue.CompareC(KScutParamValueMissedCalls) == 0)
                {
                    iType = EScutLogsMissedCallsView;
                }
                else if (iParamValue.CompareC(KScutParamValueDialledCalls) == 0)
                {
                    iType = EScutLogsDialledCallsView;
                }
                else if (iParamValue.CompareC(KScutParamValueReceivedCalls) == 0)
                {
                    iType = EScutLogsReceivedCallsView;
                }
                else if (iParamValue.CompareC(KScutParamValueMainView) == 0)
                {
                    iType = EScutLogsMainView;
                }
            }

            if (iUid == KScutGeneralSettingsUid)
            {
                if (ParseUid(iParamValue) == KScutParamValueConnectivityView)
                {
                    iType = EScutConnectivityStatusView;
                }
                else if (ParseUid(iParamValue) == KScutInstallationViewId)
                {
                    iType = EScutApplicationManagerView;
                }
            }
            
            
        }
        else if (iUid == KScutMessagingUid)
        {
            if (iParamName.CompareC(KScutParamNameNew) == 0)
            {
                if (iParamValue.CompareC(KScutParamValueMsg) == 0)
                {
                    iType = EScutNewMessage;
                }
                else if (iParamValue.CompareC(KScutParamValueEmail) == 0)
                {
                    iType = EScutNewEmail;
                }
#ifdef __SYNCML_DS_EMAIL
                else if (iParamValue.CompareC(KScutParamValueSyncMLMail) == 0)
                {
                    iType = EScutNewSyncMLMail;
                }
#endif
                else if (iParamValue.CompareC(KScutParamValuePostcard) == 0)
                {
                    iType = EScutNewPostcard;
                }
                else if (iParamValue.CompareC(KScutParamValueAudioMsg) == 0)
                {
                    iType = EScutNewAudioMsg;
                }
                else
                {
                    iType = EScutNewMsgType;
                }
            }
            else if (iParamName.CompareC(KScutParamNameMailbox) == 0)
            {
                iType = EScutMailbox;
            }
        }
        else if (iUid == KScutKeyLockUid)
        {
            iType = EScutKeylock;
        }
        else if (iUid == KScutSettingsDllUid || iUid == KScutBrowserUid)
        {
            if (iParamName.CompareC(KScutParamNameBookmark) == 0)
            {
                iType = EScutBookmark;
                iIcon.iViewId = ParseUid(iParamValue);
            }
            else if (iParamName.CompareC(KScutParamNoEffect) == 0)
            {
                iType = EScutNoEffect;
            }
        }
        delete tempParams;
    }
}

TInt TAiScutParser::ChecksumForString( const TDesC& aDefinition) const 
    {
    return CreateChecksumFromString( aDefinition );
    }

TInt TAiScutParser::CustomTitle( TDes& aTarget ) const
    {
    TPtrC params(iUriParser.Extract(EUriQuery));
    HBufC *tempParams = params.Alloc();
    if ( !tempParams )
        {
        return KErrNoMemory;
        }
        
    TPtr tempParamsPtr = tempParams->Des();
    
    TInt err = ExtractValueFromQueryString(tempParamsPtr,
            KScutParamNameCustomTitle, aTarget, EFalse);
    
    delete tempParams;
    return err;
    }

void TAiScutParser::RemoveExtraDefinitionsL( TDes &aString ) const
    {
    HBufC *temp = HBufC::NewL( aString.Length( ));
    TPtr tempPtr = temp->Des();
    ExtractValueFromQueryString(aString,
                    KScutParamNameCBAIcon, tempPtr, ETrue);
    
    ExtractValueFromQueryString(aString,
                KScutParamNameIconSkinId,tempPtr, ETrue);
    
    ExtractValueFromQueryString(aString,
                KScutParamNameIconMifPath,tempPtr, ETrue);
    
    ExtractValueFromQueryString(aString,
                KScutParamNameCustomTitle, tempPtr, ETrue);
    ExtractValueFromQueryString(aString,
                KScutParamNameIconPath, tempPtr, ETrue);
    delete temp;
    }

// End of File.
