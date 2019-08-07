#pragma once
#include "CvvImage.h"


// CDlgTab1 对话框

class CDlgTab1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab1)

public:
	CDlgTab1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTab1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()

//public:
//	void DrawIplImageMFC(IplImage * image, int UID, BOOL BFlipX = FALSE, BOOL BFlipY = FALSE);
//	void DisplayImage(CHAR* PictureFullPath, int UID);
//	void ShowDialogTab1(CHAR * PicturePath, CHAR * FolderFullPath);
};
