CD .\OBJ

set AR51=C:\Keil\C51\BIN\LIB51
set LIB_PATH=..\..\libsource\lib

%AR51% TRANSFER Host_Init.obj TO %LIB_PATH%\Host_Init.lib
%AR51% TRANSFER Host_Command_Scsi.obj TO %LIB_PATH%\Host_Command_Scsi.lib
%AR51% TRANSFER Host_PHY.obj TO %LIB_PATH%\Host_PHY.lib

%AR51% TRANSFER SD_HW.obj TO %LIB_PATH%\SD_HW.lib
%AR51% TRANSFER SD_CMD.obj TO %LIB_PATH%\SD_CMD.lib

%AR51% TRANSFER DSPphysic.obj TO %LIB_PATH%\DSPphysic.lib
%AR51% TRANSFER DSPuser.obj TO %LIB_PATH%\DSPuser.lib

%AR51% TRANSFER DOSinit.obj TO %LIB_PATH%\DOSinit.lib
%AR51% TRANSFER DOSfdb.obj TO %LIB_PATH%\DOSfdb.lib
%AR51% TRANSFER DOSfile.obj TO %LIB_PATH%\DOSfile.lib
%AR51% TRANSFER DOSComm.obj TO %LIB_PATH%\DOSComm.lib
%AR51% TRANSFER DOS_fileplay.obj TO %LIB_PATH%\DOS_fileplay.lib
CD ..
