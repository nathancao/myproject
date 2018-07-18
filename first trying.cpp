#include "zbar.h"
#include "cv.h"        
#include <opencv.hpp> 
#include <imgproc.hpp>
#include <iostream> 
#include <string>

using namespace std;
using namespace zbar;  //���zbar���ƿռ�      
using namespace cv;
#define h 800
#define w 1000
void zbar_find(Mat img);
string  zbar_detect(Mat bar);
float area;
Rect rect;


void zbar_find(Mat img)
{
	Mat  image, imageGray, imageGuussian;
	Mat imageSobelX, imageSobelY, imageSobelOut;

	//2. ת��Ϊ�Ҷ�ͼ 
	cvtColor(img, imageGray, CV_RGB2GRAY);
	/*imshow("2.�Ҷ�ͼ", imageGray);*/

	//3. ��˹ƽ���˲� 
	GaussianBlur(imageGray, imageGuussian, Size(3, 3), 0);
	/*imshow("3.��˹ƽ���˲�", imageGuussian);*/

	//4.���ˮƽ�ʹ�ֱ����Ҷ�ͼ����ݶȺ�,ʹ��Sobel���� 
	Mat imageX16S, imageY16S;
	Sobel(imageGuussian, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(imageGuussian, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(imageX16S, imageSobelX, 1, 0);
	convertScaleAbs(imageY16S, imageSobelY, 1, 0);
	imageSobelOut = imageSobelX + imageSobelY;
	/*imshow("4.X�����ݶ�", imageSobelX);
	imshow("4.Y�����ݶ�", imageSobelY);
	imshow("4.XY�����ݶȺ�", imageSobelOut);
	*/
	
	//5.��ֵ�˲���������Ƶ���� 
	blur(imageSobelOut, imageSobelOut, Size(3, 3));
	/*imshow("5.��ֵ�˲�", imageSobelOut);*/

	//6.��ֵ�� 
	Mat imageSobleOutThreshold;
	//threshold(imageSobelOut, imageSobleOutThreshold, 180, 255, CV_THRESH_BINARY);
	adaptiveThreshold(imageSobelOut, imageSobleOutThreshold, 255,ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 33, 1);

	imshow("6.��ֵ��", imageSobleOutThreshold);

	//7.�����㣬����������϶ 
	Mat element = getStructuringElement(0, Size(7, 7));
	morphologyEx(imageSobleOutThreshold, imageSobleOutThreshold, MORPH_CLOSE, element);
	/*imshow("7.������", imageSobleOutThreshold);*/

	//8. ��ʴ��ȥ�������ĵ� 
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	/*imshow("8.��ʴ", imageSobleOutThreshold);*/

	//9. ���ͣ������������϶�����ݺ˵Ĵ�С���п�����Ҫ2~3�����Ͳ��� 
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	//dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	imshow("9.����", imageSobleOutThreshold);

	vector<vector<Point>> contours;
	vector<Vec4i> hiera;
	//10.ͨ��findContours�ҵ�����������ľ��α߽� 
	findContours(imageSobleOutThreshold, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i<(contours.size() - 1); i++)
	{

		if (contourArea(contours[i])>contourArea(contours[i + 1]))
			rect = boundingRect((Mat)contours[i]);
		else
			rect = boundingRect((Mat)contours[i + 1]);
	}
	rectangle(img, rect, Scalar(255), 2);
	namedWindow("�ҳ���ά���������");
	imshow("�ҳ���ά���������", img);
	waitKey(5);

}

string zbar_detect(Mat bar)
{
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	Mat imageGray;
	cvtColor(bar, imageGray, CV_RGB2GRAY);
	int width = imageGray.cols;
	int height = imageGray.rows;
	uchar *raw = (uchar *)imageGray.data;
	Image imageZbar(width, height, "Y800", raw, width * height);
	scanner.scan(imageZbar); //ɨ������    
	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	/*if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
	cout << "��ѯ����ʧ�ܣ�����ͼƬ��" << endl;
	}*/
	for (; symbol != imageZbar.symbol_end(); ++symbol)
	{
		cout << "���ͣ�" << endl << symbol->get_type_name() << endl << endl;
		cout << "���룺" << endl << symbol->get_data() << endl << endl;
	}
	/*imshow("Source Image", bar);
	waitKey();*/
	string data = symbol->get_data();
	imageZbar.set_data(NULL, 0);
	return data;
}

int main()
{
	Mat img = imread("good.jpg");
	//Mat img = imread("yy.png");
	resize(img, img, Size(img.cols/4,img.rows/4),(0,0),(0,0),3);  //ԭͼ���С�������������Ч�� 
	
	//namedWindow("1", 0);
	//imshow("1", img);
	/*
	float d = 0;
	float h1 = 0;
	//float h1 = h / 3 * 2;    //��ȡͼƬ��1/3��
	float t = w / 3;            //ROI����Ŀ��
	float h2 = h / 2;        //roi�߶� =
	*/
	Mat roi;
	roi = img(Rect(0, 0, img.cols/3, img.rows/3));   //����roi�ߴ�
	imshow("2", roi);
	 
	zbar_find(roi);
	/*
	string data = zbar_detect(roi);  //ʶ��roi��������
	string data1;
	if (string(data1) != string(data))
	{
		data1 = data;
		cout << "���룺" << endl << data1 << endl;
	}
	*/
	
	waitKey(0);


}




/*
int main()
{
	Mat img = imread("good.jpg");
	resize(img, img, Size(w, h));  //ԭͼ���С�������������Ч�� 
	namedWindow("1", 0);
	imshow("1", img);
	float d = 0;
	float h1 = 0;
	//float h1 = h / 3 * 2;    //��ȡͼƬ��1/3��
	float t = w/2;            //ROI����Ŀ��
	float h2 = h / 2;        //roi�߶� 

	Mat roi;
	for (d = 0; d < w-t; d += 50)        //dλ��ɨ��ͼƬ
	{
		Mat img1 = img.clone();     //ˢ��ԭͼ ����ROI���ֶ��
		//roi = img;
		roi = img1(Rect(d, h1, t, h2));   //����roi�ߴ�
		zbar_find(roi);                 //����roi����
		string data = zbar_detect(roi);  //ʶ��roi��������
		string data1;
		if (string(data1) != string(data))
		{
			data1 = data;
			cout << "���룺" << endl << data1 << endl;
		}


		waitKey(500);
	}
	waitKey(0);
}
*/