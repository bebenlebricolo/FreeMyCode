/*
Here are defined the tools which are used in this project to log messages.
They provide several handlers such as a File Handler (yourfile.log) and a Console Handler.
The logger object is declared as static and its usage its pretty simple :
1) First include the LoggingTools.h header
2) Add a logger::Logger* pointer to your class / project
3) Initialise it with the logger::Logger::get_logger() method. This will return a pointer to the global logger object (static)
4) Add some Handlers to it!  mylogger->add_handler(new logger::ConsoleHandler(logger::ConsoleHandler::Severity Log_Info));
5) Use it like this : mylogger->logInfo(<my Message>, <Line number> , <File name>, <Function name>, <Class name>);
	Note: I like to use the preprocessor tools to handle the __LINE__, __FILE__, __func__ stuff.
----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 07/04/2018 |	First functional version of the LoggingTools. Does not handle the rolling file and max file size yet.
0.2     | 15/04/2018 |  Declared the main logging system as STATIC so that only one logger is used in the project. 
						It makes also development easier as the logger system only relies on ONE class (other ones only need to fetch the Logger class)
*/


#include "stdafx.h"
#include "LoggingTools.h"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <regex>

#include <iomanip>
#include <ctime>
#include <sstream>
#include <iostream>

#include"PathUtils.h"

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

using namespace logger;
using namespace std;
namespace fs = std::experimental::filesystem;
namespace pu = pathutils;

/*
**************************************************************************************
**************************************************************************************
Logger class definition
**************************************************************************************
**************************************************************************************
*/

// Logger pointer initialisation!
logger::Logger* logger::Logger::log_object = NULL;

logger::Logger::Logger(bool show_date) : display_date(show_date)
{
	handlers.clear();
}

logger::Logger::~Logger()
{
	for (unsigned int i = 0; i < handlers.size(); i++) {
		if (handlers[i] != NULL) {
			delete handlers[i];
		}
	}
	handlers.clear();
}

// Check if date should be displayed
bool logger::Logger::is_date_displayed()
{
	return display_date;
}

Logger * logger::Logger::get_logger(bool display_date)
{
	if (Logger::log_object == NULL) {
		Logger::log_object = new Logger(display_date);
	}
	return Logger::log_object;
}

// Returns a formatted version of date based on std::time library (C++11)
// https://stackoverflow.com/a/16358111/8716917
std::string logger::Logger::get_current_date()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::ostringstream oss;
	oss << std::put_time(&tm, "[%d/%m/%Y]:%H-%M'-%S\"");
	return oss.str();
}

// Writes a simple new session message (init logger)
void logger::Logger::log_init_message(const string &message)
{
	string init_message ;
	if (message == "") {
		const unsigned int separator_length = 15;
		const char separator_char = '-';
		for (unsigned int i = 0; i < separator_length; i++) { init_message += separator_char; }
		init_message += " Session initialized ";
		for (unsigned int i = 0; i < separator_length; i++) { init_message += separator_char; }
	}
	else { init_message = message; }
	
	init_message = "[ " + init_message + " ]";
	log_data(init_message, LoggerHandler::Severity::Log_Init);
}

// Adds a handler to the stack
const int Logger::add_handler(LoggerHandler *new_handler) {
	try {
		// TODO : test if a similar handler exist (preventing writing in the same file twice -or more- )
		handlers.push_back(new_handler);
	}
	catch(...){
		// Cannot add handler to handlers vector
		return -1;
	}
	// Success !
	return (handlers.size()-1);
}

// Remove every handlers from stack
void Logger::clear_handlers() {
	handlers.clear();
}

// Select the active state of the targeted handler
void Logger::set_handler_active_state(const unsigned int& index, bool active_state) {
	if (index <= handlers.size() - 1) {
		handlers[index]->set_state(active_state);
	}
	else {
		// throw index out of range exception
	}
}

// Remove a given handler
bool Logger::remove_handler(const unsigned int& index) {
	if (index <= handlers.size()) {
		handlers.erase(handlers.begin() + index);
		return true;
	}
	return false;
}

// Send the signal to log data on each handler (chaining handlers log_data() method)
void Logger::log_data(string message, LoggerHandler::Severity level, unsigned int line, string file, string function, string class_name) {
	string severity_string;
	ostringstream full_message_string;
	string output_string = "";

	switch (level)
	{
	case logger::LoggerHandler::Log_Info:
		severity_string = "INFO";
		break;
	case logger::LoggerHandler::Log_Warning:
		severity_string = "WARNING";
		break;
	case logger::LoggerHandler::Log_Error:
		severity_string = "ERROR";
		break;
	case logger::LoggerHandler::Log_Fatal:
		severity_string = "FATAL";
		break;
	case logger::LoggerHandler::Log_Init:
		severity_string = "INIT";
		break;
	default:
		severity_string = "UNKNOWN";
		break;
	}

	if (file != "") {
		file = pu::get_filename(file);
	}
	ostringstream inspection_block;
	if (file != "" || class_name != "" || function != "" || line != 0) {
		inspection_block << "[ " << file <<(file != "" ? " : " : "") << class_name << (class_name!=""?"::":"") << function << "()-ln:" << line << " ] = ";
	}

	full_message_string << "[" << severity_string << " ]:" << inspection_block.str() << message;
	output_string = full_message_string.str();

	// Add date if necessary
	if (is_date_displayed()) {
		output_string = get_current_date() + " - " + output_string;
	}

	// Send message to every handlers of the stack
	for (unsigned int i = 0; i < handlers.size(); i++) {
		handlers[i]->log_data(output_string, level);
	}
}



//const unsigned int Logger::get_handler_index()
void Logger::logInfo( string message, unsigned int line, string file, string function, string class_name) {
	log_data(message, LoggerHandler::Severity::Log_Info, line, file, function, class_name);
}

void Logger::logWarning(string message, unsigned int line, string file, string function, string class_name) {
	log_data(message, LoggerHandler::Severity::Log_Warning, line, file, function, class_name);
}

void Logger::logError(string message, unsigned int line, string file, string function, string class_name) {
	log_data(message, LoggerHandler::Severity::Log_Error, line, file, function, class_name);
}

void Logger::logFatal(string message, unsigned int line, string file, string function, string class_name) {
	log_data(message, LoggerHandler::Severity::Log_Fatal, line, file, function, class_name);
}


/*
**************************************************************************************
**************************************************************************************
LoggerHandler class definition
**************************************************************************************
**************************************************************************************
*/

LoggerHandler::LoggerHandler(const LoggerHandler::Severity level):is_active(true),sev_level(level){}

void LoggerHandler::set_state(const bool state) { is_active = state; }
const bool LoggerHandler::is_activated(void) { return is_active; }
void LoggerHandler::set_severity(const LoggerHandler::Severity level) { sev_level = level; }
const LoggerHandler::Severity LoggerHandler::get_severity(void) { return sev_level; }



/*
**************************************************************************************
**************************************************************************************
FileHandler class definition
**************************************************************************************
**************************************************************************************
*/

FileHandler::FileHandler(string _filepath, LoggerHandler::Severity level,
	const long max_file_size, bool rolling_file) : 
	LoggerHandler(level) , 	filepath(_filepath),max_size(max_file_size),is_rolling_file(rolling_file)
{
	if (!fs::exists(filepath)) {
		// Create the file
		filepath = pu::format_filepath(filepath,".log");
		logfile = new ofstream(pu::format_filepath(filepath), std::ios::out | std::ios::app);
		logfile->close();
	}
	else {
		// If file exists
		if (!is_rolling_file){
			// And if its size is above the autorized max_size
			if (pu::filesize(filepath.c_str()) >= max_size) {
				// Extract file path properties
				string parent_dir_path = pu::get_parent_dir(filepath);
				string filename = pu::get_filename(filepath);
				string file_ext = pu::get_extension(filename);

				// Increment the current file name (version)
				filename = pu::remove_extension(pu::get_filename(filepath));
				filename = pu::increment_version(filename);
				filename += file_ext;
				filepath = parent_dir_path + filename;

				// Time to create the new file
				logfile = new ofstream(filepath, std::ios::out);
				logfile->close();
			}
		}
		else {
			// Is a rolling file
			// And the file is oversized
			if (pu::filesize(filepath.c_str()) >= max_size) {
			 // TODO:: do some fancy stuff here
			}
		}
	}
}

FileHandler::~FileHandler() {
	if (logfile != NULL) {
		delete logfile;
	}
}

void FileHandler::log_data(const string &message, LoggerHandler::Severity level) {
	if (level != LoggerHandler::Severity::Log_Init) {
		if (level < sev_level) {
			// Do nothing
			return;
		}
	}
	// Else, log data!
	if (logfile == NULL) {
		logfile = new ofstream(filepath, std::ios::out | std::ios::app);
	}
	logfile->open(filepath, std::ios::out | std::ios::app);
	*logfile << message << endl;
	logfile->close();
}

/*
**************************************************************************************
**************************************************************************************
ConsoleHandler class definition
**************************************************************************************
**************************************************************************************
*/

ConsoleHandler::ConsoleHandler(LoggerHandler::Severity level) : LoggerHandler(level) {
}

void ConsoleHandler::log_data(const string &message, LoggerHandler::Severity level) {
	if (level < sev_level) {
		// Do nothing
		return;
	}
	cout << message << endl;
}
