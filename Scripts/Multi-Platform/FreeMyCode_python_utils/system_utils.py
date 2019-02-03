import platform

DEBUG = 1
POSIX_sep = '/'
WIN_sep = '\\'
sepList = "{}{}".format(POSIX_sep,WIN_sep)

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