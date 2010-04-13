/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Ids for private plug-in providers.
*
*/



#ifndef GSPRIVATEPROVIDERIDS_H
#define GSPRIVATEPROVIDERIDS_H

// CONSTANTS

/**
* Private id used for S60 internally provided GS plug-ins. This must not be 
* disclosed to other parties. Using this id allows the plugin to be handled 
* with more rights than other plugins:
*   - Inserted before other plugins in listboxes.
*   - Can define their order in the listbox.
*   - Plug-in can belong also to other views than applications-view.
*
*/
const TInt KGSPluginProviderInternal = 87329234;


#endif // GSPRIVATEPROVIDERIDS_H
// End of File
