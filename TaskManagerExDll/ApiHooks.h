#ifndef APIHOOKS_H_INCLUDED
#define APIHOOKS_H_INCLUDED

#include "HookImportFunction.h"

#define USER32APIHOOKCOUNT             1
#define HOOKSETMENU                    0

extern HOOKFUNCDESC   pUser32APIHooks[];
extern PROC           pUser32APIOrigins[];

typedef BOOL (WINAPI *FnSetMenu)(
  HWND hWnd,  // handle to window
  HMENU hMenu // handle to menu
);

BOOL WINAPI SetMenuHooked(
  HWND hWnd,  // handle to window
  HMENU hMenu // handle to menu
);

BOOL HookImportedFunctions();

#endif