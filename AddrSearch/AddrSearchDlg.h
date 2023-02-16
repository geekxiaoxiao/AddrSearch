// AddrSearchDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CAddrSearchDlg dialog
class CAddrSearchDlg : public CDialog
{
// Construction
public:
	CAddrSearchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ADDRSEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	DWORD GetProcessID(CString strName);
	DWORD GetCallAddr(HANDLE hProcess,CString strCode,int nOffset);
	void  StrToBuf(CString strData,BYTE buf[],int &len);
	int   FindCode(BYTE page[],int nPageLen,BYTE code[],int nCodeLen);
	void  AddText(CString strText);
	void AddAddrInfo(CString strInfo);
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CListCtrl m_lstInfo;
	CString m_strName;
	CString m_strCode;
	CString m_strOffset;
	CEdit m_edtMsg;
	int m_Row = -1; int m_Col = -1;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_cbType;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMclkListTest(NMHDR* pNMHDR, LRESULT* pResult);
	
	CString M_exeName;
	CString Me_ModuleName;
	BOOL Mcheck;
	afx_msg void OnBnClickedButton5();
	// define 格式
	BOOL B_define;
	// uint64_t格式输出
	BOOL B_Ifuint64;
};
