
// TrafficSignRecognitionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficSignRecognition.h"
#include "TrafficSignRecognitionDlg.h"
#include "afxdialogex.h"
#include "TrafficSignRecognitionMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DIALOG_TAB_1  1
#define DIALOG_TAB_2  2
#define DIALOG_TAB_3  3
#define DIALOG_TAB_4  4


ULONG g_dx = 0;
CHAR g_PicturePath[MAX_PATH] = { 0 };

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTrafficSignRecognitionDlg 对话框



CTrafficSignRecognitionDlg::CTrafficSignRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAFFICSIGNRECOGNITION_DIALOG, pParent)
	, m_SourcePicturePath(_T(""))
{
	ZeroMemory(m_PicturePath, MAX_PATH);
	ZeroMemory(m_FolderFullPath, MAX_PATH);
	m_ThreadHandle = INVALID_HANDLE_VALUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrafficSignRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SOURCE_PICTURE_PATH, m_SourcePicturePath);
	DDX_Control(pDX, IDC_TAB_MAIN, m_TabMain);
}

BEGIN_MESSAGE_MAP(CTrafficSignRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SOURCE_PICTURE, &CTrafficSignRecognitionDlg::OnBnClickedButtonOpenSourcePicture)
	ON_MESSAGE(UM_SHOWRESULT, ShowResult)
	ON_WM_TIMER()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &CTrafficSignRecognitionDlg::OnTcnSelchangeTabMain)
END_MESSAGE_MAP()


// CTrafficSignRecognitionDlg 消息处理程序

BOOL CTrafficSignRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 添加标签
	m_TabMain.InsertItem(0, _T("识别结果"));
	m_TabMain.InsertItem(1, _T("标识切割"));
	m_TabMain.InsertItem(2, _T("过程图解"));
	
	OnInitTabControl();

	CProgressCtrl *ProgressCtrl = (CProgressCtrl*)m_DlgTab1.GetDlgItem(IDC_PROGRESS);
	ProgressCtrl->SetRange(0, 100);
    ProgressCtrl->SetPos(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTrafficSignRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTrafficSignRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTrafficSignRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 初始化 TabConctrl 控件
VOID CTrafficSignRecognitionDlg::OnInitTabControl()
{
	m_TabMain.GetClientRect(&m_TabRect);
	m_TabRect.left += 1;
	m_TabRect.right -= 1;
	m_TabRect.top += 21;
	m_TabRect.bottom -= 1;

	g_dx = m_TabRect.right;
	g_dx -= 15;

	m_DlgTab1.Create(IDD_DIALOG_TAB_1, GetDlgItem(IDC_TAB_MAIN));
	m_DlgTab2.Create(IDD_DIALOG_TAB_2, GetDlgItem(IDC_TAB_MAIN));
	m_DlgTab3.Create(IDD_DIALOG_TAB_3, GetDlgItem(IDC_TAB_MAIN));

	m_DlgTab1.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
	m_DlgTab2.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
	m_DlgTab3.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
}

// 绑定窗口
void CTrafficSignRecognitionDlg::DrawIplImageMFC(IplImage* image, int UID, int Dialog, BOOL BFlipX, BOOL BFlipY)
{
	CWnd *pWnd = NULL;
	switch (Dialog)
	{
	case DIALOG_TAB_1:
		pWnd = m_DlgTab1.GetDlgItem(UID);
		break;
	case DIALOG_TAB_2:
		pWnd = m_DlgTab2.GetDlgItem(UID);
		break;
	case DIALOG_TAB_3:
		pWnd = m_DlgTab3.GetDlgItem(UID);
		break;
	default:
		break;
	}
	CDC*pDC = pWnd->GetDC();
	HDC hdc = pDC->GetSafeHdc();
	CRect rect;
	pWnd->GetClientRect(&rect);
	CvvImage cimg;

	if (BFlipX)
	{
		cvFlip(image, NULL, 0);  //垂直镜像翻转,与CCD相对应
	}
	if (BFlipY)
	{
		cvFlip(image, NULL, 1);  // 水平镜像翻转,与CCD相对应
	}

	cimg.CopyOf(image, image->nChannels);
	cimg.DrawToHDC(hdc, &rect);
}
// 显示
void CTrafficSignRecognitionDlg::DisplayImage(CHAR* g_PictureFullPath,  int UID, int Dialog)
{
	IplImage * SourceImage = 0;
	SourceImage = cvLoadImage(g_PictureFullPath, CV_LOAD_IMAGE_COLOR);
	DrawIplImageMFC(SourceImage, UID, Dialog);
	cvReleaseImage(&SourceImage);
}


// 拖拽文件
void CTrafficSignRecognitionDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CProgressCtrl *ProgressCtrl = (CProgressCtrl*)m_DlgTab1.GetDlgItem(IDC_PROGRESS);
	ProgressCtrl->SetPos(0);
	DragQueryFile(hDropInfo, 0, m_PicturePath, MAX_PATH);    // 取得第一个文件的路径

	char* v1 = NULL;
	char v2[5][5] = { "bmp","jpg","gif","tiff","png" };
	char v3[5] = { 0 };
	int i = 0;
	v1 = strrchr(m_PicturePath, '.');
	while (*v1++)
	{
		v3[i++] = *v1;
	}
	int j = 0;
	for (j = 0; j < 5; j++)
	{
		if (strcmp(v3, v2[j]) == 0)
		{
			m_SourcePicturePath.Format("%s", m_PicturePath);
			UpdateData(FALSE);
			DisplayImage(m_PicturePath, IDC_STATIC_SOURCE_PICTURE, DIALOG_TAB_1);
			SetTimer(1, 100, NULL);

			CHAR *Pos = NULL;
			strcpy(m_FolderFullPath, m_PicturePath);
			Pos = strrchr(m_FolderFullPath, '.');
			*Pos = '\0';

			strcpy(g_PicturePath, m_PicturePath);

			m_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartProcess, NULL, 0, NULL);
			if (m_ThreadHandle == INVALID_HANDLE_VALUE)
			{
				return;
			}

			goto Exit;
		}
	}
	MessageBox("文件格式错误");

Exit:
	CDialogEx::OnDropFiles(hDropInfo);
}
// 打开原图像文件
void CTrafficSignRecognitionDlg::OnBnClickedButtonOpenSourcePicture()
{
	// TODO: 在此添加控件通知处理程序代码
	CProgressCtrl *ProgressCtrl = (CProgressCtrl*)m_DlgTab1.GetDlgItem(IDC_PROGRESS);
	ProgressCtrl->SetPos(0);
	CString Filter;
	Filter = "所有文件(*.bmp,*.jpg,*.gif,*.tiff,*.png)|*.bmp;*.jpg;*.gif;*.tiff;*.png| BMP(*.bmp)|*.bmp| \
		JPG(*.jpg)|*.jpg| GIF(*.gif)|*.gif| TIFF(*.tiff)|*.tiff| PNG(*.png)|*.png||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, Filter, NULL);
	// 按下确定按钮 dlg.DoModal() 函数显示对话框
	if (dlg.DoModal() == IDOK)
	{
		// 打开对话框获取图像信息
		m_SourcePicturePath = dlg.GetPathName();	// 获取文件路径名   如 D:\pic\abc.bmp
		strcpy_s(m_PicturePath, m_SourcePicturePath);
		UpdateData(FALSE);
		DisplayImage(m_PicturePath, IDC_STATIC_SOURCE_PICTURE, DIALOG_TAB_1);
		SetTimer(1, 20, NULL);

		CHAR *Pos = NULL;
		strcpy(m_FolderFullPath, m_PicturePath);
		Pos = strrchr(m_FolderFullPath, '.');
		*Pos = '\0';

		strcpy(g_PicturePath, m_PicturePath);

		m_ThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartProcess, NULL, 0, NULL);
		if (m_ThreadHandle == INVALID_HANDLE_VALUE)
		{
			return;
		}

	}
}

// 删除文件夹
bool CTrafficSignRecognitionDlg::RemoveDir(const char* szFileDir)
{
	std::string strDir = szFileDir;
	if (strDir.at(strDir.length() - 1) != '\\')
		strDir += '\\';
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile((strDir + "*.*").c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
				return false;
	}
	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (stricmp(wfd.cFileName, ".") != 0 &&
				stricmp(wfd.cFileName, "..") != 0)
				RemoveDir((strDir + wfd.cFileName).c_str());
		}
		else
		{
			DeleteFile((strDir + wfd.cFileName).c_str());
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
	RemoveDirectory(szFileDir);
	return true;
}

// 定时器
void CTrafficSignRecognitionDlg::OnTimer(UINT_PTR nIDEvent)
{
 	CProgressCtrl *ProgressCtrl = (CProgressCtrl*)m_DlgTab1.GetDlgItem(IDC_PROGRESS);
	int iPos = 0;
	switch (nIDEvent)
	{
	case 1:
		// TODO: 在此添加消息处理程序代码和/或调用默认值
		iPos = ProgressCtrl->GetPos();  //获取当前进度条位置
		iPos += 5;
		ProgressCtrl->SetPos(iPos);

		// 当达到100的时候就关闭定时器
		if (iPos == 100)
		{
			KillTimer(1);
			WaitForSingleObject(m_ThreadHandle,INFINITE);
			CloseHandle(m_ThreadHandle);
			PostMessage(UM_SHOWRESULT);
		}
		break;
	case 2:
		ShowDialogTab1();
		KillTimer(2);
		break;
	
	case 3:
		ShowDialogTab2();
		KillTimer(3);
		break;
	case 4:
		ShowDialogTab3();
		KillTimer(4);
		break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

// 启动算法程序
DWORD CTrafficSignRecognitionDlg::StartProcess(LPVOID ParameterData)
{

	TCHAR v1[MAX_PATH] = { 0 };
	memcpy(v1, g_PicturePath, MAX_PATH);
	
	TrafficSignRecognitionMain(v1);
	/********************************
		此处填写其他实现代码
	*******************************/

	return 0;
}

// 自定义消息函数 —— 显示结果
LRESULT CTrafficSignRecognitionDlg::ShowResult(WPARAM wParam, LPARAM lParam)
{
	char v1[MAX_PATH] = { 0 };
	strcpy(v1, m_FolderFullPath);
	strcat(v1, "\\Result.jpg");

	Mat srcImg = imread(v1);
	if (srcImg.data != NULL)
	{
		DisplayImage(v1, IDC_STATIC_RESULT_PICTURE, DIALOG_TAB_1);
	}
	else
	{
		RemoveDir(m_FolderFullPath);
		MessageBox("未检测到交通标识！！");
	}
	
	return LRESULT();
}

// 改变 TAB 控件
void CTrafficSignRecognitionDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ULONG m_SelectTab = 0;
	m_SelectTab = m_TabMain.GetCurSel();

	switch (m_SelectTab)
	{
	case 0:
	{
		m_DlgTab1.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
		m_DlgTab2.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		m_DlgTab3.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		SetTimer(2, 1, NULL);
		break;
	}
	case 1:
	{
		m_DlgTab1.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		m_DlgTab2.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
		m_DlgTab3.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		SetTimer(3, 1, NULL);
		break;
	}
	case 2:
	{
		m_DlgTab1.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		m_DlgTab2.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_HIDEWINDOW);
		m_DlgTab3.SetWindowPos(NULL, m_TabRect.left, m_TabRect.top, m_TabRect.Width(), m_TabRect.Height(), SWP_SHOWWINDOW);
		SetTimer(4, 1, NULL);
		break;
	}

	default:
		break;
	}
	*pResult = 0;
}

// 显示 Tab1 窗口
void CTrafficSignRecognitionDlg::ShowDialogTab1()
{
	char v1[MAX_PATH] = { 0 };
	char v2[MAX_PATH] = { 0 };
	strcpy(v1, m_PicturePath);

	strcpy(v2, m_FolderFullPath);
	strcat(v2, "\\Result.jpg");

	Mat srcImg1 = imread(v1);
	Mat srcImg2 = imread(v1);
	if (srcImg1.data != NULL && srcImg2.data != NULL)
	{
		DisplayImage(v1, IDC_STATIC_SOURCE_PICTURE, DIALOG_TAB_1);
		DisplayImage(v2, IDC_STATIC_RESULT_PICTURE, DIALOG_TAB_1);
	}
	else
	{
		return;
	}
}

// 显示 Tab2 窗口
void CTrafficSignRecognitionDlg::ShowDialogTab2()
{
	INT  g_Order = 0;
	CHAR v1[MAX_PATH] = { 0 };
	while (TRUE)
	{
		strcpy(v1, m_FolderFullPath);
		CHAR v2[10] = { 0 };
		sprintf(v2, "\\%d.jpg", g_Order);
		strcat(v1, v2);
		Mat srcImg = imread(v1);
		if (srcImg.data != NULL)
		{
			DisplayImage(v1, IDC_STATIC_0 + g_Order, DIALOG_TAB_2);
			g_Order++;
		}
		else
		{
			return;
		}
	}
}

// 显示 Tab3 窗口
void CTrafficSignRecognitionDlg::ShowDialogTab3()
{
	char v1[MAX_PATH] = { 0 };
	char v2[MAX_PATH] = { 0 };
	char v3[MAX_PATH] = { 0 };
	strcpy(v1, m_FolderFullPath);
	strcat(v1, "\\PhologyRed.jpg");

	strcpy(v2, m_FolderFullPath);
	strcat(v2, "\\PhologyBlue.jpg");

	strcpy(v3, m_FolderFullPath);
	strcat(v3, "\\PhologyYellow.jpg");

	Mat srcImg1 = imread(v1);
	Mat srcImg2 = imread(v2);
	Mat srcImg3 = imread(v3);
	if (srcImg1.data != NULL && srcImg2.data != NULL && srcImg3.data != NULL)
	{

		DisplayImage(v1, IDC_STATIC_PHOLOGY_RED, DIALOG_TAB_3);
		DisplayImage(v2, IDC_STATIC_PHOLOGY_BLUE, DIALOG_TAB_3);
		DisplayImage(v3, IDC_STATIC_PHOLOGY_YELLOW, DIALOG_TAB_3);
	}
	else
	{
		return;
	}

}