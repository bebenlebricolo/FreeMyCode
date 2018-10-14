SET ExecutableDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\bin\LibTesters\Debug
SET SpectrumFilesDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources\Spectrums
REM SET SpectrumFile2=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources\Spectrums\Test_Wrong.spec

"%ExecutableDir%\testLicenseChecker.exe" -p "%SpectrumFilesDir%" "%SpectrumFile2%"
PAUSE