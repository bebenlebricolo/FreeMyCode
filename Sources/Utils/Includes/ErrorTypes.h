#ifndef ERROR_TYPES_HEADER
#define ERROR_TYPES_HEADER

// Generic error types 
// Note : C++ provides exception throwing ; however it could be lighter to use generic errors (such as in C programs).
enum errorType {
    FATAL = -2,
    NO_ERROR = 0,
    BAD_ARGUMENTS,
    NO_SUCH_FILE_OR_DIRECTORY,
    NULL_POINTER
};

// TODO add some custom exception here to replace the above error codes

#endif