#pragma once
#include <opencv2/core/core.hpp>
#include "vibe.hpp"
#include "utils.hpp"

class Remnamts {
public:
	Remnamts(int, float, cv::Size, cv::Size, cv::Mat&, uint64_t, float, unsigned long, bool, unsigned long);     // ���캯��
	void initSamples(cv::Mat& img);                                   // ��ʼ��samples
	void reInitSamples(cv::Mat& img);                                 // ���³�ʼ��������
	void reInitPartSamples(cv::Mat& img, std::vector<cv::Rect> vt);   // �ֲ�����samples�����������µĲ���Ϊ_vt_rect
	void findFgMask(cv::Mat& img);									  // �����ܳ�Ϊ�����������
	cv::Mat getFGMask();                                              // ���ؿ��ܳ�Ϊ�����������
	unsigned long getFgNum();                                         // ���ؿ��ܳ�Ϊ����������ص����
	std::vector<cv::Rect> getRect();                                  // ���ز���������

	void setReqMathces(int);
	void setRadius(int);
	void setSubsamplingFactor(int);
	void setMinSize(cv::Size);
	void setMaxSize(cv::Size);
private:
	unsigned long _remnStayTimeThr;                                   // �������������ﱣ��ʱ��;��ֹͬһλ�ö�μ��
	bool _remnReInitSam;                                              // ��⵽�������Ƿ����³�ʼ��Samples
	float _motionlessThr;                                             // ���Ʋ�����͵�ǰ���������IOU�������_motionlessThr,����Ϊ��ֹ����
	unsigned long _stayTimeThr;                                       // �����ﾲֹ���������Ϊ���������ֵ����λΪ��
	float _reInitThr;                                                 // ���³�ʼ������������Ҫ���������ƹ�仯����ɵĸ�������
	vibe_hw::VIBE _vibe;                                              // vibe�㷨����
	int _cameraID;                                                    // ����ͷ��ID��Ŀǰû������
	cv::Mat _zone;                                                    // ��������Mask
	uint64_t _dataTime;                                               // ���ؿ�����ʱ��
	cv::Mat _fgMask;                                                  // ���Ʋ������Mask
	cv::Mat _zoneFgMask;                                              // ����������Ʋ������Mask
	unsigned long _fgNum;                                             // _fgMask�������صĸ���
	unsigned long _zoneFgNum;                                         // _zoneFgMask�������صĸ���
	unsigned long _zoneNum;                                           // _zone�������صĸ���
	std::vector<cv::Rect> _vt_rect;                                   // ���Ϊ������Ĵ洢����
	int _img_w;
	int _img_h;
	cv::Size _minSize;                                                // ���������С�ߣ���
	cv::Size _maxSize;                                                // ����������ߣ���
	std::vector<SuspFgMask> _vt_suspFgMask;                           // ���Ʋ�����Ĵ洢����
	std::vector<SuspFgMask> _vt_remnFgMask;                           // �Ѿ����Ϊ�����������洢ĳһ��ʱ��

	void processFgMask();                                             // ��ͼ����ķ���������������
};