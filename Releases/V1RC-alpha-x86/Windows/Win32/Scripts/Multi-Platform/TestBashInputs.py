#!/usr/bin/python

import sys
import os
from pathlib import Path
import platform

DEBUG = 1
platform = platform.system()

if(platform == 'Linux' ):
    FreeMyCode_exeName = 'FreeMyCode'
    CLI_ErrorTag = '\033[1;91m'
    CLI_NormalTag = '\033[0;m'
elif(platform == 'Windows'):
    FreeMyCode_exeName = 'FreeMyCode.exe'
    CLI_ErrorTag = ''
    CLI_NormalTag = ''

def logInfo(message) :
    # Debug stuff
    if(DEBUG == 1 ) :
        print (message)
    return 0

def logError(message):
    print("{}[ ERROR ] : {}{}".format(CLI_ErrorTag, message ,CLI_NormalTag))

currentWorkingDir = Path(os.getcwd())
logInfo("Current working dir is = {}".format(str(currentWorkingDir)) )
POSIX_sep = '/'
WIN_sep = '\\'
sepList = "{}{}".format(POSIX_sep,WIN_sep)


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
        if c == sepList:
            return True
    return False

#Converts input path to posix Path whenever a separator (like '\') is detected
def convertToStandard(path, locale) :
    for c in path :
        if isASeparator(c) :
            c = locale
    return path 

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


# Main program section
def main():
    for arg in sys.argv :
        logInfo("Arg is {}".format(arg) )
        if os.path.isabs(arg) :
            logInfo("Current arg is an absolute path")
        else :
            # Check if it looks like a relative path
            # If relative : converts it to absolute path (resolve location in current working tree)
            arg = handleRelativePath(arg)
            if not os.path.exists(arg):
                logError("Current path does not exist (yet)")
    return 

main ()
os.system("pause")