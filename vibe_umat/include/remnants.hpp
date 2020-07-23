#pragma once
#include <opencv2/core/core.hpp>
#include "vibe.hpp"
#include "utils.hpp"

class Remnamts {
public:
	Remnamts(int, float, cv::Size, cv::Size, cv::UMat&, uint64_t, float, unsigned long, bool, unsigned long, unsigned int);     // ���캯��
	void initSamples(cv::UMat& img);                                   // ��ʼ��samples
	void reInitSamples(cv::UMat& img);                                 // ���³�ʼ��������
	void reInitPartSamples(cv::UMat& img, std::vector<cv::Rect> vt);   // �ֲ�����samples�����������µĲ���Ϊ_vt_rect
	void findFgMask(cv::UMat& img);									   // �����ܳ�Ϊ�����������
	cv::UMat getFGMask();                                              // ���ؿ��ܳ�Ϊ�����������
	unsigned long getFgNum();                                          // ���ؿ��ܳ�Ϊ����������ص����
	std::vector<SuspFgMask> getRect();                                 // ���ز���������

	void setReqMathces(int);
	void setRadius(int);
	void setSubsamplingFactor(int);
	void setMinSize(cv::Size);
	void setMaxSize(cv::Size);

private:
	cv::UMat _imgGradPre;                                              // ��ʼ��ʱͼƬ���ݶ�
	cv::UMat _imgGradCur;                                              // ��ǰ֡ͼƬ���ݶ�
	int _gradientThr;                                                  // �����ݶȸ����ж��Ƿ���������:0��ʾ�����ã�����0��ʾ����,����Ϊ��ֵ
	unsigned long _remnStayTimeThr;                                    // �������������ﱣ��ʱ��;��ֹͬһλ�ö�μ��
	bool _remnReInitSam;                                               // ��⵽�������Ƿ����³�ʼ��Samples
	float _motionlessThr;                                              // ���Ʋ�����͵�ǰ���������IOU�������_motionlessThr,����Ϊ��ֹ����
	unsigned long _stayTimeThr;                                        // �����ﾲֹ���������Ϊ���������ֵ����λΪ��
	float _reInitThr;                                                  // ���³�ʼ������������Ҫ���������ƹ�仯����ɵĸ�������
	vibe_hw::VIBE _vibe;                                               // vibe�㷨����
	int _cameraID;                                                     // ����ͷ��ID��Ŀǰû������
	cv::UMat _zone;                                                    // ��������Mask
	uint64_t _dataTime;                                                // ���ؿ�����ʱ��
	cv::UMat _fgMask;                                                  // ���Ʋ������Mask
	cv::UMat _zoneFgMask;                                              // ����������Ʋ������Mask
	unsigned long _fgNum;                                              // _fgMask�������صĸ���
	unsigned long _zoneFgNum;                                          // _zoneFgMask�������صĸ���
	unsigned long _zoneNum;                                            // _zone�������صĸ���
	std::vector<SuspFgMask> _vt_ret;                                   // ���Ϊ������Ĵ洢����
	int _img_w;
	int _img_h;
	cv::Size _minSize;                                                 // ���������С�ߣ���
	cv::Size _maxSize;                                                 // ����������ߣ���
	std::vector<SuspFgMask> _vt_suspFgMask;                            // ���Ʋ�����Ĵ洢����
	std::vector<SuspFgMask> _vt_remnFgMask;                            // �Ѿ����Ϊ�����������洢ĳһ��ʱ��

	void processFgMask();                                              // ��ͼ����ķ���������������
};