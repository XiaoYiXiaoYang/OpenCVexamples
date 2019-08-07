#pragma once
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include "stdlib.h"

#define PI 3.1415926

using namespace std;
using namespace cv;

void preTreat(Mat &image, Mat &result);
void GammaCorrectiom(Mat src, int iWidth, int iHeight, float fGamma, Mat &dst);
BOOL TrafficSignRecognitionMain(CHAR* FilePath);    // 外部接口
void locateAreaRGB(Mat &image, Mat &result);	    // 基于RGB颜色空间的定位
void locateArea(Mat &srcImg, Mat &matR, Mat &matB,Mat &matY);        // 基于HSI颜色空间的定位
void Tmorphology(Mat &image, Mat &result);          // 形态学二值图填充目标
void Shapetest(Mat &srcImg, Mat &imageR, Mat &imageB, Mat &imageY, Mat &result);   // 基于形状的筛选
void RGBtoHSV(double red, double green, double blue, double& hue, double& saturation, double& intensity);
void fillHole(const Mat srcBw, Mat &dstBw);
bool isInside(Rect rect1, Rect rect2);            // 判断两个矩形是否有交集
