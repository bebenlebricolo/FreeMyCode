#!/bin/bash

RepoPath=/home/ife/bebenlebricoloRepos/FreeMyCode
testLicenseChecker=${RepoPath}/build/bin/LibTesters/testLicenseChecker

TargetDirPath=${RepoPath}/Scripts/Dummy_Directory
ConfigFilePath=${TargetDirPath}/Config.json
FILE1=${TargetDirPath}/toto.h

${testLicenseChecker} -c ${ConfigFilePath} ${FILE1}
