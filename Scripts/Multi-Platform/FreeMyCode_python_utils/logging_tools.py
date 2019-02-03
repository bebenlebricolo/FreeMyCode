import system_utils as su

def logInfo(message) :
    # Debug stuff
    if(su.DEBUG == 1 ) :
        print (message)
    return 0

def logError(message):
    print("{}[ ERROR ] : {}{}".format(su.CLI_ErrorTag, message ,su.CLI_NormalTag))