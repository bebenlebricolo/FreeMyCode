# FreeMyCode
Licenser tool which batch-writes a piece of text inside all specific targeted files of a directory (filtered by their extension : e.g.- {.cpp , .c , .h , .hpp , .py...}

# This tool is still a WORK IN PROGRESS

## External libraries used :
  - Rapidjson  : [Github repo](https://github.com/Tencent/rapidjson/)
### Functionalities to be implemented: (TODO)
- [ ] Check for previous license files
   * Prevent re-licensing of a file
- [ ] Prevent multiple license writing in a file
- [ ] Implement GUI with Qt
- [x] Use tags to add specific content
   - ~~@author~~
   - ~~@license-url~~
   - ~~@freemycode-version~~
   * **Every tag is parsed as "custom" tag and formatted with a YAML-like format**
    ```
    FreeMyCode :
       Version : 0.1
       Author : bebenlebricolo
       Url : https://github.com/bebenlebricolo/FreeMyCode
    ```
- [x] Finalise Secondary Input tags formatting
- [x] Supports FreeMyCode specific tags as well (FreeMyCode version)
- [ ] Exclude directories functionality (exclude path) 
   * This is to prevent re-licensing of external libs, for example
- [x] Add the LicenseWriter component
   * Writes license file according to the parsed arguments
  
### Corrections and improvements : (TODO)
- [ ] Harmonize logger usage
     * Upgrade logger as singleton
     * Do not pass logger ptr as parameter anymore (unnecessary noise)
 - [ ] Input simplification (make it easier to use !)
 
## Cross platform adaptations
- [x] Provide gcc support
     * Create MakeFiles
- [x] Port code to GNU/Linux systems
- [x] Provide back support for Windows systems after code adaptations (CMake used)
 
### Known issues : To be repaired
- ~~Crashes when SecondaryInput file is not found~~  - Done
- ~~Crashes when configuration file is not found~~   - Done
- Input is too complex
