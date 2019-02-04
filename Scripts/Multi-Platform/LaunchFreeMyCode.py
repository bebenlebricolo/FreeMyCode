#!/usr/bin/python

# Handle Python versions (Compatibility )
import sys
import os
import subprocess

import FreeMyCode_python_utils.system_utils as su
import FreeMyCode_python_utils.path_utils as pu
import FreeMyCode_python_utils.logging_tools as log


pythonVersion = sys.version_info
major = pythonVersion[0]
minor = pythonVersion[1]

# Required is 3.4
reqMajor = 3
reqMinor = 4

DEBUG = 1
POSIX_sep = '/'
WIN_sep = '\\'
sepList = "{}{}".format(POSIX_sep,WIN_sep)

# Handle Python versions
if(major >= reqMajor):
    needNewVersion = False
    if(minor >= reqMinor):
        from pathlib import Path
    else:
        needNewVersion = True
else:
    needNewVersion = True

if(needNewVersion):
    log.logError("Your version of Python is too old. Please update your version of Python before launching this script again")
    log.logError("Python version should be > %d.%d "% (reqMajor, reqMinor) )
    sys.exit(1)



def showUsage() :
    print("---------- USAGE ---------")
    print("This tool is used as an abstraction layer for FreeMyCode user\n")
    print("Usage example : $ {} python LaunchFreeMyCode.py <InstallationFolder> <TargetedDirectory> <RessourcesDirectory>{}\n".format(su.CLI_SuccessTag,su.CLI_NormalTag))
    print(" Where : ")
    print("   InstallationFolder stands for the FreeMyCode installation folder ")
    print("   TargetedDirectory is the project's directory to be analysed")
    print("   RessourcesDirectory is the directory where the user may store ressources files\n")
    print(" Note : ")
    print("   Installation folder may contain : ")
    print("   * bin/ folder ")
    print("   * Config.json file ")
    print("   * Ressources/ folder ")
    print("        * Ressources/Spectrums folder ")
    print("        * Ressources/AvailableLicenses folder ")
    print("   RessourcesDirectory folder may contain : ")
    print("        * SecondaryInput.json file")
    print("        * License.txt file -> License to be used throughout the project")
    print("             -> Note 2 : License.txt should have EXACTLY that name for the program to start.")


def handleArgs() :
    foundErrors = False
    argList = list()
    if len(sys.argv) != 4 :
        log.logError("Args count is too small, please check your input")
        foundErrors = True
    else:
        for i in range(1,len(sys.argv)) :
            arg = pu.handleRelativePath(sys.argv[i])
            if not os.path.exists(arg) :
                log.logError("Current arg does not point to anything : no such file or directory ( {} )".format(arg))
                foundErrors = True
            elif foundErrors == False :
                argList.append(arg)
    if foundErrors == True :
        showUsage()
        sys.exit(1)

    log.logInfo("Successfully parsed input Arguments !")
    return argList

# Dipslays a list of args on logInfo output
def displayArgs(argList) :
    if DEBUG == 1:
        counter = 0
        for arg in argList :
            log.logInfo("Arg[{}] = {}".format(counter,arg))
            counter +=1

def displayCommandLine(argList) :
    print("Full Command Line is : ")
    for arg in argList :
        print("\"{}\"".format(arg), end=" ")
    print("")

################################## Main function's body ####################################

def main() :
    argList = handleArgs()

    # Args from user input
    installationDir = argList[0]
    targetedDirectory = argList[1]
    userRessourcesDir = argList[2]

    # Dynamically retrieve the other ones
    freeMyCodeExecutable = pu.find (su.FreeMyCode_exeName , installationDir )
    spectrumsDir = os.path.join(installationDir , "Ressources/Spectrums")
    spectrumsDir = pu.convertToStandard(spectrumsDir , su.localeSep)
    configFile = pu.find ('Config.json' , installationDir)
    secondaryInputFile = pu.find( 'Secondary_input.json' , userRessourcesDir)
    licenseFile = pu.find ('License.txt',userRessourcesDir)
    logFile = os.path.join(userRessourcesDir, 'logfile.log')

    # Check file paths for any error (mispelling, etc.)
    pu.checkArgs({'FreeMyCode_executable':freeMyCodeExecutable ,
               'Targeted_directory' : targetedDirectory ,
               'Configuration_file' : configFile ,
               'Secondary_input_file' : secondaryInputFile ,
               'License_file' : licenseFile ,
               'Logging_file' : logFile})


    # Check for all available flags at the end of this file :)
    args = [freeMyCodeExecutable ,
            targetedDirectory ,
            "-A",
            licenseFile ,
            "-p",
            "-c",
            configFile ,
            "-L",
            logFile ,
            "-Si",
            "-sr",
            secondaryInputFile,
            "-Sd",
            spectrumsDir]

    displayArgs(args)
    displayCommandLine(args)
    subprocess.call(args)
    return 0


# Script execution
main ()


#### AVAILABLE FLAGS:
#
# DIRECTORY Specifics
# -A : analyse directory

# LICENSE Specifics
# -a   : append to file
# -p   : prepend to file
# -lf  : log formatted license (DEBUG option)
# -fsl : force single line : force single line comments if possible while writing license to files

# CONFIG Specifics:
# -c : positional flag ->  -c < Config file location >  : triggers the config file parsing from commandl line parser

# LOGFILES Specifics
# -v   : verbose
# -sr  : show parsing results (DEBUG option)

# SECONDARY INPUT FILE Specifics
# -Si : positional flag ->   -Si < Secondary input file location > : triggers Secondary input file parsing

# SPECTRUMS DIRECTORY
# -Sd : positional flag ->   -Sd < Spectrums directory> : allows to use licenses while parsing
# -lg : log block comments while parsing source files