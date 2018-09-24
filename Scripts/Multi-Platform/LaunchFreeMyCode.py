#!/usr/bin/python

import sys
pythonVersion = sys.version_info
major = pythonVersion[0]
minor = pythonVersion[1]

# Required is 3.4
reqMajor = 3
reqMinor = 4

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
    print('Your version of Python is too old',
          'Please update your version of Python before launching this script again')
    print('Python version should be > %d.%d '% (reqMajor, reqMinor) )
    sys.exit(1)


import os
import platform
from pathlib import Path

platform = platform.system()

if(platform == 'Linux' ):
    FreeMyCode_exeName = 'FreeMyCode'
elif(platform == 'Windows'):
    FreeMyCode_exeName = 'FreeMyCode.exe'
 



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

DEBUG = 1


# https://stackoverflow.com/questions/1724693/find-a-file-in-python
def find(name, path):
    for root, dirs, files in os.walk(str(path)):
        if name in files:
            return os.path.join(root, name)


def logInfo(message) :
    # Debug stuff
    if(DEBUG == 1 ) :
        print (message)
    return 0


def main() :
    currentDir = os.getcwd()
    scriptDir = os.path.dirname(os.path.realpath(__file__)) 
    baseDir = Path(scriptDir).parent.parent 
    buildDir = baseDir.joinpath ('build')
    binDir = buildDir.joinpath('bin')

    freeMyCodeExecutable = find (FreeMyCode_exeName , binDir )


    logInfo("Starting script ")
    logInfo("ScriptDir = %s" % scriptDir )
    logInfo("baseDir = %s" % baseDir )
    logInfo("buildDir = %s" % buildDir )
    logInfo("FreeMyCode executable  = %s" % freeMyCodeExecutable )

    print ("Current working directory is :", currentDir)



    return 0


# Script execution
main ()
