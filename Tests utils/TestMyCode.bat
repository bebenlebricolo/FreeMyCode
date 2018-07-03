@echo off
REM FreeMyCode_cmd.exe directory -A License Config Logfile -sr
REM FreeMyCode_cmd.exe directory -A License -a -c Config -L Logfile -v 
SET RootDir = D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Buildx64\Debug
SET Directory=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Buildx64\Debug\Dummy_Directory
REM ECHO "%Directory%"
SET License=%Directory%\License.txt
REM ECHO "%License%"
SET Config=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\FreeMyCode\Sources\Config.json
REM ECHO "%Config%"
SET Logfile=%Directory%\logfile.log
REM ECHO "%Logfile%"
SET SecondaryInputFile=%Directory%\Secondary_input.json
@echo on

 FreeMyCode.exe "%Directory%" -A "%License%" -a -p -lf -fsl -c "%Config%" -L "%Logfile%" -v -sr -Si "%SecondaryInputFile%"
