#ifndef _HEADERS_INTERFACE_H
#define _HEADERS_INTERFACE_H

#include <windows.h>

void interface_show_message_printf(const char* title, const char* fmt, ...);
void interface_show_message_warning_printf(const char* title, const char* fmt, ...);
void interface_show_message_error_printf(const char* title, const char* fmt, ...);
BOOL interface_show_yesno_printf(const char* title, BOOL def, const char* fmt, ...);
int interface_multiple_choice(const char* title, int defaultSel, const char** choicesList, const char* descriptionFmt, ...);
char* interface_select_directory(const char* title, const char* defaultPath, BOOL mustExist, const char* message, ...);

#endif

