#!/usr/bin/python

# Handle Python versions (Compatibility )
import sys
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
    logError("Your version of Python is too old. Please update your version of Python before launching this script again")
    logError("Python version should be > %d.%d "% (reqMajor, reqMinor) )
    sys.exit(1)

#####################################################""
# Start 'Real' script part
#####################################################""

import os
import platform
from pathlib import Path
import subprocess

platform = platform.system()

if(platform == 'Linux' ):
    FreeMyCode_exeName = 'FreeMyCode'
    CLI_ErrorTag = '\033[1;91m'
    CLI_SuccessTag = '\033[1;32m'
    CLI_NormalTag = '\033[0;m'
    localeSep = POSIX_sep
elif(platform == 'Windows'):
    FreeMyCode_exeName = 'FreeMyCode.exe'
    CLI_ErrorTag = ''
    CLI_SuccessTag = ''
    CLI_NormalTag = ''
    localeSep = WIN_sep

def logInfo(message) :
    # Debug stuff
    if(DEBUG == 1 ) :
        print (message)
    return 0

def logError(message):
    print("{}[ ERROR ] : {}{}".format(CLI_ErrorTag, message ,CLI_NormalTag))


# https://stackoverflow.com/questions/1724693/find-a-file-in-python
# Outputs 'None' if given filename is not found
def find(name, path):
    output = None
    path = str(path)
    for root, dirs, files in os.walk(path):
        if name in files:
            output = os.path.join(root, name)
    if(output == None):
        logError("Cannot get file for %s with given path : %s" % (name,path))
        if(os.path.exists(path) == False):
            logError("Path %s : no such file or directory" % path)
    return output


def checkArgs(args):
    foundError = False
    for key, value in args.items():
        if(value == None):
            logError("%s does not exist : no such file or directory" % key)
            foundError = True
    if (foundError):
        logError("Found errors in args ,  aborting execution")
        sys.exit(1)

#######################
##### PATH UTILS
#######################

currentWorkingDir = Path(os.getcwd())
logInfo("Current working dir is = {}".format(str(currentWorkingDir)) )




# Use cases :
# myExecutable path1 path2
# myExecutable ../../file1 ./file2 ./../../file3 dir/file4 file5 ./file6

# POSIX location flags :
# "." : means from current working dir
# ".." : means parent dir
# Pairs : ./ ../

# GLOBAL meanings
# "file" means file in current working dir
# "./file" means file in current working dir
# "dir/file" means file in dir, looking from current working 

# Recursive find parent 
# Note: does not verify if path exists.
def getParent(path, recursionDepth) :
    if recursionDepth >= 2 :
        [ path , recursionDepth ] = getParent(str(Path(path).parent) , recursionDepth - 1)
        return [ path , recursionDepth ]
    elif recursionDepth == 1 :
        return [ str(Path(path).parent) , recursionDepth - 1]
    else:
        return [ path , 0]

# Checks whether c is a POSIX separator (either '/' or '\' )
def isASeparator(c):
    for s in sepList :
        if c == s:
            return True
    return False

#Converts input path to posix Path whenever a separator (like '\') is detected
def convertToStandard(path, locale) :
    tmpPath = ""
    for c in path :
        if isASeparator(c) :
            c = locale
        tmpPath +=c
    return tmpPath 

# C style parent depth resolver ()
def resolveRelativePath(path , count) :
    tmpArg = ""
    copyTriggered = False
    count = 0
    for c in path :
        if copyTriggered == True:
            tmpArg += c
        else :
            if c == '.' : count += 1
            elif c.isalnum() :
                copyTriggered = True
                tmpArg += c

    print("Tmp arg  = {}".format(tmpArg) )
    return [ tmpArg , count ]


def handleRelativePath(path) :
    stringedCWD = str(currentWorkingDir)
    if path[0] == '.' : 
        dotCounter = 0
        [ path , dotCounter ] = resolveRelativePath(path,dotCounter)
        if dotCounter == 1 :
            # Local reference
            path = os.path.join(stringedCWD, str(Path(path)))
        elif dotCounter >=2 and dotCounter % 2  == 0 :
            # Parent dir
            parDir = currentWorkingDir
            [ parDir , dotCounter ] = getParent(currentWorkingDir,dotCounter / 2)
            path = os.path.join(parDir, str(Path(path)))
        elif dotCounter >= 3 and dotCounter % 2 == 1:
            # Found at least one local
            dotCounter -= 1
            parDir = currentWorkingDir
            [ parDir , dotCounter ] = getParent(currentWorkingDir,dotCounter / 2)
            path = os.path.join(parDir, str(Path(path)))
        else :
            # dotCounter == 0
            path = os.path.join(stringedCWD , path)
    else :
        path = os.path.join(stringedCWD , path)

    logInfo("Targeted path is : {}".format(path) )

    return path

def showUsage() :
    print("---------- USAGE ---------")
    print("This tool is used as an abstraction layer for FreeMyCode user\n")
    print("Usage example : $ {} python LaunchFreeMyCode.py <InstallationFolder> <TargetedDirectory> <RessourcesDirectory>{}\n".format(CLI_SuccessTag,CLI_NormalTag))
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
        logError("Args count is too small, please check your input")
        foundErrors = True
    else: 
        for i in range(1,len(sys.argv)) :
            arg = handleRelativePath(sys.argv[i])
            if not os.path.exists(arg) :
                logError("Current arg does not point to anything : no such file or directory ( {} )".format(arg))
                foundErrors = True
            elif foundErrors == False :
                argList.append(arg)
    if foundErrors == True :
        showUsage()
        sys.exit(1)

    logInfo("Successfully parsed input Arguments !")
    return argList

# Dipslays a list of args on logInfo output
def displayArgs(argList) :
    if DEBUG == 1:
        counter = 0
        for arg in argList :
            logInfo("Arg[{}] = {}".format(counter,arg))
            counter +=1

def displayCommandLine(argList) :
    print("Full Command Line is : ")
    for arg in argList :
        print("\"{}\"".format(arg), end=" ")
    print("")

################################## Main function's body ####################################

def main() :
    currentDir = os.getcwd()
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    argList = handleArgs()

    # Args from user input
    installationDir = argList[0]
    targetedDirectory = argList[1]
    userRessourcesDir = argList[2]

    # Dynamically retrieve the other ones
    freeMyCodeExecutable = find (FreeMyCode_exeName , installationDir )
    spectrumsDir = os.path.join(installationDir , "Ressources/Spectrums")
    spectrumsDir = convertToStandard(spectrumsDir , localeSep)
    configFile = find ('Config.json' , installationDir)
    secondaryInputFile = find( 'Secondary_input.json' , userRessourcesDir)
    licenseFile = find ('License.txt',userRessourcesDir)
    logFile = os.path.join(userRessourcesDir, 'logfile.log')

    # Check file paths for any error (mispelling, etc.)
    checkArgs({'FreeMyCode_executable':freeMyCodeExecutable ,
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