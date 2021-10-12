#include "util.h"
#include <io.h>     // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <stdio.h>
#include <shlobj.h>

BOOL util_is_directory_exists(const char* path) {
  if(_access(path, 0) == 0) {
    struct stat status;
    stat(path, &status);

    return (status.st_mode & S_IFDIR) != 0;
  }
  return FALSE;
}

char* util_readline() {
  size_t lineSz = 0;
  char* line = NULL;
  int curChar = 0;
  do {
    curChar = getchar();
    if (curChar != EOF) {
      //Empty line
      if (curChar == '\n' && line == NULL) {
        return calloc(1, 1);
      }
      
      //Filling line
      if (line == NULL) {
        line = malloc(2);
        line[0] = curChar;
        line[1] = 0;
        lineSz++;
      } else {
        line = realloc(line, lineSz + 2);
        line[lineSz] = curChar;
        line[lineSz + 1] = 0;
        lineSz++;
      }
    } else {
      free(line);
      return NULL;
    }
  } while (curChar != '\n');
  
  return line;
}

int util_create_directory_structure(const char* path) {
  return SHCreateDirectoryEx( NULL, path, NULL ); 
}


