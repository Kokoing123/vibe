#pragma once
#include <opencv2/core/core.hpp>

// �������Ŀ�������Լ�ʱ��
struct SuspFgMask
{
	cv::Rect rect;
	time_t startTime;
	time_t endTime;
	double meanGradPre;                                         // û��Ŀ��ʱ�������ƽ���ݶ�
	double meanGradCur;                                         // Ŀ���Ե��ƽ���ݶ�
};

std::string getLocSTDTime();                   // ��ȡ���ظ�ʽ��ʱ�䣺  2020/04/24 14:04:20
std::string getLocNameTime();                  // ��ȡ��ʱ��Ϊ���ĸ�ʽ��20200424_140420

cv::UMat getImgGradient(cv::UMat);                                  // ��ȡͼƬ���ݶ�
double getPartImgMeanGradient_1(cv::UMat, cv::UMat, cv::Rect);      // ����ͼƬ�ֲ�����Ŀ���Ե��ƽ���ݶ�;mask������ʴ���ͣ���Ե���ش���3
double getPartImgMeanGradient_2(cv::UMat, cv::UMat, cv::Rect);      // ����ͼƬ�ֲ�����Ŀ���Ե��ƽ���ݶȣ�maskû�д�����Ե���ص���1

void points2Mask(cv::Mat& src, std::vector<cv::Point> mask_points); // ���㼯ת��mask
void drawingLine(cv::Mat& img, std::vector<cv::Point> tri);