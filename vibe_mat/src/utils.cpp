#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "utils.hpp"

// ���ظ�ʽ��ʱ�䣺2020/04/26 15:09:25
std::string getLocSTDTime() {
	struct tm t;              //tm�ṹָ��
	time_t now;               //����time_t���ͱ���
	time(&now);               //��ȡϵͳ���ں�ʱ��
	localtime_s(&t, &now);    //��ȡ�������ں�ʱ��

	std::string time_std = cv::format("%d", t.tm_year + 1900) + "/" + cv::format("%.2d", t.tm_mon + 1) + "/" + cv::format("%.2d", t.tm_mday) + " " +
		cv::format("%.2d", t.tm_hour) + ":" + cv::format("%.2d", t.tm_min) + ":" + cv::format("%.2d", t.tm_sec);
	return time_std;
}

// ���ظ�ʽ��ʱ�䣺20200426_150925
std::string getLocNameTime() {
	struct tm t;              //tm�ṹָ��
	time_t now;               //����time_t���ͱ���
	time(&now);               //��ȡϵͳ���ں�ʱ��
	localtime_s(&t, &now);    //��ȡ�������ں�ʱ��

	std::string time_name = cv::format("%d", t.tm_year + 1900) + cv::format("%.2d", t.tm_mon + 1) + cv::format("%.2d", t.tm_mday) + "_" +
		cv::format("%.2d", t.tm_hour) + cv::format("%.2d", t.tm_min) + cv::format("%.2d", t.tm_sec);
	return time_name;
}