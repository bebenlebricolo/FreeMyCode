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
Logging utilities. It comes with a FileHandler and a ConsoleHandler.

----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 07/04/2018 | Implemented Logger, LoggerHandler (abstract) , FileHandler and ConsoleHandler classes
0.2		| 14/04/2018 | Implemented LoggerSlot struct -> handles logger management inside owner classes
*/

#ifndef LOGGINGTOOLS_HEADER
#define LOGGINGTOOLS_HEADER

#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>

namespace logger { 
	
	// Abstract class whose pattern is used to define any specific Handler
	class LoggerHandler {
	public:
		enum Severity { Log_Debug = 0, Log_Info=1, Log_Warning = 2, Log_Error = 3, Log_Fatal = 4 , Log_Init = 10};
		std::string name;

		LoggerHandler(const Severity level);
		virtual ~LoggerHandler();
		virtual void log_data(const std::string &message, Severity sev_level) = 0;
		void set_state(const bool state);
		bool is_activated(void) const;
		void set_severity(const Severity level);
		Severity get_severity(void) const;

	protected:
		Severity sev_level;
		bool is_active;

	};

// Logger class implemented as a singleton
	class Logger {
		std::vector<LoggerHandler*> handlers;

		// generic logging method, called by the public ones
		void log_data(
			std::string message,
			LoggerHandler::Severity level,
			unsigned int line = 0,
			std::string file = "",
			std::string function = "",
			std::string class_name = "");
		bool display_date;
		static Logger* log_object;

	public:
		Logger(bool display_date = true);
		~Logger();
		int add_handler(LoggerHandler *new_handler);
		void clear_handlers(void);
		void set_handler_active_state(const unsigned int& index, bool active_state);
		bool remove_handler(const unsigned int& index);
		bool is_date_displayed();
		std::string get_current_date();
		void log_init_message(const std::string &message = "");
		static Logger* get_logger(bool display_date = true);
		static void destroy_logger();
		
		// Dedicated functions to handle logging functionalities
		void logDebug(
			std::string message = "",
			unsigned int line = 0,
			std::string file = "",
			std::string function = "",
			std::string class_name = "");

		void logError(
			std::string message = "",
			unsigned int line = 0 ,
			std::string file = "" ,
			std::string function="",
			std::string class_name = "");

		void logWarning(
			std::string message = "",
			unsigned int line = 0,
			std::string file = "",
			std::string function = "",
			std::string class_name = "");

		void logInfo(
			std::string message = "",
			unsigned int line = 0,
			std::string file = "",
			std::string function = "",
			std::string class_name = "");

		void logFatal(
			std::string message = "",
			unsigned int line = 0,
			std::string file = "",
			std::string function = "",
			std::string class_name = "");
	};


	// Writes data to a file
	class FileHandler : public LoggerHandler{
		std::string filepath;
		std::ofstream *logfile;
		long max_size;
		bool is_rolling_file;
	public:
		FileHandler(std::string _filepath,Severity level,
			long max_file_size = 1000000000 , bool rolling_file = false);
		~FileHandler();
		void log_data(const std::string &message, Severity sev_level);

	};
	// console handler
	class ConsoleHandler : public LoggerHandler {
	public:
		ConsoleHandler(Severity level);
		void log_data(const std::string &message, Severity sev_level);

	};

    // Aliases 
    Logger* getLogger();
}
#endif // !LOGGINGTOOLS_HEADER