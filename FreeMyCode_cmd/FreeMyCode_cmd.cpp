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

*/

#include "stdafx.h"
#include <iostream>
#include "Command_Line_parser.h"
#include "DirectoryAnalyser.h"
#include "ConfigTools.h"
#include "LoggingTools.h"

using namespace std;

void init_Parser(CommandLineParser *parser);

int main(int argc , char* argv[])
{
	CommandLineParser parser;
	init_Parser(&parser);
	parser.parse_arguments(argc, argv);
	cout << "Analyse flag state = " <<  (parser.get_flag("--Analyse") ? "true" : "false" ) << endl;
	cout << "Directory argument = : " << parser.get_arg("Directory") << endl;

	vector<string>* files_in_dir = DirectoryAnalyser::get_files_in_dir(parser.get_arg("Directory"),".ilk .pdb .exe .h");
	delete (files_in_dir);

	ConfObject config;
	//config.parse_conf_file("D:\\Dossier Perso\\Programmation\\Visual C++\\FreeMyCode_cmd\\FreeMyCode_cmd\\Config.json");
	config.parse_conf_file(parser.get_arg("Config"));
	cout << ".cpp file extension end bloc comment tag is : "<< config.get_bloc_comment_end(".cpp") << endl;

	logger::Logger mylog;
	mylog.add_handler(new logger::FileHandler("D:/Dossier Perso/Programmation/Visual C++/FreeMyCode_cmd/Debug/Dummy_Directory/mylauwwg1.dump", logger::LoggerHandler::Severity::Log_Info));
	mylog.add_handler(new logger::ConsoleHandler(logger::LoggerHandler::Severity::Log_Debug));

	mylog.log_init_message();
	mylog.logInfo("Coucou 1", __LINE__, __FILE__, __func__);
	mylog.logError("Coucou error", __LINE__, __FILE__, __func__);


	cin.ignore();

	return 0;
}

void init_Parser(CommandLineParser *parser) {
	ParserResult* directory = new ParserResult("Directory", "Directory container : catches the Directory path to be analysed", "<Directory>[Flags...] ");
	ParserResult* license = new ParserResult("License", "License container : catches the License file path which will be added to the source files. Note : it can also be regular text.", "<License>[Flags ...] ");
	ParserResult* config = new ParserResult("Config", "Config container : catches the Config file path. The config file holds informations about each file type supported by the tool", "[foreflag] <Config>");
	ParserResult* logoption = new ParserResult("Logoption", "LogOption container : catches the Log file output path. The log file holds informations about the application behavior", "[foreflag] <LogFile path> [Flags ...]");

	directory->available_flags.push_back(ParserFlags(vector<string>({ "-A" , "--Analyse" }),
		"Analyse flag : Tells the application to first analyse the directory and build a map of the directory for a faster run",
		""));
	license->available_flags.push_back(ParserFlags(vector<string>({ "-p", "--prepend" }),
		"Prepend flag : write the passed text on top of the targeted file", ""));
	license->available_flags.push_back(ParserFlags(vector<string>({ "-a", "--append" }),
		"Append flag : write the passed text at the end of the targeted file", ""));
	config->available_flags.push_back(ParserFlags(vector<string>({ "-c", "--config" }),
		"Config flag : allows the parser to know we are targeting a config flag (foreflag)", ""));
	logoption->available_flags.push_back(ParserFlags(vector<string>({ "-L" , "--Log" }),
		"Log flag : Tells the application to use a log file (foreflag)", ""));
	logoption->available_flags.push_back(ParserFlags(vector<string>({ "-v" , "--verbose" }),
		"Verbose flag : Tells the application to set the logger to verbose (info, warning and error logs will be written", ""));

	parser->add_container(new vector<ParserResult*>({ directory,license,config,logoption }));
}