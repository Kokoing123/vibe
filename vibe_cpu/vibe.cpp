#include "vibe.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include<cmath>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

namespace vibe
{
	/*
	��ʼ��vibe����*/
	VIBE::VIBE(int num_sam = 20, int min_match = 2, int radiu = 20, int rand_sam = 16, int fgCount_thr=80, cv::Size minSize = cv::Size(50, 60), cv::Size maxSize = cv::Size(640,480)) :
		defaultNbSamples(num_sam),
		defaultReqMatches(min_match),
		defaultRadius(radiu),
		defaultSubsamplingFactor(rand_sam),
		fgCount_thr(fgCount_thr),
		minSize(minSize),
		maxSize(maxSize){
		background = 0;
		foreground = 255;
	}

	/*
	��ʼ������ģ��:
		1����ʼ��ÿ�����ص�����������
		2����ʼ��ǰ�������mask
		3����ʼ��ǰ�����صļ���������
		������
		img: �����numpyͼ�����飬Ҫ��Ҷ�ͼ��*/
	void VIBE::initBGModel(cv::Mat img) {
		buildNeighborArray(img);
		int sz[2] = { img.rows, img.cols };  // {�ߣ���}
		fgCount = cv::Mat::zeros(2, sz, CV_8UC1);
		fgMask = cv::Mat::zeros(2, sz, CV_8UC1);
	}

	int VIBE::getRandom(int low, int up) {
		return ((rand() % (up - low + 1)) + low);
	}

	/*
	����һ��ͼ����ÿ�����ص���������(ʵ����ÿ������ֵ��һ�������һ�����򲻱�)
		����������Ҷ�ͼ��
		����ֵ��ÿ������9�������飬���浽self.samples��*/
	void VIBE::buildNeighborArray(cv::Mat img) {
		srand((int)time(0)); // �����������
		// ����һ�� width * height * defaultNbSamples ͼƬ��
		samples = cv::Mat::zeros(img.size().height, img.size().width, CV_8UC(20));
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				for (int c = 0; c < defaultNbSamples; c++) {
					//int a = img.at<uchar>(i, j);
					if (img.at<uchar>(i, j) <= 4)
						samples.at<Vec20u>(i, j)[c] = 0;
					else if (img.at<uchar>(i, j) >= 251)
						samples.at<Vec20u>(i, j)[c] = 255;
					else {
						//int b = getRandom(-4, 4);
						samples.at<Vec20u>(i, j)[c] = img.at<uchar>(i, j) + getRandom(-4, 4);
					}
					//cout << int(img.at<uchar>(i, j)) << " ";
					//cout << samples.at<Vec20f>(i, j)[c] << " ";
				}
			}
			//cout << endl;
		}
		/*for (int i = 0; i < 10; i++)
			cout << getRandom(-4, 4) << " ";
		cout << endl;
		int x = 20;
		int y = 20;
		cout << int(img.at<uchar>(x, y)) << endl;

		for (int i = 0; i < defaultNbSamples; i++)
			cout << int(samples.at<Vec20u>(x, y)[i]) << " ";
		cout << endl;*/
	}

	/*
	����������������ã�
	��һ���ҵ�ǰ��Ŀ������أ�
		���򣺱ȶԵ�ǰ����ֵ��defaultRadius�����ڣ���Ӧλ�ã��������ĸ�����С����Ϊǰ��
	�ڶ���������������������
		���±������ص������������Է��������裺
		1.ÿ������������1 / defaultSubsamplingFactor���ʸ����Լ���������
		  ������������ʽΪ���ѡȡ�������������е�һ��Ԫ�أ�����Ϊ��ǰ���ص�ֵ
		2.ÿ������������1 / defaultSubsamplingFactor���ʸ��������������
		  ����������������ʽΪ���ѡȡһ������㣬���ڸ������������������ѡ��һ������Ϊ��ǰ����ֵ
		  �����Լ�������*/
	// ׷���ٶȰ汾:���ǰ���͸�������������ԭ����Ӧ�÷ŵ�ͬһ��ѭ�������Ϊ��׷���ٶȷŵ�ͬһ��ѭ����
	void VIBE::update(cv::Mat img) {
		fgNum = 0;
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				// ���㵱ǰ����ֵ����������ֵ֮��С�ڷ�ֵ��ΧRADIUS�ĸ���
				int sum = 0;
				for (int c = 0; c < defaultNbSamples; c++) {
					// �ȶԵ�ǰ����ֵ��defaultRadius�����ڣ���Ӧλ�ã��������ĸ���
					if (abs(int(samples.at<Vec20u>(i, j)[c]) - int(img.at<uchar>(i, j))) < defaultRadius)
						sum += 1;
				}
				// ���С��ƥ��������ֵ��Ϊǰ��
				if (sum < defaultReqMatches) {
					fgMask.at<uchar>(i, j) = foreground;
					if (abs(fgCount_thr)) {
						fgCount.at<uchar>(i, j) += 1;
						// ���ĳ����������fgCount_thr�α����Ϊǰ��������Ϊһ�龲ֹ��������Ϊ�˶����������Ϊ������
						if (fgCount.at<uchar>(i, j) >= abs(fgCount_thr)) {
							fgMask.at<uchar>(i, j) = background;
							fgCount.at<uchar>(i, j) = 0;
							updataPixel(img, i, j);
						}
						else {
							fgNum++;
						}
					}
					else {
						fgNum++;
					}
				}
				else {
					fgMask.at<uchar>(i, j) = background;  // ������ڵ���ƥ��������ֵ�����Ǳ���
					if (abs(fgCount_thr)) {
						fgCount.at<uchar>(i, j) = 0;
					}
					updataPixel(img, i, j);
				}
			}
		}
	}

	void VIBE::updataPixel(cv::Mat& img, int i, int j) {
		int p1 = getRandom(1, defaultSubsamplingFactor);  // ��[1, defaultSubsamplingFactor]���������һ����
		// 1 / defaultSubsamplingFactor���ʸ����Լ���������
		if (p1 == 1) {
			samples.at<Vec20u>(i, j)[getRandom(0, defaultNbSamples - 1)] = img.at<uchar>(i, j);
		}
		int p2 = getRandom(1, defaultSubsamplingFactor);  // ��[1, defaultSubsamplingFactor]���������һ����
		// 1 / defaultSubsamplingFactor���ʸ��������������
		if (p2 == defaultSubsamplingFactor) {
			// 1:��ʾ���µ�ǰ������һ������ 2:��ʾ���µ�ǰ������һ������
			// 3:��ʾ���µ�ǰ������һ������ 4:��ʾ���µ�ǰ������һ������
			switch (getRandom(1, 4))
			{
			case 1:
				if ((j - 1) >= 0) {
					samples.at<Vec20u>(i, j - 1)[getRandom(0, defaultNbSamples - 1)] = img.at<uchar>(i, j);
					break;
				}
			case 2:
				if ((i + 1) < img.size().height) {
					samples.at<Vec20u>(i + 1, j)[getRandom(0, defaultNbSamples - 1)] = img.at<uchar>(i, j);
					break;
				}
			case 3:
				if ((j + 1) < img.size().width) {
					samples.at<Vec20u>(i, j + 1)[getRandom(0, defaultNbSamples - 1)] = img.at<uchar>(i, j);
					break;
				}
			case 4:
				if ((i - 1) >= 0) {
					samples.at<Vec20u>(i - 1, j)[getRandom(0, defaultNbSamples - 1)] = img.at<uchar>(i, j);
					break;
				}
			}
		}
	}

	vector<cv::Rect> VIBE::getRect() {
		vt_rect.clear();
		processFgMask();
		vector<vector<cv::Point>> contours;
		vector<cv::Vec4i> hierarchy;
		cv::findContours(fgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
		cv::Rect boundRect;
		for (int i = 0; i < contours.size(); i++) {
			boundRect = cv::boundingRect((cv::Mat)contours[i]); //����ÿ����������Ӿ���
			if ((boundRect.width <= maxSize.width)&&(boundRect.width >= minSize.width)&& (boundRect.height <= maxSize.height) && (boundRect.height >= minSize.height)) {
				vt_rect.push_back(cv::Rect(boundRect.x, boundRect.y, boundRect.width, boundRect.height));
			}
		}
		return vt_rect;
	}

	cv::Mat VIBE::getFGMask(bool process) {
		if (process) {
			processFgMask();
			return fgMask;
		}
		else {
			return fgMask;
		}
	}

	unsigned long VIBE::getFgNum() {
		return fgNum;
	}

	void VIBE::processFgMask() {
		cv::Mat element_3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		//cv::morphologyEx(fgMask, fgMask, cv::MORPH_OPEN, element_3);   // ������

		cv::Mat element_7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
		cv::erode(fgMask, fgMask, element_3);  // ��ʴ
		cv::dilate(fgMask, fgMask, element_7);
	}
}