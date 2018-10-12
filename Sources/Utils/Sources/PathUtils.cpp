#include "stdafx.h"
#include "PathUtils.h"
#include <string>
#include <cstring>

// Filesystem related stuff
#include FS_INCLUDE

using namespace std;
namespace fs = FS_CPP;


static const char* POSIX_PathSeparators = "/\\";
static const size_t maxPathLength = 1024;

#ifdef _WIN_OS 
    static const char OS_PreferedSeparator = '\\';
#elif defined _UNIX_OS
    static const char OS_PreferedSeparator = '/';
#endif



std::string pathutils::remove_illegal_chars(std::string string_to_clean, std::string characters_list) {
	string output_string;
	for (unsigned int i = 0; i < string_to_clean.size(); i++) {
		for (unsigned int j = 0; j < characters_list.size(); j++) {
			if (string_to_clean[i] == characters_list[j]) {
				continue;
			}
		}
		output_string.push_back(string_to_clean[i]);
	}
	return output_string;
}

// Non-regex extension finder
// Simply returns the current file extension
const std::string pathutils::get_filename(std::string input_string) {
	int substring_length = 0;
	unsigned i = input_string.length();
	while ((input_string[i] != '\\' && input_string[i] != '/') && i != 0) {
		substring_length++;
		i--;
	}
	string output = input_string.substr(input_string.length() - substring_length + 1, substring_length);

	// If substring is the same -> we simply have a file without extension
	if (output == input_string) return "";
	return output;
}

// fetches the file extension and returns it as a string
const std::string pathutils::get_extension(std::string input) {
	smatch result_array;
	regex pattern("(?:.*)\\.(\\w+)$");
	if (!regex_search(input, result_array, pattern)) {
		//throw runtime_error(("DirectoryAnalyser::find_extension: cannot retrieve extension from %s", filename));
		//logger.warning("DirectoryAnalyser::find_extension: this file doesn't have any
		return "";
	}
	return "." + string(result_array[1]);
}



std::string pathutils::format_filepath(std::string input_string, std::string extension) {
	//filepath ->  D:/Example.foo/Boo_udi/Bar.1.2.3+65.dump
	string filename = pathutils::get_filename(input_string);
	// filename -> Bar.1.2.3+65.dump
	string truncated_filepath = pathutils::get_parent_dir(input_string);
	//truncated_filepath -> D:/Example.foo/Boo_udi/
	if (extension != "") {
		if (extension[0] != '.') extension = '.' + extension;
		// Remove old extension
		filename = pathutils::remove_extension(filename);
	}
	// filename -> Bar.1.2.3+65
	filename = pathutils::remove_illegal_chars(filename, "+-=$^%*!:;,#{}[]()@~&");
	// filename -> Bar.1.2.365
	return truncated_filepath + filename + extension;
	// filepath -> D:/Example.foo/Boo_udi/Bar.1.2.365.log

}


// Returns the last directory of the filePath
const std::string pathutils::get_parent_dir(std::string filepath) {
	//filepath ->  D:/Example.foo/Boo_udi/Bar.1.2.3+65.dump
	string filename = pathutils::get_filename(filepath);
	// filename -> Bar.1.2.3+65.dump
	string truncated_filepath = filepath.substr(0, filepath.size() - filename.size());
	//truncated_filepath -> D:/Example.foo/Boo_udi/
	return truncated_filepath;
}

// Returns the first existing directory, reverse propagating from targetedPath to root
const std::string pathutils::get_closest_exisiting_parent_dir(std::string targetedPath)
{
    string outputPath = targetedPath;
    while ((fs::exists(outputPath) == false || fs::is_directory(outputPath) == false) && outputPath != "")
    {
        // Recursively searchs final directory
        outputPath = pathutils::get_parent_dir(outputPath);
    }
    return outputPath;
}

// Checks if current char is considered as a standard POSIX path separator
bool pathutils::is_POSIX_separator(char in)
{
    uint8_t POSIX_sepStrLen = strnlen(POSIX_PathSeparators, 3);
    for (uint8_t i = 0; i < POSIX_sepStrLen ; i++)
    {
        if (in == POSIX_PathSeparators[i])
        {
            return true;
        }
    }
    return false;
}


const std::string pathutils::get_file_version(std::string filename) {
	smatch result_array;
	regex pattern("(\\d*)$");
	if (!regex_search(filename, result_array, pattern)) {
		//throw runtime_error(("DirectoryAnalyser::find_extension: cannot retrieve extension from %s", filename));
		//logger.warning("DirectoryAnalyser::find_extension: this file doesn't have any
		return "";
	}
	return string(result_array[1]);
}

const std::string pathutils::join(std::string path1, std::string path2)
{
    string out;
    reformat_POSIX_separator_in_string(path1);
    reformat_POSIX_separator_in_string(path2);
    const char *p1 = path1.c_str();
    const char *p2 = path2.c_str();
    char tmpString[maxPathLength];
    memset(&tmpString, 0, maxPathLength);
    size_t counter = 0;
    size_t p1End = 0;
    
    // Remove all unnecessary / or \ at the end of the path, C-style
    for (unsigned int i = path1.length() - 1 ; i < path1.length() && is_POSIX_separator(p1[i]) == true ; i--)
    {
        counter++;
    }
    p1End = path1.length() - counter;
    strncpy(tmpString, p1, p1End);
    counter = 0;
    for (unsigned int i = 0; i < path2.length() && is_POSIX_separator(p2[i]) == true; i++)
    {
        counter++;
    }
    tmpString[p1End] = OS_PreferedSeparator;
    strncpy(tmpString + p1End + 1, p2 + counter , path2.length());

    out = string(tmpString);
    return out;
}

// Replaces all separators found in path by the ones prefered by the targeted platform
void pathutils::reformat_POSIX_separator_in_string(std::string &in)
{
    for (unsigned int i = 0; i < in.length(); i++)
    {
        if (is_POSIX_separator(in[i]))
        {
            in[i] = OS_PreferedSeparator;
        }
    }
}


std::string pathutils::remove_version(std::string filename) {
	filename = filename.substr(0, filename.size() - pathutils::get_file_version(filename).size());
	return filename;
}

std::string pathutils::increment_version(std::string filename) {
	string file_number = pathutils::get_file_version(filename);
	int version_number;
	if (!(istringstream(file_number) >> version_number)) version_number = 0;
	filename = pathutils::remove_version(filename) + static_cast<ostringstream*>(&(ostringstream() << version_number + 1))->str();
	return filename;
}

std::string pathutils::remove_extension(std::string filename) {
	filename = filename.substr(0, filename.size() - pathutils::get_extension(filename).size());
	return filename;
}

// https://stackoverflow.com/a/5840160/8716917
const std::ifstream::pos_type pathutils::filesize(const char* filename) {
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

// Finds all files in the given directory path, recursively.
void pathutils::getAllFilesInDir(vector<string> &vec, string rootPath)
{
    if (fs::exists(rootPath))
    {
        for (auto & p : fs::recursive_directory_iterator(fs::path(rootPath)))
        {
            if (!fs::is_directory(p.path()))
            {
                // For each regular file -> push it in the file stack
                vec.push_back(p.path().string());
            }
        }
    }
}
