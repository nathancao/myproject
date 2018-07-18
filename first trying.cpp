#include "zbar.h"
#include "cv.h"        
#include <opencv.hpp> 
#include <imgproc.hpp>
#include <iostream> 
#include <string>

using namespace std;
using namespace zbar;  //添加zbar名称空间      
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

	//2. 转化为灰度图 
	cvtColor(img, imageGray, CV_RGB2GRAY);
	/*imshow("2.灰度图", imageGray);*/

	//3. 高斯平滑滤波 
	GaussianBlur(imageGray, imageGuussian, Size(3, 3), 0);
	/*imshow("3.高斯平衡滤波", imageGuussian);*/

	//4.求得水平和垂直方向灰度图像的梯度和,使用Sobel算子 
	Mat imageX16S, imageY16S;
	Sobel(imageGuussian, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
	Sobel(imageGuussian, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
	convertScaleAbs(imageX16S, imageSobelX, 1, 0);
	convertScaleAbs(imageY16S, imageSobelY, 1, 0);
	imageSobelOut = imageSobelX + imageSobelY;
	/*imshow("4.X方向梯度", imageSobelX);
	imshow("4.Y方向梯度", imageSobelY);
	imshow("4.XY方向梯度和", imageSobelOut);
	*/
	
	//5.均值滤波，消除高频噪声 
	blur(imageSobelOut, imageSobelOut, Size(3, 3));
	/*imshow("5.均值滤波", imageSobelOut);*/

	//6.二值化 
	Mat imageSobleOutThreshold;
	//threshold(imageSobelOut, imageSobleOutThreshold, 180, 255, CV_THRESH_BINARY);
	adaptiveThreshold(imageSobelOut, imageSobleOutThreshold, 255,ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 33, 1);

	imshow("6.二值化", imageSobleOutThreshold);

	//7.闭运算，填充条形码间隙 
	Mat element = getStructuringElement(0, Size(7, 7));
	morphologyEx(imageSobleOutThreshold, imageSobleOutThreshold, MORPH_CLOSE, element);
	/*imshow("7.闭运算", imageSobleOutThreshold);*/

	//8. 腐蚀，去除孤立的点 
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	erode(imageSobleOutThreshold, imageSobleOutThreshold, element);
	/*imshow("8.腐蚀", imageSobleOutThreshold);*/

	//9. 膨胀，填充条形码间空隙，根据核的大小，有可能需要2~3次膨胀操作 
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	//dilate(imageSobleOutThreshold, imageSobleOutThreshold, element);
	imshow("9.膨胀", imageSobleOutThreshold);

	vector<vector<Point>> contours;
	vector<Vec4i> hiera;
	//10.通过findContours找到条形码区域的矩形边界 
	findContours(imageSobleOutThreshold, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i<(contours.size() - 1); i++)
	{

		if (contourArea(contours[i])>contourArea(contours[i + 1]))
			rect = boundingRect((Mat)contours[i]);
		else
			rect = boundingRect((Mat)contours[i + 1]);
	}
	rectangle(img, rect, Scalar(255), 2);
	namedWindow("找出二维码矩形区域");
	imshow("找出二维码矩形区域", img);
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
	scanner.scan(imageZbar); //扫描条码    
	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	/*if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
	cout << "查询条码失败，请检查图片！" << endl;
	}*/
	for (; symbol != imageZbar.symbol_end(); ++symbol)
	{
		cout << "类型：" << endl << symbol->get_type_name() << endl << endl;
		cout << "条码：" << endl << symbol->get_data() << endl << endl;
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
	resize(img, img, Size(img.cols/4,img.rows/4),(0,0),(0,0),3);  //原图像大小调整，提高运算效率 
	
	//namedWindow("1", 0);
	//imshow("1", img);
	/*
	float d = 0;
	float h1 = 0;
	//float h1 = h / 3 * 2;    //截取图片下1/3段
	float t = w / 3;            //ROI区域的宽度
	float h2 = h / 2;        //roi高度 =
	*/
	Mat roi;
	roi = img(Rect(0, 0, img.cols/3, img.rows/3));   //定义roi尺寸
	imshow("2", roi);
	 
	zbar_find(roi);
	/*
	string data = zbar_detect(roi);  //识别roi中条形码
	string data1;
	if (string(data1) != string(data))
	{
		data1 = data;
		cout << "条码：" << endl << data1 << endl;
	}
	*/
	
	waitKey(0);


}




/*
int main()
{
	Mat img = imread("good.jpg");
	resize(img, img, Size(w, h));  //原图像大小调整，提高运算效率 
	namedWindow("1", 0);
	imshow("1", img);
	float d = 0;
	float h1 = 0;
	//float h1 = h / 3 * 2;    //截取图片下1/3段
	float t = w/2;            //ROI区域的宽度
	float h2 = h / 2;        //roi高度 

	Mat roi;
	for (d = 0; d < w-t; d += 50)        //d位移扫描图片
	{
		Mat img1 = img.clone();     //刷新原图 避免ROI出现多次
		//roi = img;
		roi = img1(Rect(d, h1, t, h2));   //定义roi尺寸
		zbar_find(roi);                 //查找roi区域
		string data = zbar_detect(roi);  //识别roi中条形码
		string data1;
		if (string(data1) != string(data))
		{
			data1 = data;
			cout << "条码：" << endl << data1 << endl;
		}


		waitKey(500);
	}
	waitKey(0);
}
*/