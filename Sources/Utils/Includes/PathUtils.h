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