/*#include <opencv2/opencv.hpp>
#include <math.h>
#include <iostream>
using namespace cv;
using namespace std;
bool isCorner(Mat &image);
Mat transformCorner(Mat &image, RotatedRect &rect);
int main(int argc, char** argv) {
	Mat src = imread("E:/github/QRscan/QRScan/test.jpg");
	if (src.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("input image", CV_WINDOW_AUTOSIZE);
	imshow("input image", src);
	Mat gray, binary;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	imwrite("D:/gloomyfish/outimage/qrcode_gray.jpg", gray);
	threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
	imwrite("D:/gloomyfish/outimage/qrcode_binary.jpg", binary);
	// detect rectangle now
	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	Moments monents;
	findContours(binary.clone(), contours, hireachy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point());
	Mat result = Mat::zeros(src.size(), CV_8UC3);
	for (size_t t = 0; t < contours.size(); t++) {
		double area = contourArea(contours[t]);
		if (area < 100) continue;
		RotatedRect rect = minAreaRect(contours[t]);
		// ���ݾ����������м��η���
		float w = rect.size.width;
		float h = rect.size.height;
		float rate = min(w, h) / max(w, h);
		if (rate > 0.85 && w < src.cols / 4 && h<src.rows / 4) {
			printf("angle : %.2f\n", rect.angle);
			Mat qr_roi = transformCorner(src, rect);
			if (isCorner(qr_roi)) {
				drawContours(src, contours, static_cast<int>(t), Scalar(255, 0, 0), 2, 8);
				imwrite(format("D:/gloomyfish/outimage/contour_%d.jpg", static_cast<int>(t)), qr_roi);
				drawContours(result, contours, static_cast<int>(t), Scalar(255, 0, 0), 2, 8);
			}
		}
	}
	imshow("result", src);
	imwrite("D:/gloomyfish/outimage/qrcode_patters.jpg", result);
	waitKey(0);
	return 0;
}*/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace cv;
using namespace std;

Mat src; Mat src_gray;

RNG rng(12345);
//Scalar colorful = CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

Point Center_cal(vector<vector<Point> > contours, int i)//�ҵ�����ȡ���������ĵ�
{
	int centerx = 0, centery = 0, n = contours[i].size();
	//����ȡ��С�����εı߽���ÿ���ܳ���������ȡһ��������꣬������ȡ�ĸ����ƽ�����꣨��ΪС�����εĴ������ģ�
	centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
	centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
	Point point1 = Point(centerx, centery);
	return point1;
}


int main(int argc, char** argv[])
{


	src = imread("E:/github/QRscan/QRScan/test.jpg",1);
	Mat src_all = src.clone();
	imshow("test", src);

	cvtColor(src, src_gray, CV_BGR2GRAY);
	//  src_gray = Scalar::all(255) - src_gray;
	blur(src_gray, src_gray, Size(3, 3));
	equalizeHist(src_gray, src_gray);
	imshow("�˲���", src_gray);

	Scalar color = Scalar(1, 1, 255);
	Mat threshold_output;
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	Mat drawing2 = Mat::zeros(src.size(), CV_8UC3);
	threshold(src_gray, threshold_output, 112, 255, THRESH_BINARY);
	//Canny(src_gray,threshold_output,136,196,3);
	//imshow("Ԥ�����",threshold_output);

	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
	//CHAIN_APPROX_NONEȫ��,CV_CHAIN_APPROX_SIMPLE,,,RETR_TREE    RETR_EXTERNAL    RETR_LIST   RETR_CCOMP

	int c = 0, ic = 0, k = 0, area = 0;
	//
	//����ĺ���ɸѡ
	//����ĺ���ɸѡ
	int parentIdx = -1;
	for (int i = 0; i< contours.size(); i++)
	{
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		else if (hierarchy[i][2] == -1)
		{
			ic = 0;
			parentIdx = -1;
		}


		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			drawContours(drawing, contours, parentIdx, CV_RGB(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 1, 8);
			ic = 0;
			parentIdx = -1;
			area = contourArea(contours[i]);//�ó�һ����ά�붨λ�ǵ�������Ա������߳���area_side�������ݸ�������ν��������λ��������һ�����ݶ����ԣ�
		}
		//cout<<"i= "<<i<<" hierarchy[i][2]= "<<hierarchy[i][2]<<" parentIdx= "<<parentIdx<<" ic= "<<ic<<endl;


	}


	for (int i = 0; i<contours2.size(); i++)
		drawContours(drawing2, contours2, i, CV_RGB(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255)), -1, 4, hierarchy[k][2], 0, Point());


	Point point[3];
	for (int i = 0; i<contours2.size(); i++)
	{
		point[i] = Center_cal(contours2, i);
	}

	area = contourArea(contours2[1]);//Ϊʲô��һ���ǰ��һ�����������һ����
	int area_side = cvRound(sqrt(double(area)));
	for (int i = 0; i<contours2.size(); i++)
	{
		line(drawing2, point[i%contours2.size()], point[(i + 1) % contours2.size()], color, area_side / 2, 8);
	}


	imshow("��ȡ��", drawing2);
	printf("%d\n", contours.size());
	//imshow( "Contours", drawing );


	//������Ҫ�����������ά��
	Mat gray_all, threshold_output_all;
	vector<vector<Point> > contours_all;
	vector<Vec4i> hierarchy_all;
	cvtColor(drawing2, gray_all, CV_BGR2GRAY);


	threshold(gray_all, threshold_output_all, 45, 255, THRESH_BINARY);
	findContours(threshold_output_all, contours_all, hierarchy_all, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));//RETR_EXTERNAL��ʾֻѰ�����������


																												   //����С��Χ���Σ�б��Ҳ����Ŷ
	RotatedRect rectPoint = minAreaRect(contours_all[0]);
	Point2f fourPoint2f[4];


	//��rectPoint�����д洢������ֵ�ŵ� fourPoint��������  
	rectPoint.points(fourPoint2f);


	for (int i = 0; i < 4; i++)
	{
		line(src_all, fourPoint2f[i % 4], fourPoint2f[(i + 1) % 4]
			, Scalar(20, 21, 237), 3);
	}


	imshow("��ά��", src_all);


	waitKey(0);
	return(0);
}
