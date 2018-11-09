/*
PathUtils is a small portion of code which may be used to check filepathes and do some 
stuff to correct filename, filepath etc.

----------------------------------------------------------------------------
Version	|	 Date	 |	Comments
----------------------------------------------------------------------------
0.1     | 07/04/2018 | Implemented first functions
*/

#ifndef PATHUTILS_HEADER
#define PATHUTILS_HEADER

#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;

namespace pathutils {
	// Forward declarations
	std::string remove_illegal_chars(std::string string_to_clean, std::string characters_list = "'-_\\/.|[]}~#{^@$+-=*()%!:;,<>&��012345689");
	const std::string get_parent_dir(std::string);
	const std::string get_filename(std::string input_string);
	const std::string get_extension(std::string input);
	const std::string get_parent_dir(std::string filepath);
    const std::string get_closest_exisiting_parent_dir(std::string targetedPath);
	const std::string get_file_version(std::string filename);
    bool is_POSIX_separator(char in);
    void reformat_POSIX_separator_in_string(std::string &in);
    const std::string join(std::string path1, std::string path2);
	const std::ifstream::pos_type filesize(const char* filename);
	std::string remove_version(std::string filename);
	std::string format_filepath(std::string input_string, std::string extension ="");
	std::string increment_version(std::string filename);
	std::string remove_extension(std::string filename);
	const std::ifstream::pos_type filesize(const char* filename);
    void getAllFilesInDir(vector<string> &vec, string rootPath);
}
#endif // !PATHUTILS_HEADER