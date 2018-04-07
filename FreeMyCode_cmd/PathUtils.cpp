#include "stdafx.h"
#include "PathUtils.h"
#include <string>

using namespace std;

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
#ifdef FILENAME_REGEX
// Return the file's name using the proto-path of the input_string
const std::string pathutils::get_filename(std::string input_string) {
	smatch result_array;
	regex pattern("(?:.*)[\\/\\](.*)$");
	if (!regex_search(input_string, result_array, pattern)) {
		//throw runtime_error(("DirectoryAnalyser::find_extension: cannot retrieve extension from %s", filename));
		//logger.warning("DirectoryAnalyser::find_extension: this file doesn't have any 
		return "";
	}
	return string(result_array[1]);
}
#else
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
#endif //!FILENAME_REGEX

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

const std::string pathutils::get_parent_dir(std::string filepath) {
	//filepath ->  D:/Example.foo/Boo_udi/Bar.1.2.3+65.dump
	string filename = pathutils::get_filename(filepath);
	// filename -> Bar.1.2.3+65.dump
	string truncated_filepath = filepath.substr(0, filepath.size() - filename.size());
	//truncated_filepath -> D:/Example.foo/Boo_udi/
	return truncated_filepath;
}

const std::string pathutils::get_file_version(std::string filename) {
	smatch result_array;
	regex pattern("(\d*)$");
	if (!regex_search(filename, result_array, pattern)) {
		//throw runtime_error(("DirectoryAnalyser::find_extension: cannot retrieve extension from %s", filename));
		//logger.warning("DirectoryAnalyser::find_extension: this file doesn't have any 
		return "";
	}
	return string(result_array[1]);
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
