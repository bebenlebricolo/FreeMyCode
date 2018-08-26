#ifndef LICENSE_CHECKER_HEADER
#define LICENSE_CHECKER_HEADER 

// Precompiled header includes
#include "stdafx.h"

// Project-related includes
#include "LoggingTools.h"
#include "PathUtils.h"

// Standard includes
#include <iostream>
#include <vector>
#include <list>
#include <string>


struct LicenseSpectrum;

class LicenseChecker
{
public:
	//LicenseChecker();
	~LicenseChecker();
	// returns a pointer to a list of already licenses files
	// Removes already licenses files in input file list
	std::vector<std::string> *checkForLicenses(std::vector<std::string> &fileList);
	void parseSpectrums(std::vector<std::string> &fileList);
	void buildLicensesSpectrum(std::vector < std::string > &filesList);
	void generateSpectrumFiles(std::string outputPath);
	void buildGenericLicenseSpectrum();
	void printLicenses();
	void printSpectrums();
private:
	std::vector<LicenseSpectrum*> recordedLicenses;
};

struct LicenseSpectrum
{
	std::string licenseName;
	std::vector<std::pair<std::string, unsigned short int>> wordBasedDictionary;
	void printContent();
};

#endif // LICENSE_CHECKER_HEADER
