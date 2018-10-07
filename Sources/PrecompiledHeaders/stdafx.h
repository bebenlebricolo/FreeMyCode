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
    #define OS WIN
    #if _MSC_VER <= 1915
        #define EXPERIMENTAL_FS
    #endif
#endif // _MSC_VER

// Handle GNU GCC / G++ Compilers versions
#ifdef __GNUC__
    #define OS UNIX
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

