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

if exist menusatinterfacetest.sisx del menusatinterfacetest.sisx

makesis menusatinterfacetest.pkg
signsis menusatinterfacetest.sis menusatinterfacetest.sisx rd.cer rd-key.pem


if exist menusatinterfacetest.sisx (
echo menusatinterfacetest.sisx creation SUCCEEDED
del menusatinterfacetest.sis
)

if not exist menusatinterfacetest.sisx (
echo menusatinterfacetest.sisx creation FAILED
)