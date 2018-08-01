## CMakeLists file use to compile project sources

set( COMPILE_TYPE "Debug")

set( TARGETED_OS "WINDOWS" )

if( ${TARGETED_OS} MATCHES "LINUX" )
	set ( CCompiler "GCC" )
elseif ( ${TARGETED_OS} MATCHES "WINDOWS")
	set (CCompiler "MSVC")
endif(${TARGETED_OS} MATCHES "LINUX")


### GCC with Linux systems
if( CCompiler MATCHES "GCC")
	set (CXX_FLAGS "-std=c++17 -Wall -Wextra -lstdc++fs")
	set (CXX_OPTIMISATIONS "-O1")
	set (FS_LIB "stdc++fs")
	if(${COMPILE_TYPE} MATCHES "Debug")
		set (CXX_COMPILE_TYPE "-g -DDEBUG")
	else(${COMPILE_TYPE} MATCHES "Debug")
		set (CXX_COMPILE_TYPE "")
	endif(${COMPILE_TYPE} MATCHES "Debug")

### Microsoft Visual Studio Compiler
elseif( ${CCompiler} MATCHES "MSVC")
	set (CXX_FLAGS "/std:c++17 /Wall /W2 /EHsc stdc++fs.lib")
	set (CXX_OPTIMISATIONS "/O1")
	set (FS_LIB "")
	if(${COMPILE_TYPE} MATCHES "Debug")
		set (CXX_COMPILE_TYPE "/Z7 /Zi /DDEBUG")
	else(${COMPILE_TYPE} MATCHES "Debug")
		set (CXX_COMPILE_TYPE "")
	endif(${COMPILE_TYPE} MATCHES "Debug")
endif(CCompiler MATCHES "GCC")

set(CMAKE_CXX_FLAGS "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE}")


add_subdirectory( ThirdPartyLibs )
add_subdirectory( Utils )

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
	${FS_LIB}
)






