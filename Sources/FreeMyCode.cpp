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

/*
FreeMyCode is a piece of software which is aimed to help you license your projects under open-sources licenses.
It may also be used to batch write a specific portion of text into the files which are matching the extensions supported by the tool.
----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 22/03/2018 | Implemented CommandLineParser tool
0.2		| 27/03/2018 | Implemented DirectoryAnalyser tool
0.25	| 01/04/2018 | Improved support of DirectoryAnalyser tool
0.3     | 04/04/2018 | Implemented ConfigTools
0.35	| 07/04/2018 | Implemented LogginTools
0.36	| 15/04/2018 | Added logging facilities to ConfigTools, CommandLineParser and DirectoryAnalyser
0.37	| 23/04/2018 | Added first support for LicenseWriter tool. Added special flags for LicenseWriter

*/

#include "stdafx.h"
#include <iostream>

#include "CommandLineParser.h"
#include "DirectoryAnalyser.h"
#include "ConfigTools.h"
#include "LoggingTools.h"
#include "LicenseWriter.h"
#include "LicenseChecker.h"
#include "PathUtils.h"

// Filesystem C++ standard library inclusion depends on which version of compiler is actually running on your machine
// this is why we need to abstract inclusion mechanism. Could either be std::experiment::filesystem or std::filesystem (C++17 native feature ) 
#include FS_INCLUDE

// Used for debugging purposes : will log some more informations such as complete input command
#define DEBUG


using namespace std;
namespace fs = FS_CPP;

// Parser initialiser and debugging functions
static errorType init_parser(CommandLineParser *parser);
static void printArgs(int argc, char** argv);
static void print_wrong_files_list(vector<string> &file_list);


int main(int argc , char* argv[])
{
    // ------------------- Initialisation -------------------

    // First, take care of logging tools
    logger::Logger* mylog = logger::Logger::get_logger();

    // Parsers and results
    CommandLineParser *parser = CommandLineParser::getParser();
    ConfObject *config = ConfObject::getConfig();
    
    mylog->add_handler(new logger::ConsoleHandler(logger::ConsoleHandler::Severity::Log_Info));
    init_parser(parser);
    printArgs(argc, argv);

    // Parse arguments passed by the user and exit if arg count is not right
    if (parser->parse_arguments(argc, argv) == false) {
        // Ends the programm
        cout << "Programm will quit. Press \" Enter \" to exit." << endl;
        cin.ignore();
        return 1;
    }

    // Check if user asked for a verbose output
    if (parser->get_flag("-v") == true){
        mylog->add_handler(new logger::FileHandler(parser->get_arg(LOGGING_FILE), logger::FileHandler::Severity::Log_Debug));
    }else{
        mylog->add_handler(new logger::FileHandler(parser->get_arg(LOGGING_FILE), logger::FileHandler::Severity::Log_Warning));
    }

    // Initialise new logging session (display init message) -> helps identifying new session
    mylog->log_init_message();


    // Abort execution if we cannot find configuration file
    if (false == config->parse_conf_file(parser->get_arg(CONFIG_FILE))) {
        // Ends the programm
        cout << "Programm will quit. Press \" Enter \" to exit." << endl;
        cin.ignore();
        return -1;
    }


    // ------------------- Start working -------------------

    // Make sure passed arguments are reliable, otherwise it could do harm to our files
    // Otherwise, close program immediately 
    if( parser->check_args() != NO_ERROR )
    {
        mylog->logFatal("At least one of the input is wrong. Please check your inputs ; aborting execution.");
        return -2;
    }

    // Normal excution flow

    // List all files in given directory that match targeted extensions
    vector<string>* files_in_dir = DirectoryAnalyser::get_files_in_dir(parser->get_arg(PROJECT_DIRECTORY), config->get_supported_ext_list());
    LicenseWriter writer(parser,config);
    LicenseChecker checker;
    
    // Parse spectrums from files given as input
    checker.parseSpectrums(parser->get_arg(SPECTRUMS_DIRECTORY));
    InOut_CheckLicenses list;
    list.fileList = *files_in_dir;
    checker.checkForLicenses(&list);
    
    // TODO / NOTE : this should be obsolete as list->unlicensedFiles should contain only wanted files
    checker.removeWrongFiles(files_in_dir, &list);
    
    // Build licenses texts in RAM for each file type
    writer.build_formatted_license_list(files_in_dir);
    vector<string> wrongFiles = writer.write_license(files_in_dir);
    print_wrong_files_list(wrongFiles);

    cout << "End of Program. Press any key to exit" << endl;
    cin.ignore();
    mylog->logInfo("Program is ending");


    // Do not forget to destroy all dynamically allocated objects
    delete (files_in_dir);
    CommandLineParser::destroyParser();
    mylog->destroy_logger();
    config->removeConfig();

    return 0;
}


// Initialises our command line parser with its parser results 
// -> simply add one PR with its description, parser will take care of filling them at run time.
static errorType init_parser(CommandLineParser *parser) {
    logger::Logger *logger = logger::Logger::get_logger();

    ParserResult* directory = nullptr;
    ParserResult* license = nullptr;
    ParserResult* config = nullptr;
    ParserResult* logoption = nullptr;
    ParserResult* secondary_input = nullptr;
    ParserResult* spectrums_dir = nullptr;

    // Project directory to be analysed
    try
    {    
        directory = new ParserResult( PROJECT_DIRECTORY,
                                                    convert_ParserResultElement_to_string(PROJECT_DIRECTORY),
                                                    ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY,
                                                    "Directory container : catches the Directory path to be analysed", "<Directory>[Flags...] ");
    } 
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }

    // Targeted license which will be used to write to files    
    try
    {
        license = new ParserResult(   TARGETED_LICENSE,
                                                    convert_ParserResultElement_to_string(TARGETED_LICENSE),
                                                    ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY,
                                                    "License container : catches the License file path which will be added to the source files. "
                                                    "Note : it can also be regular text.", "[foreflag] <License>[Flags ...] ");
    }
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }
    
    // Configuration file used to tell FreeMyCode how to write to files
    try
    {
        config = new ParserResult( CONFIG_FILE,
                                                convert_ParserResultElement_to_string(CONFIG_FILE),
                                                ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY,
                                                "Config container : catches the Config file path. The config file holds informations about "
                                                "each file type supported by the tool", "[foreflag] <Config>");
    }
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }

    // Logging file which will be used
    try
    {
        logoption = new ParserResult( LOGGING_FILE,
                                                    convert_ParserResultElement_to_string(LOGGING_FILE),
                                                    ParserResult::ContentType::FILE_TO_BE_CREATED,
                                                    "LogOption container : catches the Log file output path. The log file holds informations "
                                                    "about the application behavior", "[foreflag] <LogFile path> [Flags ...]");
    }
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }

    // Secondary input file which is used to handle user preferences
    try
    {
        secondary_input = new ParserResult( SECONDARY_INPUT_FILE,
                                                          convert_ParserResultElement_to_string(SECONDARY_INPUT_FILE),
                                                          ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY,
                                                          "Secondary input container : catches the secondary input file path where user may define "
                                                          "optional tags for license writting", "[foreflag] <Secondary Input file path>");

    }
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }

    // Spectrums directory which contains all spectrum files to be used for the current session
    try
    {
        spectrums_dir = new ParserResult( SPECTRUMS_DIRECTORY,
                                                        convert_ParserResultElement_to_string(SPECTRUMS_DIRECTORY),
                                                        ParserResult::ContentType::EXISTING_FILE_OR_DIRECTORY,
                                                        "Spectrums dir container : Points to the Spectrums directory used when comparing ",
                                                        "[foreflag] <Spectrum dir path>");
    }
    catch (string e)
    {
        logger->logError(e);
        return FATAL;
    }

    // Directory container
    directory->available_flags.push_back(ParserFlags(vector<string>({ "-A" , "--Analyse" }),
        "Analyse flag : Tells the application to first analyse the directory and build a map of the directory for a faster run", ""));
    
    // License container
    license->available_flags.push_back(ParserFlags(vector<string>({ "-p", "--prepend" }),
        "Prepend flag : write the passed text on top of the targeted file", "<License> --prepend"));
    license->available_flags.push_back(ParserFlags(vector<string>({ "-a", "--append" }),
        "Append flag : write the passed text at the end of the targeted file", "<License> --append"));
    license->available_flags.push_back(ParserFlags(vector<string>({ "-lf","--log-formatted-license" }),
        "Log formatted license flag : logs the content of each pre-formatted license","<License> -lf"));
    license->available_flags.push_back(ParserFlags(vector<string>({ "-fsl","--force-single-line" }),
        "Force single line flag : Forces the usage of single line comments if possible","<License> -fsl"));
    
    // Config container
    config->available_flags.push_back(ParserFlags(vector<string>({ "-c", "--config" }),
        "Config flag : allows the parser to know we are targeting a config flag (foreflag)", "-c <ConfigFile>"));
   
    // Log option container
    logoption->available_flags.push_back(ParserFlags(vector<string>({ "-L" , "--Log" }),
        "Log flag : Tells the application to use a log file (foreflag)", "-L <LogFile>"));
    logoption->available_flags.push_back(ParserFlags(vector<string>({ "-v" , "--verbose" }),
        "Verbose flag : Tells the application to set the logger to verbose (info, warning and error logs will be written)", "<LogFile> -v"));
   
    // Secondary Input container
    secondary_input->available_flags.push_back(ParserFlags(vector<string>({ "-Si","--Secondary-Input" }),
        "Secondary Input flag : Adds a container for secondary input filepath","-Si <SecondaryInputFile>"));
    
    // Spectrums directory container
    spectrums_dir->available_flags.push_back(ParserFlags(vector<string>({ "-Sd","--Spectrums-dir" }),
        "Spectrums Directory flag : Adds a container for spectrums directory path", "-Sd <SpectrumsDir>"));
    spectrums_dir->available_flags.push_back(ParserFlags(vector<string>({ "-lb","--log-block-comments" }),
        "log block comment flag : logs block comments when found in parsed files", "<SpectrumsDir> -lb"));

    parser->add_container(new vector<ParserResult*>({ directory,license,config,logoption,secondary_input ,spectrums_dir}));
    return NO_ERROR;
}

// Debugging stuff : show full command line arguments (really usefull while working with tools such as Visual Studio C++)
static void printArgs(int argc, char** argv)
{
#ifdef DEBUG
    cout << "\nFull command line \n" << endl;
    for(int i = 0 ; i < argc ; i++)
    {
        cout << argv[i] << ' ' ;
    }
    cout << endl << endl;

    // Prints all args sequentially
    for (int i = 0; i < argc; i++)
    {
        cout << "Arg" << to_string(i) << " = " << argv[i] << endl;
    }
    cout << endl;
#endif
}


void print_wrong_files_list(vector<string> &file_list)
{
    logger::Logger *logger = logger::Logger::get_logger();
    if(file_list.size() != 0)
    {
        logger->logWarning("Printing list of files that FreeMyCode could not process : " );
        for(unsigned int i = 0 ; i < file_list.size() ; i++)
        {
            logger->logWarning("     " + file_list[i]);
        }
    }
    else
    {
        logger->logInfo("Good news ! FreeMyCode has successfully processed all required files!");
    }
}
