FreeMyCodeDir="/home/ife/bebenlebricoloRepos/FreeMyCode"
TestExecutable="${FreeMyCodeDir}/build/bin/LibTesters/testLicenseChecker"
ConfigFile="${FreeMyCodeDir}/Scripts/Dummy_Directory/Config.json"
FILE1="${FreeMyCodeDir}/Scripts/Dummy_Directory/toto.h"
License1="${FreeMyCodeDir}/Ressources/Spectrums/GNU GPL V2.spec"
License2="${FreeMyCodeDir}/Ressources/Spectrums/GNU GPL V3.spec"
License3="${FreeMyCodeDir}/Ressources/Spectrums/DummyLicense.spec"

${TestExecutable} -c ${ConfigFile} ${FILE1} ${License1} ${License2} ${License3}
