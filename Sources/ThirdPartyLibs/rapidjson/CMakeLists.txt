##

project(libRapidjson)


add_library(libRapidjson INTERFACE)


if(LINUX)
    set( OsSpecificHeaders )
endif(LINUX)

# Include specific files for windows
if(WIN32)
    set( OsSpecificHeaders msinttypes )
endif(WIN32)
    

target_include_directories(libRapidjson INTERFACE 
    error
    internal
    ${OsSpecificHeaders}
)
