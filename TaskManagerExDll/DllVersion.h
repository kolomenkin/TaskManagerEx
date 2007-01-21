#ifndef DLLVERSION_H_INCLUDED
#define DLLVERSION_H_INCLUDED

BOOL GetModuleVersion( LPCTSTR lpszModulePath, CString& lpszFileVersion );
BOOL GetModuleProductVersion( LPCTSTR lpszModulePath, CString& lpszProductVersion );
BOOL GetModuleComment( LPCTSTR lpszModulePath, CString& lpszComment );

#endif