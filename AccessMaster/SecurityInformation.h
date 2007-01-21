//////////////////////////////////////////////////////////////
// File:		// SecurityInformation.h
// File time:	// 25.04.2005	12:34
// Version: 	// 1.0
/******************************************************************************
Module:  AccessData.h
Notices: Copyright (c) 2000 Jeffrey Richter
Modified: Sergey Kolomenkin, 2005
******************************************************************************/
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#ifndef _SECURITYINFORMATION_H_UID000000064B899E35
#define _SECURITYINFORMATION_H_UID000000064B899E35

//#include <ACLAPI.h>
#include <ACLUI.h>
#include "../TaskManagerExDll/WinObjects.h"

///////////////////////////////////////////////////////////////////////////////

#ifndef UNICODE
//#error This module must be compiled natively using Unicode.
#endif

///////////////////////////////////////////////////////////////////////////////

typedef struct _ObjectInformation {
	HANDLE			m_hHandle;
	OB_TYPE_ENUM	m_objInternalType;
	TCHAR			m_szName[1024];			// Object name. If empty, then m_hHandle member is used.

	TCHAR			m_szObjectName[2048];	// Screen Name
} ObjectInformation;


///////////////////////////////////////////////////////////////////////////////

class CSecurityInformation: public ISecurityInformation
{
public:

	static CSecurityInformation* CreateInstance(ObjectInformation Info, BOOL fBinary);

	BOOL EditSecurity( HWND hwnd );

private:

	CSecurityInformation(ObjectInformation Info, BOOL fBinary);
	virtual ~CSecurityInformation();

	struct OBJECT_SI_ACCESS
	{
		OB_TYPE_ENUM		m_objInternalType;
		SE_OBJECT_TYPE		m_objSecurType;
		SI_ACCESS m[19+4];
	};

	static GUID				m_guidNULL;
	static OBJECT_SI_ACCESS	m_siAccessAllRights[];
	static SI_ACCESS		m_siAccessBinaryRights[];
	static SI_INHERIT_TYPE	m_siInheritType[];

	ObjectInformation	m_Info;
	ObjTypeInf			m_Type;
	WCHAR				m_szObjectName[2048];	// Screen Name

	BOOL				m_fBinary;

	ULONG				m_nRef;

public:
	HRESULT WINAPI QueryInterface(REFIID riid, PVOID* ppvObj);
	ULONG WINAPI AddRef();
	ULONG WINAPI Release();

private:
	HRESULT WINAPI GetObjectInformation(PSI_OBJECT_INFO pObjectInfo);
	HRESULT WINAPI GetSecurity(SECURITY_INFORMATION RequestedInformation,
						PSECURITY_DESCRIPTOR* ppSecurityDescriptor, BOOL fDefault);
	HRESULT WINAPI SetSecurity(SECURITY_INFORMATION SecurityInformation,
						PSECURITY_DESCRIPTOR pSecurityDescriptor);
	HRESULT WINAPI GetAccessRights(const GUID* pguidObjectType,
						DWORD dwFlags, // si_edit_audits, si_edit_properties
						PSI_ACCESS* ppAccess, ULONG* pcAccesses, ULONG* piDefaultAccess);
	HRESULT WINAPI MapGeneric(const GUID* pguidObjectType,
						UCHAR* pAceFlags, ACCESS_MASK* pMask);
	HRESULT WINAPI GetInheritTypes(PSI_INHERIT_TYPE* ppInheritTypes, 
						ULONG* pcInheritTypes);
	HRESULT WINAPI PropertySheetPageCallback(HWND hwnd, UINT uMsg, 
						SI_PAGE_TYPE uPage);
};


#endif //ifndef _SECURITYINFORMATION_H_UID000000064B899E35
