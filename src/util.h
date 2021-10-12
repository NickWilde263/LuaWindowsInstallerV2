#ifndef _HEADERS_UTIL_H
#define _HEADERS_UTIL_H

#include <windows.h>
BOOL util_is_directory_exists(const char* path);
char* util_readline();
int util_create_directory_structure(const char* path);

#endif 




