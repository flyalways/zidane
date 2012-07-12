@C:\Progra~1\WinRAR\WinRAR.exe a -pG1E2N3E4R5A6L7P8L9U0S ..\Sample.hzk  release.bat make_lib.bat SPDA23*.* host\*.* ..\libsource\SD\*.* ..\libsource\DOS\*.* ..\libsource\FLASH\*.* ..\libsource\DSP\*.*

@del host\*.c  /Q
@del SPDA23*.*	/Q
@del ..\libsource\SD\*.c /Q
@del ..\libsource\DOS\DOS*.c /Q
@del ..\libsource\DOS\FREE*.c /Q
@del ..\libsource\FLASH\*.c /Q
@del ..\libsource\DSP\*.c	/Q
@del make_lib.bat	/Q
@del release.bat /Q
:END