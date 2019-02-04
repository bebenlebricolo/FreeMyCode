import os, sys
from pathlib import Path
import subprocess

import FreeMyCode_python_utils.system_utils as su
import FreeMyCode_python_utils.logging_tools as log


# https://stackoverflow.com/questions/1724693/find-a-file-in-python
# Outputs 'None' if given filename is not found
def find(name, path):
    output = None
    path = str(path)
    for root, dirs, files in os.walk(path):
        if name in files:
            output = os.path.join(root, name)
    if(output == None):
        log.logError("Cannot get file for %s with given path : %s" % (name,path))
        if(os.path.exists(path) == False):
            log.logError("Path %s : no such file or directory" % path)
    return output


def checkArgs(args):
    foundError = False
    for key, value in args.items():
        if(value == None):
            log.logError("%s does not exist : no such file or directory" % key)
            foundError = True
    if (foundError):
        log.logError("Found errors in args ,  aborting execution")
        sys.exit(1)

#######################
##### PATH UTILS
#######################

currentWorkingDir = Path(os.getcwd())
log.logInfo("Current working dir is = {}".format(str(currentWorkingDir)) )




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
    for s in su.sepList :
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

    log.logInfo("Targeted path is : {}".format(path) )

    return path