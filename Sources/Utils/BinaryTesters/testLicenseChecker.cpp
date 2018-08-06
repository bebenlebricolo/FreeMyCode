#include "stdafx.h"
#include "LicenseChecker.h"
#include "LoggingTools.h"
#include <iostream>
#include <filesystem>

namespace fs = std::experimental::filesystem;
static const char* valid_options = "-p -h -u";
using namespace std;
#define DEBUG

static void printUsage();
static void parsingTest(int argc, char** argv);

void parse_option(int argc, char* argv[])
{
	if (argv[1][0] == '-')
	{
		// option detected
		switch (argv[1][1])
		{
		case 'p':
			parsingTest(argc, argv);
			break;
		case 'h':
		case 'u':
		default :
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


int main(int argc, char* argv[])
{
	logger::Logger *log = logger::Logger::get_logger();

#ifdef DEBUG
	log->add_handler(new logger::ConsoleHandler(logger::LoggerHandler::Severity::Log_Debug));
	for (unsigned int i = 0; i < argc; i++)
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
	cout << " -p <file1> <file2>     :  Parse spectrum files and display results on standard output" << endl;
	cout << " ---------------------" << endl;
	cout << " -u / -h                :  Displays this help " << endl << endl;

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
		LicenseChecker licenseChecker;
		vector<string> fileList;
		bool foundErrors = false;
		for (unsigned int i = 2; i < argc; i++)
		{
			string arg = argv[i];
			if (!fs::exists(arg))
			{
				log->logWarning("argument \" " + arg + " \" is not rigth. Please check your input.");
				foundErrors = true;
				continue;
			}
			fileList.push_back(arg);
		}

		licenseChecker.parseSpectrums(fileList);
		licenseChecker.printLicenses();
	}
}