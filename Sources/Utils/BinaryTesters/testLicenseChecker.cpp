#include "stdafx.h"
#include "LicenseChecker.h"
#include "LoggingTools.h"
#include "ConfigTools.h"
#include <iostream>

namespace fs = FS_CPP;
static const char* valid_options = "-p -b -h -u -c";
using namespace std;

static void printUsage();
static void parsingTest(int argc, char** argv);
static void spectrumBuilding(int argc, char** argv);
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
	cout << " -p <file1> <file2> ...                   :  Parse spectrum files and display results on std out" << endl;
	cout << " ---------------------" << endl;
	cout << " -b <file1> <file2> ...                   :  Builds spectrum from regular text file and display results on std out" << endl;
	cout << " ---------------------" << endl;
    cout << " -c <config.json file><file1> <file2> ... :  check for previous licenses in file and displays results on std out" << endl;
    cout << " ---------------------" << endl;
	cout << " -u / -h                :  Displays this help " << endl << endl;

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
			log->logWarning("argument \" " + arg + " \" is not rigth. Please check your input.");
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
		LicensesLists list;

        vector<string> fileList = filterInput(argc, argv);
        ConfObject* config = ConfObject::getConfig();
        config->parse_conf_file(fileList[0]);
        LicenseChecker licenseChecker;
        fileList.erase(fileList.begin());
		list.fileList = fileList;

        licenseChecker.checkForLicenses(&list);
        ConfObject::removeConfig();
    }
}
