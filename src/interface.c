#define _GNU_SOURCE

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <shlobj.h>

#include "config.h"
#include "util.h"

struct InterfaceMultipleChoiceData {
  const char** choicesList;
  int* choiceOutput;
  const char* description;
  int numOfChoices;
  int defaultSel; 
  
  HWND* radioButtons;
  HWND okButton;
  HWND verticalScrollbar;
  HWND horizontalScrollbar;
  BOOL buttonPressed;
  int maxClientWidth;
  int maxClientHeight;
};

void interface_show_message_warning_printf(const char* title, const char* fmt, ...) {
  char* message;
  va_list vararg;
  va_start(vararg, fmt);
  #ifdef CONFIG_IS_CONSOLE
  printf("!!!WARNING!!!\n");
  printf("Title: %s\n", title);
  vprintf(fmt, vararg);
  printf("\n!!!WARNING!!!\n");
  puts("\n");
  #else
  vasprintf(&message, fmt, vararg);
  #endif
  va_end(vararg);
  #ifndef CONFIG_IS_CONSOLE
  MessageBox(NULL, message, title, MB_OK | MB_ICONWARNING);
  #endif 
}

void interface_show_message_error_printf(const char* title, const char* fmt, ...) {
  char* message;
  va_list vararg;
  va_start(vararg, fmt);
  #ifdef CONFIG_IS_CONSOLE
  printf("!!!ERROR!!!\n");
  printf("Title: %s\n", title);
  vprintf(fmt, vararg);
  printf("\n!!!ERROR!!!\n");
  puts("\n");
  #else
  vasprintf(&message, fmt, vararg);
  #endif
  va_end(vararg);
  #ifndef CONFIG_IS_CONSOLE
  MessageBox(NULL, message, title, MB_OK | MB_ICONEXCLAMATION);
  #endif 
}

BOOL interface_show_yesno_printf(const char* title, BOOL def, const char* fmt, ...) {
  char* message;
  BOOL result;
  va_list vararg;
  va_start(vararg, fmt);
  #ifdef CONFIG_IS_CONSOLE
  printf("Title: %s\n", title);
  vprintf(fmt, vararg);
  printf("[%s/%s] ", def ? "Y" : "y", def ? "n" : "N");
  int response = getchar();
  if (response == EOF) {
    result = def;
  } else if (response == 'y' || response == 'Y') {
    result = TRUE;
  } else if (response == 'n' || response == 'N') {
    result = FALSE;
  } else {
    result = def;
  }
  puts("\n");
  #else
  vasprintf(&message, fmt, vararg);
  #endif
  va_end(vararg);
  #ifndef CONFIG_IS_CONSOLE
  MessageBox(NULL, message, title, MB_YESNO);
  #endif 
  
  return result;
}

int CALLBACK interface_internal_select_director_browse_callback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
  if (uMsg == BFFM_INITIALIZED) {
    LPCTSTR path = (LPCTSTR) lpData;
    //SendMessage(hwnd, BFFM_SETEXPANDED, TRUE, (LPARAM) path);
    SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM) path);
    printf("SELECT PATH: %s\n", path);
  }
  return 0;
}

char* interface_select_directory(const char* title, const char* defaultPath, int mustExist, const char* messageFmt, ...) {
  char* path;
  va_list vararg;
  va_start(vararg, messageFmt);
  #ifdef CONFIG_IS_CONSOLE
  printf("Title: %s\n", title);
  vprintf(messageFmt, vararg);
  puts("\n");
  printf("Select a directory (Default: '%s')\n", defaultPath);
  
  char* line = NULL;
  int curChar = 0;
  char* defaultPathClone = strdup(defaultPath);
  BOOL isExist = FALSE;
  do {
    if (line != NULL) {
      free(line);
      line = NULL;
    }
    
    printf("> ");
    
    size_t lineLen = 0;
    do {
      curChar = getchar();
      if (curChar == EOF) {
        if (line != NULL) {
          free(line);
          line = NULL;
        }
        break;
      } else {
        if (curChar != '\n') {
          if (line == NULL) {
            line = malloc(2);
            line[0] = (char) curChar;
            line[1] = 0;
            lineLen++;
          } else {
            line = realloc(line, lineLen + 2);
            line[lineLen] = (char) curChar;
            line[lineLen + 1] = 0;
            lineLen++;
          }
        } 
      }
    } while (curChar != '\n');
    
    if (curChar == EOF) {
      line = NULL;
      break;
    }
    
    if (line == NULL) {
      line = defaultPathClone;
    } else {
      if (line[0] == '\n') {
        free(line);
        line = defaultPathClone;
        break;
      }
    }
    
    if (mustExist) {
      if (!util_is_directory_exists(line)) {
        printf("%s directory is not exist\n", line);
      } 
    }
  } while (!util_is_directory_exists(line) && mustExist && (curChar != EOF));
  if (line != defaultPathClone) {
    free(defaultPathClone);
  }
  path = line;
  
  #else
  char* message;
  vasprintf(&message, messageFmt, vararg);
  #endif
  va_end(vararg);
  #ifndef CONFIG_IS_CONSOLE
  BOOL exist = FALSE;
  char* defaultPathClone = strdup(defaultPath);
  char* tmp;
  asprintf(&tmp, "%s\n\n%s", title, message);
  while (1) {
    BROWSEINFO browseInfo = {};
    browseInfo.lParam = (LPARAM) defaultPath;
    browseInfo.lpfn = interface_internal_select_director_browse_callback;
    browseInfo.lpszTitle = tmp;
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS;;
    LPITEMIDLIST pathObj = SHBrowseForFolder(&browseInfo);
    
    if (pathObj == NULL) {
      path = NULL;
      break;
    } else {
      path = malloc(PATH_MAX);
      SHGetPathFromIDList(pathObj, path);
    }
    
    if (path == defaultPathClone) {
      break;
    }
    
    exist = util_is_directory_exists(path);
    if (exist) {
      free(defaultPathClone);
      break;
    } else {
      free(path);
      path = NULL;
    }
  }
  free(tmp);
  free(message);
  #endif 
  
  return path;
}

void interface_show_message_printf(const char* title, const char* fmt, ...) {
  char* message;
  va_list vararg;
  va_start(vararg, fmt);
  #ifdef CONFIG_IS_CONSOLE
  printf("Title: %s\n", title);
  vprintf(fmt, vararg);
  puts("\n");
  #else
  vasprintf(&message, fmt, vararg);
  #endif
  va_end(vararg);
  #ifndef CONFIG_IS_CONSOLE
  MessageBox(NULL, message, title, MB_OK);
  #endif 
}

LRESULT CALLBACK interface_internal_multiple_choice_win_callback(HWND winHandle, UINT message, WPARAM data, LPARAM data2) {
  struct InterfaceMultipleChoiceData* choicesContext = (void*) GetWindowLongPtr(winHandle, GWLP_USERDATA);
  
  switch (message) {
    case WM_NCCREATE: {
      //puts("Window Pre-create (WM_CREATE)!");
      CREATESTRUCT* tmp = (CREATESTRUCT*) data2;
      choicesContext = tmp->lpCreateParams;
      SetWindowLongPtr(winHandle, GWLP_USERDATA, (long) choicesContext);
      *choicesContext->choiceOutput = choicesContext->defaultSel;
      choicesContext->buttonPressed = FALSE;
      break;
    }
    case WM_CREATE: {
      //puts("Window Create (WM_CREATE)!");
      const char** choicesList = choicesContext->choicesList;
      RECT size = {};
      GetClientRect(winHandle, &size);
      
      int width = size.right - size.left;
      int height = size.bottom - size.top;
      int maxWidthUse = width;
      int maxHeightUse = 0;
      int x = 10;
      int y = 10;
      HWND text = CreateWindowEx(0,
                     "STATIC",
                     choicesContext->description,
                     WS_CHILD | WS_VISIBLE | SS_LEFT,
                     x, y, width - (x * 2), 16, 
                     winHandle,
                     NULL, GetModuleHandle(NULL),
                     NULL); 
      y += 16 + 5;
      choicesContext->radioButtons = malloc(sizeof(HWND) * choicesContext->numOfChoices);
      
      for (int i = 0; choicesList[i] != NULL; i = i + 2) {
        HWND radioButton = CreateWindowEx(0,
                                        "BUTTON",
                                        choicesList[i],
                                        BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | (i == 0 ? WS_GROUP : 0),
                                        x, y, width - (x * 2), 17, 
                                        winHandle,
                                        NULL, GetModuleHandle(NULL),
                                        NULL);
        choicesContext->radioButtons[i / 2] = radioButton;
        if ((i / 2) == choicesContext->defaultSel) {
          SendMessage(radioButton, BM_SETCHECK, BST_CHECKED, 0);
        }
        
        y += 17 + 1;
        if (choicesList[i + 1] != NULL) {
          CreateWindowEx(0,
                         "STATIC",
                         choicesList[i + 1],
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         x + 20, y, width - (x * 2), 17, 
                         winHandle,
                         NULL, GetModuleHandle(NULL),
                         NULL);
          y += 17 + 10;
        }
      }
      
      choicesContext->okButton = CreateWindowEx(0,
                                     "BUTTON",
                                     "Ok",
                                     WS_CHILD | WS_VISIBLE,
                                     x, y, width - (x * 2), 27, 
                                     winHandle,
                                     NULL, GetModuleHandle(NULL),
                                     NULL);
      y = y + 27;
      
      /*choicesContext->verticalScrollbar = CreateWindowEx(0,
                                     "SCROLLBAR",
                                     NULL,
                                     WS_CHILD | WS_VISIBLE | SBS_VERT,
                                     width - 20, 0, 20, height, 
                                     winHandle,
                                     NULL, GetModuleHandle(NULL),
                                     NULL); 
      choicesContext->horizontalScrollbar = CreateWindowEx(0,
                                     "SCROLLBAR",
                                     NULL,
                                     WS_CHILD | WS_VISIBLE | SBS_HORZ,
                                     0, height - 20, width, 20, 
                                     winHandle,
                                     NULL, GetModuleHandle(NULL),
                                     NULL);*/  
      //x = x + 20;
      //y = y + 20;
      
      choicesContext->maxClientHeight = y;
      choicesContext->maxClientWidth = maxWidthUse;
      
      return 0;
    }
    case WM_SIZE: {
      //Resize event
      int width = (int) LOWORD(data2);
      int height = (int) HIWORD(data2);
      
      SCROLLINFO vscrollInfo = {};
      SCROLLINFO hscrollInfo = {};
      GetScrollInfo(winHandle, SB_VERT, &vscrollInfo);
      GetScrollInfo(winHandle, SB_HORZ, &hscrollInfo);
      
      vscrollInfo.cbSize = sizeof(vscrollInfo);
      hscrollInfo.cbSize = sizeof(hscrollInfo);
      
      vscrollInfo.fMask = SIF_RANGE | SIF_DISABLENOSCROLL | SIF_PAGE;
      hscrollInfo.fMask = SIF_RANGE | SIF_DISABLENOSCROLL | SIF_PAGE;
      
      vscrollInfo.nMin = 0;
      hscrollInfo.nMin = 0;
      
      vscrollInfo.nMax = choicesContext->maxClientHeight;
      hscrollInfo.nMax = choicesContext->maxClientWidth;
      
      vscrollInfo.nPos = 0;
      hscrollInfo.nPos = 0;
      
      vscrollInfo.nPage = height;//height * 100 * 60 / 100;
      hscrollInfo.nPage = width;//width * 100 * 60 / 100;
      
      SetScrollInfo(winHandle, SB_VERT, &vscrollInfo, TRUE);
      SetScrollInfo(winHandle, SB_HORZ, &hscrollInfo, TRUE);
      
      return 0;
    }
    case WM_VSCROLL: {
      //Vertical scroll
      SCROLLINFO si;
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(winHandle, SB_VERT, &si);
      int pos = si.nPos;
      
      switch (LOWORD(data)) {
        case SB_TOP:
          si.nPos = si.nMin;
          break;
        case SB_BOTTOM:
          si.nPos = si.nMax;
          break;
        case SB_LINEUP:
          si.nPos -= 1;
          break;
        case SB_LINEDOWN:
          si.nPos += 1;
          break;
        case SB_PAGEUP:
          si.nPos -= si.nPage;
          break;
        case SB_PAGEDOWN:
          si.nPos += si.nPage;
          break;
        case SB_THUMBTRACK:
          si.nPos = si.nTrackPos;
          break;
        default:
          break; 
      }
      
      si.fMask = SIF_POS;
      SetScrollInfo(winHandle, SB_VERT, &si, TRUE);
      GetScrollInfo(winHandle, SB_VERT, &si);
      
      if (si.nPos != pos) {
        ScrollWindow(winHandle, 0, pos - si.nPos, NULL, NULL);
      }
      
      return 0;
    }
    case WM_HSCROLL: {
      //Horiontal scroll
      SCROLLINFO si;
      si.cbSize = sizeof (si);
      si.fMask  = SIF_ALL;
      GetScrollInfo(winHandle, SB_HORZ, &si);
      int pos = si.nPos;
      
      switch (LOWORD(data)) {
        case SB_LINELEFT: 
          si.nPos -= 1;
          break;
        case SB_LINERIGHT: 
          si.nPos += 1;
          break;
        case SB_PAGELEFT:
          si.nPos -= si.nPage;
          break;
        case SB_PAGERIGHT:
          si.nPos += si.nPage;
          break;
        case SB_THUMBTRACK: 
          si.nPos = si.nTrackPos;
          break;
        default:
          break;
      }
      
      si.fMask = SIF_POS;
      SetScrollInfo(winHandle, SB_HORZ, &si, TRUE);
      GetScrollInfo(winHandle, SB_HORZ, &si);
      
      if (si.nPos != pos) {
        ScrollWindow(winHandle, pos - si.nPos, 0, NULL, NULL);
      }
      
      return 0;
    }
    case WM_COMMAND: {//BN_CLICKED: {
      if (data == BN_CLICKED && (HWND) data2 == choicesContext->okButton) {
        BOOL selectedOne = FALSE;
        for (int i = 0; i < choicesContext->numOfChoices; i++) {
          if (SendMessage(choicesContext->radioButtons[i], BM_GETCHECK, 0, 0) == BST_CHECKED) {
            *choicesContext->choiceOutput = i;
            selectedOne = TRUE;
            break;
          }
        }
        
        if (!selectedOne) {
          MessageBox(winHandle, "You need to select atleast one (close this window to cancel)", "Setup", MB_OK | MB_ICONEXCLAMATION);
        } else {
          PostMessage(winHandle, WM_CLOSE, 0, 0);
          choicesContext->buttonPressed = TRUE;
        }
      }
      return 0;
    }
    
    case WM_PAINT: {
      //puts("Window paint (WM_PAINT)!");
      PAINTSTRUCT paintCtx;
      HDC displayContext = BeginPaint(winHandle, &paintCtx);
      FillRect(displayContext, &paintCtx.rcPaint, (HBRUSH) (COLOR_WINDOW));
      
      EndPaint(winHandle, &paintCtx);
      return 0;
    }
    case WM_CLOSE: {
      //puts("Window close (WM_CLOSE)!");
      BOOL selectedOne = FALSE;
      if (choicesContext->buttonPressed) {
        for (int i = 0; i < choicesContext->numOfChoices; i++) {
          if (SendMessage(choicesContext->radioButtons[i], BM_GETCHECK, 0, 0) == BST_CHECKED) {
            *choicesContext->choiceOutput = i;
            selectedOne = TRUE;
            break;
          }
        }
      }
      
      if (!selectedOne) {
        if (choicesContext->buttonPressed) {
          assert(FALSE /* Should not be reach */);
          //*choicesContext->choiceOutput = choicesContext->defaultSel;
        } else {
          *choicesContext->choiceOutput = -1;
        }
      }
      
      free(choicesContext->radioButtons);
      
      PostQuitMessage(0);
      DestroyWindow(winHandle);
    }
  } 
  return DefWindowProc(winHandle, message, data, data2);
}

int interface_multiple_choice(const char* title, int defaultSel, const char** choicesList, const char* descriptionFmt, ...) {
  int choice;
  char* descriptionStr;
  
  va_list vararg;
  va_start(vararg, descriptionFmt);
  vasprintf(&descriptionStr, descriptionFmt, vararg);
  va_end(vararg); 
  #ifdef CONFIG_IS_CONSOLE
  //char* message = calloc(1, 1);
  
  printf("Title: %s\n", title);
  puts(descriptionStr);
  puts("\n");
  int numChoices = 0;
  for (int i = 0; choicesList[i] != NULL; i = i + 2) {
    char* tmp;
    asprintf(&tmp, "%d) %s", i / 2 + 1, choicesList[i]);
    printf("%s", tmp);
    if (choicesList[i + 1] != NULL) {
      printf("    %s", choicesList[i + 1]);
    }
    puts("");
    numChoices++;
    free(tmp);
  }
  
  assert(defaultSel < numChoices);
  
  do {
    printf("Select between 1 to %d\n", numChoices, defaultSel);
    if (defaultSel >= 0) {
      printf("Default: %d (%s)\n", defaultSel + 1, choicesList[defaultSel * 2]);
    }
    printf("> ");
    char* line = util_readline();
    if (line == NULL) {
      choice = -1;
      break;
    }
    
    if (line[0] == 0) {
      choice = defaultSel;
      break;
    }
    
    choice = strtol(line, NULL, 10);
    free(line);
  } while (!(choice >= 1 && choice <= numChoices));
  
  if (choice > 0) {
    choice -= 1;
    printf("Selected %d (%s)\n", choice + 1, choicesList[choice * 2]);
  } 
  
  puts("\n");
  #else
  WNDCLASS winClass = {};
  winClass.lpfnWndProc = interface_internal_multiple_choice_win_callback;
  winClass.hInstance = GetModuleHandle(NULL);
  winClass.lpszClassName = "main.class.win.select_version";
  winClass.style = CS_HREDRAW | CS_VREDRAW;
  
  if (RegisterClass(&winClass) == 0) {
    int tmp = GetLastError();
    //printf("[RegisterClass] 0x%08X   %d\n", tmp, tmp);
    abort();
  }
  
  struct InterfaceMultipleChoiceData* choicesContext = malloc(sizeof(struct InterfaceMultipleChoiceData));
  choicesContext->choiceOutput = &choice;
  choicesContext->choicesList = choicesList;
  choicesContext->defaultSel = defaultSel;
  choicesContext->description = descriptionStr;
  for (int i = 0; choicesList[i] != NULL; i = i + 2) {
    choicesContext->numOfChoices++;
  }
  
  assert(defaultSel < choicesContext->numOfChoices);
  
  HWND winHandle = CreateWindowEx(
    0,
    "main.class.win.select_version",
    "Select version", 
    WS_TILEDWINDOW | WS_VISIBLE | WS_CAPTION | WS_HSCROLL | WS_VSCROLL,
    CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, 
    NULL, NULL, GetModuleHandle(NULL), 
    choicesContext //essentially userdata
  );
  if (winHandle == NULL) {
    int tmp = GetLastError();
    //printf("[CreateWindow] 0x%08X   %d\n", tmp, tmp);
    abort();
  }
  
  ShowWindow(winHandle, SW_NORMAL);
  
  //Events/message poll loop
  MSG message = {};
  do {
    GetMessage(&message, NULL, 0, 0);
    TranslateMessage(&message);
    DispatchMessage(&message);
  } while (message.message != WM_QUIT);
  
  free(choicesContext);
  
  UnregisterClass("main.class.win.select_version", GetModuleHandle(NULL));
  #endif
  
  return choice;
}



