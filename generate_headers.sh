#!/bin/bash

set -e

declare -A ZIP_FILES_URL_0=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.1.5/Tools%20Executables/lua-5.1.5_Win32_bin.zip' 
                   [ZIP_NAME]="Lua5.1_x86.zip" 
                   [VAR_PREFIX]="data_zip_x86_lua_5_1_"
                   [HEADER_FILENAME]="data_zip_x86_lua_5_1.h"
                   [SOURCE_FILENAME]="data_zip_x86_lua_5_1.c" )
declare -A ZIP_FILES_URL_1=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.2.4/Tools%20Executables/lua-5.2.4_Win32_bin.zip' 
                   [ZIP_NAME]="Lua5.2_x86.zip"
                   [VAR_PREFIX]="data_zip_x86_lua_5_2_"
                   [HEADER_FILENAME]="data_zip_x86_lua_5_2.h"
                   [SOURCE_FILENAME]="data_zip_x86_lua_5_2.c" )
declare -A ZIP_FILES_URL_2=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.3.6/Tools%20Executables/lua-5.3.6_Win32_bin.zip' 
                   [ZIP_NAME]="Lua5.3_x86.zip"
                   [VAR_PREFIX]="data_zip_x86_lua_5_3_" 
                   [HEADER_FILENAME]="data_zip_x86_lua_5_3.h"
                   [SOURCE_FILENAME]="data_zip_x86_lua_5_3.c" )
declare -A ZIP_FILES_URL_3=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.4.2/Tools%20Executables/lua-5.4.2_Win32_bin.zip' 
                   [ZIP_NAME]="Lua5.4_x86.zip"
                   [VAR_PREFIX]="data_zip_x86_lua_5_4_"
                   [HEADER_FILENAME]="data_zip_x86_lua_5_4.h"
                   [SOURCE_FILENAME]="data_zip_x86_lua_5_4.c" )
declare -A ZIP_FILES_URL_4=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.1.5/Tools%20Executables/lua-5.1.5_Win64_bin.zip' 
                   [ZIP_NAME]="Lua5.1_x86_64.zip" 
                   [VAR_PREFIX]="data_zip_x86_64_lua_5_1_"
                   [HEADER_FILENAME]="data_zip_x86_64_lua_5_1.h"
                   [SOURCE_FILENAME]="data_zip_x86_64_lua_5_1.c" )
declare -A ZIP_FILES_URL_5=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.2.4/Tools%20Executables/lua-5.2.4_Win64_bin.zip' 
                   [ZIP_NAME]="Lua5.2_x86_64.zip"
                   [VAR_PREFIX]="data_zip_x86_64_lua_5_2_"
                   [HEADER_FILENAME]="data_zip_x86_64_lua_5_2.h"
                   [SOURCE_FILENAME]="data_zip_x86_64_lua_5_2.c" )
declare -A ZIP_FILES_URL_6=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.3.6/Tools%20Executables/lua-5.3.6_Win64_bin.zip' 
                   [ZIP_NAME]="Lua5.3_x86_64.zip"
                   [VAR_PREFIX]="data_zip_x86_64_lua_5_3_" 
                   [HEADER_FILENAME]="data_zip_x86_64_lua_5_3.h"
                   [SOURCE_FILENAME]="data_zip_x86_64_lua_5_3.c" )
declare -A ZIP_FILES_URL_7=( [URL]='https://sourceforge.net/projects/luabinaries/files/5.4.2/Tools%20Executables/lua-5.4.2_Win64_bin.zip' 
                   [ZIP_NAME]="Lua5.4_x86_64.zip"
                   [VAR_PREFIX]="data_zip_x86_64_lua_5_4_"
                   [HEADER_FILENAME]="data_zip_x86_64_lua_5_4.h"
                   [SOURCE_FILENAME]="data_zip_x86_64_lua_5_4.c" )

ZIP_FILES_URL=( ZIP_FILES_URL_0 ZIP_FILES_URL_1 ZIP_FILES_URL_2 ZIP_FILES_URL_3 # For x86
                ZIP_FILES_URL_4 ZIP_FILES_URL_5 ZIP_FILES_URL_6 ZIP_FILES_URL_7 ) # For x86_64
SOURCE_PATH="./src/data/"
INCLUDE_PATH="./src/data/"

DATA_PATH="./data"
mkdir -p "$DATA_PATH"
#cat "$DATA_PATH/files.txt" | xargs rm   
#(echo "$DATA_PATH/files.txt" | tee "$DATA_PATH/files.txt") > /dev/null

echo "Downloading files"
declare -n FILE
for FILE in "${ZIP_FILES_URL[@]}"; do
  # Download files
  if [ ! -f "$DATA_PATH/${FILE[ZIP_NAME]}" ]; then
    echo "Downloading $DATA_PATH/${FILE[ZIP_NAME]}"
    wget -O "$DATA_PATH/${FILE[ZIP_NAME]}" -c -nv -4 --show-progress "${FILE[URL]}"
    #curl -s -S "${FILE[URL]}" -o "$DATA_PATH/${FILE[ZIP_NAME]}"
    #(echo "$DATA_PATH/${FILE[ZIP_NAME]}" | tee -a "$DATA_PATH/files.txt") > /dev/null
  else
    echo "File $DATA_PATH/${FILE[ZIP_NAME]} is present"
  fi
done

echo "Generating headers and source files.."
for FILE in "${ZIP_FILES_URL[@]}"; do
  SOURCE_FILE_PATH="$SOURCE_PATH/${FILE[SOURCE_FILENAME]}"
  HEADER_FILE_PATH="$INCLUDE_PATH/${FILE[HEADER_FILENAME]}"
  ZIP_PATH="$DATA_PATH/${FILE[ZIP_NAME]}"
  ZIP_SIZE_HUMAN=$(du -b "$ZIP_PATH" | grep -o "[0-9]\+" | head -1 | xargs printf "%'d")
  ZIP_SIZE=$(du -b "$ZIP_PATH" | grep -o "[0-9]\+" | head -1 | xargs printf "%d")
  #echo $ZIP_SIZE $ZIP_SIZE_HUMAN $(du -b "$ZIP_PATH" | grep -o "^[0-9]\+" | head -1)
  
  echo "Generating $SOURCE_FILE_PATH"
  DEF_NAME="_HEADER_$RANDOM"
  # Creating header file
  (echo "#ifndef $DEF_NAME" &&
    echo "#define $DEF_NAME" &&
    echo "#include <stdint.h>" &&
    echo "//Zip file size: $ZIP_SIZE_HUMAN bytes" &&
    echo "extern const size_t ${FILE[VAR_PREFIX]}length;" &&
    echo "//Raw bytes of the zip file" &&
    echo "extern const char ${FILE[VAR_PREFIX]}data[];" &&
    echo "//Zip filename: \"${FILE[ZIP_NAME]}\"" &&
    echo "extern const char* ${FILE[VAR_PREFIX]}filename;" &&
    echo "#endif") | tee $HEADER_FILE_PATH > /dev/null
  
  BYTES_BEFORE_NEWLINE=500
  # Creating C source containing the raw bytes
  (echo "#include <stdint.h>" &&
    echo "const size_t ${FILE[VAR_PREFIX]}length = $ZIP_SIZE;" &&
    echo "const char* ${FILE[VAR_PREFIX]}filename = \"${FILE[ZIP_NAME]}\";" &&
    printf "const char ${FILE[VAR_PREFIX]}data[] = {" &&
    #Generate hex dump delimited by comma
    (cat "$ZIP_PATH" | hexdump -ve '1/1 "0x%02X, "') &&
    printf "};") | tee $SOURCE_FILE_PATH > /dev/null 
done
unset FILE


