## CMakeLists file used to compile rapidjson library as a static library (linked against by upper layers)

## Header-only library : 
## http://mariobadr.com/creating-a-header-only-library-with-cmake.html
project(rapidjson)

add_library(libRapidJson INTERFACE)

target_include_directories( libRapidJson INTERFACE
    error
    internal
    msinttypes
)
