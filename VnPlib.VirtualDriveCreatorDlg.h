
// VnPlib.VirtualDriveCreatorDlg.h : header file
//

#pragma once


// CVnPlibVirtualDriveCreatorDlg dialog
class CVnPlibVirtualDriveCreatorDlg : public CDialogEx
{
// Construction
public:
	CVnPlibVirtualDriveCreatorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VNPLIBVIRTUALDRIVECREATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CString sMappedPath;
	TCHAR* psz24Characters;
	CStringArray oDriveArrayWithoutSlash;
	CStringArray oDriveArrayWithSlash;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void ScanToUpdateDriveStatus();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonSelectMappedFolder();
	CEdit m_MappedFolder;
	CListCtrl mDriveList;
	CImageList mImageList;
	int mImageActiveID[26] = { IDI_ICON_A_ACTIVE, IDI_ICON_B_ACTIVE, IDI_ICON_C_ACTIVE, IDI_ICON_D_ACTIVE, IDI_ICON_E_ACTIVE, IDI_ICON_F_ACTIVE, IDI_ICON_G_ACTIVE, 
								IDI_ICON_H_ACTIVE, IDI_ICON_I_ACTIVE, IDI_ICON_J_ACTIVE, IDI_ICON_K_ACTIVE, IDI_ICON_L_ACTIVE, IDI_ICON_M_ACTIVE, IDI_ICON_N_ACTIVE, 
								IDI_ICON_O_ACTIVE, IDI_ICON_P_ACTIVE, IDI_ICON_Q_ACTIVE, IDI_ICON_R_ACTIVE, IDI_ICON_S_ACTIVE, IDI_ICON_T_ACTIVE, IDI_ICON_U_ACTIVE,
								IDI_ICON_V_ACTIVE, IDI_ICON_W_ACTIVE, IDI_ICON_X_ACTIVE, IDI_ICON_Y_ACTIVE, IDI_ICON_Z_ACTIVE };
	int mImageDeactiveID[26] = { IDI_ICON_A_DEACTIVE, IDI_ICON_B_DEACTIVE, IDI_ICON_C_DEACTIVE, IDI_ICON_D_DEACTIVE, IDI_ICON_E_DEACTIVE, IDI_ICON_F_DEACTIVE, IDI_ICON_G_DEACTIVE,
								IDI_ICON_H_DEACTIVE, IDI_ICON_I_DEACTIVE, IDI_ICON_J_DEACTIVE, IDI_ICON_K_DEACTIVE, IDI_ICON_L_DEACTIVE, IDI_ICON_M_DEACTIVE, IDI_ICON_N_DEACTIVE,
								IDI_ICON_O_DEACTIVE, IDI_ICON_P_DEACTIVE, IDI_ICON_Q_DEACTIVE, IDI_ICON_R_DEACTIVE, IDI_ICON_S_DEACTIVE, IDI_ICON_T_DEACTIVE, IDI_ICON_U_DEACTIVE,
								IDI_ICON_V_DEACTIVE, IDI_ICON_W_DEACTIVE, IDI_ICON_X_DEACTIVE, IDI_ICON_Y_DEACTIVE, IDI_ICON_Z_DEACTIVE };
	afx_msg void OnLvnItemchangedListDrive(NMHDR* pNMHDR, LRESULT* pResult);
	CButton mDelete;
	CButton mCreate;
	afx_msg void OnNMCustomdrawListDrive(NMHDR* pNMHDR, LRESULT* pResult);
};
