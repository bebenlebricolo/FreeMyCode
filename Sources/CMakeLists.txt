## CMakeLists file use to compile project sources

project(FreeMyCode)

add_executable(FreeMyCode 
    FreeMyCode.cpp
)

set(ExternalLibs_DIR ThirdPartyLibs)
set(UTILS Utils)
set(PCH PrecompiledHeaders)

add_subdirectory( ${ExternalLibs_DIR}/rapidjson )
add_subdirectory( ${UTILS} )
add_subdirectory( ${PCH} )


target_link_libraries( FreeMyCode
    libRapidJson
    libCmdLineParser
    libConfigTools
    libDirectoryAnalyser
    libFormatUtils
    libLogTools
    libPathUtils
)

