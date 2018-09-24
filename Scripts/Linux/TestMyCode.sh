#!/bin/bash
# FreeMyCode_cmd.exe directory -A License Config Logfile -sr
# FreeMyCode_cmd.exe directory -A License -a -c Config -L Logfile -v 


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

DEBUG=1

function logInfo()
{
    if [ $DEBUG -eq 1 ] ; then
        echo $@
    fi
}

ScriptDir=$(dirname "$0")
pushd $ScriptDir
RootDir=$(cd ../../build; pwd)

BinDir="$RootDir/bin"
BinaryTestersDir="$BinDir/LibTesters"
LibDir="$RootDir/lib"

Directory=$(cd ../Dummy_Directory ; pwd)
FreeMyCode_executable="$BinDir/FreeMyCode"
License="$Directory/License.txt"
Config="$Directory/Config.json"
Logfile="$Directory/logfile.log"
SecondaryInputFile="$Directory/Secondary_input.json"

 logInfo "ScriptDir = $ScriptDir"
 logInfo "FreeMyCode executable path = $FreeMyCode_executable "
 logInfo "$Directory"
 logInfo "$License"
 logInfo "$Config"
 logInfo "$Logfile"

"$FreeMyCode_executable" "$Directory" -A "$License" -a -p -lf -fsl -c "$Config" -L "$Logfile" -v -sr -Si "$SecondaryInputFile"

popd
