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

#include "Command_Line_parser.h"
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

// List of parser results available for this program
vector<string> PRList =
{
    "Directory",
    "License",
    "Config",
    "Logoption",
    "Secondary Input",
    "Spectrums Dir"
};

// Generic error types 
// TODO migrate this elsewhere, we shall not bother with those in the main file
// Note : C++ provides exception throwing ; however it could be lighter to use generic errors (such as in C programs).
enum errorType {
    FATAL = -2,
    NO_ERROR = 0,
    BAD_ARGUMENTS,
    NO_SUCH_FILE_OR_DIRECTORY,
    NULL_POINTER
};

// Parser initialiser and debugging functions
static void init_parser(CommandLineParser *parser);
static errorType check_args(CommandLineParser *parser);
static void printArgs(int argc, char** argv);


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
        mylog->add_handler(new logger::FileHandler(parser->get_arg(PRList[3]), logger::FileHandler::Severity::Log_Debug));
    }else{
        mylog->add_handler(new logger::FileHandler(parser->get_arg(PRList[3]), logger::FileHandler::Severity::Log_Warning));
    }

    // Initialise new logging session (display init message) -> helps identifying new session
    mylog->log_init_message();


    // Abort execution if we cannot find configuration file
    if (config->parse_conf_file(parser->get_arg(PRList[2])) == false) {
        // Ends the programm
        cout << "Programm will quit. Press \" Enter \" to exit." << endl;
        cin.ignore();
        return -1;
    }


    // ------------------- Start working -------------------

    // Make sure passed arguments are reliable, otherwise it could do harm to our files 
    if( check_args(parser) == NO_ERROR )
    {
        // List all files in given directory that match targeted extensions
        vector<string>* files_in_dir = DirectoryAnalyser::get_files_in_dir(parser->get_arg(PRList[0]), config->get_supported_ext_list());
        LicenseWriter writer(parser,config);
        LicenseChecker checker;
        checker.parseSpectrums(parser->get_arg(PRList[5]));
        InOut_CheckLicenses list;
        list.fileList = *files_in_dir;
        checker.checkForLicenses(&list);
        
        // TODO / NOTE : this should be obsolete as list->unlicensedFiles should contain only wanted files
        checker.removeWrongFiles(files_in_dir, &list);
        
        // Build licenses texts in RAM for each file type
        writer.build_formatted_license_list(files_in_dir);
        vector<string> wrongFiles = writer.write_license(files_in_dir);

        cout << "End of Program. Press any key to exit" << endl;
        cin.ignore();
        mylog->logInfo("Program is ending");

        delete (files_in_dir);
        CommandLineParser::destroyParser();
        mylog->destroy_logger();
        config->removeConfig();
    }
    else
    {
        mylog->logFatal("At least one of the input is wrong. Please check your inputs ; aborting execution.");
        return -2;
    }
    return 0;
}


// Initialises our command line parser with its parser results 
// -> simply add one PR with its description, parser will take care of filling them at run time.
static void init_parser(CommandLineParser *parser) {
    ParserResult* directory = new ParserResult(PRList[0], "Directory container : catches the Directory path to be analysed", "<Directory>[Flags...] ");
    ParserResult* license = new ParserResult(PRList[1], "License container : catches the License file path which will be added to the source files. Note : it can also be regular text.", "[foreflag] <License>[Flags ...] ");
    ParserResult* config = new ParserResult(PRList[2], "Config container : catches the Config file path. The config file holds informations about each file type supported by the tool", "[foreflag] <Config>");
    ParserResult* logoption = new ParserResult(PRList[3], "LogOption container : catches the Log file output path. The log file holds informations about the application behavior", "[foreflag] <LogFile path> [Flags ...]");
    ParserResult* secondary_input = new ParserResult(PRList[4], "Secondary input container : catches the secondary input file path where user may define optional tags for license writting", "[foreflag] <Secondary Input file path>");
    ParserResult* spectrums_dir = new ParserResult(PRList[5], "Spectrums dir container : Points to the Spectrums directory used when comparing ", "[foreflag] <Spectrum dir path>");

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

// Checks user input. Passed args should point to something reachable (existing files / directories) for it to complete
static errorType check_args(CommandLineParser *parser)
{
    errorType _rc = NO_ERROR;
    string current_file;
    logger::Logger *logger = logger::Logger::get_logger();
    if(parser == nullptr)
    {
        return NULL_POINTER;
    }

    for(unsigned int i = 0 ;  i < PRList.size() ; i ++)
    {
        current_file = parser->get_arg(PRList[i]);
        if(fs::exists(current_file))
        {
            logger->logInfo("Found " + current_file + " !");
        }
        else if (i == 3)
        {
            logger->logWarning("Logfile does not exist yet. Creation pending.");
            // Does not matter for logfile -> will be created on the fly
        }
        else
        {
            logger->logError("Cannot find " + current_file + ". No such file or directory here." );
            _rc = NO_SUCH_FILE_OR_DIRECTORY;
        }
    }

    return _rc;
}