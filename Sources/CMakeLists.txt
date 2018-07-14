## CMakeLists file use to compile project sources


set(LINUX)

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
    stdc++fs
)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -g -lstdc++fs")



