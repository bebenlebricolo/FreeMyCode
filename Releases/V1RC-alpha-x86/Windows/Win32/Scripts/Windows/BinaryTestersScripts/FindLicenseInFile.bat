SET FreeMyCodeDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode

SET TestExecutable=%FreeMyCodeDir%\build\bin\Libtesters\Debug\testLicenseChecker.exe
SET ConfigFile=%FreeMyCodeDir%\Scripts\Dummy_Directory\Config.json
SET FILE1=%FreeMyCodeDir%\Scripts\Dummy_Directory\toto.h
SET License1=%FreeMyCodeDir%\Ressources\Spectrums\GNU GPL V2.spec
SET License2=%FreeMyCodeDir%\Ressources\Spectrums\GNU GPL V3.spec
SET License3=%FreeMyCodeDir%\Ressources\Spectrums\DummyLicense.spec

"%TestExecutable%" -c "%ConfigFile%" "%FILE1%" "%License1%" "%License2%" "%License3%"
PAUSE