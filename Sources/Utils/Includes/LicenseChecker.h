#ifndef LICENSE_CHECKER_HEADER
#define LICENSE_CHECKER_HEADER

// Precompiled header includes
#include "stdafx.h"

// Project-related includes
#include "LoggingTools.h"
#include "ConfigTools.h"
#include "PathUtils.h"

// Standard includes
#include <iostream>
#include <vector>
#include <list>
#include <string>

// Forward declarations
struct Spectrum;
struct LicenseSpectrum;
struct LicenseInFileMatchResult;

// Exchange class 
struct LicensesLists
{
    std::vector<std::string> fileList;
    std::vector<std::string> wrongFilesList;
    std::vector<std::string> alreadyLicensedFiles;
    std::vector<std::string> unlicensedFiles;
    LicensesLists(){}
};

class LicenseChecker
{
public:
	//LicenseChecker();
	~LicenseChecker();

    // ################
    // License checking related stuff
    // ################

	// returns a pointer to a list of already licenses files
	// Removes already licenses files in input file list
	bool checkForLicenses(LicensesLists* list);
    void findInRegularFile(LicenseInFileMatchResult* match);
    void findInPlainTextFile(LicenseInFileMatchResult* match);

    // ################
    // Spectrum-related stuff
    // ################

	void parseSpectrums(std::vector<std::string> &fileList);
	void buildLicensesSpectrum(std::vector < std::string > &filesList);
    void buildBasicSpectrum(vector<string> &wordsList, Spectrum *spec);
	void generateSpectrumFiles(std::string outputPath);
	void buildGenericLicenseSpectrum();

    // Utils / debug / test functionalities
	void printLicenses();
	void printSpectrums();
private:
	std::vector<LicenseSpectrum*> recordedLicenses;
    std::vector<std::string> unlicensedFiles;
    // List of pair of <file / Potential License name>
    std::vector<LicenseInFileMatchResult *> alreadyLicensedFiles;
};

struct Spectrum
{
    std::vector<std::pair<std::string, unsigned short int>> wordBasedDictionary;
    void printContent();
    uint8_t compareWithSpectrum(Spectrum *other);
    unsigned int getTotalWordsNb();
};


struct LicenseSpectrum : public Spectrum
{
	std::string licenseName;
	void printContent();
};

// Used to exchange result data
struct LicenseInFileMatchResult
{
    std::string filePath;
    CommentMarkers markers;
    bool foundLicense;
    std::string licenseName;
    uint8_t degreeOfConfidence;
};

// Helps to know how much markers were found in one line
// -> Used to know if we are reading an opening / closing block comment, or a single line comment
struct markersNumbers {
    enum markerType {line, block , undetermined , uncommented};
    uint8_t singleLine;
    uint8_t blockOpen;
    uint8_t blockClose;
    bool foundMarker;

    markersNumbers() : singleLine(0), blockOpen(0) , blockClose(0), foundMarker(false) {}
    void resolveMarker(uint8_t index)
    {
        switch (index)
        {
        case 0:
            singleLine++;
            foundMarker = true;
            break;
        case 1:
            blockOpen++;
            foundMarker = true;
            break;
        case 2:
            blockClose++;
            foundMarker = true;
            break;
        default:
            return;
        }
    }
    markerType getMarkerType()
    {
        if (singleLine != 0 ||
               ( (blockOpen == blockClose) &&
                blockOpen != 0 ) )
        {
            return markerType::line;
        }
        else if (singleLine == 0 &&
                (blockOpen != 0 ||
                blockClose != 0))
        {
            return markerType::block;
        }
        else if (singleLine == 0 && blockClose ==0 && blockOpen == 0)
        {
            return markerType::uncommented;
        }
        else
        {
            // singleLine != 0
            // blockOpen != blockClose
            return markerType::undetermined;
        }
    }

    void reset()
    {
        singleLine = 0;
        blockOpen = 0;
        blockClose = 0;
        foundMarker = false;
    }

};


// Comment handlong stuff and structures

enum commentType { Line, Block, None };

struct CommentTypeHandlingStruct
{
    bool isSeparatorLine;
    bool switchToNewBlockFlag;
    bool pushNewData;
    uint8_t commentBlockLineNb;
    markersNumbers mNumb;
    commentType activeCommentBlockType;
    string *buffer;

    CommentTypeHandlingStruct():isSeparatorLine(false), switchToNewBlockFlag(false), pushNewData(false), commentBlockLineNb(0), activeCommentBlockType(commentType::None), buffer(nullptr){}
};


#endif // LICENSE_CHECKER_HEADER
