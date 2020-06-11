
// VnPlib.VirtualDriveCreatorDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "VnPlib.VirtualDriveCreator.h"
#include "VnPlib.VirtualDriveCreatorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVnPlibVirtualDriveCreatorDlg dialog

//Ex. cmd = L"subst " + oDriveArrayWithoutSlash.GetAt(index) + L" /d";
BOOL RunSubSTProcess(CString cmd, CString result) {
	BOOL bResult = false;
	HANDLE hPipeRead, hPipeWrite;

	// Pipe handles are inherited by child process.
	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
		return false;
	}

	// parameters to execute external process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;
	si.wShowWindow = SW_HIDE;
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcessW(0,		// Application name
		cmd.GetBuffer(),        // Application arguments
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		0,						// Working directory
		&si,
		&pi) == TRUE)
	{
		bool bProcessEnded = false;
		for (; !bProcessEnded;)
		{
			// Successfully created the process.  Wait for it to finish.
			bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

			// Even if process exited - we continue reading, if
			// there is some data available over pipe.
			for (;;)
			{
				char buf[1024];
				DWORD dwRead = 0;
				DWORD dwAvail = 0;

				if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL)) {
					break;
				}

				if (!dwAvail) {
					break;		// No data available, return
				}

				if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead) {
					break;		// Error, the child process might ended
				}

				buf[dwRead] = 0;
				result += buf;
			}
		}

		if (result.GetLength() <= 0) {
			result = L"SUCCESS";
			bResult = true;
		}

		TRACE(_T("::executeCommandLine() success at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmd, result);
		// Close the handles.
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {

		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);

		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

		// Display the error
		CString strError = (LPTSTR)lpMsgBuf;
		TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmd, strError);

		// Free resources created by the system
		LocalFree(lpMsgBuf);
	}
	return bResult;
}

void CVnPlibVirtualDriveCreatorDlg::ScanToUpdateDriveStatus() {
	DWORD bFlag;
	TCHAR Buf[MAX_PATH];     // temporary buffer for volume name
	
	for (int i = 0; i < oDriveArrayWithoutSlash.GetSize(); i++) {
		CString sDrive = oDriveArrayWithoutSlash.GetAt(i);
		bFlag = QueryDosDeviceW(sDrive, Buf, ARRAYSIZE(Buf));

		if (bFlag == 0)
		{
			HICON hIcon;
			hIcon = reinterpret_cast<HICON>(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(mImageActiveID[i]), IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR));
			mImageList.Replace(i, hIcon);
			TRACE(_T("QueryDosDeviceW failed.\nError=%lu\nDrive: %s\n"), GetLastError(), sDrive);
		}
		else {
			HICON hIcon;
			hIcon = reinterpret_cast<HICON>(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(mImageDeactiveID[i]), IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR));
			mImageList.Replace(i, hIcon);

			mDriveList.Invalidate();
			TRACE(_T("QueryDosDeviceW Success.\nResult=%s\nDrive: %s\n"), Buf, sDrive);
		}
	}
}

CVnPlibVirtualDriveCreatorDlg::CVnPlibVirtualDriveCreatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VNPLIBVIRTUALDRIVECREATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVnPlibVirtualDriveCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MAPPED_FOLDER, m_MappedFolder);
	DDX_Control(pDX, IDC_LIST_DRIVE, mDriveList);
	DDX_Control(pDX, ID_BUTTON_DELETE, mDelete);
	DDX_Control(pDX, ID_BUTTON_CREATE, mCreate);
}

BEGIN_MESSAGE_MAP(CVnPlibVirtualDriveCreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(ID_BUTTON_CREATE, &CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonCreate)
	ON_BN_CLICKED(ID_BUTTON_DELETE, &CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_MAPPED_FOLDER, &CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonSelectMappedFolder)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DRIVE, &CVnPlibVirtualDriveCreatorDlg::OnLvnItemchangedListDrive)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DRIVE, &CVnPlibVirtualDriveCreatorDlg::OnNMCustomdrawListDrive)
END_MESSAGE_MAP()


// CVnPlibVirtualDriveCreatorDlg message handlers

BOOL CVnPlibVirtualDriveCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here
	mImageList.Create(64, 64, ILC_COLOR16 | ILC_MASK, 1, 1);
	HICON hIcon;

	for (int i = 0; i < 26; i++) {
		hIcon = reinterpret_cast<HICON>(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(mImageActiveID[i]), IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR));
		mImageList.Add(hIcon);
	}


	mDriveList.SetImageList(&mImageList, LVSIL_SMALL);
	mDriveList.SetImageList(&mImageList, LVSIL_NORMAL);

	psz24Characters = L"abcdefjhijklmnopqrstuvwxyz";

	for (int i = 0; i < _tcslen(psz24Characters); i++) {
		TCHAR szDriveLetter[3];
		szDriveLetter[0] = psz24Characters[i];
		szDriveLetter[1] = TEXT(':');
		szDriveLetter[2] = TEXT('\0');
		oDriveArrayWithoutSlash.Add(szDriveLetter);

		TCHAR szDriveLetterAndSlash[4];
		szDriveLetterAndSlash[0] = psz24Characters[i];
		szDriveLetterAndSlash[1] = TEXT(':');
		szDriveLetterAndSlash[2] = TEXT('\\');
		szDriveLetterAndSlash[3] = TEXT('\0');
		oDriveArrayWithSlash.Add(szDriveLetterAndSlash);


		mDriveList.InsertItem(i, oDriveArrayWithoutSlash[i], i);
	}
	mDriveList.SetIconSpacing(90, 90);
	mDriveList.SetExtendedStyle(LVS_EX_FLATSB);

	ScanToUpdateDriveStatus();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVnPlibVirtualDriveCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVnPlibVirtualDriveCreatorDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVnPlibVirtualDriveCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonCreate()
{
	// TODO: Add your control notification handler code here
	int index = mDriveList.GetSelectionMark();
	if (sMappedPath.GetLength() <= 0 || index < 0) {
		return;
	}

	CString result;
	BOOL bResult = RunSubSTProcess((L"subst " + oDriveArrayWithoutSlash.GetAt(index) + L" " + sMappedPath), result);

	/*
	TCHAR* pszDriveLetter, * pszOptions;
	TCHAR szUniqueVolumeName[MAX_PATH];

	
	BOOL  fResult;
	fResult = DefineDosDeviceW(NULL, oDriveArrayWithoutSlash[index], sMappedPath);
	
	if (fResult && false)
	{
		// If GetVolumeNameForVolumeMountPoint fails, then 
		// SetVolumeMountPoint will also fail. However, 
		// DefineDosDevice must be called to remove the temporary symbolic link. 
		// Therefore, set szUniqueVolume to a known empty string.

		if (!GetVolumeNameForVolumeMountPoint(oDriveArrayWithSlash[index],
			szUniqueVolumeName,
			MAX_PATH))
		{
			TRACE(_T("GetVolumeNameForVolumeMountPoint failed.\nError=%lu\n"), GetLastError());
			szUniqueVolumeName[0] = '\0';
		}

		fResult = DefineDosDevice( NULL, oDriveArrayWithoutSlash[index], sMappedPath);

		if (!fResult) {
			TRACE(_T("DefineDosDevice failed.\nError=%lu\n"), GetLastError());
		}

		fResult = SetVolumeMountPoint(oDriveArrayWithSlash[index],
			szUniqueVolumeName);

		if (!fResult) {
			TRACE(_T("error %lu: could not add %s\n"), GetLastError(), oDriveArrayWithSlash[index]);
		}
	}
	*/
	if (bResult == true) {
		mCreate.EnableWindow(false);
		mDelete.EnableWindow(true);
		ScanToUpdateDriveStatus();
	}
}

void CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
	int index = mDriveList.GetSelectionMark();

	//BOOL  fResult;
	//fResult = DeleteVolumeMountPoint(oDriveArrayWithSlash.GetAt(index));
	//if (!fResult) {
	//	TRACE(_T("error % lu: couldn't remove %s\n"), GetLastError(), oDriveArrayWithSlash.GetAt(index));
	//}
	
	CString result;
	BOOL bResult = RunSubSTProcess((L"subst " + oDriveArrayWithoutSlash.GetAt(index) + L" /d"), result);

	if (bResult == true) {
		mDelete.EnableWindow(false);
		m_MappedFolder.SetWindowTextW(L"");
		sMappedPath = L"";
		mCreate.EnableWindow(true);
		ScanToUpdateDriveStatus();
	}
}


void CVnPlibVirtualDriveCreatorDlg::OnBnClickedButtonSelectMappedFolder()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR   szDisplayName[MAX_PATH];
	szDisplayName[0] = ' ';

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder that stored video files :");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lParam = NULL;
	bi.iImage = 0;
	bi.lpfn = NULL;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH];
	if (NULL != pidl)
	{
		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (FALSE == bRet)
		{
			return;
		}

		sMappedPath = (CString)szPathName;
		m_MappedFolder.SetWindowTextW(sMappedPath);
	}
	
}


void CVnPlibVirtualDriveCreatorDlg::OnLvnItemchangedListDrive(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if ((pNMLV->uChanged & LVIF_STATE)
		&& (pNMLV->uNewState & LVIS_SELECTED))
	{
		// do stuff...
		mDelete.EnableWindow(false);
		int index = pNMLV->iItem;

		DWORD bFlag;
		TCHAR Buf[MAX_PATH];     // temporary buffer for volume name
		CString sDrive = oDriveArrayWithoutSlash.GetAt(index);

		bFlag = QueryDosDeviceW(sDrive, Buf, ARRAYSIZE(Buf));

		if (bFlag == 0 )
		{
			m_MappedFolder.SetWindowTextW(L"");
			sMappedPath = L"";
			mCreate.EnableWindow(true);
			TRACE(_T("QueryDosDeviceW failed.\nError=%lu\nDrive: %s\n"), GetLastError(), sDrive);
		}
		else {
			mCreate.EnableWindow(false);
			if (Buf[0] == '\\' && Buf[3] == '\\' && Buf[1] == '?' && Buf[2] == '?') {
				CString sTmp = &Buf[4];
				m_MappedFolder.SetWindowTextW(sTmp);
				sMappedPath = sTmp;
				mDelete.EnableWindow(true);
			}
			else {
				m_MappedFolder.SetWindowTextW(Buf);
				sMappedPath = L"";
			}
			TRACE(_T("QueryDosDeviceW Success.\nResult=%s\nDrive: %s\n"), Buf, sDrive);
		}
	}

	*pResult = 0;
}


void CVnPlibVirtualDriveCreatorDlg::OnNMCustomdrawListDrive(NMHDR* pNMHDR, LRESULT* pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = 0;	// Initialize value

	switch (lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	case CDDS_SUBITEM:
		*pResult = CDRF_NOTIFYPOSTPAINT;

		//if (lpLVCustomDraw->nmcd.uItemState & CDIS_SELECTED)
		//{
			// Your color definitions here:
		//	lpLVCustomDraw->clrText = RGB(255, 255, 255);
		//	lpLVCustomDraw->clrTextBk = RGB(0, 70, 60);
		//}
		break;
	case CDDS_ITEMPOSTPAINT:
		{
			// If this item is selected, re-draw the icon in its normal
			// color (not blended with the highlight color).

			LVITEM rItem;
			int    nItem = static_cast<int>(lpLVCustomDraw->nmcd.dwItemSpec);

			// Get the image index and state of this item.  Note that we need to
			// check the selected state manually.  The docs _say_ that the
			// item's state is in pLVCD->nmcd.uItemState, but during my testing
			// it was always equal to 0x0201, which doesn't make sense, since
			// the max CDIS_* constant in commctrl.h is 0x0100.

			ZeroMemory(&rItem, sizeof(LVITEM));
			rItem.mask = LVIF_IMAGE | LVIF_STATE;
			rItem.iItem = nItem;
			rItem.stateMask = LVIS_SELECTED;
			mDriveList.GetItem(&rItem);

			// If this item is selected, redraw the icon with its normal colors.
			if (rItem.state & LVIS_SELECTED)
			{
				CDC* pDC = CDC::FromHandle(lpLVCustomDraw->nmcd.hdc);
				CRect rcIcon;

				// Get the rect that holds the item's icon.
				//mDriveList.GetItemRect(nItem, &rcIcon, LVIR_ICON);

				// Draw the icon.
				//mImageList.Draw(pDC, rItem.iImage, rcIcon.TopLeft(), ILD_TRANSPARENT);

				*pResult = CDRF_SKIPDEFAULT;
			}
			break;
		}
	default:
		break;
	}

	//*pResult |= CDRF_NOTIFYPOSTPAINT;
	//*pResult |= CDRF_NOTIFYITEMDRAW;
	//*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}
