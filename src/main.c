#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <windows.h> 

#include "data/data_zip_x86_lua_5_1.h"
#include "data/data_zip_x86_lua_5_2.h"
#include "data/data_zip_x86_lua_5_3.h"
#include "data/data_zip_x86_lua_5_4.h"
#include "data/data_zip_x86_64_lua_5_1.h"
#include "data/data_zip_x86_64_lua_5_2.h"
#include "data/data_zip_x86_64_lua_5_3.h"
#include "data/data_zip_x86_64_lua_5_4.h"
#include "interface.h"
#include "util.h"
#include "zip.h"

#define LUA_PREFIX_X86 "C:\\Program Files (x86)\\Lua"
#define LUA_PREFIX_X86_64 "C:\\Program Files\\Lua"
#define MAIN_AUTHOR "Fox"
#define MAIN_VERSION "1.0"

#define MAIN_ARCH_UNKNOWN   0
#define MAIN_ARCH_X86 1
#define MAIN_ARCH_MIPS 2
#define MAIN_ARCH_ALPHA 3
#define MAIN_ARCH_PPC 4
#define MAIN_ARCH_SHX 5
#define MAIN_ARCH_ARM 6
#define MAIN_ARCH_IA64 7
#define MAIN_ARCH_ALPHA64 8
#define MAIN_ARCH_MSIL 9
#define MAIN_ARCH_X86_64 10
#define MAIN_ARCH_AARCH64 13
#define MAIN_NUM_ARCHS 14

int main_getArchitectureRaw() {
  SYSTEM_INFO sysInfo;
  GetNativeSystemInfo(&sysInfo);
  return sysInfo.wProcessorArchitecture;
}

int main_getArchitecture() {
  SYSTEM_INFO sysInfo;
  GetNativeSystemInfo(&sysInfo);
  switch (sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_IA64:
      return MAIN_ARCH_IA64;
    case PROCESSOR_ARCHITECTURE_ALPHA64:
      return MAIN_ARCH_ALPHA64;
    case PROCESSOR_ARCHITECTURE_ALPHA:
      return MAIN_ARCH_ALPHA;
    case PROCESSOR_ARCHITECTURE_INTEL:
      return MAIN_ARCH_X86;
    case PROCESSOR_ARCHITECTURE_AMD64:
      return MAIN_ARCH_X86_64;
    case PROCESSOR_ARCHITECTURE_ARM64:
      return MAIN_ARCH_AARCH64;
    case PROCESSOR_ARCHITECTURE_ARM:
      return MAIN_ARCH_ARM;
    case PROCESSOR_ARCHITECTURE_SHX:
      return MAIN_ARCH_SHX;
    case PROCESSOR_ARCHITECTURE_PPC:
      return MAIN_ARCH_PPC;
    case PROCESSOR_ARCHITECTURE_MSIL:
      return MAIN_ARCH_MSIL;
    case PROCESSOR_ARCHITECTURE_MIPS:
      return MAIN_ARCH_MIPS;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
      return MAIN_ARCH_UNKNOWN;
    default:
      return MAIN_ARCH_UNKNOWN;
  }
}

const char* main_getArchitectureName(int arch) {
  switch (arch) {
    case MAIN_ARCH_IA64:
      return "Itanium64";
    case MAIN_ARCH_ALPHA64:
      return "Alpha64";
    case MAIN_ARCH_ALPHA:
      return "Alpha";
    case MAIN_ARCH_X86:
      return "x86";
    case MAIN_ARCH_X86_64:
      return "x86_64";
    case MAIN_ARCH_AARCH64:
      return "AArch64";
    case MAIN_ARCH_ARM:
      return "ARM";
    case MAIN_ARCH_SHX:
      return "SHX";
    case MAIN_ARCH_PPC:
      return "PowerPC";
    case MAIN_ARCH_MSIL:
      return "MSIL";
    case MAIN_ARCH_MIPS:
      return "MIPS";
    case MAIN_ARCH_UNKNOWN:
      return "Unknown";
    default:
      return "Unknown";
  }
}

typedef struct {
  int versionNumber;
  const char* verName;
  const char* architectureName;
  int architectureID;
  const char* installPath;
  void* udata;
  const char* zipName;
  const size_t zipLength;
  const char* rawData; 
} LuaVersion;

char main_tmpFilePath[MAX_PATH] = {0};
void abort_main() {
  interface_show_message_error_printf("Setup", "Aborted installation!");
  puts("\nPress enter to continue...");
  if (main_tmpFilePath[0] != 0) {
    DeleteFile(main_tmpFilePath);
  }
  free(util_readline());
  exit(EXIT_FAILURE);
}

int main_on_extract_entry(const char *filename, void* _arg) {
  static int i = 0;
  int* arg = _arg;
  arg[0]++;
  printf("Extracted: %s (%d of %d)\n", filename, arg[0], arg[1]);
  
  return 0;
}

int main() { 
  //Initialize COM library
  //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  
  LuaVersion versions[] = {
    {500100, "Lua 5.1", "x86", MAIN_ARCH_X86, LUA_PREFIX_X86 "\\5.1", NULL, data_zip_x86_lua_5_1_filename, data_zip_x86_lua_5_1_length, data_zip_x86_lua_5_1_data},
    {500200, "Lua 5.2", "x86", MAIN_ARCH_X86, LUA_PREFIX_X86 "\\5.2", NULL, data_zip_x86_lua_5_2_filename, data_zip_x86_lua_5_2_length, data_zip_x86_lua_5_2_data},
    {500300, "Lua 5.3", "x86", MAIN_ARCH_X86, LUA_PREFIX_X86 "\\5.3", NULL, data_zip_x86_lua_5_3_filename, data_zip_x86_lua_5_3_length, data_zip_x86_lua_5_3_data},
    {500400, "Lua 5.4", "x86", MAIN_ARCH_X86, LUA_PREFIX_X86 "\\5.4", NULL, data_zip_x86_lua_5_4_filename, data_zip_x86_lua_5_4_length, data_zip_x86_lua_5_4_data},
    
    {500100, "Lua 5.1", "x86_64", MAIN_ARCH_X86_64, LUA_PREFIX_X86_64 "\\5.1", NULL, data_zip_x86_64_lua_5_1_filename, data_zip_x86_64_lua_5_1_length, data_zip_x86_64_lua_5_1_data},
    {500200, "Lua 5.2", "x86_64", MAIN_ARCH_X86_64, LUA_PREFIX_X86_64 "\\5.2", NULL, data_zip_x86_64_lua_5_2_filename, data_zip_x86_64_lua_5_2_length, data_zip_x86_64_lua_5_2_data},
    {500300, "Lua 5.3", "x86_64", MAIN_ARCH_X86_64, LUA_PREFIX_X86_64 "\\5.3", NULL, data_zip_x86_64_lua_5_3_filename, data_zip_x86_64_lua_5_3_length, data_zip_x86_64_lua_5_3_data},
    {500400, "Lua 5.4", "x86_64", MAIN_ARCH_X86_64, LUA_PREFIX_X86_64 "\\5.4", NULL, data_zip_x86_64_lua_5_4_filename, data_zip_x86_64_lua_5_4_length, data_zip_x86_64_lua_5_4_data},
  };
  int numVersions = sizeof(versions) / sizeof(LuaVersion);
  int recommendedVersion = -1;
  int currentVersionNumber = 0;
  int cpuArch = MAIN_ARCH_ALPHA64; //main_getArchitecture();
  int cpuArchRaw = PROCESSOR_ARCHITECTURE_ALPHA64; //main_getArchitectureRaw();
  
  printf("CPU Architecture: %s\n", main_getArchitectureName(cpuArch));
  assert(cpuArch != MAIN_ARCH_UNKNOWN);
  
  //Find best version 
  //Criteria
  //1. Same architecture as the CPU
  //2. Latest version for that CPU
  for (int i = 0; i < numVersions; i++) {
    if (versions[i].architectureID == cpuArch && versions[i].versionNumber > currentVersionNumber) {
      recommendedVersion = i;
      currentVersionNumber = versions[i].versionNumber;
    }
  }
  
  if (recommendedVersion != -1) {
    printf("Best version %s %s\n", versions[recommendedVersion].verName, versions[recommendedVersion].architectureName);
  } else {
    printf("No recommended version for this CPU\n");
  }
  
  interface_show_message_printf("About", "LuaInstaller %s Copyright (C) 2021 %s\n"
                                "Licensed under MIT License", MAIN_VERSION, MAIN_AUTHOR); 
  
  if (recommendedVersion < 0) {
    interface_show_message_warning_printf("Warning", "No architecture compatible with this CPU. You can select one but no guarantee it will work.");
  }
  
  /*const char* choices[] = {
    "Lua 5.1 x86", "Install Lua 5.1 x86 version",
    "Lua 5.2 x86", "Install Lua 5.2 x86 version",
    "Lua 5.3 x86", "Install Lua 5.3 x86 version",
    "Lua 5.4 x86", "Install Lua 5.4 x86 version",
    NULL
  };*/
  
  const char** choices = malloc(sizeof(const char*) * (numVersions + 1) * 2);
  int choicesIndex = 0;
  choices[numVersions * 2] = NULL;
  choices[numVersions * 2 + 1] = NULL;
  
  if (recommendedVersion != -1) {
    char* tmp;
    asprintf(&tmp, "%s %s", versions[recommendedVersion].verName, versions[recommendedVersion].architectureName);
    choices[0] = tmp;
    asprintf(&tmp, "Install %s %s (Recommended)", versions[recommendedVersion].verName, versions[recommendedVersion].architectureName);
    choices[1] = tmp;
    choicesIndex++;
  }
  
  int* choicesMapper = malloc(sizeof(int) * numVersions);
  choicesMapper[0] = recommendedVersion;
  
  for (int i = 0; i < numVersions; i++) {
    if (i != recommendedVersion) {
      //choices[(choicesIndex * 2)] = versions[i].verName;
      
      char* string2;
      asprintf(&string2, "%s %s", versions[i].verName, versions[i].architectureName);
      choices[choicesIndex * 2] = string2;
      
      char* string;
      asprintf(&string, "Install %s %s", versions[i].verName, versions[i].architectureName);
      choices[choicesIndex * 2 + 1] = string;
      choicesMapper[choicesIndex] = i;
      choicesIndex++;
    } 
  }
  
  int choice = interface_multiple_choice("Select version", recommendedVersion != -1 ? 0 : -1, choices, "Select lua version to install"); 
  /*do {
    choice = interface_multiple_choice("Select version", 0, choices, "Select lua version to install");
    if (choice < 0) {
      interface_show_message_error_printf("Select version", "You must select one version to install! (Control + C to cancel on the console)");
    }
  } while (choice < 0);*/
  
  if (choice < 0) {
    abort_main();
  }
  
  choice = choicesMapper[choice];
  
  printf("Installing %s %s\n", versions[choice].verName, versions[choice].architectureName);
  
  for (int i = 0; i < numVersions; i++) {
    free((void*) choices[i * 2]);
    free((void*) choices[i * 2 + 1]);
  }
  
  int err;
  /*err = util_create_directory_structure(versions[choice].installPath);
  if (err != ERROR_SUCCESS ) {
    if (err = ERROR_ALREADY_EXISTS) {
    } else if (err = ERROR_ACCESS_DENIED) {
      if (interface_show_yesno_printf("Setup", FALSE, "You didnt have sufficient permission to do this. Do you want to continue? ")) {
        abort_main();
      }
    } else {
      printf("Cant create directory reason 0x%08X", err);
    }
  }*/
  
  char* installPath = interface_select_directory("Select installation directory", versions[choice].installPath, FALSE, "Choose install location");
  if (installPath == NULL) {
    abort_main();
  }
  printf("Installation path: '%s'\n", installPath);
  printf("Creating directory '%s'\n", installPath);
  
  /*if (strcmp(installPath, versions[choice].installPath) != 0) {
    util_create_directory_structure(installPath);
  }*/
  
  //Creating TMP file
  char tmpDirectory[MAX_PATH];
  GetTempPath(MAX_PATH, tmpDirectory);
  printf("Temporary path: %s\n", tmpDirectory);
  GetTempFileName(tmpDirectory, "LUA_SETUP", 0, main_tmpFilePath);
  FILE* handle = fopen(main_tmpFilePath, "wb");
  assert(handle != NULL);
  size_t writeSize = fwrite(versions[choice].rawData, 1, versions[choice].zipLength, handle) ;
  if (writeSize != versions[choice].zipLength) {
    printf("Cant write temporary file '%s'\n", main_tmpFilePath);
    fclose(handle);
    abort_main();
  }
  fclose(handle);
  
  //Extract
  int arg[2] = {0, 0};
  struct zip_t* zipHandle = zip_open(main_tmpFilePath, 0, 'r');
  arg[1] = zip_entries_total(zipHandle);
  zip_close(zipHandle);
  err = zip_extract(main_tmpFilePath, installPath, main_on_extract_entry, &arg);
  
  if (err < 0) {
    printf("Cant extract zip file %s\n", main_tmpFilePath);
    exit(1);
    abort_main();
  }
  
  DeleteFile(main_tmpFilePath);
  free(installPath);
  free(choicesMapper);
  exit(EXIT_SUCCESS);
}





