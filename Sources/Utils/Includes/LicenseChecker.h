/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    Contributors :
        FoxP
    License :
        name : GPL V3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    About the author :
        url : https://github.com/bebenlebricolo
    Date : 16/10/2018 (16th of October 2018)
    Motivations : This is part of a Hobby ; I wanted a tool to help open-source developers to keep their hard work traceable and protected.
<FreeMyCode>@
------------------

FreeMyCode is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
struct InOut_CheckLicenses
{
    std::vector<std::string> fileList;
    std::vector<std::string> wrongFilesList;
    std::vector<std::string> alreadyLicensedFiles;
    std::vector<std::string> unlicensedFiles;
    InOut_CheckLicenses(){}
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
    bool checkForLicenses(InOut_CheckLicenses* list);
    void findInRegularFile(LicenseInFileMatchResult* match);
    void findInPlainTextFile(LicenseInFileMatchResult* match);

    // ################
    // Spectrum-related stuff
    // ################

    // Parses spectrum files (.spec) and register them in recordedLicenses list
    void parseSpectrums(std::vector<std::string> &fileList);
    // Overloading with a directory structure
    void parseSpectrums(std::string);
    // Build License Sectrum from text license (e.g. GPLV3.txt) and adds it to recordedLicenses list
    void buildLicensesSpectrum(std::vector < std::string > &filesList);
    // Builds a Spectrum from a word based vector (containing every word of a given text section) and outputs a spectrum object.
    void buildBasicSpectrum(vector<string> &wordsList, Spectrum *spec);
    // Overloading wrapper to buildBasicSpectum ...
    void buildBasicSpectrum(stringstream *stream, Spectrum *spec);

    // Writes all previously recorded Licenses Spectrum on disk into the targeted directory (outputPath)
    void writeSpectrumsOnDisk(std::string outputPath);
    // Builds a generic License Spectrum from all previously recorded Licenses.
    // TODO : to be implemented
    // void buildGenericLicenseSpectrum();

    static void removeWrongFiles(vector<string>* fileList, InOut_CheckLicenses* list);


    // Utils / debug / test functionalities
    void printLicenses();
    void printSpectrums();
private:
    // Vector of recorded Licenses (like GPL V2 / V3 , etc.)
    std::vector<LicenseSpectrum*> recordedLicenses;
    // Vector of files which does not contain any license (not found any in them)
    std::vector<std::string> unlicensedFiles;
    // List of pair of <file / Potential License name>
    std::vector<LicenseInFileMatchResult *> alreadyLicensedFiles;
};

// Spectrum base class : any text spectrum.
// A file spectrum (text) represents all words that could be found in a targeted file and
// list their frequency throughout the file. It used to compare comment blocks in input file with known licenses,
// thus allowing our program to determine whether the file already contains a license or not.
struct Spectrum
{
    std::vector<std::pair<std::string, unsigned short int>> wordBasedDictionary;
    void printContent();
    uint8_t compareWithSpectrum(LicenseSpectrum *other);
    void compareWithSpectrumList(vector<LicenseSpectrum* >* other, LicenseInFileMatchResult *match);
    unsigned int getTotalWordsNb();
};

// Same as parent class , but dedicated to one targeted license (embeds License name along with its spectrum)
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
    uint8_t sLine_count;
    uint8_t bOpen_count;
    uint8_t bClose_count;
    bool foundMarker;

    markersNumbers() : sLine_count(0), bOpen_count(0) , bClose_count(0), foundMarker(false) {}
    void resolveMarker(uint8_t index)
    {
        switch (index)
        {
        case 0:
            sLine_count++;
            foundMarker = true;
            break;
        case 1:
            bOpen_count++;
            foundMarker = true;
            break;
        case 2:
            bClose_count++;
            foundMarker = true;
            break;
        default:
            return;
        }
    }

    // Extract informations about the currently parsed markers
    // So that we could know if we are facing an opening block comment, closing, or single line comment section
    markerType getMarkerType()
    {
        if (sLine_count != 0 ||
               ( (bOpen_count == bClose_count) &&
                bOpen_count != 0 ) )
        {
            return markerType::line;
        }
        else if (sLine_count == 0 &&
                (bOpen_count != 0 ||
                bClose_count != 0))
        {
            return markerType::block;
        }
        else if (sLine_count == 0 && bClose_count ==0 && bOpen_count == 0)
        {
            return markerType::uncommented;
        }
        else
        {
            // sLine_count != 0
            // bOpen_count != bClose_count
            return markerType::undetermined;
        }
    }

    // Resets current marker container counters for a new session
    void reset()
    {
        sLine_count = 0;
        bOpen_count = 0;
        bClose_count = 0;
        foundMarker = false;
    }

};


// Comment handlong stuff and structures
struct CommentTypeHandlingStruct
{
    bool isSeparatorLine;
    bool switchToNewBlockFlag;
    bool pushNewData;
    uint8_t commentBlockLineNb;
    markersNumbers mNumb;
    CommentTag::commentType activeCommentBlockType;
    string *buffer;

    CommentTypeHandlingStruct():
        isSeparatorLine(false),
        switchToNewBlockFlag(false),
        pushNewData(false),
        commentBlockLineNb(0),
        activeCommentBlockType(CommentTag::commentType::unknown),
        buffer(nullptr)
    {}
};


#endif // LICENSE_CHECKER_HEADER
