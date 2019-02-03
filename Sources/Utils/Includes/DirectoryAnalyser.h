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

#ifndef DIRECTORYANALYSER_HEADER
#define DIRECTORYANALYSER_HEADER

#include "stdafx.h"

#include <string>
#include <vector>
#include <iostream>
#include "LoggingTools.h"

// Directory analyser class whose role is to list all files in the targeted directory, and 
// to store each file's extension.
// Files extensions are stored to target only supported files types (by comparing both lists)
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