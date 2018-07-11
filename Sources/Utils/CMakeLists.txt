## CMakeLists used to compile libs as static

project(libUtils)

set(SRC Sources)

list(APPEND INCLUDE_DIRS 
    Includes    
    ${CMAKE_SOURCE_DIR}/Sources/PrecompiledHeaders    
)


add_library(libCmdLineParser STATIC
    ${SRC}/Command_Line_parser.cpp
)

add_library(libConfigTools STATIC
    ${SRC}/ConfigTools.cpp
)

add_library(libDirectoryAnalyser STATIC
    ${SRC}/DirectoryAnalyser.cpp
)

add_library(libFormatUtils STATIC
    ${SRC}/FormattingUtils.cpp
)

add_library(libLogTools STATIC
    ${SRC}/LoggingTools.cpp
)

add_library(libPathUtils STATIC
    ${SRC}/PathUtils.cpp
    #${CMAKE_SOURCE_DIR}/PrecompiledHeaders/stdafx.h   
)

target_include_directories( libCmdLineParser PUBLIC ${INCLUDE_DIRS})
target_include_directories( libConfigTools PUBLIC ${INCLUDE_DIRS})
target_include_directories( libDirectoryAnalyser PUBLIC ${INCLUDE_DIRS})
target_include_directories( libFormatUtils PUBLIC ${INCLUDE_DIRS})
target_include_directories( libLogTools PUBLIC ${INCLUDE_DIRS})
target_include_directories( libPathUtils PUBLIC ${INCLUDE_DIRS})


