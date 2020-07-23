#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//#define NDEBUG
#include <assert.h>

#include "vibe.h"
#include "utils.hpp"

class ApplyVibe {
public:
	~ApplyVibe();
	void initPara(std::string &configPath);
	int initVibe(cv::Mat& initImg);
	void runningVibe(cv::Mat& src, cv::Mat& zone, cv::Size minSize, cv::Size maxSize, std::vector<cv::Rect>& ret);
	void printPara();
private:
	int _deleteDataThr;
	unsigned long _zoneNum = 0;                                        // ����������صĸ���
	unsigned long _fgNum = 0;                                          // ������ǰ�������ظ���
	bool _isReturnRet = false;                                         // �Ƿ��н������
	cv::Mat _mask;                                                     // vibe���ĳ���ǰ��
	cv::Mat _processMask;                                              // ����ͼ������ǰ��
	cv::Mat _imgGradOld;                                               // ��ʼ��ʱͼƬ���ݶ�
	cv::Mat _imgGradNew;                                               // ��ǰ֡ͼƬ���ݶ�
	int _gradientThr;                                                  // �����ݶȸ����ж��Ƿ���������:0��ʾ�����ã�����0��ʾ����,����Ϊ��ֵ
	unsigned long _remnStayTimeThr;                                    // �������������ﱣ��ʱ��;��ֹͬһλ�ö�μ��
	bool _remnReInitSam;                                               // ��⵽�������Ƿ����³�ʼ��Samples
	float _motionlessThr;                                              // ���Ʋ�����͵�ǰ���������IOU�������_motionlessThr,����Ϊ��ֹ����
	unsigned long _stayTimeThr;                                        // �����ﾲֹ���������Ϊ���������ֵ����λΪ��
	float _reInitThr;                                                  // ���³�ʼ������������Ҫ���������ƹ�仯����ɵĸ�������
	std::vector<SuspFgMask> _vt_suspRemn;                              // ���Ʋ�����Ĵ洢����
	int _numberOfSamples = 0;                                          // ÿ�����ص�����������
	int	_matchingThreshold = 0;                                        // ǰ������ƥ�����������������ֵ������Ϊ�Ǳ�������
	int	_matchingNumber = 0;                                           // ƥ��뾶�����ڸð뾶������Ϊ��ƥ������
	int	_updateFactor = 0;                                             // ��������ӣ����Ϊǰ����ÿ��������1/_updateFactor���ʸ���
	vibeModel_Sequential_t *_model = NULL;

	void getDetectResults(std::vector<cv::Rect>& ret);
	// Ϊÿ��Rect���ʱ�����Ϣ
	void rectAddTime(std::vector<cv::Rect>& vt_rect);
	// ��ȡ���������ľ�������
	void getRect(cv::Mat& zone, cv::Size minSize, cv::Size maxSize, std::vector<cv::Rect>& rects);
	void processFgMask();                                              // ��ͼ����ķ�������
	void split(std::string& src, std::string& result);
	void readTxt(std::string &configPath);
};