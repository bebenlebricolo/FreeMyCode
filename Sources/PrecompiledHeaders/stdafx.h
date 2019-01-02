// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//



#ifndef STDAFX_HEADER
#define STDAFX_HEADER

    #include "targetver.h"

    #include <stdio.h>
    #include <vector>
    #include <string>
    #include "ErrorTypes.h"

    // Handle Microsoft Visual Studio compiler
    #ifdef _MSC_VER 
        #define _WIN_OS
        #if _MSC_VER <= 1916
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

#endif