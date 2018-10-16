## CMakeLists.txt file used for binary testers tools (unitary tests)

project(libTesters)

set(GeneralInclusions PARENT_SCOPE)
## LicenseChecker binary tool
add_executable(testLicenseChecker
	testLicenseChecker.cpp
)

target_link_libraries ( testLicenseChecker
	libLicenseChecker
	libLogTools
	libConfigTools
	${FS_LIB}
)

target_include_directories( testLicenseChecker PUBLIC
	${GeneralInclusions}
)

set_target_properties( testLicenseChecker
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/LibTesters"
)

## PathUtils binary tool
add_executable(testPathUtils
	testPathUtils.cpp
)

target_link_libraries ( testPathUtils
	libPathUtils
	${FS_LIB}
)

target_include_directories( testPathUtils PUBLIC
	${GeneralInclusions}
)

set_target_properties( testPathUtils
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/LibTesters"
)

