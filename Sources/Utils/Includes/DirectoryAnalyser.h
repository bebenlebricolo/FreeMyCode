#ifndef DIRECTORYANALYSER_HEADER
#define DIRECTORYANALYSER_HEADER

#include "stdafx.h"

#include <string>
#include <vector>
#include <iostream>
#include "LoggingTools.h"

class DirectoryAnalyser {
	static std::vector<std::string> isolate_file_ext(std::string extension_string);
	static std::vector<std::string> split(const char * str, char c = ' ');
	static bool found_ext(std::string extension, std::vector < std::string> extension_vect);
	static bool character_in_list(char c, std::string c_list);

public:
	static std::vector<std::string>* get_files_in_dir( const std::string &path, std::string targeted_extensions = "" );
	static std::string convert_string(std::string path);
};

#endif // !DIRECTORYANALYSER_HEADER