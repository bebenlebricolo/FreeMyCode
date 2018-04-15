#ifndef DIRECTORYANALYSER_HEADER
#define DIRECTORYANALYSER_HEADER

#include "stdafx.h"

#include <string>
#include <vector>
#include <iostream>
#include "LoggingTools.h"

//#define REGEX_BASED

// C++ 14
// Sources : 
// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c



class DirectoryAnalyser {
	static logger::Logger* logsys;
	static std::vector<std::string> isolate_file_ext(std::string extension_string);
	static std::vector<std::string> split(const char * str, char c = ' ');
	//static std::string find_extension(std::string filename);
	static bool found_ext(std::string extension, std::vector < std::string> extension_vect);
// Conditional compilation : if regex are not used, then we need this extra method
#ifndef REGEX_BASED
	static bool character_in_list(char c, std::string c_list);
#endif // !REGEX_BASED

public:
	static std::vector<std::string>* get_files_in_dir( const std::string &path, std::string targeted_extensions = "" );
	static std::string convert_string(std::string path);


};

#endif // !DIRECTORYANALYSER_HEADER