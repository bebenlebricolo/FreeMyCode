@echo off
REM FreeMyCode_cmd.exe directory -A License Config Logfile -sr
REM FreeMyCode_cmd.exe directory -A License -a -c Config -L Logfile -v 
SET RootDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\Sources\Debug
SET Directory=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Tests utils\Dummy_Directory
SET FreeMyCode_executable=%RootDir%\FreeMyCode.exe
REM ECHO "%Directory%"
SET License=%Directory%\License.txt
REM ECHO "%License%"
SET Config=%Directory%\Config.json
REM ECHO "%Config%"
SET Logfile=%Directory%\logfile.log
REM ECHO "%Logfile%"
SET SecondaryInputFile=%Directory%\Secondary_input.json

@echo on

 "%FreeMyCode_executable%" "%Directory%" -A "%License%" -a -p -lf -fsl -c "%Config%" -L "%Logfile%" -v -sr -Si "%SecondaryInputFile%"
 