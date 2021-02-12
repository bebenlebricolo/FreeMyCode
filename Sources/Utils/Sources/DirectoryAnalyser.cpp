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
#include FS_INCLUDE

namespace fs = FS_CPP;
namespace pu = pathutils;

using namespace std;

// Trial to have a NullPtrException
struct NullPtrException : public runtime_error {
    NullPtrException(std::string message) : runtime_error(message) {
        // logger.error(message);
        std::cout << message << std::endl;
    }
};




// Getting list of files using C++ standard features
// C++ 14
// Sources :
// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c

// Returns a vector of the file paths which contains potential code / text files.
std::vector<std::string>* DirectoryAnalyser::get_files_in_dir(const std::string &root_path, std::string targeted_extensions) {

    logger::Logger *logsys = logger::getLogger();
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
// Splits a string in two halves based on a given delimiter (character)
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