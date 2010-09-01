@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@ECHO OFF
SETLOCAL

SET PATH_LINT=C:\Apps\libxml2-2.6.23.win32\bin\xmllint.exe
SET PATH_MANIFEST_XSD=.\common\hsps_manifest.xsd
SET PATH_DEFINITION_XSD=.\common\hsps_definition.xsd

IF NOT EXIST %PATH_LINT% GOTO ERR_LINT
IF NOT EXIST %PATH_MANIFEST_XSD% GOTO ERR_XSD
IF NOT EXIST %PATH_DEFINITION_XSD% GOTO ERR_XSD

SET ARGS_MANIFEST=--schema %PATH_MANIFEST_XSD% --noout
SET ARGS_DEFINITION=--schema %PATH_DEFINITION_XSD% --noout
CLS

SET FOLDER=MinimalConfiguration
echo.
echo Validating %FOLDER% against HSPS schema files...
echo.
%PATH_LINT% .\%FOLDER%\root\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\root\rootconfiguration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\view\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\view\viewconfiguration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\widget\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\widget\widgetconfiguration.xml %ARGS_DEFINITION%


SET FOLDER=TypicalConfiguration
echo.
echo.
echo Validating %FOLDER% against HSPS schema files...
echo.
%PATH_LINT% .\%FOLDER%\root\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\root\rootconfiguration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\view1\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\view1\viewconfiguration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\view2\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\view2\viewconfiguration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\widget\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\widget\widgetconfiguration.xml %ARGS_DEFINITION%


SET FOLDER=OperatorConfiguration
echo.
echo.
echo Validating %FOLDER% against HSPS schema files...
echo.
%PATH_LINT% .\%FOLDER%\root\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\root\configuration.xml %ARGS_DEFINITION%

%PATH_LINT% .\%FOLDER%\view\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\view\configuration.xml %ARGS_DEFINITION%


SET FOLDER=FinnishWidget
echo.
echo.
echo Validating %FOLDER% against HSPS schema files...
echo.
%PATH_LINT% .\%FOLDER%\widget\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\widget\widgetconfiguration.xml %ARGS_DEFINITION%


SET FOLDER=InstalledWidget
echo.
echo.
echo Validating %FOLDER% against HSPS schema files...
echo.
%PATH_LINT% .\%FOLDER%\manifest.dat %ARGS_MANIFEST%
%PATH_LINT% .\%FOLDER%\widgetconfiguration.xml %ARGS_DEFINITION%


GOTO END

:ERR_LINT
ECHO XMLLint was not found! Personlize the PATH_LINT variable in %0
GOTO END

:ERR_XSD
ECHO Schemas were not found at %PATH_MANIFEST_XSD% and %PATH_DEFINITION_XSD%!
GOTO END

:END

ENDLOCAL