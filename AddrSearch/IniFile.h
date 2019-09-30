// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__4D7A97DE_A567_431A_AC27_1A3D608B70F0__INCLUDED_)
#define AFX_INIFILE_H__4D7A97DE_A567_431A_AC27_1A3D608B70F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CIniFile  
{
public:
	virtual ~CIniFile();
	static CIniFile* GetInstance();
	void   RegisterModule(CString strName);
	void   SetValue(CString strKey, CString strValue);
	void   GetValue(CString strKey, CString &strValue,CString strDefault="");
	void   SetValue(CString strKey, int nValue);
	void   GetValue(CString strKey, int &nValue, int nDefault=0);
private:
	static CIniFile* m_pInstance;
	CString m_strModuleName;
	CIniFile();
	CString GetAppPath();
};

#endif // !defined(AFX_INIFILE_H__4D7A97DE_A567_431A_AC27_1A3D608B70F0__INCLUDED_)
