#!/usr/bin/python

import sys
import os
from pathlib import Path

currentWorkingDir = Path(os.getcwd())
print("Current working dir is = {}".format(str(currentWorkingDir)) )
sepList = "/\\"

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
            elif not isASeparator(c) and c.isalnum() :
                copyTriggered = True
                tmpArg += c

    print("Tmp arg  = {}".format(tmpArg) )
    return [ tmpArg , count ]


def handleRelativePath(path) :
    if path[0] == '.' : 
        countDotNumb = 0
        [ path , countDotNumb ] = resolveRelativePath(path,countDotNumb)
        if countDotNumb == 1 :
            # Local reference
            stringedCWD = str(currentWorkingDir)
            path = os.path.join(stringedCWD, str(Path(path)))
        elif countDotNumb >=2 and countDotNumb % 2  == 0 :
            # Parent dir
            parDir = currentWorkingDir
            [ parDir , countDotNumb ] = getParent(currentWorkingDir,countDotNumb / 2)
            path = os.path.join(parDir, str(Path(path)))
        else : 
            print("Wrong recursion")
        print("Targeted path is : {}".format(path) )
    else :
        print("Don't know how to handle {} : this is not an absolute path, neither a relative one".format(path))
    return path


# Main program section
def main():
    for arg in sys.argv :
        print("Arg is {}".format(arg) )
        if os.path.isabs(arg) :
            print("Current arg is an absolute path")
        else :
            # Check if it looks like a relative path
            # If relative : converts it to absolute path (resolve location in current working tree)
            arg = handleRelativePath(arg)
    return 

main ()
os.system("pause")