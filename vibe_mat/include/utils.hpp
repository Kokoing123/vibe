#pragma once
#include <opencv2/core/core.hpp>

// �������Ŀ�������Լ�ʱ��
struct SuspFgMask
{
	cv::Rect rect;
	time_t startTime;
	time_t endTime;
};

std::string getLocSTDTime();                                  // ��ȡ���ظ�ʽ��ʱ�䣺  2020/04/24 14:04:20
std::string getLocNameTime();                                 // ��ȡ��ʱ��Ϊ���ĸ�ʽ��20200424_140420