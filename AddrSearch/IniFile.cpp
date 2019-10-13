// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CONFIG_FILE_NAME			"config.ini"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile* CIniFile::m_pInstance=NULL;
CIniFile::CIniFile()
{

}

CIniFile::~CIniFile()
{

}

CIniFile* CIniFile::GetInstance()
{
	if (m_pInstance==NULL)
		m_pInstance=new CIniFile;
	return m_pInstance;
}

void  CIniFile::RegisterModule(CString strName)
{
	m_strModuleName=strName;
}

void  CIniFile::GetValue(CString strKey, CString &strValue,CString strDefault)
{
	char szBuf[65535];
	::GetPrivateProfileString(m_strModuleName,strKey,strDefault,szBuf,65535,GetAppPath()+CONFIG_FILE_NAME);
	strValue=szBuf;
}

void  CIniFile::SetValue(CString strKey, CString strValue)
{
	::WritePrivateProfileString(m_strModuleName,strKey,strValue,GetAppPath()+CONFIG_FILE_NAME);
}

void  CIniFile::SetValue(CString strKey, int nValue)
{
	CString strValue;
	strValue.Format("%d",nValue);
	SetValue(strKey,strValue);
}

void  CIniFile::GetValue(CString strKey, int &nValue, int nDefault)
{
	nValue=::GetPrivateProfileInt(m_strModuleName,strKey,nDefault,GetAppPath()+CONFIG_FILE_NAME);
}

CString CIniFile::GetAppPath()
{
	char szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    CString strPath = szPath;
    strPath = strPath.Left(strPath.ReverseFind('\\')+1);
	return strPath;
}