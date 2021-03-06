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
#include "ConfigTools.h"
#include <iostream>
#include FS_INCLUDE


namespace fs = FS_CPP;
static const char* valid_options = "-p -b -h -u -c -w";
using namespace std;

static void printUsage();
static void parsingTest(int argc, char** argv);
static void spectrumBuilding(int argc, char** argv);
static void writeSpectrumsOnDisk(int argc, char** argv);
static void checkLicenseInFile(int argc, char** argv);
static vector<string> filterInput(int argc, char** argv);

void parse_option(int argc, char* argv[])
{
    if (argc > 1)
    {

        if (argv[1][0] == '-')
        {
            // option detected
            switch (argv[1][1])
            {
            case 'p':
                parsingTest(argc, argv);
                break;
            case 'b':
                spectrumBuilding(argc, argv);
                break;
            case 'c':
                checkLicenseInFile(argc, argv);
                break;
            case 'w':
                writeSpectrumsOnDisk(argc, argv);
                break;
            case 'h':
            case 'u':
            default:
                printUsage();
                break;
            }
        }
        else
        {
            cerr << "Wrong option detected. Please use one of the following options with the '-' starting character : " << valid_options << endl;
            printUsage();
        }
    }
    else
    {
        cerr << "No input detected. Please provide arguments to command" << endl;
        printUsage();
    }
}


int main(int argc, char* argv[])
{
	logger::Logger *log = logger::Logger::get_logger();

#ifdef DEBUG
	log->add_handler(new logger::ConsoleHandler(logger::LoggerHandler::Severity::Log_Debug));
	for ( int i = 0; i < argc; i++)
	{
		log->logDebug("arg[" + to_string(i) + "] = " + string(argv[i]));
	}
#else
	log->add_handler(new logger::ConsoleHandler(logger::LoggerHandler::Severity::Log_Info));
#endif

	parse_option(argc, argv);

	cin.ignore();
	return 0;
}

void printUsage()
{
	cout << "Usage : " << endl;
	cout << " -p [<file1> <file2> || <directory> ] ... "<< endl;
    cout << "    -> Parse spectrum files and display results on std out" << endl;
	cout << " ---------------------" << endl;
    cout << " -b <file1> <file2> ... " << endl;
    cout << "    -> Builds spectrum from regular text file and display results on std out" << endl;
	cout << " ---------------------" << endl;
    cout << " -c <config.json file> <file> <License1> <License2> ... " << endl;
    cout << "    -> Check for previous licenses in file and displays results on std out" << endl;
    cout << " ---------------------" << endl;
    cout << " -w [<file1> <file2> || <directory> ] ... <out dir> " << endl;
    cout << "    -> Parse licences from given files and write them on disk" << endl;
    cout << " ---------------------" << endl;
    cout << " -u / -h " << endl;
    cout << "    -> Displays this help " << endl << endl;

}


vector<string> filterInput(int argc, char** argv)
{
	logger::Logger *log = logger::Logger::get_logger();
	vector<string> fileList;
	for ( int i = 2; i < argc; i++)
	{
		string arg = argv[i];
		if (!fs::exists(arg))
		{
			log->logWarning("argument \" " + arg + " \" is not right : Does not exist ! Please check your input.");
			continue;
		}
		fileList.push_back(arg);
	}
	return fileList;
}

void parsingTest(int argc, char** argv)
{
	logger::Logger *log = logger::Logger::get_logger();
	log->logInfo("Starting test tool for LicenseChecker library : parser unit.");
	if (argc < 3)
	{
		log->logError("Please provide at least one file path.");
	}
	else
	{
		vector<string> fileList = filterInput(argc, argv);
		LicenseChecker licenseChecker;
		licenseChecker.parseSpectrums(fileList);
		licenseChecker.printLicenses();
	}
}

void spectrumBuilding(int argc, char** argv)
{
	logger::Logger *log = logger::Logger::get_logger();
	log->logInfo("Starting test tool for LicenseChecker library : parser unit.");
	if (argc < 3)
	{
		log->logError("Please provide at least one file path.");
	}
	else
	{
		vector<string> fileList = filterInput(argc, argv);
		LicenseChecker licenseChecker;
		licenseChecker.buildLicensesSpectrum(fileList);
		licenseChecker.printSpectrums();
	}
}



static void checkLicenseInFile(int argc, char** argv)
{
    logger::Logger *log = logger::Logger::get_logger();
    log->logInfo("Starting test tool for LicenseChecker library : parser unit.");
    if (argc < 4)
    {
        log->logError("Please provide at least one file path and a valid config file");
    }
    else
    {
        InOut_CheckLicenses list;


        vector<string> fileList = filterInput(argc, argv);
        ConfObject* config = ConfObject::getConfig();
        config->parse_conf_file(fileList[0]);
        LicenseChecker licenseChecker;

        // Remove config.json file from input list
        fileList.erase(fileList.begin());
		list.fileList.push_back(fileList[0]);
        fileList.erase(fileList.begin());
        licenseChecker.parseSpectrums(fileList);

        licenseChecker.checkForLicenses(&list);
        ConfObject::removeConfig();
    }
}


void writeSpectrumsOnDisk(int argc, char **argv)
{
    logger::Logger *log = logger::Logger::get_logger();
    log->logInfo("Starting test tool for LicenseChecker library : parser unit.");
    if (argc < 4)
    {
        log->logError("Please provide at least one file path and a valid config file");
    }
    else
    {

        // Last path is used as destination directory
        vector<string> fileList = filterInput(argc - 1, argv);
        LicenseChecker licenseChecker;
        licenseChecker.buildLicensesSpectrum(fileList);

        // Write to the output directory (last argument in list)
        licenseChecker.writeSpectrumsOnDisk(argv[argc - 1]);

        ConfObject::removeConfig();
    }
}

