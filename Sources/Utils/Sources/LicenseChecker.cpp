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

#include "stdafx.h"
#include "LicenseChecker.h"
#include "LoggingTools.h"
#include "PathUtils.h"
#include "ParsingUtils.h"
#include "ConfigTools.h"
#include "CommandLineParser.h"

#include FS_INCLUDE
#include <fstream>
#include <cstring>

namespace fs = FS_CPP;
namespace pu = pathutils;
namespace lg = logger;
using namespace std;

static const char* spectrumNameToken = "Name";
static const char spectrumDelimiter = ':';
static const char spectrumLineDelimiter = '\n';
static const char spectrumCommentDelimiter = '#';
static const char* spectrumSpecificChars = "#:\n";
static const unsigned int spectrumMaxSpecificCharsCount = 10;
static const char* spectrumFileExtension = ".spec";


// Generate license spectrum from license files
static const uint8_t maxLettersThreshold = 3;
static const uint8_t minimumSpacesBeforeCommentTag = 3;

static const unsigned int maxWordSize = 50;

static void trimWhiteSpaces(string &word);
static bool hasOneCharacter(char letter,const char* testChars);

// ##############################
// License detection related stuff
// ##############################
// TODO : make this configurable with a file or via command line

static const uint8_t minimumDegreeOfConfidenceRequired = 50U;

// Only look in the 100 first lines of each file
static const uint8_t lineCounterLimit = 100;
// Minimum comment block size ; comment block is dropped if its size is below this number
static const uint8_t minimumCommentBlockLineSize = 5;
// Usual characters used to print human readable separator lines
static const char* separatorCharList = "-#_*/+~<>";
// Minimum separator char count under which parser will not detect a separator line
static const uint8_t minimumSeparatorCount = 10;




// Parses spectrum Files and for each line, discriminates the left member (called token by now) of the right member
// E.g : foo : 53
//        ^     ^
//        |   right member (number of word occurences)
//    left member, alias "token" (word)
void LicenseChecker::parseSpectrums(std::vector<std::string> &fileList)
{
    logger::Logger* log = logger::Logger::get_logger();
    if (fileList.size() == 0)
    {
        log->logWarning("Nothing to parse : input vector is empty. Aborting parsing session.", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }
    // first check file exists
    for (unsigned int f = 0; f < fileList.size(); f++)
    {
        string currentPath = fileList[f];
        string filename = pu::get_filename(currentPath);
        if (fs::exists(currentPath) == false)
        {
            log->logWarning("File " + filename + " does not exist.", __LINE__, __FILE__, __func__, "LicenseChecker");
        }
        else
        {
            // If we found a directory, parse all spectrums inside it!
            if (fs::is_directory(currentPath))
            {
                vector<string> filesInDir;
                pu::getAllFilesInDir(filesInDir, currentPath);
                parseSpectrums(filesInDir);
            }
            else
            {
                // Otherwise, check all files
                ifstream spectrumFile(currentPath);
                string currentWord;
                string lineBuffer;

                if (pu::get_extension(currentPath) != spectrumFileExtension)
                {
                    log->logWarning("Current file < " + pu::get_filename(currentPath) + " > does not have the required extension < " + spectrumFileExtension + " > . Aborting parsing of this file", __LINE__, __FILE__, __func__, "LicenseChecker");
                    spectrumFile.close();
                    continue;
                }
                else
                {
                    LicenseSpectrum *curLicense = new LicenseSpectrum();
                    unsigned int lineNumber = 0;
                    bool caughtError = false;

                    // Parse the current file
                    while (getline(spectrumFile, lineBuffer, spectrumLineDelimiter) && caughtError == false)
                    {
                        lineNumber++;
                        bool foundDelimiter = false;
                        string token;
                        string rightMember;
                        log->logDebug("Parsing line : \" " + lineBuffer + " \" ");
                        if (lineBuffer[0] == spectrumCommentDelimiter)
                        {
                            // Found comment section
                            log->logDebug("Found commented section");
                            continue;
                        }
                        else
                        {
                            for (unsigned int i = 0; i < lineBuffer.size(); i++)
                            {
                                char curLetter = lineBuffer[i];
                                if (hasOneCharacter(curLetter, spectrumSpecificChars) == false)
                                {
                                    if (foundDelimiter == false)
                                    {
                                        token += curLetter;
                                    }
                                    else
                                    {
                                        rightMember += curLetter;
                                    }
                                }
                                else
                                {
                                    if (curLetter == spectrumDelimiter)
                                    {
                                        log->logDebug("Found delimiter");
                                        foundDelimiter = true;
                                    }
                                    if (curLetter == spectrumCommentDelimiter)
                                    {
                                        log->logDebug("Found comment marker");
                                        break;
                                    }
                                }
                            }

                            trimWhiteSpaces(token);
                            trimWhiteSpaces(rightMember);
                            log->logDebug("token = \"" + token + "\"");
                            log->logDebug("rightMember = \"" + rightMember + "\"");

                            if (token.size() == 0 || rightMember.size() == 0)
                            {
                                if (foundDelimiter == true)
                                {
                                    log->logError("At least one of the line member is empty. Line number is [" + to_string(lineNumber) + "]");
                                }
                                else
                                {
                                    log->logError("Cannot find delimiter for this line. Line number is [" + to_string(lineNumber) + "]");
                                }
                                log->logError("Line content : \"" + lineBuffer + "\"");
                                caughtError = true;
                            }
                        }

                        if (caughtError == false)
                        {
                            // Check for License Header
                            // Name : GNU GPL V3
                            // token : rigthMember
                            if (token == spectrumNameToken)
                            {
                                curLicense->licenseName = rightMember;
                            }
                            else
                            {
                                pair <std::string, unsigned int> curPair(token, atoi(rightMember.c_str()));
                                curLicense->wordBasedDictionary.push_back(curPair);
                            }
                        }
                        else
                        {
                            log->logInfo("Error caught while parsing. Aborting parsing for file " + currentPath);
                            delete curLicense;
                            break;
                        }
                    }
                    if (caughtError == false)
                    {
                        log->logInfo("Successfully parsed file " + pu::get_filename(currentPath));
                        recordedLicenses.push_back(curLicense);
                    }
                }
            }
        }
    }
}

// Wrapper to parseSpectrums, capable of handling full directory
void LicenseChecker::parseSpectrums(string dir)
{
    vector<string> fileList;
    pu::getAllFilesInDir(fileList, dir);
    parseSpectrums(fileList);
}

// Trims whitespaces from both side of a given word
// "   foobar   Foobar!foobaaaar..   " will become
// -> "foobar   Foobar!foobaaaar.."
// TODO Put all that stuff in ParsingUtils lib
void trimWhiteSpaces(string &word)
{
    string buffer;
    bool detectedWord = false;
    unsigned int wordStartBoundary = 0;
    unsigned int wordEndBoundary = 0;

    for (unsigned int i = 0; i < word.size(); i++)
    {
        char curLetter = word[i];
        if(curLetter != ' ')
        {
            // found word boundary
            wordStartBoundary = i;
            detectedWord = true;
            break;
        }
    }

    if (detectedWord == true)
    {
        // Same algorithm running from the end of the word to the beginning
        for (unsigned int i = word.size() - 1; i != 0 ; i --)
        {
            char curLetter = word[i];
            if(curLetter != ' ')
            {
                // found word boundary
                wordEndBoundary = i;
                break;
            }
        }
        // Copy cleaned result to input word
        for (unsigned int i = wordStartBoundary; i < wordEndBoundary + 1; i++)
        {
            buffer += word[i];
        }
    }

    word = buffer;
}



LicenseChecker::~LicenseChecker()
{
    for (unsigned int i = 0; i < recordedLicenses.size(); i++)
    {
        delete(recordedLicenses[i]);
    }
}

// TODO put all of this in ParsingUtils
bool hasOneCharacter(char letter, const char* testChars)
{
    bool out = false;
    unsigned int size = strnlen(testChars, spectrumMaxSpecificCharsCount);
    for (unsigned int i = 0; i < size; i++)
    {
        if (letter == testChars[i])
        {
            out = true;
            break;
        }
    }
    return out;
}

void LicenseChecker::printLicenses()
{
    logger::Logger *log = logger::Logger::get_logger();
    log->logDebug("Starting recorded licenses printing.", __LINE__, __FILE__, __func__, "LicenseChecker");
    if (recordedLicenses.size() == 0)
    {
        log->logWarning("No licenses found in memory. Try to parse one before attempting to print it.", __LINE__, __FILE__, __func__, "LicenseChecker");
    }
    else
    {
        for (unsigned int i = 0; i < recordedLicenses.size(); i++)
        {
            recordedLicenses[i]->printContent();
        }
    }
}

// Sorting functor
struct isEqual
{
    isEqual(string _s) : s(_s){}
    bool operator()(const pair <string, unsigned short int> &element)
    {
        return element.first == s;
    }
    string s;
};

bool sortAlpha (const pair <string, unsigned short int> &a, const pair <string, unsigned short int> &b)
{
    return a.first < b.first;
}


void LicenseChecker::buildLicensesSpectrum(std::vector < std::string > &filesList)
{
    logger::Logger *log = logger::Logger::get_logger();

    // list words which are bigger than letters count trigger (e.g. 3)
    for (unsigned int i = 0; i < filesList.size(); i++)
    {
        // opens the file
        string path = filesList[i];
        ifstream file;
        if (fs::exists(path))
        {
            // Process all files in pointed directory first
            if (fs::is_directory(path))
            {
                vector<string> filesInDir;
                pu::getAllFilesInDir(filesInDir, path);
                buildLicensesSpectrum(filesInDir);
            }
            else
            {
                // This should be a file, then
                file.open(filesList[i]);
            }
        }
        else
        {
            log->logWarning("file " + path + " doesn't exist.", __LINE__, __FILE__, __func__, "LicenseChecker");
            // Loop until next file in list
            continue;
        }
        // If we get here, process file.
        string licenseName = pu::remove_extension(pu::get_filename(path));
        LicenseSpectrum *lic = new LicenseSpectrum;
        lic->licenseName = licenseName;


        // Build words list
        string line;
        vector<string> wordsList;

        while (getline(file, line))
        {
            vector<string> wordsFromLine;
            tokenizeWords(line, wordsFromLine);

            // Filters out words which are smaller than the required threshold
            for (unsigned int j = 0; j < wordsFromLine.size(); j++)
            {
                string word = wordsFromLine[j];
                if (word.size() > maxLettersThreshold)
                {
                    wordsList.push_back(word);
                }
            }
        }
        sort(wordsList.begin(), wordsList.end());

        buildBasicSpectrum(wordsList, lic);

        recordedLicenses.push_back(lic);
        file.close();
    }
}

// Builds a Spectrum from a words based vector (containing every word of a given text section) and outputs a spectrum object.
void LicenseChecker::buildBasicSpectrum(vector<string> &wordsList, Spectrum *spec)
{
    lg::Logger *log = lg::getLogger();

    if (spec == nullptr)
    {
        log->logError("Input is nullptr. Aborting function", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }

    vector<std::pair<std::string, unsigned short int>>::iterator it;
    for (unsigned int j = 0; j < wordsList.size(); j++)
    {
        it = std::find_if(spec->wordBasedDictionary.begin(), spec->wordBasedDictionary.end(), isEqual(wordsList[j]));
        if (it == spec->wordBasedDictionary.end())
        {
            spec->wordBasedDictionary.push_back({ wordsList[j] , 1 });
        }
        else
        {
            // increment second element of the targeted pair
            it->second++;
        }
    }

    log->logInfo("Sorting Spectrum ... Spectrum building successfull", __LINE__, __FILE__, __func__, "LicenseChecker");
    sort(spec->wordBasedDictionary.begin(), spec->wordBasedDictionary.end(), sortAlpha);
}


// Writes all previously recorded Licenses Spectrum on disk into the targeted directory (outputPath)
void LicenseChecker::writeSpectrumsOnDisk(std::string outputPath)
{
    lg::Logger *log = lg::getLogger();
    bool createDirFlag = false;
    // fs::perms userPermissions;
    // string closestParentDir = pu::get_closest_exisiting_parent_dir(outputPath);

    // First check if output path directory exist
    if (fs::exists(outputPath) == false)
    {
        log->logInfo("Directory " + outputPath + "does not exist yet. Creating one...", __LINE__, __FILE__, __func__, "LicenseChecker");
        createDirFlag = true;
    }
    else if (fs::is_directory(outputPath) == false)
    {
        log->logError("Targeted output path for Licenses Spectrum is not a directory. Aborting execution. Given path is: " + outputPath , __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }

    if (createDirFlag == true)
    {
        fs::create_directories(outputPath);
    }

    for (unsigned int i = 0; i < recordedLicenses.size(); i++)
    {
        string fileName = recordedLicenses[i]->licenseName + spectrumFileExtension;
        string filePath = pu::join(outputPath, fileName);
        if (fs::exists(filePath) == true)
        {
            log->logInfo("Targeted License spectrum file already exist. Skipping it. License spectrum name = " + fileName, __LINE__, __FILE__, __func__, "LicenseChecker");
        }
        else
        {
            ofstream file(filePath);
            if (file.is_open())
            {
                file << spectrumCommentDelimiter << "License Spectrum file built upon license " << recordedLicenses[i]->licenseName << endl;
                file << spectrumNameToken << spectrumDelimiter << recordedLicenses[i]->licenseName << endl;
                for (unsigned int wordIndex = 0; wordIndex < recordedLicenses[i]->wordBasedDictionary.size(); wordIndex++)
                {
                    file << recordedLicenses[i]->wordBasedDictionary[wordIndex].first << spectrumDelimiter << recordedLicenses[i]->wordBasedDictionary[wordIndex].second << endl;
                }
            }
            file.close();
            log->logInfo("Successfully written license spectrum : " + fileName, __LINE__, __FILE__, __func__, "LicenseChecker");
        }
    }
}


void LicenseChecker::printSpectrums()
{
    for (unsigned int i = 0; i < recordedLicenses.size(); i++)
    {
        recordedLicenses[i]->printContent();
    }
}


// Takes a raw file list (we don't know yet if they contains licenses or not)
// And extracts informations about potential licenses text in the file's body
bool LicenseChecker::checkForLicenses(InOut_CheckLicenses* list)
{
    // Utils declaration / initialisation
    ConfObject *config = ConfObject::getConfig();
    lg::Logger* log = lg::getLogger();

    // File-related stuff
    string filePath;
    string fileExtension;

    // Comment sections markers
    string block_comment_open;
    string block_comment_close;
    string single_line_comment;


    if(list == nullptr)
    {
        log->logError("Input pointer is null ", __LINE__, __FILE__, __func__, "LicenseChecker");
        return false;
    }


    for (unsigned int i = 0; i < list->fileList.size(); i++)
    {
        LicenseInFileMatchResult* match = new LicenseInFileMatchResult();
        if (match == nullptr)
        {
            log->logError("Cannot allocate memory for LicenseInFileMatchResult. Aborting execution ", __LINE__, __FILE__, __func__, "LicenseChecker");
            return false;
        }

        match->filePath = list->fileList[i];
        fileExtension = pu::get_extension(match->filePath);

        // Get all comment markers ;  We don't know yet the targeted file's encoding
        // So we need to target all kind of comments blocks
        match->markers.bStart.value = config->get_bloc_comment_start(fileExtension);
        match->markers.bEnd.value = config->get_bloc_comment_end(fileExtension);
        match->markers.sgLine.value = config->get_single_line_com(fileExtension);

        ostringstream errorMessage;
        if (match->markers.checkForMissingCommentMarker(&errorMessage) == true)
        {
            log->logWarning("Current extension does not implement all comment marker types : " + errorMessage.str(), __LINE__, __FILE__, __func__, "LicenseChecker");
        }

        match->markers.checkIfPlainText();

        // Case where no comment markers were found : might be plain text.
        // Special treatment might be applied for this kind of file
        if (match->markers.isPlainText == true)
        {
            log->logWarning("Current file's extension did not give any comment markers. This file might be plain text. File = " + filePath, __LINE__, __FILE__, __func__, "LicenseChecker");
            log->logWarning("Default current behavior is to ignore such file and withdraw it from list.", __LINE__, __FILE__, __func__, "LicenseChecker");
            list->wrongFilesList.push_back(filePath);
            // TODO : find License texts within file with plain text research strategy ; look for special words such as "License", "Warranty", "Property", etc.
        }
        else
        {
            findInRegularFile(match);
            if (match->foundLicense == true)
            {
                // Remove this license from the editing pipeline
                list->alreadyLicensedFiles.push_back(match->filePath);
            }
            else
            {
                // Discard this License and keep it in the License editing pipeline
                list->unlicensedFiles.push_back(match->filePath);
            }
        }

    }
    return true;
}

// Little tool to reset Match result when error is detected
static void resetMatch(LicenseInFileMatchResult *match)
{
    match->degreeOfConfidence = 0;
    match->foundLicense = false;
    match->licenseName = "Unknown";
    match->markers.reset();
}

static void stripCommentMarker(CommentMarkers *markers, string *bufferLine , markersNumbers *mNumb)
{
    lg::Logger* log = lg::getLogger();
    if (markers == nullptr || bufferLine == nullptr)
    {
        log->logError("Input parameter is null. Aborting function execution", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }
    else
    {
        string buffer = *bufferLine;
        vector<CommentTag> markersVect;
        markers->vectorizeMembers(&markersVect);

        // Iterate over each marker and check for presence in line
        for (uint8_t m = 0; m < markersVect.size(); m++)
        {
            string currentMarker = markersVect[m].value;
            string markerDescription = markersVect[m].name;
            size_t markerStartOffset = buffer.find(currentMarker);

            if (markersVect[m].type == CommentTag::commentType::single_line)
            {
                if (markerStartOffset >= minimumSpacesBeforeCommentTag)
                {
                    // We found a single line comment with some text right before
                    // This kind of comments should be discarded as they are only context-related single line comment types
                    continue;
                }
            }


            if (currentMarker.size() == 0)
            {
                // skip marker -> this marker is not set!
                continue;
            }

            // While we can find markers in string, keep deleting it!
            while (markerStartOffset != string::npos)
            {
                // Increment marker discover number
                mNumb->resolveMarker(m);
                log->logDebug("Found "+ markerDescription + " marker \" " + currentMarker+ " \" in string \" " + *bufferLine + " \". Let's remove it", __LINE__, __FILE__, __func__, "LicenseChecker");
                buffer.erase(markerStartOffset, currentMarker.length());
                markerStartOffset = buffer.find(currentMarker);
            }
        }
        *bufferLine = buffer;
    }
}

static bool isASeparator(string *buffer)
{
    uint8_t charCount = 0;
    for (unsigned int i = 0; (i < buffer->size() && charCount <= minimumSeparatorCount) ; i++)
    {
        if(findCharInList((*buffer)[i] , separatorCharList , strnlen(separatorCharList,20)) == true)
        {
            charCount++;
        }
    }
    if (charCount >= minimumSeparatorCount)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void printBlockCommentSections(vector<stringstream*> *vec)
{
    for (unsigned int i = 0; i < vec->size(); i++)
    {
        cout << ">>>> Printing block comment number " << to_string(i) << endl;
        cout << (*vec)[i]->str() << endl << endl;
    }
}

// Resolves what kind of comment we are currently parsing (block, single line, closing block, opening block, etc...)
static void handleCommentType(CommentTypeHandlingStruct* input)
{
    lg::Logger *log = lg::getLogger();
    // Handle comment block type and what to do next
    if (input->mNumb.foundMarker || input->isSeparatorLine || input->activeCommentBlockType == CommentTag::commentType::single_line)
    {
        if (input->isSeparatorLine == true)
        {
            if (input->commentBlockLineNb != 0)
            {
                // If last comment block is not empty, this means we are going to parse a new comment block
                input->switchToNewBlockFlag = true;
            }
            // Else write data to active comment Block
            else input->pushNewData = true;
        }
        else
        {
            switch (input->mNumb.getMarkerType())
            {
            case markersNumbers::markerType::line:
                switch (input->activeCommentBlockType)
                {
                case CommentTag::commentType::single_line:
                case CommentTag::commentType::block:
                    input->pushNewData = true;
                    break;
                default:
                    input->activeCommentBlockType = CommentTag::commentType::single_line;
                    input->pushNewData = true;
                    break;
                }
                break;
            case markersNumbers::markerType::block:
                switch (input->activeCommentBlockType)
                {
                case CommentTag::commentType::single_line:
                    input->pushNewData = true;
                    break;
                case CommentTag::commentType::block:
                    input->activeCommentBlockType = CommentTag::commentType::unknown;
                    if (input->commentBlockLineNb != 0)
                    {
                        // Found closing comment block marker
                        input->switchToNewBlockFlag = true;
                    }
                    else input->pushNewData = true;
                    break;
                default:
                    if (input->commentBlockLineNb == 0)
                    {
                        // Found Opening comment block
                        input->activeCommentBlockType = CommentTag::commentType::block;
                        input->pushNewData = true;
                    }
                    else
                    {
                        // Maybe we had a single line comment just before,
                        // which could be merged with a block comment
                        input->activeCommentBlockType = CommentTag::commentType::block;
                        input->pushNewData = true;
                    }
                }
                break;
            case markersNumbers::markerType::uncommented:
                switch (input->activeCommentBlockType)
                {
                case CommentTag::commentType::block:
                    input->pushNewData = true;
                    break;
                case CommentTag::commentType::single_line:
                    input->activeCommentBlockType = CommentTag::commentType::unknown;
                    break;
                default:
                    break;
                }
                break;
            default:
                log->logError("Cannot resolve current line ! Block comment might be wrong. Line is : " + *(input->buffer), __LINE__, __FILE__, __func__, "LicenseChecker");
                log->logWarning(" Default behavior is : do not fill comment block .", __LINE__, __FILE__, __func__, "LicenseChecker");
                input->switchToNewBlockFlag = false;
                break;
            }
        }
    }
    else
    {
        if (input->activeCommentBlockType != CommentTag::commentType::unknown)
        {
            input->pushNewData = true;
        }
    } // End of comment block type handling
}

void LicenseChecker::findInRegularFile(LicenseInFileMatchResult* match)
{
    lg::Logger* log = lg::getLogger();
    CommandLineParser *parser = CommandLineParser::getParser();
    if (match == nullptr)
    {
        log->logError("Input parameter is null. Aborting function execution", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }

    log->logInfo("Opening " + match->filePath + " file.", __LINE__, __FILE__, __func__, "LicenseChecker");
    string buffer;
    ifstream currentFile(match->filePath);
    if (currentFile.is_open() == false)
    {
        log->logError("Cannot open " + match->filePath + " file ! Aborting file parsing", __LINE__, __FILE__, __func__, "LicenseChecker");
        resetMatch(match);
        return;
    }

    // Else, start parsing the file

    // Tells whether it's a single line comment block or a regular (multiline) comment block
    CommentTypeHandlingStruct handStr;

    stringstream* commentBlock = new stringstream;
    vector<stringstream*> commentBlockCollection;
    uint8_t blank_lines_counter = 0;
    CommentTag::commentType previous_comment_type = CommentTag::commentType::unknown;


    for (uint8_t lineCounter = 0; (getline(currentFile, buffer) && lineCounter <= lineCounterLimit ); lineCounter++)
    {
        handStr.mNumb.reset();
        handStr.switchToNewBlockFlag = false;
        handStr.isSeparatorLine = false;
        handStr.pushNewData = false;
        handStr.buffer = &buffer;

        // List all comments and regroup them in block comments sections
        // After processing : second pass -> clearing block comments which are too small ( e.g. less than 5 lines )

        trimWhiteSpaces(buffer);
        stripCommentMarker(&(match->markers), &buffer , &(handStr.mNumb));
        handStr.isSeparatorLine = isASeparator(&buffer);

        // Determine which comment type we are facing
        handleCommentType(&handStr);

        // Handle data
        if (handStr.switchToNewBlockFlag == true)
        {
            if (handStr.commentBlockLineNb < minimumCommentBlockLineSize)
            {
                log->logInfo("Dropping current comment block : block size is too small.", __LINE__, __FILE__, __func__, "LicenseChecker");
                delete commentBlock;
            }
            else
            {
                // Store the last line before switching to another comment block
                if (handStr.commentBlockLineNb != 0)
                {
                    handStr.commentBlockLineNb++;
                    *commentBlock << buffer << endl;
                }
                commentBlockCollection.push_back(commentBlock);
            }
            commentBlock = new stringstream;
            handStr.commentBlockLineNb = 0;
            blank_lines_counter = 0;
        }
        else if (handStr.pushNewData == true)
        {
            if (handStr.isSeparatorLine == false && buffer.size() != 0)
            {
                handStr.commentBlockLineNb++;
                *commentBlock << buffer << endl;
                blank_lines_counter = 0;
            }
        }
        else if(handStr.commentBlockLineNb > 0)
        {
            // Discard blank lines
            blank_lines_counter++;
            if (blank_lines_counter >= 3)
            {
                if (handStr.commentBlockLineNb >= minimumCommentBlockLineSize)
                {
                    commentBlockCollection.push_back(commentBlock);
                }
                else
                {
                    delete commentBlock;
                }
                commentBlock = new stringstream;
                handStr.commentBlockLineNb = 0;
                blank_lines_counter = 0;
            }
        }
        // If the currently parser comment type is something different than before
        else if (   handStr.commentBlockLineNb != 0
                &&  (	(handStr.activeCommentBlockType != CommentTag::commentType::unknown)
                    ||  (previous_comment_type != CommentTag::commentType::unknown) )
                && (previous_comment_type != handStr.activeCommentBlockType))
        {
            if (handStr.commentBlockLineNb >= minimumCommentBlockLineSize)
            {
                commentBlockCollection.push_back(commentBlock);
            }
            else
            {
                delete commentBlock;
                commentBlock = new stringstream;
            }
            handStr.commentBlockLineNb = 0;
            blank_lines_counter = 0;

        }
        previous_comment_type = handStr.activeCommentBlockType;

    } // End of for loop
    currentFile.close();
    if (handStr.commentBlockLineNb != 0 && commentBlock != nullptr)
    {
        commentBlockCollection.push_back(commentBlock);
    }
    // Finished iterating over the 100 first file's lines
    if (parser->get_flag("-lb") == true)
    {
        printBlockCommentSections(&commentBlockCollection);
    }


    // Build Spectrums for each found commented section
    vector<Spectrum *> specList;
    for (unsigned int i = 0; i < commentBlockCollection.size(); i++)
    {
        Spectrum *spec = new Spectrum;

        buildBasicSpectrum(commentBlockCollection[i],spec);
        specList.push_back(spec);

        if (parser->get_flag("-lg") == true)
        {
            cout << "comment block " << to_string(i) << " word based dictionary : \n";
            spec->printContent();
            cout << endl;
        }
    }

    // Compare with all available licenses onboard
    for (unsigned int sp = 0; sp < specList.size(); sp++)
    {
        specList[sp]->compareWithSpectrumList(&recordedLicenses, match);
    }



    // Do not forget to delete all comment blocks after match
    for (unsigned int i = 0; i < commentBlockCollection.size(); i++)
    {
        delete commentBlockCollection[i];
    }
    for(unsigned int i = 0 ; i < specList.size() ; i++)
    {
        delete specList[i];
    }
}


void LicenseChecker::buildBasicSpectrum(stringstream *stream, Spectrum *spec)
{
    // Build words list
    string line;
    vector<string> wordsList;

    while (getline(*stream, line))
    {
        vector<string> wordsFromLine;
        tokenizeWords(line, wordsFromLine);

        // Filters out words which are smaller than the required threshold
        for (unsigned int j = 0; j < wordsFromLine.size(); j++)
        {
            string word = wordsFromLine[j];
            if (word.size() > maxLettersThreshold)
            {
                wordsList.push_back(word);
            }
        }
    }
    sort(wordsList.begin(), wordsList.end());
    buildBasicSpectrum(wordsList, spec);
}

void LicenseChecker::findInPlainTextFile(LicenseInFileMatchResult* match)
{
    lg::Logger* log = lg::getLogger();
    if (match == nullptr)
    {
        log->logError("Input parameter is null. Aborting function execution", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }
    log->logWarning("Not yet implemented", __LINE__, __FILE__, __func__, "LicenseChecker");
    resetMatch(match);
}


static inline void removeItems(vector<string> *target, vector<string> *reference)
{
    if (target != nullptr && reference != nullptr)
    {
        vector<string>::iterator it;
        for (unsigned int i = 0; i < reference->size(); i++)
        {
            string currentItem = (*reference)[i];
            it = std::find(target->begin(), target->end(), currentItem);
            if (it != target->end())
            {
                // Remove current item in list
                target->erase(it);
            }
        }
    }
}

// Removes wrong files and already licensed files from targeted files list
void LicenseChecker::removeWrongFiles(vector<string>* fileList, InOut_CheckLicenses* list)
{
    logger::Logger *log = logger::getLogger();
    if (fileList == nullptr || list == nullptr)
    {
        log->logError("Input parameter is null. Aborting function execution", __LINE__, __FILE__, __func__, "LicenseChecker");
        return;
    }
    removeItems(fileList, &(list->wrongFilesList));
    // Same goes for alreadyLicensedList
    removeItems(fileList, &(list->alreadyLicensedFiles));
}

// #######################################
// Spectrum struct implementation
// #######################################
void Spectrum::printContent()
{
    for (vector<pair<string, unsigned short>>::const_iterator iterator = wordBasedDictionary.begin(), end = wordBasedDictionary.end(); iterator != end; ++iterator)
    {
        cout << iterator->first << " : count = " << to_string(iterator->second) << endl;
    }
}

uint8_t Spectrum::compareWithSpectrum(LicenseSpectrum *other)
{
    lg::Logger *log = lg::getLogger();
    if (other == nullptr)
    {
        log->logError("Input is nullptr. Aborting comparison", __LINE__, __FILE__, __func__, "Spectrum");
        return 0;
    }

    uint8_t percentageResult = 0;
    unsigned int matchedItemNb = 0;
    unsigned int other_total = other->getTotalWordsNb();
    unsigned int self_total = getTotalWordsNb();
    unsigned int total_words = (other_total >= self_total) ? other_total : self_total;
    unsigned int index = 0;

    // For all words in wordBasedDictionary, check if there is a match
    // between the 'other' words and the current license spectrum
    for (index = 0; index < wordBasedDictionary.size(); index++)
    {
        if(find_if(other->wordBasedDictionary.begin() , other->wordBasedDictionary.end() ,isEqual(wordBasedDictionary[index].first)) != other->wordBasedDictionary.end())
        {
            // IncrementMatch number
            matchedItemNb += wordBasedDictionary[index].second;
        }
    }
    percentageResult = (matchedItemNb * 100) / total_words;
    return percentageResult;
}


void Spectrum::compareWithSpectrumList(vector<LicenseSpectrum* > *other, LicenseInFileMatchResult *match)
{
    lg::Logger *log = lg::getLogger();
    if (match == nullptr)
    {
        log->logError("Input is nullptr. Aborting comparison", __LINE__, __FILE__, __func__, "Spectrum");
        return;
    }

    uint8_t maxMatch = 0;
    uint8_t currentMatch = 0;
    for (unsigned int sp = 0; sp < other->size(); sp++)
    {
        currentMatch = compareWithSpectrum((*other)[sp]);
        if (currentMatch >= maxMatch)
        {
            maxMatch = currentMatch;
            match->degreeOfConfidence = maxMatch;
            match->licenseName = (*other)[sp]->licenseName;
            if (match->degreeOfConfidence > minimumDegreeOfConfidenceRequired)
            {
                log->logInfo("Probably found license in file "+ pu::get_filename(match->filePath), __LINE__, __FILE__, __func__, "Spectrum");
                log->logInfo("Candidate license is : " + match->licenseName + ". Match is : ( " + to_string(match->degreeOfConfidence) + " % )", __LINE__, __FILE__, __func__, "Spectrum");
                match->foundLicense = true;
            }
        }
    }
    if (match->degreeOfConfidence < minimumDegreeOfConfidenceRequired && match->foundLicense == false)
    {
        log->logInfo("Could not find any potential license in file " + pu::get_filename(match->filePath), __LINE__, __FILE__, __func__, "Spectrum");
    }
}


unsigned int Spectrum::getTotalWordsNb()
{
    unsigned int out = 0;
    for (unsigned int i = 0; i < wordBasedDictionary.size(); i++)
    {
        out += wordBasedDictionary[i].second;
    }
    return out;
}



// #######################################
// LicenseSpectrum class implementation
// #######################################


void LicenseSpectrum::printContent()
{
    cout << "License name = " << licenseName << endl;
    Spectrum::printContent();
}
