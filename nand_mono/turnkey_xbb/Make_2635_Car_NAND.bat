CD .\OBJ

..\..\EXE\HEX2TSK SPDA23xx.H00  SUNMM00.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H01  SUNMM01.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H02  SUNMM02.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H03  SUNMM03.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H04  SUNMM04.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H05  SUNMM05.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H06  SUNMM06.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H07  SUNMM07.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H08  SUNMM08.BIN /O
..\..\EXE\HEX2TSK SPDA23xx.H09  SUNMM09.BIN /O

..\..\EXE\bm.exe -f ..\bm_2635_Car_NAND.ini
@if errorlevel 1 goto bmerror

CD ..
@if exist make_lib.bat make_lib.bat
@goto end

:bmerror
@echo Build fails!!!
@goto end

:end

