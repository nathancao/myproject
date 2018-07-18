#include <iostream>
#include <zbar.h>
#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>


using namespace std;
using namespace cv;
using namespace zbar;

//zbar�ӿ�
string ZbarDecoder(Mat img)
{
	string result;
	ImageScanner scanner;
	const void *raw = (&img)->data;
	// configure the reader
	scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE, 1);
	// wrap image data
	Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
	// scan the image for barcodes
	scanner.scan(image); //ɨ������      
	Image::SymbolIterator symbol = image.symbol_begin();
	if (image.symbol_begin() == image.symbol_end())
	{
		cout << "��ѯ����ʧ�ܣ�����ͼƬ��" << endl;
	}
	for (; symbol != image.symbol_end(); ++symbol)
	{
		cout << "���ͣ�" << endl << symbol->get_type_name() << endl << endl;
		cout << "���룺" << endl << symbol->get_data() << endl << endl;
	}
	//imshow("Source Image", src);
	//waitKey();
	image.set_data(NULL, 0);
	return result;
}

//�Զ�ֵͼ�����ʶ�����ʧ����������ж���ʶ��
string GetQRInBinImg(Mat binImg)
{
	string result = ZbarDecoder(binImg);
	if (result.empty())
	{
		Mat openImg;
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(binImg, openImg, MORPH_OPEN, element);
		imshow("morphology img", openImg);
		result = ZbarDecoder(openImg);
	}
	return result;
}

//main function
string GetQR(Mat img)
{
	Mat binImg;
	//��otsu��ֵ����Ļ����ϣ�����������ֵ������ʶ��ģ��ͼ��
	//int thre = threshold(img, binImg, 0, 255, cv::THRESH_OTSU);
	int thre = 0;
	string result;
	while (result.empty() && thre<255)
	{
		threshold(img, binImg, thre, 255, cv::THRESH_BINARY);
		result = GetQRInBinImg(binImg);
		cout << "���룺" << endl << result << endl << endl;
		thre +=20;//��ֵ������Ϊ20������Խ��ʶ����Խ�ͣ��ٶ�Խ��
	}
	return result;
}

int main()
{
	Mat imag = imread("test.png");
    GetQR(imag);
	waitKey();
}