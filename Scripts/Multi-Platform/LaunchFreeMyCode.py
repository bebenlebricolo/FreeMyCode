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

def logInfo(message) :
    # Debug stuff
    if(DEBUG == 1 ) :
        print (message)
    return 0

def logError(message):
    print("{}[ ERROR ] : {}{}".format(CLI_ErrorTag, message ,CLI_NormalTag))



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
    CLI_NormalTag = '\033[0;m'
elif(platform == 'Windows'):
    FreeMyCode_exeName = 'FreeMyCode.exe'
    CLI_ErrorTag = ''
    CLI_NormalTag = ''


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


def main() :
    currentDir = os.getcwd()
    scriptDir = os.path.dirname(os.path.realpath(__file__))
    # FreeMyCode base directory
    baseDir = Path(scriptDir).parent.parent
    buildDir = baseDir.joinpath ('build')
    binDir = buildDir.joinpath('bin')
    ressourcesDir = os.path.join(str(baseDir),'Ressources')

    freeMyCodeExecutable = find (FreeMyCode_exeName , binDir )
    targetedDirectory = os.path.join(str(baseDir),'Scripts/Dummy_Directory')
    configFile = find ('Config.json' , ressourcesDir)
    secondaryInputFile = find( 'Secondary_input.json' , ressourcesDir)
    licenseFile = find ('License.txt',targetedDirectory)
    logFile = os.path.join(targetedDirectory, 'logfile.txt')

    checkArgs({'FreeMyCode_executable':freeMyCodeExecutable ,
               'Targeted_directory' : targetedDirectory ,
               'Configuration_file' : configFile ,
               'Secondary_input_file' : secondaryInputFile ,
               'License_file' : licenseFile ,
               'Logging_file' : logFile})

    logInfo("Current working directory is : %s" % currentDir)
    logInfo("Starting script ")
    logInfo("ScriptDir = %s" % scriptDir )
    logInfo("baseDir = %s" % baseDir )
    logInfo("buildDir = %s" % buildDir )
    logInfo("FreeMyCode executable  = %s" % freeMyCodeExecutable )
    logInfo("targetedDirectory  = %s" % targetedDirectory )
    logInfo("configFile  = %s" % configFile )
    logInfo("secondaryInputFile = %s" % secondaryInputFile )
    logInfo("logFile = %s" % logFile )

    args = [freeMyCodeExecutable , targetedDirectory , "-A", licenseFile , "-p", "-lf", "-fsl" , "-c", configFile , "-L", logFile , "-v", "-sr", "-Si", secondaryInputFile]
    subprocess.call(args)
    return 0


# Script execution
main ()
