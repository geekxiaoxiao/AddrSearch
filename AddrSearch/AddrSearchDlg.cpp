// AddrSearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddrSearch.h"
#include "AddrSearchDlg.h"
#include "tlhelp32.h"
#include "MemOpt.h"
#include "ProcessOpt.h"
#include "FindPattern.h""

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAddrSearchDlg dialog




CAddrSearchDlg::CAddrSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddrSearchDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_strCode(_T(""))
	, m_strOffset(_T(""))
	, M_exeName(_T(""))
	, Me_ModuleName(_T(""))
	, Mcheck(FALSE)
	, B_define(TRUE)
	, B_Ifuint64(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAddrSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstInfo);
	DDX_Text(pDX, IDC_EDIT4, m_strName);
	DDX_Text(pDX, IDC_EDIT2, m_strCode);
	DDX_Text(pDX, IDC_EDIT3, m_strOffset);
	DDX_Control(pDX, IDC_EDIT1, m_edtMsg);
	DDX_Control(pDX, IDC_COMBO1, m_cbType);
	DDX_Text(pDX, IDC_EDIT5, M_exeName);
	DDX_Text(pDX, IDC_EDIT6, Me_ModuleName);
	DDX_Check(pDX, IDC_CHECK1, Mcheck);
	DDX_Radio(pDX, IDC_RADIO1, B_define);
	DDX_Radio(pDX, IDC_RADIO2, B_Ifuint64);
}

BEGIN_MESSAGE_MAP(CAddrSearchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CAddrSearchDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAddrSearchDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CAddrSearchDlg::OnBnClickedButton3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CAddrSearchDlg::OnNMDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAddrSearchDlg::OnNMclkListTest)
	ON_BN_CLICKED(IDC_BUTTON4, &CAddrSearchDlg::OnBnClickedButton4)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CAddrSearchDlg::OnLvnItemchangedList1)
	
	ON_BN_CLICKED(IDC_BUTTON5, &CAddrSearchDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CAddrSearchDlg message handlers

BOOL CAddrSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_lstInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// TODO: Add extra initialization here
	 
	M_exeName="xxxxxx.exe";
	Me_ModuleName = "xxxxxx.exe";
	UpdateData(false);
	CRect rect;
	m_lstInfo.GetClientRect(&rect);
	m_lstInfo.InsertColumn(0,"名称",LVCFMT_LEFT,rect.Width()* 2 / 10 );
	m_lstInfo.InsertColumn(1,"特征码",LVCFMT_LEFT, rect.Width() *5 / 10);
	m_lstInfo.InsertColumn(2,"偏移",LVCFMT_LEFT, rect.Width() *1 / 10 );
	m_lstInfo.InsertColumn(3,"类型",LVCFMT_LEFT, rect.Width() * 1/10);
	m_lstInfo.InsertColumn(4, "模块", LVCFMT_LEFT, rect.Width() * 1 / 10);
	CIniFile::GetInstance()->RegisterModule("保存");
	CString strList;
	CIniFile::GetInstance()->GetValue("地址信息",strList);
	CString str=strtok(strList.GetBuffer(0),";");
	while (str!="")
	{
		AddAddrInfo(str);
		str=strtok(NULL,";");
	}
	m_cbType.AddString("地址");
	m_cbType.AddString("内容(4字节)");
	m_cbType.AddString("内容(2字节)");
	m_cbType.AddString("内容(1字节)");
	
	m_cbType.AddString("函数");
	m_cbType.AddString("[指针内容](x_64 8字节)");
	m_cbType.AddString("内容(8字节)");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddrSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAddrSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAddrSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAddrSearchDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	DWORD dwProcessID=GetProcessID(M_exeName);
	if (dwProcessID==0)
	{
		MessageBox("进程文件 "+ M_exeName +" 未打开");
		return;
	}
	m_edtMsg.SetWindowText("");
	CProcessOpt po(dwProcessID);
	for (int i=0;i<m_lstInfo.GetItemCount();i++)
	{
		UpdateData();
		CString strCode=m_lstInfo.GetItemText(i,1);
		int nOffset;
		sscanf(m_lstInfo.GetItemText(i,2),"%X",&nOffset);
		CString ModName = m_lstInfo.GetItemText(i, 4);
		strCode.Replace(" ","");

		InitializeSigCopyMemory(dwProcessID, ModName.GetBuffer());
		DWORD_PTR modBase=0;
		DWORD_PTR dwData = GeekXFindPattern(strCode.GetBuffer(), &modBase, nOffset);//新的寻址核心函数 采用目标进程块内存全复制dump 到 本进程  的方式 用 指针法遍历地址 速度极快
		DWORD_PTR AddressVal = 0;
		//CString str;
		//str.Format("modBase:0x%llX", modBase); //调试输出64位16进制
		//OutputDebugString(str);
							
		FinalizeScan();
		
		//DWORD dwData=GetCallAddr(po.GetProcessHandle(),strCode,nOffset);
		if (m_lstInfo.GetItemText(i, 3) == "0")
		{
			AddressVal = dwData;
			if (Mcheck == true)
				AddressVal = AddressVal - modBase;
		}
		if (m_lstInfo.GetItemText(i,3)=="1")


		{
			CMemOpt mo(po.GetProcessHandle(),dwData);
			mo.ReadMem(&AddressVal,4);
		}
		else if (m_lstInfo.GetItemText(i,3)=="2")
		{
			CMemOpt mo(po.GetProcessHandle(),dwData);
			mo.ReadMem(&AddressVal,2);
		}
		else if (m_lstInfo.GetItemText(i,3)=="3")
		{
			CMemOpt mo(po.GetProcessHandle(),dwData);
			mo.ReadMem(&AddressVal,1);
		}
		else if (m_lstInfo.GetItemText(i,3)=="4")
		{
			int nCallOffset=0;
			CMemOpt mo(po.GetProcessHandle(),dwData);
			mo.ReadMem(&nCallOffset,4);
			AddressVal =dwData+nCallOffset+4;
			if (Mcheck == true)
				AddressVal = AddressVal - modBase;
		}
		else if (m_lstInfo.GetItemText(i, 3) == "5")
		{
			int nCallOffset = 0;
			CMemOpt mo(po.GetProcessHandle(), dwData);
			mo.ReadMem(&nCallOffset, 4);
			AddressVal = dwData + nCallOffset + 4;
			if (Mcheck == true)
				AddressVal = AddressVal - modBase;
		}
		else if (m_lstInfo.GetItemText(i, 3) == "6")
		{
			CMemOpt mo(po.GetProcessHandle(), dwData);
			mo.ReadMem(&AddressVal, 8);
		}
		
		CString strMsg;
		if (B_Ifuint64 == FALSE)
		{
			strMsg.Format("constexpr uint64_t %s 0x%llX", m_lstInfo.GetItemText(i, 0), AddressVal);
		}
		else
		{
			strMsg.Format("#define %s 0x%llX", m_lstInfo.GetItemText(i, 0), AddressVal);
		}
		
		AddText(strMsg);
	}

}

DWORD CAddrSearchDlg::GetProcessID(CString strName)
{
	HANDLE hSnapshot= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet=Process32First(hSnapshot,&pe32);
	while(bRet)
	{
		CString strProcess=pe32.szExeFile;
		strProcess.MakeLower();
		if(strProcess==strName)
			return pe32.th32ProcessID;
		bRet = Process32Next(hSnapshot, &pe32);
	}
	CloseHandle(hSnapshot);
	return 0;
}

DWORD CAddrSearchDlg::GetCallAddr(HANDLE hProcess,CString strCode,int nOffset)//老的 寻址方法 慢 且不支持 ?号 通配符 已废弃不用
{
	BYTE code[512]={0};
	int len=0;
	StrToBuf(strCode,code,len);

	
	DWORD dwStart=0x400000;
	DWORD dwEnd=0xFFFFFF;
	DWORD dwPos=dwStart;
	int nPageIndex=0;
	int nPageSize=4096;
	BYTE page[4096];
	while (dwPos<=dwEnd-len)
	{
		CMemOpt mo(hProcess,dwPos);
		mo.ReadMem(page,nPageSize);
		int nIndex=FindCode(page,nPageSize,code,len);
		if (nIndex==-1)
		{
			nPageIndex++;
			dwPos+=nPageSize;
			continue;
		}
		return dwStart+nPageSize*nPageIndex+nIndex+nOffset;
	}
	return 0;
}

void  CAddrSearchDlg::StrToBuf(CString strData,BYTE buf[],int &len)//老的 寻址方法 慢 且不支持 ?号 通配符 已废弃不用
{
	len=0;
	CString str=strtok(strData.GetBuffer(0)," ");
	while (str!="")
	{
		int nValue;
		sscanf(str,"%02X",&nValue);
		buf[len++]=nValue;
		str=strtok(NULL," ");
	}
}

int CAddrSearchDlg::FindCode(BYTE page[],int nPageLen,BYTE code[],int nCodeLen)//老的 寻址方法 慢 且不支持 ?号 通配符 已废弃不用
{
	for (int i=0;i<nPageLen;i++)
	{
		int k=0;
		for (;k<nCodeLen;k++)
		{
			if (code[k]!=page[i+k])
				break;
		}
		if (k==nCodeLen)
			return i;
	}
	return -1;
}
void CAddrSearchDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int nCount=m_lstInfo.GetItemCount();
	m_lstInfo.InsertItem(nCount,m_strName);
	m_lstInfo.SetItemText(nCount,1,m_strCode);
	m_lstInfo.SetItemText(nCount,2,m_strOffset);
	CString str;
	str.Format("%d",m_cbType.GetCurSel());
	m_lstInfo.SetItemText(nCount,3,str);

	m_lstInfo.SetItemText(nCount, 4, Me_ModuleName);
}

void CAddrSearchDlg::AddText(CString strText)
{
	int nLen=m_edtMsg.GetWindowTextLength();
	m_edtMsg.SetSel(nLen,nLen);
	m_edtMsg.ReplaceSel(strText+"\r\n");
	m_edtMsg.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}
void CAddrSearchDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CString strList="";
	for (int i=0;i<m_lstInfo.GetItemCount();i++)
	{
		CString str;
		str.Format("%s,%s,%s,%s,%s",m_lstInfo.GetItemText(i,0),m_lstInfo.GetItemText(i,1),m_lstInfo.GetItemText(i,2), m_lstInfo.GetItemText(i, 3),m_lstInfo.GetItemText(i, 4));
		strList+=str+";";

	}
	CIniFile::GetInstance()->RegisterModule("保存");
	CIniFile::GetInstance()->SetValue("地址信息",strList);
	
}

void CAddrSearchDlg::AddAddrInfo(CString strInfo)
{
	int nStart=0;
	int nPos=strInfo.Find(",",nStart);
	CString str=strInfo.Left(nPos);
	int nCount=m_lstInfo.GetItemCount();
	m_lstInfo.InsertItem(nCount,str);
	nStart=nPos+1;

	nPos=strInfo.Find(",",nStart);
	str=strInfo.Mid(nStart,nPos-nStart);
	m_lstInfo.SetItemText(nCount,1,str);
	nStart=nPos+1;


nPos=strInfo.Find(",",nStart);
str=strInfo.Mid(nStart,nPos-nStart);
	m_lstInfo.SetItemText(nCount,2,str);
nStart = nPos + 1;


	nPos = strInfo.Find(",", nStart);
 str = strInfo.Mid(nStart, nPos - nStart);
	m_lstInfo.SetItemText(nCount, 3, str);

	 
	str = strInfo.Mid(nPos + 1);
	m_lstInfo.SetItemText(nCount, 4, str);
}



void CAddrSearchDlg::OnBnClickedButton4()
{
	UpdateData();
	CString str;
	int nId;
	//首先得到点击的位置
	POSITION pos = m_lstInfo.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox("请至少选择一项", "提示", MB_ICONEXCLAMATION);
		return;
	}
	//得到行号，通过POSITION转化
	nId = (int)m_lstInfo.GetNextSelectedItem(pos);
	 
	m_lstInfo.SetItemText(nId, 0, m_strName);
	m_lstInfo.SetItemText(nId, 1, m_strCode);
	m_lstInfo.SetItemText(nId, 2, m_strOffset);
	
	str.Format("%d", m_cbType.GetCurSel());
	m_lstInfo.SetItemText(nId, 3, str);
	m_lstInfo.SetItemText(nId, 4, Me_ModuleName);
}


void CAddrSearchDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CAddrSearchDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (pNMListView->iItem != -1)
		m_lstInfo.DeleteItem(pNMListView->iItem);
	*pResult = 0;
}
void CAddrSearchDlg::OnNMclkListTest(NMHDR* pNMHDR, LRESULT* pResult)
{

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码

	POSITION ps;
	int nIndex;

	ps = m_lstInfo.GetFirstSelectedItemPosition();
	nIndex = m_lstInfo.GetNextSelectedItem(ps);
	//TODO:添加多选的操作。
	if (nIndex == -1)
	{
		//m_list.SetItemState(-1,LVNI_SELECTED,LVNI_SELECTED);

	}
	m_lstInfo.SetItemState(nIndex, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	CString str;

	m_strName=  m_lstInfo.GetItemText(nIndex, 0);
	m_strCode= m_lstInfo.GetItemText(nIndex, 1);
	m_strOffset = m_lstInfo.GetItemText(nIndex, 2);
	
	str = m_lstInfo.GetItemText(nIndex, 3);
	m_cbType.SetCurSel(atoi(str.GetBuffer()));
	Me_ModuleName = m_lstInfo.GetItemText(nIndex, 4);

UpdateData(false);
	  

	 

}



void CAddrSearchDlg::OnBnClickedButton5()
{

	CString source ;
	GetDlgItemText(IDC_EDIT1, source);
	//put your text in source
	if (OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char* buffer;
		EmptyClipboard();
		char* src = source.GetBuffer();
		int a = strlen(src) + 1;
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, a);
		buffer = (char*)GlobalLock(clipbuffer);

		strcpy(buffer, src);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}

	// TODO: 在此添加控件通知处理程序代码
}
