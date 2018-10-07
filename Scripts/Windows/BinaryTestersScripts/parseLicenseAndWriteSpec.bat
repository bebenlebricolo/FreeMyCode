SET ExecutableDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\bin\LibTesters\Debug
SET RessourcesDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources
SET LicensesDir=%RessourcesDir%\AvailableLicenses
SET License1=%LicensesDir%\GNU GPL V2.txt
SET License2=%LicensesDir%\GNU GPL V3.txt
SET License3=%LicensesDir%\DummyLicense.txt



SET SpectrumOutDir=%RessourcesDir%\Spectrums_generated

"%ExecutableDir%\testLicenseChecker.exe" -w "%License1%" "%License2%" "%License3%" "%SpectrumOutDir%"
PAUSE