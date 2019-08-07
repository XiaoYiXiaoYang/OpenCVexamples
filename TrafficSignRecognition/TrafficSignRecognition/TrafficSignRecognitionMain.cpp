#include "stdafx.h"
#include "TrafficSignRecognitionMain.h"

CHAR g_PictureFullPath[MAX_PATH] = { 0 };		//  原始图像路径
CHAR g_FolderFullPath[MAX_PATH] = { 0 };		//  结果文件夹路径
INT  g_Order = 0;
typedef unsigned char UNIT8; //用 8 位无符号数表示 0～255 之间的整数
UNIT8 g_GammaLUT[256];//全局数组：包含256个元素的gamma校正查找表

BOOL TrafficSignRecognitionMain(CHAR* Path)
{
	g_Order = 0;
	if (Path == NULL)
	{
		return FALSE;
	}
	strcpy(g_PictureFullPath, Path);

	CHAR *v1 = NULL;
	strcpy(g_FolderFullPath, g_PictureFullPath);
	v1 = strrchr(g_FolderFullPath, '.');
	*v1 = '\0';
	BOOL IsOk = CreateDirectory(g_FolderFullPath, NULL);

	Mat srcImg = imread(g_PictureFullPath);

	// 灰度化
	Mat grayImage;
	cvtColor(srcImg, grayImage, CV_RGB2GRAY);

	Mat before_result;
	preTreat(srcImg, before_result); //得到预处理结果

	// 颜色分割
	Mat matR = Mat::zeros(srcImg.size(), CV_8UC1);
	Mat matB = Mat::zeros(srcImg.size(), CV_8UC1);
	Mat matY = Mat::zeros(srcImg.size(), CV_8UC1);
	locateArea(before_result, matR, matB,matY);  //红色matR  蓝色matB

	// 形态学操作
	Mat phoImgR, phoImgB,phoImgY;
	Tmorphology(matR, phoImgR);
	Tmorphology(matB, phoImgB); 
	Tmorphology(matY, phoImgY);


	CHAR PhologyFullPath[MAX_PATH] = { 0 };
	strcpy(PhologyFullPath, g_FolderFullPath);
	strcat(PhologyFullPath, "\\PhologyRed.jpg");
	imwrite(PhologyFullPath, phoImgR);
	strcpy(PhologyFullPath, g_FolderFullPath);
	strcat(PhologyFullPath, "\\PhologyBlue.jpg");
	imwrite(PhologyFullPath, phoImgB);
	strcpy(PhologyFullPath, g_FolderFullPath);
	strcat(PhologyFullPath, "\\PhologyYellow.jpg");
	imwrite(PhologyFullPath, phoImgY);

	// 基于轮廓形状的检测
	Mat shape_result;
	Shapetest(srcImg, phoImgR, phoImgB, phoImgY,shape_result);
	

	// 分割

	// 识别

}

// 图像预处理   增强   去噪    复原
void preTreat(Mat &image, Mat &result) 
{

	//Mat  equalImg;
	//分割通道，存储到splitBGR中
	//vector<Mat> splitBGR(image.channels());
	//split(image, splitBGR);
	//对各个通道分别进行直方图均衡化
	//for (int i = 0; i<image.channels(); i++)
	//	equalizeHist(splitBGR[i], splitBGR[i]);
	//合并通道
	//merge(splitBGR, equalImg);

	//result = equalImg.clone();

	image.copyTo(result);
	GammaCorrectiom(image, image.cols, image.rows, 0.9, result);
}

void BuildTable(float fPrecompensation)
{
	int i;
	float f;
	for (i = 0; i<256; i++)
	{
		f = (i + 0.5F) / 256;//归一化
		f = (float)pow(f, fPrecompensation);
		g_GammaLUT[i] = (UNIT8)(f * 256 - 0.5F);//反归一化
	}
}

void GammaCorrectiom(Mat src, int iWidth, int iHeight, float fGamma, Mat &dst)
{
	int iCols, iRows;
	BuildTable(1 / fGamma);//gamma校正查找表初始化
						   //对图像的每个像素进行查找表矫正
	for (iRows = 0; iRows<iHeight; iRows++)
	{
		for (iCols = 0; iCols<iWidth; iCols++)
		{
			dst.at<Vec3b>(iRows, iCols)[0] = g_GammaLUT[src.at<Vec3b>(iRows, iCols)[0]];
			dst.at<Vec3b>(iRows, iCols)[1] = g_GammaLUT[src.at<Vec3b>(iRows, iCols)[1]];
			dst.at<Vec3b>(iRows, iCols)[2] = g_GammaLUT[src.at<Vec3b>(iRows, iCols)[2]];
		}
	}
}

void locateAreaRGB(Mat &image, Mat &result) 
{
	// RGB颜色空间的定位
	result = Mat::zeros(image.size(), CV_8UC1);
	int rows = image.rows;
	int cols = image.cols;

	double B = 0.0, G = 0.0, R = 0.0;
	for (int i = 0; i < rows; i++) 
	{
		for (int j = 0; j < cols; j++) 
		{
			//  获取BGR值  
			B = image.at<Vec3b>(i, j)[0];
			G = image.at<Vec3b>(i, j)[1];
			R = image.at<Vec3b>(i, j)[2];
			if ((R - B) >= 50 && (R - G) >= 50) // 红色
				result.at<uchar>(i, j) = 255;
			if ((R - B) >= 50 && (R - G) >= 50)
				result.at<uchar>(i, j) = 255;// 蓝色
		}
	}
}

void locateArea(Mat &srcImg, Mat &matR, Mat &matB,Mat &matY) 
{
	// 三色色彩分割
	int width = srcImg.cols;//图像宽度  
	int height = srcImg.rows;//图像高度  
	double B = 0.0, G = 0.0, R = 0.0, H = 0.0, S = 0.0, V = 0.0;
	//matR = Mat::zeros(srcImg.size(), CV_8UC1);
	//matB = Mat::zeros(srcImg.size(), CV_8UC1);

	int x, y; //循环  
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// 获取BGR值  
			B = srcImg.at<Vec3b>(y, x)[0];
			G = srcImg.at<Vec3b>(y, x)[1];
			R = srcImg.at<Vec3b>(y, x)[2];
			RGBtoHSV(R, G, B, H, S, V);
			//红色范围  0 
			if ((H >= 330 && H <= 360 || H >= 0 && H <= 10) && S >= 21 && S <= 100 && V > 16 && V < 99) //H不能低于10，H不能大于344,S不能高于21，V不能变
			{
				matR.at<uchar>(y, x) = 255;
			}
			//蓝色范围  240 
			if ((H>=200 && H<=255) && S >= 21 && S <= 100 && V > 16 && V < 99) //H不能低于10，H不能大于344,S不能高于21，V不能变
			{
				matB.at<uchar>(y, x) = 255;
			}
			//黄色范围  60
			if ((H >= 45 && H <= 50) && S >= 21 && S <= 100 && V > 16 && V < 99) //H不能低于10，H不能大于344,S不能高于21，V不能变
			{
				matY.at<uchar>(y, x) = 255;
			}
		}
	}
}

void RGB2HSV(double red, double green, double blue, double& hue, double& saturation, double& intensity)
{

	double r, g, b;
	double h, s, i;

	double sum;
	double minRGB, maxRGB;
	double theta;

	r = red / 255.0;
	g = green / 255.0;
	b = blue / 255.0;

	minRGB = ((r < g) ? (r) : (g));
	minRGB = (minRGB < b) ? (minRGB) : (b);

	maxRGB = ((r > g) ? (r) : (g));
	maxRGB = (maxRGB > b) ? (maxRGB) : (b);

	sum = r + g + b;
	i = sum / 3.0;

	if (i < 0.001 || maxRGB - minRGB < 0.001)
	{

		h = 0.0;
		s = 0.0;
	}
	else
	{
		s = 1.0 - 3.0*minRGB / sum;
		theta = sqrt((r - g)*(r - g) + (r - b)*(g - b));
		theta = acos((r - g + r - b)*0.5 / theta);
		if (b <= g)
			h = theta;
		else
			h = 2 * PI - theta;
		if (s <= 0.01)
			h = 0;
	}

	hue = (int)(h * 180 / PI);
	saturation = (int)(s * 100);
	intensity = (int)(i * 100);
}


void Tmorphology(Mat &image, Mat &result) 
{
	// blur(image,result,Size(3,3),Point(-1,-1)); // 均值滤波
	medianBlur(image, result, 3);// 中值滤波
	// GaussianBlur(image,result,Size(5,5),5,5);  // 高斯滤波
	medianBlur(result, result, 3);// 中值滤波
	medianBlur(result, result, 5);// 中值滤波


	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * 1 + 1, 2 * 1 + 1), Point(1, 1));
	Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(2 * 3 + 1, 2 * 3 + 1), Point(3, 3));
	erode(result, result, element);// 腐蚀    

	dilate(result, result, element1);// 膨胀    

	fillHole(result, result);// 填充

}

void Shapetest(Mat &srcImg, Mat &imageR, Mat &imageB,Mat &imageY,Mat &result) 
{
	vector<vector<Point>>contoursR; //红色轮廓  
	vector<vector<Point>>contoursB; //蓝色轮廓 
	vector<vector<Point>>contoursY; //黄色轮廓 
	
	vector<Vec4i> hierarchy;//分层    
	findContours(imageR, contoursR, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//寻找轮廓    
	findContours(imageB, contoursB, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//寻找轮廓    
	findContours(imageY, contoursY, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));//寻找轮廓   

	vector<vector<Point>>contours(contoursR.size() + contoursB.size()+ contoursY.size()); //全部轮廓  																							  //整合到一个轮廓
	for(int i = 0; i <(contoursR.size() + contoursB.size()+ contoursY.size()); i++)
	{
		if (i < contoursR.size())
			contours[i] = contoursR[i];
		else if(i<(contoursR.size() + contoursB.size()))
			contours[i] = contoursB[i- contoursR.size()];
		else
			contours[i] = contoursY[i- (contoursR.size() + contoursB.size())];
	}

	vector<vector<Point>> contours_poly(contours.size());  //近似后的轮廓点集    
	vector<Rect> boundRect(contours.size());  //包围点集的最小矩形vector    
																  //画轮廓
	for (int i = 0; i <contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true); //对多边形曲线做适当近似，contours_poly[i]是输出的近似点集    
		boundRect[i] = boundingRect(Mat(contours_poly[i])); //计算并返回包围轮廓点集的最小矩形       
	}

	for (int i = 0; i < (contours.size()); i++)
	{
		Rect rect = boundRect[i];
		//首先进行一定的限制，筛选出区域

		/*	//若轮廓矩形内部还包含着矩形，则将被包含的小矩形取消
		bool inside = false;
		for (int j = 0; j < contours.size(); j++)
		{
		Rect t = boundRect[j];
		if (rect == t)
		continue;
		else if (isInside(rect, t))
		{
		inside = true;
		break;
		}
		}
		if (inside)
		continue;*/

		double _P = arcLength(contours[i], true);
		double _S = -contourArea(contours[i], true);

		//圆形度  矩形度  伸长度  **圆形  三角形  矩形
		double _C = 4.0 * acos(-1)*_S / _P / _P;
		double _R = _S / boundRect[i].width / boundRect[i].height;
		double _E = (double)min(boundRect[i].width, boundRect[i].height) / (double)max(boundRect[i].width, boundRect[i].height);
		if (!((_C > 0.85&&_R > 0.7&&_E > 0.85) || (_C > 0.35&&_C<0.7&&_R>0.4&&_R<0.65&&_E>0.8) || (_C > 0.60&&_C<0.85&&_R>0.7&&_E > 0.85)))
		{
			continue;
		}
			

		//高宽比限制  
		float ratio = (float)rect.width / (float)rect.height;
		//轮廓面积限制       
		float Area = (float)rect.width * (float)rect.height;
		float dConArea = (float)contourArea(contours[i]);
		float dConLen = (float)arcLength(contours[i], 1);
		if (dConArea < 700)
			continue;
		if (ratio > 1.3 || ratio < 0.4)
			continue;


		//对提取出的轮廓进行去噪，筛选出交通标志
		Mat drawing = Mat::zeros(imageR.size(), CV_8UC3);

		Mat srccopy;
		srcImg.copyTo(srccopy);
		//画轮廓识别框
		Scalar color = (0, 0, 255);//蓝色线画轮廓    
		drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		rectangle(srcImg, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

		//Harris角点检测，去除掉不规则轮廓
		Mat grayImg, dstImg, normImg, scaledImg;
		cvtColor(drawing, grayImg, COLOR_BGR2GRAY);
		cornerHarris(grayImg, dstImg, 2, 3, 0.04);

		//归一化与转换
		normalize(dstImg, normImg, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
		convertScaleAbs(normImg, scaledImg);

		int harrisNum = 0;
		for (int j = 0; j < normImg.rows; j++)
		{
			for (int i = 0; i < normImg.cols; i++)
			{
				if ((int)normImg.at<float>(j, i) > 160)
				{
					circle(scaledImg, Point(i, j), 4, Scalar(0, 10, 255), 2, 8, 0);
					harrisNum++;
				}
			}
		}
		if (harrisNum > 33)//当角点数目大于33，则忽略此轮廓
			continue;

		Mat Mat_rgb_copy;
		imageR.copyTo(Mat_rgb_copy);   //分割图

		Mat roiImage;
		Mat_rgb_copy(rect).copyTo(roiImage);
		Mat temp;
		srccopy(rect).copyTo(temp);

		srccopy(rect).copyTo(roiImage);

		CHAR RoiFullPath[MAX_PATH] = { 0 };
		strcpy(RoiFullPath, g_FolderFullPath);
		CHAR v2[10] = { 0 };
		sprintf(v2, "\\%d.jpg", g_Order);
		g_Order++;
		strcat(RoiFullPath, v2);
		imwrite(RoiFullPath, roiImage);


		Mat temp2 = Mat::zeros(temp.size(), CV_8UC1);
		cvtColor(temp, temp2, CV_BGR2GRAY);;

		const int image_cols = 45;//图片分为列,可自行调整，这里只是随意写的  
		const int image_rows = 45;//图片分为行 

		CvANN_MLP bp; //bp神经网络 
		bp.load("BP_trafficSign.xml");//这是网络训练好后直接调用   

		Mat test_temp;
		resize(temp2, test_temp, Size(image_cols, image_rows), (0, 0), (0, 0), CV_INTER_AREA);//使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现  
		threshold(test_temp, test_temp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//二值化  
		Mat_<float>sampleMat(1, image_rows*image_cols);
		for (int i = 0; i<image_rows*image_cols; ++i) {
			sampleMat.at<float>(0, i) = (float)test_temp.at<uchar>(i / image_rows, i % image_cols);//将test数据（unchar）copy到sampleMat(float)中图像特征  
		}

		Mat responseMat;
		bp.predict(sampleMat, responseMat); //过调用predict函数，我们得到一个输出向量，它是一个1*nClass的行向量， 识别  
											//其中每一列说明它与该类的相似程度（0-1之间），也可以说是置信度  
		Point maxLoc;
		double maxVal = 0;

		minMaxLoc(responseMat, NULL, &maxVal, NULL, &maxLoc);//最小最大值  
		string judge[500] = { "30","60","no entry","straight","turn right","roundabout","careful with passenger" ,"careful with construction","danger ","no passing" };
		//cout << "识别结果：" << maxLoc.x << endl;
		//cout << "识别结果：" << judge[maxLoc.x] << endl;

		RNG rng(12345);

		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		rectangle(imageR, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		if (maxLoc.x <= 10)
			putText(srcImg, judge[maxLoc.x], cvPoint(boundRect[i].x, boundRect[i].y  +60), cv::FONT_HERSHEY_PLAIN, 4, CV_RGB(0, 0,0), 5);
		
		CHAR ResultFullPath[MAX_PATH] = { 0 };
		strcpy(ResultFullPath, g_FolderFullPath);
		strcat(ResultFullPath, "\\Result.jpg");
		imwrite(ResultFullPath, srcImg);
	}
}
void fillHole(const Mat srcBw, Mat &dstBw)
{
	Size m_Size = srcBw.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));

	Mat cutImg;
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

void RGBtoHSV(double red, double green, double blue, double& hue, double& saturation, double& intensity)
{

	double r, g, b;
	double h, s, i;

	double sum;
	double minRGB, maxRGB;
	double theta;

	r = red / 255.0;
	g = green / 255.0;
	b = blue / 255.0;

	minRGB = ((r < g) ? (r) : (g));
	minRGB = (minRGB < b) ? (minRGB) : (b);

	maxRGB = ((r > g) ? (r) : (g));
	maxRGB = (maxRGB > b) ? (maxRGB) : (b);

	sum = r + g + b;
	i = sum / 3.0;

	if (i < 0.001 || maxRGB - minRGB < 0.001)
	{

		h = 0.0;
		s = 0.0;
	}
	else
	{
		s = 1.0 - 3.0*minRGB / sum;
		theta = sqrt((r - g)*(r - g) + (r - b)*(g - b));
		theta = acos((r - g + r - b)*0.5 / theta);
		if (b <= g)
			h = theta;
		else
			h = 2 * PI - theta;
		if (s <= 0.01)
			h = 0;
	}

	hue = (int)(h * 180 / PI);
	saturation = (int)(s * 100);
	intensity = (int)(i * 100);
}

// 判断rect1与rect2是否有交集  
bool isInside(Rect rect1, Rect rect2)
{
	Rect t = rect1&rect2;
	if (rect1.area() > rect2.area())
	{
		return false;
	}
	else
	{
		if (t.area() != 0)
			return true;
	}
}