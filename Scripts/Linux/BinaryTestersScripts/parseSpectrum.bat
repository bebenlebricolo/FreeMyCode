SET ExecutableDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\Sources\Utils\BinaryTesters\Debug
SET SpectrumFilesDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources\Spectrum files
SET SpectrumFile1=%SpectrumFilesDir%\Test.spec
SET SpectrumFile2=%SpectrumFilesDir%\Test_Wrong.spec

"%ExecutableDir%\testLicenseChecker.exe" -p "%SpectrumFile1%" "%SpectrumFile2%"
PAUSE