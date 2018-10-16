/*

------------------
@<FreeMyCode>
FreeMyCode version : 0.1
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

// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//



#pragma once

#include "targetver.h"

#include <stdio.h>
//#include <tchar.h>
#include <vector>
#include <string>

// Handle Microsoft Visual Studio compiler
#ifdef _MSC_VER 
    #define _WIN_OS
    #if _MSC_VER <= 1915
        #define EXPERIMENTAL_FS
    #endif
#endif // _MSC_VER

// Handle GNU GCC / G++ Compilers versions
#ifdef __GNUC__
    #define _UNIX_OS
    #if __GNUC__ < 8
        #define EXPERIMENTAL_FS
    #endif
#endif



#ifdef EXPERIMENTAL_FS
    #define FS_INCLUDE <experimental/filesystem>
    #define FS_CPP std::experimental::filesystem
#else
    #define FS_INCLUDE <filesystem>
    #define FS_CPP std::filesystem
#endif // !EXPERIMENTAL
#include <fstream>
#include <iostream>

