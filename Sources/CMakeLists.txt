## CMakeLists file use to compile project sources

project(FreeMyCodeProject)


set( COMPILE_TYPE "Release")

if( ${COMPILE_TYPE} MATCHES "Debug")
	set(CMAKE_BUILD_TYPE Debug)
else()
	set(CMAKE_BUILD_TYPE Release)
endif()


set( TARGETED_OS ${CMAKE_SYSTEM_NAME} )

if( ${TARGETED_OS} MATCHES "Linux" )
	set ( CCompiler "GCC" )
elseif ( ${TARGETED_OS} MATCHES "Windows")
	set (CCompiler "MSVC")
endif(${TARGETED_OS} MATCHES "Linux")

## ------------------------------------------
## Platform-related
## ------------------------------------------

### GCC with GNU/Linux systems
if( CCompiler MATCHES "GCC")
	set (CXX_WARNINGS "-Wall -Wextra -Wfloat-equal -Wundef -Wshadow ")
	set (CXX_FLAGS "-std=c++17 -lstdc++fs ")
	set (CXX_OPTIMISATIONS "-O1")
	set (FS_LIB "stdc++fs")
	set (DEFINES "-DGCC_COMPILER ")

### Microsoft Visual Studio Compiler (MSVC)
elseif( ${CCompiler} MATCHES "MSVC")
	set (CXX_FLAGS "/std:c++17 /Wall /W2 /EHsc stdc++fs.lib")
	set (CXX_OPTIMISATIONS "/O1")
	set (FS_LIB "")
endif(CCompiler MATCHES "GCC")

set(CMAKE_CXX_FLAGS "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE} ${CXX_WARNINGS} ${DEFINES}")

## Subdirectories -------------------------------------
add_subdirectory( ThirdPartyLibs )
add_subdirectory( Utils )

## Executable -----------------------------------------
add_executable(FreeMyCode
    FreeMyCode.cpp
)

target_include_directories( FreeMyCode PUBLIC
    PrecompiledHeaders
    ThirdPartyLibs/rapidjson
    Utils/Includes
)

target_link_libraries( FreeMyCode
    libCmdLineParser
    libConfigTools
    libDirectoryAnalyser
    libFormatUtils
    libLogTools
    libPathUtils
    libLicenseWriter
	libLicenseChecker
	libParsingUtils
	${FS_LIB}
)






