#pragma once
#include "CvvImage.h"


// CDlgTab1 �Ի���

class CDlgTab1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab1)

public:
	CDlgTab1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTab1();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TAB_1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


	DECLARE_MESSAGE_MAP()

//public:
//	void DrawIplImageMFC(IplImage * image, int UID, BOOL BFlipX = FALSE, BOOL BFlipY = FALSE);
//	void DisplayImage(CHAR* PictureFullPath, int UID);
//	void ShowDialogTab1(CHAR * PicturePath, CHAR * FolderFullPath);
};
