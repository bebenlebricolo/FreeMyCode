SET ExecutableDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\build\Sources\Utils\BinaryTesters\Debug
SET LicensesFileDir=D:\Dossier Perso\Programmation\Visual C++\FreeMyCode\Ressources\Licenses files
SET LicensesFile1=%LicensesFileDir%\GNU GPL V2.txt
SET LicensesFile2=%LicensesFileDir%\GNU GPL V3.txt

"%ExecutableDir%\testLicenseChecker.exe" -b "%LicensesFile1%" "%LicensesFile2%"
PAUSE