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
)

target_include_directories( testLicenseChecker PUBLIC
	${GeneralInclusions}
)