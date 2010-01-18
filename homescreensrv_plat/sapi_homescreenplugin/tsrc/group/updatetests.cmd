@echo off
rem  
rem For testing/debugging in emulator after the SISX package has been installed
rem
 
if exist \epoc32\winscw\c\sys\bin\mt_*.dll del \epoc32\winscw\c\sys\bin\mt_*.dll
if exist \epoc32\winscw\c\sys\hash\mt_*.dll del \epoc32\winscw\c\sys\hash\mt_*.dll
copy \epoc32\release\winscw\udeb\mt_*.dll \epoc32\release\winscw\udeb\z\sys\bin