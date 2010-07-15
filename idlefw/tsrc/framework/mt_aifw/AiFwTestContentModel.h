/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AIFWTESTCONTENTMODEL_H
#define AIFWTESTCONTENTMODEL_H

//  INCLUDES
#include "aicontentmodel.h"

// CONSTANTS
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestPluginContent[] =
{
    { 1, L"Counter", "text/plain" },
    { 2, L"Status",  "text/plain" },
    { 3, L"Runtime",  "text/plain" }
};

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestPluginResources[] =
{
    { 1, L"Even", "text/plain" },
    { 2, L"Odd",  "text/plain" }
};

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestPluginEvents[] =
{
    { 1, L"StartTimer", "" },
    { 2, L"StopTimer",  "" },
    { 3, L"test_event",  "" }
};

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestControllerContent[] =
{
    { 1, L"Counter", "text/plain" },
    { 2, L"Status",  "text/plain" },
    { 3, L"Runtime",  "text/plain" },
    { 4, L"Downtime",  "text/plain" }
};

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestControllerResources[] =
{
    { 1, L"Even", "text/plain" },
    { 2, L"Odd",  "text/plain" }
};

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
const TAiContentItem KTestControllerEvents[] =
{
    { 1, L"StartTimer", "" },
    { 2, L"StopTimer",  "" }
};

#endif      // ?CLASSNAME_H

// End of File.
