# FreeMyCode
Licenser tool which batch-writes a piece of text inside all specific targeted files of a directory (filtered by their extension : e.g.- {.cpp , .c , .h , .hpp , .py...}

# This tool is still a WORK IN PROGRESS
### Functionalities to be implemented:
- [x] Add the LicenseWriter component
   * Writes license file according to the parsed arguments
- [ ] Unstabilities repair:
     - [ ] SecondaryInput not found -> protect against crashes
     - [ ] License file not found -> abort execution
     - ...
- [ ] Check for previous license files
   * Prevent re-licensing of a file
- [ ] Prevent multiple license writing in a file
- [ ] Implement GUI with Qt
- [x] Use tags to add specific content
   * @author
   * @license-url
   * @freemycode-version
- [x] Finalise Secondary Input tags formatting
- [x] Supports FreeMyCode specific tags as well (FreeMyCode version)
- [ ] Exclude directories functionality (exclude path) 
   * This is to prevent re-licensing of external libs, for example
