## CMakeLists file use to compile project sources

set( TARGETED_OS ${CMAKE_SYSTEM_NAME} )

if( ${TARGETED_OS} MATCHES "Linux" )
    set( CCompiler "GCC" )
elseif ( ${TARGETED_OS} MATCHES "Windows")
    set( CCompiler "MSVC" )
endif(${TARGETED_OS} MATCHES "Linux")

## ------------------------------------------
## Platform-related
## ------------------------------------------

### GCC with GNU/Linux systems
if( CCompiler MATCHES "GCC")
    set (CXX_WARNINGS "-Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wunused")
    set (CXX_FLAGS "-std=c++17 -lstdc++fs ")
    set (CXX_OPTIMISATIONS "-O1")
    set (FS_LIB "stdc++fs")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DDEBUG)
    endif()

### Microsoft Visual Studio Compiler (MSVC)
elseif( CCompiler MATCHES "MSVC")
    set (CXX_FLAGS "/std:c++17 /Wall /W2 /EHsc")
    set (CXX_OPTIMISATIONS "/O1")
    set (FS_LIB "")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(/DDEBUG)
    endif()
endif(CCompiler MATCHES "GCC")


# Setup default build configurations for all projects
set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE} ${CXX_WARNINGS} ${DEFINES}")
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE} ${CXX_WARNINGS} ${DEFINES}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE} ${CXX_WARNINGS} ${DEFINES}")
set(CMAKE_CXX_FLAGS_RELWITHDEB "${CXX_FLAGS} ${CXX_OPTIMISATIONS} ${CXX_COMPILE_TYPE} ${CXX_WARNINGS} ${DEFINES}")

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


# Installing binaries ...
install( TARGETS FreeMyCode
    DESTINATION ${CMAKE_SOURCE_DIR}/Install/bin)

# Creating Install folder
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Scripts/Multi-Platform
    DESTINATION ${CMAKE_SOURCE_DIR}/Install/Scripts)
# Install Platform-dependant scripts
if(${TARGETED_OS} MATCHES "Windows" )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/Scripts/Windows
        DESTINATION ${CMAKE_SOURCE_DIR}/Install/Scripts)
elseif (${TARGETED_OS} MATCHES "Linux")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/Scripts/Linux
        DESTINATION ${CMAKE_SOURCE_DIR}/Install/Scripts)
endif()

# Installing ressources directory
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Ressources/AvailableLicenses
    DESTINATION ${CMAKE_SOURCE_DIR}/Install/Ressources)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Ressources/Spectrums
    DESTINATION ${CMAKE_SOURCE_DIR}/Install/Ressources)
install(FILES ${CMAKE_SOURCE_DIR}/Ressources/Config.json
    DESTINATION ${CMAKE_SOURCE_DIR}/Install/Ressources)

install(FILES ${CMAKE_SOURCE_DIR}/Ressources/Secondary_input.json
    RENAME Secondary_input.template.json DESTINATION ${CMAKE_SOURCE_DIR}/Install/Ressources )

