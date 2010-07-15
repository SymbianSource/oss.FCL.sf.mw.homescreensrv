@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
@rem 
@rem Description:
@rem 
@rem 



@ECHO OFF
setlocal

ECHO ********** Clean the environment **********

call sbs -c winscw_udeb reallyclean -k
call del profile.txt
call del mon.sym
call del mon.dat

ECHO ********** Build module tests and Instrument test subjects **********

call ctcwrap -i f -2comp sbs -c winscw_udeb

ECHO Start the emulator and run your tests now. Call "generate.cmd" when you're done.
ECHO NOTE! Close the emulator before calling generate.cmd

