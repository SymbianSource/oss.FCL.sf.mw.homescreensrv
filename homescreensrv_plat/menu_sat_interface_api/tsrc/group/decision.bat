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

call cd ../../../../contentstorage/casatinterface/group

call del mon.sym
call del mon.dat
call rmdir /s /q CTCHTML

call bldmake bldfiles 
call abld makefile winscw udeb
call abld reallyclean winscw udeb
call ctcwrap -i fd -C "EXCLUDE=*" -C "NO_EXCLUDE+../*" abld build winscw udeb

call cd ../../../homescreensrv_plat/menu_sat_interface_api/tsrc/group/
call bldmake bldfiles 
call abld makefile winscw udeb
call abld reallyclean winscw udeb
call abld test build winscw udeb