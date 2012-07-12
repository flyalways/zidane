CD .\OBJ

set AR51=C:\Keil\C51\BIN\LIB51
set LIB_PATH=..\..\libsource\lib

%AR51% TRANSFER Flash.obj TO %LIB_PATH%\Flash.lib
%AR51% TRANSFER Flash_COMM.obj TO %LIB_PATH%\Flash_COMM.lib
%AR51% TRANSFER Flash_Init.obj TO %LIB_PATH%\Flash_Init.lib
%AR51% TRANSFER Flash_Read.obj TO %LIB_PATH%\Flash_Read.lib
%AR51% TRANSFER FlshScan.obj TO %LIB_PATH%\FlshScan.lib
%AR51% TRANSFER Flash_EnHance.obj TO %LIB_PATH%\Flash_EnHance.lib

%AR51% TRANSFER Host_Init.obj TO %LIB_PATH%\Host_Init.lib
%AR51% TRANSFER Host_Command_Scsi.obj TO %LIB_PATH%\Host_Command_Scsi.lib
%AR51% TRANSFER Host_PHY.obj TO %LIB_PATH%\Host_PHY.lib

%AR51% TRANSFER SD_HW.obj TO %LIB_PATH%\SD_HW.lib
%AR51% TRANSFER SD_CMD.obj TO %LIB_PATH%\SD_CMD.lib

%AR51% TRANSFER DSPphysic.obj TO %LIB_PATH%\DSPphysic.lib
%AR51% TRANSFER DSPuser.obj TO %LIB_PATH%\DSPuser.lib

%AR51% TRANSFER DOS.obj TO %LIB_PATH%\DOS.lib
%AR51% TRANSFER DOSinit.obj TO %LIB_PATH%\DOSinit.lib
%AR51% TRANSFER DOSfdb.obj TO %LIB_PATH%\DOSfdb.lib
%AR51% TRANSFER DOSfile.obj TO %LIB_PATH%\DOSfile.lib
%AR51% TRANSFER DOSFAT.obj TO %LIB_PATH%\DOSFAT.lib
%AR51% TRANSFER DOSComm.obj TO %LIB_PATH%\DOSComm.lib
%AR51% TRANSFER DOS_fileplay.obj TO %LIB_PATH%\DOS_fileplay.lib
%AR51% TRANSFER Free_Cluster.obj TO %LIB_PATH%\Free_Cluster.lib
CD ..
