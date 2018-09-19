#include "stdafx.h"
#include "LicenseChecker.h"
#include "LoggingTools.h"
#include "PathUtils.h"
#include "ParsingUtils.h"

#include <fstream>
#include <cstring>

namespace fs = FS_CPP;
namespace pu = pathutils;
using namespace std;

static const char* genericLicenseName = "Generic";
static const char* spectrumNameToken = "Name";
static const char spectrumDelimiter = ':';
static const char spectrumLineDelimiter = '\n';
static const char spectrumCommentDelimiter = '#';
static const char* spectrumSpecificChars = "#:\n";
static const unsigned int spectrumMaxSpecificCharsCount = 10;
//static const char spectrumIgnoreChar = ' ';


// Generate license spectrum from license files
static const uint8_t maxLettersThreshold = 3;

static const unsigned int maxWordSize = 50;

static void trimWhiteSpaces(string &word);
static bool hasOneCharacter(char letter,const char* testChars);


// Parses spectrum Files and for each line, discriminates the left member (called token by now) of the right member
// E.g : foo : 53
//        ^     ^
//        |   right member (number of word occurences)
//    left member, alias "token" (word)
void LicenseChecker::parseSpectrums(std::vector<std::string> &fileList)
{
	logger::Logger* log = logger::Logger::get_logger();
	// first check file exists
	for (unsigned int f = 0; f < fileList.size(); f++)
	{
		string currentFile = fileList[f];
		string filename = pu::get_filename(currentFile);
		if (fs::exists(currentFile) == false)
		{
			log->logWarning("File " + filename + " does not exist.", __LINE__, __FILE__, __func__, "LicenseChecker");
		}
		else
		{
			ifstream spectrumFile(currentFile);
			string currentWord;
			string lineBuffer;
			unsigned int currentWordFrequency;
			if (spectrumFile.is_open() == true)
			{
				log->logWarning("Spectrum file " + filename + " is already opened!", __LINE__, __FILE__, __func__, "LicenseChecker");
				spectrumFile.close();
			}
			spectrumFile.open(currentFile, ios::in);
			LicenseSpectrum *curLicense = new LicenseSpectrum();
			unsigned int lineNumber = 0;
			bool caughtError = false;

			// Parse the current file
			while (getline(spectrumFile, lineBuffer, spectrumLineDelimiter) && caughtError == false)
			{
				lineNumber++;
				bool foundDelimiter = false;
				bool foundComment = false;
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
								foundComment = true;
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
					log->logInfo("Error caught while parsing. Aborting parsing for file " + currentFile);
					delete curLicense;
					break;
				}
			}
			if (caughtError == false)
			{
				log->logInfo("Successfully parsed this file!");
				recordedLicenses.push_back(curLicense);
			}

		}
	}


}

// Trims whitespaces from both side of a given word
// "   foobar   Foobar!foobaaaar..   " will become
// -> "foobar   Foobar!foobaaaar.."
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
	// open file
	// list words which are bigger than 3 letters
	for (unsigned int i = 0; i < filesList.size(); i++)
	{
		// opens the file
		string filePath = filesList[i];
		ifstream file;
		if (fs::exists(filePath))
		{
			file.open(filesList[i]);
		}
		else
		{
			log->logWarning("file " + filePath + " doesn't exist.", __LINE__, __FILE__, __func__, "LicenseChecker");
			// Loop until next file in list
			continue;
		}
		// If we get here, process file.
		string line;
		string licenseName = pu::remove_extension(pu::get_filename(filePath));
		vector<string> wordsList;
		while (getline(file, line))
		{
			vector<string> wordsFromLine;
			tokenizeWords(line, wordsFromLine);

			// Filters out words which are smaller than the required threshold
			for (unsigned int j = 0; j < wordsFromLine.size(); j++)
			{
				string word = wordsFromLine[i];
				if (word.size() > maxLettersThreshold)
				{
					wordsList.push_back(word);
				}
			}
		}
		sort(wordsList.begin(), wordsList.end());
		LicenseSpectrum *lic = new LicenseSpectrum;
		lic->licenseName = licenseName;
		vector<std::pair<std::string, unsigned short int>>::iterator it;
		for (unsigned int j = 0; j < wordsList.size(); j++)
		{
			it = std::find_if(lic->wordBasedDictionary.begin(), lic->wordBasedDictionary.end(), isEqual(wordsList[i]));
			if (it == lic->wordBasedDictionary.end())
			{
				lic->wordBasedDictionary.push_back( { wordsList[i] , 1 } );
			}
			else
			{
				// increment second element of the targeted pair
				it->second++;
			}
		}

		sort(lic->wordBasedDictionary.begin(), lic->wordBasedDictionary.end(), sortAlpha);
		recordedLicenses.push_back(lic);
		file.close();
	}
}


void LicenseChecker::printSpectrums()
{
	for (unsigned int i = 0; i < recordedLicenses.size(); i++)
	{
		recordedLicenses[i]->printContent();
	}
}

// #######################################
// LicenseSpectrum class implementation
// #######################################


void LicenseSpectrum::printContent()
{
	cout << "License name = " << licenseName << endl;
	for (vector<pair<string,unsigned short>>::const_iterator iterator = wordBasedDictionary.begin() , end = wordBasedDictionary.end(); iterator!= end ; ++iterator)
	{
		cout << iterator->first << " : count = " << to_string(iterator->second) << endl;
	}
}
