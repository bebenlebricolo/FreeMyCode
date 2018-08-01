/*
This file implements a rather simple solution to analyse a directory thanks to C++ 14 utilites, and mostly returns a vector 
containing all file paths which matches the passed extension vector.

----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 27/03/2018 |	Files are beeign sorted depending on their file extension. Couldn't get it to work with regex 
						-> back to basics : iterating on strings!
0.11	| 01/04/2018 |  Added regex support for find_extension method. Turn on REGEX_BASED define to get it to work
						-> Note: regex based extension finder is slightly slower than the other one (turn off REGEX_BASED)
0.12	| 15/04/2018 |  Added logging tools support (now logs something!)
*/

#include "stdafx.h"
#include <regex>
#include "DirectoryAnalyser.h"
#include "PathUtils.h"
#include "LoggingTools.h"

namespace fs = std::experimental::filesystem;
namespace pu = pathutils;

using namespace std;

// Trial to have a NullPtrException
struct NullPtrException : public runtime_error {
	NullPtrException(std::string message) : runtime_error(message) {
		// logger.error(message);
		std::cout << message << std::endl;
	}
};


logger::Logger* DirectoryAnalyser::logsys = logger::Logger::get_logger();

// Returns a vector of the file paths which contains potential code / text files.
std::vector<std::string>* DirectoryAnalyser::get_files_in_dir(const std::string &root_path, std::string targeted_extensions) {
	
	unsigned file_counter = 0;

	// Uses a pointer in case of big file list -> only uses one memory location for the vector
	std::vector<std::string>* output_vector = new vector<string>();
	if (output_vector == NULL) {
		logsys->logError("Cannot allocate memory for new output_vector",__LINE__,__FILE__,__func__,"DirectoryAnalyser");
		throw runtime_error("DirectoryAnalyser::get_files_in_dir : Cannot allocate memory for new output_vector");
	}

	vector<string> extensions_vec;
	if (targeted_extensions != "") {
		logsys->logInfo("Targeted file extensions are : <" + targeted_extensions + "> ",__LINE__, __FILE__, __func__, "DirectoryAnalyser");
		extensions_vec = split(targeted_extensions.c_str());
	}
	// Else return an empty vector
	else return new vector<string>();

	if (exists(fs::path(root_path))) {
		for (auto & p : fs::recursive_directory_iterator(root_path)) {
			// Ignore directories in paths
			if (!fs::is_directory(p.path())) {
				string str = p.path().filename().generic_string();
				string current_ext;
				try { 
					current_ext = pu::get_extension(str); 
				}
				catch (const runtime_error& error){
					logsys->logError(error.what(), __LINE__, __FILE__, __func__, "DirectoryAnalyser");
					continue;
				}
				logsys->logInfo("Currently evaluated extension : <" + current_ext + "> ", __LINE__, __FILE__, __func__, "DirectoryAnalyser");
				if (found_ext(current_ext, extensions_vec)) {
					file_counter++;
					output_vector->push_back(p.path().generic_string());
				}
			}
		}
		ostringstream message;;
		message << "Supported files count = " << file_counter;
		logsys->logInfo(message.str(), __LINE__, __FILE__, __func__, "DirectoryAnalyser");
	}
	return output_vector;
}

// Breaks a continuous extension string like ".log .h .hpp .c .cpp" into individual tokens:
// .log ; .h ; .hpp ; .c ; .cpp 
std::vector<std::string> DirectoryAnalyser::isolate_file_ext(std::string extension_string) {
	// First atomise the string and extract substrings from it
	vector<string> tokens = split(extension_string.c_str());
	return tokens;
}

/*
#ifdef REGEX_BASED
// Regex-based extension finder
std::string DirectoryAnalyser::find_extension(std::string filename){
	smatch result_array;
	regex pattern("(?:.*)\\.(\\w+)$");
	if (!regex_search(filename, result_array, pattern)) {
		//throw runtime_error(("DirectoryAnalyser::find_extension: cannot retrieve extension from %s", filename));
		//logger.warning("DirectoryAnalyser::find_extension: this file doesn't have any 
		return "";
	}
	return "." + string(result_array[1]);
}

#else

// Non-regex extension finder
// Simply returns the current file extension
std::string DirectoryAnalyser :: find_extension(std::string filename) {
	int substring_length = 0;
	unsigned i = filename.length();
	while (filename[i] != '.' && i != 0) {
		if (character_in_list(filename[i], "\\/ +-_@^%*#\"\'~")) {
			// TODO : handle this with a 
			throw runtime_error("DirectoryAnalyser::find_extension: non regex : extension of file " + filename + " has illegal characters");
			return "";
		}
		substring_length++;
		i--;
	}
	string output = filename.substr(filename.length() - substring_length, substring_length);
	
	// If substring is the same -> we simply have a file without extension
	if (output == filename) return "";
	return output;
}
*/
/*
// detects if a character matches one character in the given string
bool::DirectoryAnalyser::character_in_list(char c, string list) {
	for (unsigned i = 0; i < list.size(); i++) {
		if (c == list[i]) return true;
	}
	return false;
}*/
//#endif


// Finds if a given extension matches the supported extensions.
bool DirectoryAnalyser::found_ext(std::string extension, std::vector <std::string> extension_vect) {
	for (unsigned i = 0; i <extension_vect.size(); i++) {
		if (extension_vect[i] == extension) return true;
	}
	return false;
}


std::string DirectoryAnalyser::convert_string(std::string path) {
	for (unsigned i = 0; i < path.size(); i++) {
			if (path[i] == '\\') path[i] = '/';
		}
	return path;
}

// https://stackoverflow.com/a/53878/8716917
// Very simple and straight forward
vector<string> DirectoryAnalyser::split(const char *str, char c)
{
	vector<string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(string(begin, str));
	} while (0 != *str++);

	return result;
}