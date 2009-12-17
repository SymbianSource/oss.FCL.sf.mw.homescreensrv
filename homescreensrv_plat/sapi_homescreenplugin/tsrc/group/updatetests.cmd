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
@echo off
rem  
rem For testing/debugging in emulator after the SISX package has been installed
rem
 
if exist \epoc32\winscw\c\sys\bin\mt_*.dll del \epoc32\winscw\c\sys\bin\mt_*.dll
if exist \epoc32\winscw\c\sys\hash\mt_*.dll del \epoc32\winscw\c\sys\hash\mt_*.dll
copy \epoc32\release\winscw\udeb\mt_*.dll \epoc32\release\winscw\udeb\z\sys\bin