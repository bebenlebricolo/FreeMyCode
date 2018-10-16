SET ExecutableDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\bin\LibTesters\Debug
SET RessourcesDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources
SET LicensesDir=%RessourcesDir%\AvailableLicenses



SET SpectrumOutDir=%RessourcesDir%\Spectrums_generated

"%ExecutableDir%\testLicenseChecker.exe" -w "%LicensesDir%" "%SpectrumOutDir%"
PAUSE