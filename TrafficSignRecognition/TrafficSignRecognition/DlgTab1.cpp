// DlgTab1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficSignRecognition.h"
#include "DlgTab1.h"
#include "afxdialogex.h"



// CDlgTab1 �Ի���

IMPLEMENT_DYNAMIC(CDlgTab1, CDialogEx)

CDlgTab1::CDlgTab1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_TAB_1, pParent)
{

}

CDlgTab1::~CDlgTab1()
{
}

void CDlgTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTab1, CDialogEx)
END_MESSAGE_MAP()


// CDlgTab1 ��Ϣ�������

// �󶨴���
//void CDlgTab1::DrawIplImageMFC(IplImage* image, int UID, BOOL BFlipX, BOOL BFlipY)
//{
//	CWnd *pWnd = GetDlgItem(UID);
//	CDC*pDC=pWnd->GetDC();
//	HDC hdc=pDC->GetSafeHdc();
//	CRect rect;
//	pWnd->GetClientRect(&rect);
//	CvvImage cimg;
//
//	if (BFlipX)
//	{
//		cvFlip(image, NULL, 0);  //��ֱ����ת,��CCD���Ӧ
//	}
//	if (BFlipY)
//	{
//		cvFlip(image, NULL, 1);  // ˮƽ����ת,��CCD���Ӧ
//	}
//
//	cimg.CopyOf(image, image->nChannels);
//	cimg.DrawToHDC(hdc, &rect);
//}
//
// ��ʾ
//void CDlgTab1::DisplayImage(CHAR* PictureFullPath, int UID)
//{
//	IplImage * SourceImage = 0;
//	SourceImage = cvLoadImage(PictureFullPath, CV_LOAD_IMAGE_COLOR);
//	DrawIplImageMFC(SourceImage, UID);
//	cvReleaseImage(&SourceImage);
//}
//
// �Զ�����Ϣ���� ���� ��ʾ Tab1 ����
//void CDlgTab1::ShowDialogTab1(CHAR* PicturePath, CHAR* FolderFullPath)
//{
//	strcat(FolderFullPath, "\\1.jpg");
//
//	DisplayImage(PicturePath, IDC_STATIC_SOURCE_PICTURE);
//	DisplayImage(FolderFullPath, IDC_STATIC_RESULT_PICTURE);
//	
//	return;
//
//}
