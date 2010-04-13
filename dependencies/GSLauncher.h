/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Launches GS views.
*
*/




#ifndef C_GSLAUNCHER_H
#define C_GSLAUNCHER_H

#include <e32std.h>
#include <e32def.h>
#include <apgtask.h>



class CGSLauncherExtension;
/**
 *  Launches general settings with specific plug-in opened.
 *  Using this API, a client may request a general settings view to be shown.
 *  Implementation will launch general settings if it is not yet running. 
 *
 *  Example usage of the interface:
 *  @code
 *   CGSLauncher* l = CGSLauncher::NewLC();
 *   l->LaunchGSViewL ( yourPluginUid, TUid::Uid(0), KNullDesC8 ); 
 *   CleanupStack::PopAndDestroy(l);
 *  @endcode
 *
 *  @lib gslauncher.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS(CGSLauncher) : public CBase
    {

public:

    /**
     * Two-phased constructors.
     */
    IMPORT_C static CGSLauncher* NewL();

    IMPORT_C static CGSLauncher* NewLC();
    

    /**
    * Destructor.
    */
    virtual ~CGSLauncher();

    /**
     * Launches general settings with specific plug-in activated.
     * Launching of plug-in's local views is not supported. 
     * General settings application is started if it's not running. 
     * Any application embedded into general settings will be closed.
     *
     * Needs capability: SwEvent
     * 
     * @since S60 5.0
     * @param aTarget 					Target plug-in's view id. (CAknView::Id())
     * @param aCustomMessageId  For client data. Passed to the plug-in.
     * @param aCustomMessage  	For client data. Passed to the plug-in.
     * @return none
     */
	IMPORT_C void LaunchGSViewL( const TUid aTarget,const TUid aCustomMessageId
			,const TDesC8& aCustomMessage );


protected:

private:

    CGSLauncher();
    
    void ConstructL();

private: // data
		CGSLauncherExtension* iExt;
    };

#endif // C_GSLAUNCHER_H
