#!/usr/bin/python

import sys
import os
from pathlib import Path

currentWorkingDir = Path(os.getcwd())
print("Current working dir is = {}".format(str(currentWorkingDir)) )
sepList = "/\\"

def getParent(path, recursionDepth) :
    if recursionDepth >= 2 :
        [ path , recursionDepth ] = getParent(str(Path(path).parent) , recursionDepth - 1)
        return [ path , recursionDepth ]
    elif recursionDepth == 1 :
        return [ str(Path(path).parent) , recursionDepth - 1]
    else:
        return [ path , 0]

def isASeparator(c):
    for s in sepList :
        if c == sepList:
            return True
    return False

def stripDots(path , count) :
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

def main():
    for arg in sys.argv :
        print("Arg is {}".format(arg) )
        if os.path.isabs(arg) :
            print("Current arg is an absolute path")
        elif arg[0] == '.' : 
            countDotNumb = 0
            [ arg , countDotNumb ] = stripDots(arg,countDotNumb)
            if countDotNumb == 1 :
                # Local reference
                stringedCWD = str(currentWorkingDir)
                arg = os.path.join(stringedCWD, str(Path(arg)))
            elif countDotNumb >=2 and countDotNumb % 2  == 0 :
                # Parent dir
                parDir = currentWorkingDir
                [ parDir , countDotNumb ] = getParent(currentWorkingDir,countDotNumb / 2)
                arg = os.path.join(parDir, str(Path(arg)))
            else : 
                print("Wrong recursion")
            print("Targeted path is : {}".format(arg) )
    return 

main ()
os.system("pause")