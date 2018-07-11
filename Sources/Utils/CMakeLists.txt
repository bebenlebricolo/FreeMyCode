## CMakeLists used to compile libs as static

project(libUtils)

set(SRC Sources)

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
)

include_directories(
    PUBLIC
        Includes
        ${CMAKE_SOURCE_DIR}/PrecompiledHeaders
)
