
// TrafficSignRecognitionDlg.h : 头文件
//

#pragma once
#include "CvvImage.h"
#include "afxcmn.h"
#include "DlgTab1.h"
#include "DlgTab2.h"
#include "DlgTab3.h"

#define UM_SHOWRESULT	WM_USER+1

// CTrafficSignRecognitionDlg 对话框
class CTrafficSignRecognitionDlg : public CDialogEx
{
// 构造
public:
	CTrafficSignRecognitionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAFFICSIGNRECOGNITION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CTabCtrl m_TabMain;
	CString m_SourcePicturePath;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonOpenSourcePicture();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT ShowResult(WPARAM wParam, LPARAM lParam);		 // 自定义消息
	static DWORD StartProcess(LPVOID ParameterData);	// 运算线程


	VOID OnInitTabControl();
	void DrawIplImageMFC(IplImage* image, int UID, int Dialog, BOOL BFlipX = FALSE, BOOL BFlipY = FALSE);
	void DisplayImage(CHAR* g_PictureFullPath,  int UID, int Dialog);
	bool RemoveDir(const char * szFileDir);
	void ShowDialogTab1();
	void ShowDialogTab2();
	void ShowDialogTab3();
	

private:
	CDlgTab1 m_DlgTab1;
	CDlgTab2 m_DlgTab2;
	CDlgTab3 m_DlgTab3;
	CRect m_TabRect;
	CHAR  m_PicturePath[MAX_PATH];
	CHAR  m_FolderFullPath[MAX_PATH];
	HANDLE m_ThreadHandle;
};
