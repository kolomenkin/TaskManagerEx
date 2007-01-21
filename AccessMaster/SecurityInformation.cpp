//////////////////////////////////////////////////////////////
// File:		// SecurityInformation.cpp
// File time:	// 25.04.2005	12:34
// Version: 	// 1.0
/******************************************************************************
Module:  AccessMaster.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
Modified: Sergey Kolomenkin, 2005
******************************************************************************/
// Description: //////////////////////////////////////////////
//   Some useful routines...
//   

#include "stdafx.h"
//#include <windows.h>
#include "SecurityInformation.h"
#include <ACLAPI.h>

// Force linking against the ACLUI library
#pragma comment(lib, "ACLUI.lib")   

#define ARG_USED(x)		(x) = (x);
///////////////////////////////////////////////////////////////////////////////

#ifndef UNICODE
//#error This module must be compiled natively using Unicode.
#endif

///////////////////////////////////////////////////////////////////////////////

SI_INHERIT_TYPE CSecurityInformation::m_siInheritType[] =
{
	{&m_guidNULL, CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE | INHERIT_ONLY_ACE, L"Child" },
	{&m_guidNULL, OBJECT_INHERIT_ACE | INHERIT_ONLY_ACE, L"Child" }
};

///////////////////////////////////////////////////////////////////////////////

CSecurityInformation* CSecurityInformation::CreateInstance(ObjectInformation Info, BOOL fBinary)
{
	CSecurityInformation* pSec = new CSecurityInformation( Info, fBinary );
	return pSec;
}

///////////////////////////////////////////////////////////////////////////////

CSecurityInformation::CSecurityInformation(ObjectInformation Info, BOOL fBinary)
{
	TRACE( _T("CSecurityInformation::CSecurityInformation (0x%X)...\n"), this );
	GetWinObjectTypeInfo( Info.m_objInternalType, m_Type );
	m_Info = Info;
#ifdef UNICODE
	if( Info.m_szObjectName[0] != _T('\0') )
		wsprintfW( m_szObjectName, L"%s - %s", m_Type.m_szScreenName, Info.m_szObjectName );
	else
		wsprintfW( m_szObjectName, L"%s", m_Type.m_szScreenName );
#else
	if( Info.m_szObjectName[0] != _T('\0') )
		wsprintfW( m_szObjectName, L"%hs - %hs", m_Type.m_szScreenName, Info.m_szObjectName );
	else
		wsprintfW( m_szObjectName, L"%hs", m_Type.m_szScreenName );
#endif
	m_nRef = 0;
	AddRef();
	m_fBinary = fBinary;
}

///////////////////////////////////////////////////////////////////////////////

CSecurityInformation::~CSecurityInformation()
{
	TRACE( _T("CSecurityInformation::~CSecurityInformation (0x%X)...\n"), this );

}

///////////////////////////////////////////////////////////////////////////////

BOOL CSecurityInformation::EditSecurity( HWND hwnd )
{
	TRACE( _T("Enter CSecurityInformation::EditSecurity (0x%X)...\n"), this );
	BOOL bRes = TRUE;
	if( this == NULL )
	{
		bRes = FALSE;
	}

	ULONG lErr = 0;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SECURITY_INFORMATION RequestedInformation = DACL_SECURITY_INFORMATION;

	// Get security information

	if( bRes )
	{
		if (m_Info.m_szName[0] != 0) // Is it named
		{
			lErr = GetNamedSecurityInfo(m_Info.m_szName, 
						m_Type.m_objSecurType, RequestedInformation, NULL, NULL, 
						NULL, NULL, &pSD);
		}
		else // Is it a handle case
		{
			lErr = GetSecurityInfo(m_Info.m_hHandle, m_Type.m_objSecurType,
						RequestedInformation, NULL, NULL, NULL, NULL, &pSD);
		}

		// No matter what we still display security information
		if (lErr != ERROR_SUCCESS)
		{
			// Failure produces an empty SD
//#pragma message (WARNING "Lots of hardcoded strings!")
			MessageBox(NULL,
				TEXT("An error occurred retrieving security information for this object,\n")
				TEXT("possibly due to insufficient access rights.\n"),
				TEXT("Security Notice"), MB_OK);
			bRes = FALSE;
		}
		else
		{
			LocalFree(pSD);
		}
	}

	if( bRes )
	{
		bRes = ::EditSecurity( hwnd, this );
	}

	TRACE( _T("Leave CSecurityInformation::EditSecurity (0x%X) (ret = %d)...\n"), this, bRes );
	return bRes != FALSE;
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::QueryInterface(REFIID riid, PVOID* ppvObj)
{
	HRESULT hr = E_NOINTERFACE;
	if ((riid == IID_ISecurityInformation) || (riid == IID_IUnknown))
	{
		*ppvObj = this;
		AddRef();
		hr = S_OK;
	}
	return(hr);
}

///////////////////////////////////////////////////////////////////////////////

ULONG CSecurityInformation::AddRef()
{
	TRACE( _T("CSecurityInformation::AddRef (0x%X)...\n"), this );
	m_nRef++;
	return(m_nRef);
}

///////////////////////////////////////////////////////////////////////////////

ULONG CSecurityInformation::Release()
{
	TRACE( _T("CSecurityInformation::Release (0x%X)...\n"), this );
	ULONG nRef = --m_nRef;
	if (m_nRef == 0)
		delete this;
	return(nRef);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::GetObjectInformation(
			PSI_OBJECT_INFO pObjectInfo)
{
	// We are doing both normal and advanced editing
	pObjectInfo->dwFlags = SI_EDIT_ALL  | SI_ADVANCED;

	// Is it a container?
	if (m_Type.m_bIsContainer)
	{
		pObjectInfo->dwFlags  |= SI_CONTAINER;
	}

	// Is it a child?
	if (!m_Type.m_bIsChild)
	{
		pObjectInfo->dwFlags  |= SI_NO_ACL_PROTECT;
	}

	pObjectInfo->hInstance = NULL;
	pObjectInfo->pszServerName = NULL;
	pObjectInfo->pszObjectName = m_szObjectName;
	return(S_OK);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::GetSecurity(
			SECURITY_INFORMATION RequestedInformation, 
			PSECURITY_DESCRIPTOR* ppSecurityDescriptor, BOOL fDefault)
{
	ARG_USED( fDefault );

	HRESULT hr = 1;
	PSECURITY_DESCRIPTOR pSD = NULL;

	// Get security information
	ULONG lErr;
	if (m_Info.m_szName[0] != 0) // Is it named
	{
		lErr = GetNamedSecurityInfo(m_Info.m_szName, 
					m_Type.m_objSecurType, RequestedInformation, NULL, NULL, 
					NULL, NULL, &pSD);
	}
	else // Is it a handle case
	{
		lErr = GetSecurityInfo(m_Info.m_hHandle, m_Type.m_objSecurType,
					RequestedInformation, NULL, NULL, NULL, NULL, &pSD);
	}

	// No matter what we still display security information
	if (lErr != ERROR_SUCCESS)
	{
		// Failure produces an empty SD
		MessageBox(NULL,
			TEXT("An error occurred retrieving security information for this object,\n")
			TEXT("possibly due to insufficient access rights.\n")
			TEXT("Empty security descriptor was created for editing."),
			TEXT("Security Notice"), MB_OK);
	}
	else
	{
		hr = S_OK;
		*ppSecurityDescriptor = pSD;
	}

	return(hr);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::GetAccessRights(const GUID* pguidObjectType,
		DWORD dwFlags, PSI_ACCESS* ppAccess, ULONG* pcAccesses, 
		ULONG* piDefaultAccess)
{
	ARG_USED( dwFlags );
	ARG_USED( pguidObjectType );
	// If the binary check box was set, we show only raw binary ACE information
	if (m_fBinary)
	{
		*ppAccess = m_siAccessBinaryRights;
	}
	else
	{
		// Otherwise locate the appropriate block of specific rights
		// See AccessData.H header file
		BOOL bFound = FALSE;
		for( int i=0; m_siAccessAllRights[i].m[0].pguid != NULL; i++ )
		{
			if( m_siAccessAllRights[i].m_objInternalType == m_Info.m_objInternalType )
			{
				*ppAccess = m_siAccessAllRights[i].m;
				bFound = TRUE;
				break;
			}
		}

		if( !bFound )
		{
			ASSERT( FALSE );
			*ppAccess = m_siAccessBinaryRights;
		}
		*piDefaultAccess = 0;
	}

	*pcAccesses = 0;
	while ((*ppAccess)[*pcAccesses].mask != 0)
		(*pcAccesses)++;
	return(S_OK);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::MapGeneric(const GUID* pguidObjectType, 
		UCHAR* pAceFlags, ACCESS_MASK* pMask)
{
	ARG_USED( pMask );
	ARG_USED( pAceFlags );
	ARG_USED( pguidObjectType );
	return(S_OK);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::GetInheritTypes(PSI_INHERIT_TYPE* ppInheritTypes,
		ULONG* pcInheritTypes)
{
	*pcInheritTypes = 1;
	// If it is a container pass static enherit information for containers
	if (m_Type.m_bIsContainer)
	{
		*ppInheritTypes = &(m_siInheritType[0]);
	} 
	else
	{
		// If it is a child pass static enherit information for containers
		if (m_Type.m_bIsChild)
		{
			*ppInheritTypes = &(m_siInheritType[1]);
		} 
		else
		{
			// If niether, no inheritance
			*ppInheritTypes = NULL;
			*pcInheritTypes = 0;
		}
	}
	return(S_OK);
}


///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::PropertySheetPageCallback(HWND hwnd, UINT uMsg,
			SI_PAGE_TYPE uPage)
{
	ARG_USED( uPage );
	ARG_USED( uMsg );
	ARG_USED( hwnd );
	return(S_OK);
}

///////////////////////////////////////////////////////////////////////////////

HRESULT CSecurityInformation::SetSecurity(
			SECURITY_INFORMATION SecurityInformation,
			PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
	HRESULT hr = 1;

	// Get the Dacl
	PACL pDACL = NULL;
	BOOL fPresent, fDefaulted;
	GetSecurityDescriptorDacl(pSecurityDescriptor, &fPresent, &pDACL, &fDefaulted);

	// Get the SACL
	PACL pSACL = NULL;
	GetSecurityDescriptorSacl(pSecurityDescriptor, &fPresent, &pSACL, &fDefaulted);

	// Get the owner
	PSID psidOwner = NULL;
	GetSecurityDescriptorOwner(pSecurityDescriptor, &psidOwner, &fDefaulted);

	// Get the group
	PSID psidGroup = NULL;
	GetSecurityDescriptorOwner(pSecurityDescriptor, &psidGroup, &fDefaulted);

	// Find out if DACL and SACL inherit from parent objects
	SECURITY_DESCRIPTOR_CONTROL sdCtrl = NULL;
	ULONG ulRevision;
	GetSecurityDescriptorControl(pSecurityDescriptor, &sdCtrl, &ulRevision);

	if ((sdCtrl & SE_DACL_PROTECTED) != SE_DACL_PROTECTED)
		SecurityInformation  |= UNPROTECTED_DACL_SECURITY_INFORMATION;
	else
		SecurityInformation  |= PROTECTED_DACL_SECURITY_INFORMATION;

	if ((sdCtrl & SE_SACL_PROTECTED) != SE_SACL_PROTECTED)
		SecurityInformation  |= UNPROTECTED_SACL_SECURITY_INFORMATION;
	else
		SecurityInformation  |= PROTECTED_SACL_SECURITY_INFORMATION;

	// Set the security
	ULONG lErr;
	if (m_Info.m_szName[0] != 0) // Is it named
	{
		lErr = SetNamedSecurityInfo(m_Info.m_szName, 
					m_Type.m_objSecurType, SecurityInformation, psidOwner, 
					psidGroup, pDACL, pSACL);
	}
	else
	{
		// Is it a handle case
		lErr = SetSecurityInfo(m_Info.m_hHandle, m_Type.m_objSecurType,
					SecurityInformation, psidOwner, psidGroup, pDACL, pSACL);
	}

	// Report error
	if (lErr != ERROR_SUCCESS)
	{
		MessageBox(NULL,
			TEXT("An error occurred saving security information for this object,\n")
			TEXT("possibly due to insufficient access rights.\n"),
			TEXT("Security Notice"), MB_OK);
	}
	else
	{
		hr = S_OK;
	}

	return(hr);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
