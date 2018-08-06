## CMakeLists used to compile libs as static

project(libUtils)

set(SRC Sources)

add_subdirectory( BinaryTesters )


## set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x -D__STDC_INT64__")
set(GeneralInclusions PARENT_SCOPE)
list(APPEND GeneralInclusions
    Includes
    ${CMAKE_SOURCE_DIR}/Sources/PrecompiledHeaders
    ${CMAKE_SOURCE_DIR}/Sources/ThirdPartyLibs/rapidjson
)


## libCmdLineParser ----------
add_library(libCmdLineParser STATIC
    ${SRC}/Command_Line_parser.cpp
)
target_link_libraries(libCmdLineParser
    libRapidjson
    libLogTools
)
target_include_directories( libCmdLineParser PRIVATE
   ${GeneralInclusions}
)


## libConfigTools ----------
add_library(libConfigTools STATIC
    ${SRC}/ConfigTools.cpp
)
target_link_libraries(libConfigTools 
    libRapidjson
    libLogTools
    libFormatUtils
)
target_include_directories( libConfigTools PUBLIC
    ${GeneralInclusions}
)

## libDirectoryAnalyser ----------
add_library(libDirectoryAnalyser STATIC
    ${SRC}/DirectoryAnalyser.cpp
)
target_link_libraries(libDirectoryAnalyser 
    libRapidjson
    libPathUtils
    libLogTools
)
target_include_directories( libDirectoryAnalyser PUBLIC
    ${GeneralInclusions}  
)

## libFormatUtils ----------
add_library(libFormatUtils STATIC
    ${SRC}/FormattingUtils.cpp
)
target_link_libraries(libFormatUtils 
    libRapidjson
    libCmdLineParser
    libLogTools
)
target_include_directories( libFormatUtils PUBLIC
    ${GeneralInclusions}
)


## libLogTools ----------
add_library(libLogTools STATIC
    ${SRC}/LoggingTools.cpp
)
target_include_directories( libLogTools PUBLIC
    ${GeneralInclusions}    
)


## libPathUtils ----------
add_library(libPathUtils STATIC
    ${SRC}/PathUtils.cpp
)
target_link_libraries(libPathUtils libRapidjson)
target_include_directories( libPathUtils PUBLIC
    ${GeneralInclusions}    
)

## libLicenseWriter ----------
add_library(libLicenseWriter STATIC
    ${SRC}/LicenseWriter.cpp
)
target_link_libraries(libLicenseWriter 
    libRapidjson
    libLogTools
    libFormatUtils
    libDirectoryAnalyser
    libConfigTools
)
target_include_directories( libPathUtils PUBLIC
    ${GeneralInclusions}    
)

## libLicenseChecker
add_library(libLicenseChecker STATIC
	${SRC}/LicenseChecker.cpp
)

target_link_libraries(libLicenseChecker
	libLogTools
	libPathUtils
)

target_include_directories(libLicenseChecker PUBLIC
	${GeneralInclusions}
)