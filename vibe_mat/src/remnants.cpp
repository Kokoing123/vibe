#include <iostream>
#include <opencv2/imgproc.hpp>
#include "remnants.hpp"

// ���캯������ʼ����������
Remnamts::Remnamts(int cameraID,
					float thr,
					cv::Size minSize,
					cv::Size maxSize,
					cv::Mat& zone,
					uint64_t dataTime,
					float motionlessThr,
					unsigned long stayTimeThr,
					bool remnReInitSam,
					unsigned long remnStayTimeThr) {
	_cameraID = cameraID;
	_reInitThr = thr;
	_minSize = minSize;
	_maxSize = maxSize;
	_zone = zone;
	_dataTime = dataTime;
	_motionlessThr = motionlessThr;
	_stayTimeThr = stayTimeThr;
	_remnReInitSam = remnReInitSam;
	_remnStayTimeThr = remnStayTimeThr;
	_zoneNum = cv::countNonZero(_zone);
}

// ����ͼƬ������
void Remnamts::initSamples(cv::Mat& img) {
	_vibe.initSamples(img);
	_fgMask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	_zoneFgMask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	_img_h = img.size().height;
	_img_w = img.size().width;
	_fgNum = 0;
	_vt_rect.clear();
}

// ���¹���ͼƬͼƬ������
void Remnamts::reInitSamples(cv::Mat& img) {
	_vibe.reInitSamples(img);
	_fgMask.setTo(0);
	_zoneFgMask.setTo(0);
	_img_h = img.size().height;
	_img_w = img.size().width;
	_fgNum = 0;
	_zoneFgNum = 0;
	_vt_rect.clear();
	_vt_suspFgMask.clear();
}

void Remnamts::reInitPartSamples(cv::Mat& img, std::vector<cv::Rect> vt) {
	_vibe.reInitPartSamples(img, vt);
}

// �����ܳ�Ϊ�������ǰ������
void Remnamts::findFgMask(cv::Mat& img) {
	//std::cout << "findFgMask_start:" << _vt_remnFgMask.size() << std::endl;
	if (((float)_zoneFgNum / _zoneNum) >= _reInitThr) {
		reInitSamples(img);
		_vt_remnFgMask.clear();
		std::cout << "�ƹ�仯�����ڵ��������³�ʼ����---------------" << std::endl;
	}
	else if (_remnReInitSam && (_vt_rect.size() > 0)) {
		reInitSamples(img);
		//reInitPartSamples(img, _vt_rect);
		std::cout << "��⵽�����ﵼ�����³�ʼ����---------------" << std::endl;
	}
	else {
		_vibe.findFgMask(img);
		_fgNum = _vibe.getFgNum();
		_fgMask = _vibe.getFGMask(false);
		cv::bitwise_and(_fgMask, _zone, _zoneFgMask);
		_zoneFgNum = cv::countNonZero(_zoneFgMask);
	}
	//std::cout << "findFgMask_end:" << _vt_remnFgMask.size() << std::endl;
}

cv::Mat Remnamts::getFGMask() {
	return _zoneFgMask;
}

unsigned long Remnamts::getFgNum() {
	return _zoneFgNum;
}

// ��ȡ����������
std::vector<cv::Rect> Remnamts::getRect() {
	//std::cout << "getRect_start:" << _vt_remnFgMask.size() << std::endl;
	_vt_rect.clear();
	processFgMask();
	time_t now;
	int flag = 0;                                                  // ��ʶλ���Ƿ���ԭ����������
	std::vector<SuspFgMask> tmp_vt_suspFgMask;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(_zoneFgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
	cv::Rect boundRect;

	// ����ʱ�䣻����д�������contours��ѭ�����Ϊ������ÿ֡contours.size()���������
	for (int i = 0; i < _vt_suspFgMask.size(); i++) {
		_vt_suspFgMask[i].endTime = time(&now);
	}

	for (int i = 0; i < contours.size(); i++) {
		boundRect = cv::boundingRect((cv::Mat)contours[i]);        //����ÿ����������Ӿ���
		if ((boundRect.width <= _maxSize.width) && (boundRect.width >= _minSize.width) && (boundRect.height <= _maxSize.height) && (boundRect.height >= _minSize.height)) {
			for (int j = 0; j < _vt_suspFgMask.size(); j++) {      // �����Ƿ���ԭ����������
				cv::Rect is_rect = boundRect | _vt_suspFgMask[j].rect;
				cv::Rect un_rect = boundRect & _vt_suspFgMask[j].rect;
				double IOU = un_rect.area()*1.0 / is_rect.area();
				if (IOU >= _motionlessThr) {                       // ��ԭ����������
					tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect,_vt_suspFgMask[j].startTime,time(&now) });
					flag = 1;
					break;
				}
			}
			if (!flag) {                                           // ����ԭ����������ʱ�����������
				tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect, time(&now), 0 });
				flag = 0;
			}
		}
	}

	// ����_stayTimeThr�Ĳ�������ѡ����������Ϊ�������
	_vt_suspFgMask.clear();
	for (int i = 0; i < tmp_vt_suspFgMask.size(); i++) {
		_vt_suspFgMask.push_back(tmp_vt_suspFgMask[i]);
		if ((tmp_vt_suspFgMask[i].endTime - tmp_vt_suspFgMask[i].startTime) >= _stayTimeThr) {
			_vt_rect.push_back(tmp_vt_suspFgMask[i].rect);
		}
	}

	// ����ʱ��
	for (int i = 0; i < _vt_remnFgMask.size(); i++) {
		_vt_remnFgMask[i].endTime = time(&now);
	}
	// �ų�ͬһλ�ö�������������
	std::vector<cv::Rect> tmp_vt_rect;
	std::vector<SuspFgMask> tmp_vt_remnFgMask;
	for (int i = 0; i < _vt_rect.size(); i++) {
		if (_vt_remnFgMask.size() == 0) {
			tmp_vt_rect.push_back(_vt_rect[i]);
		}
		else {
			for (int j = 0; j < _vt_remnFgMask.size(); j++) {
				cv::Rect is_rect = _vt_rect[i] | _vt_remnFgMask[j].rect;
				cv::Rect un_rect = _vt_rect[i] & _vt_remnFgMask[j].rect;
				double IOU = un_rect.area()*1.0 / is_rect.area();
				if (IOU >= _motionlessThr) {
					if ((_vt_remnFgMask[j].endTime - _vt_remnFgMask[j].startTime) < _remnStayTimeThr) {
						_vt_remnFgMask[j].endTime = time(&now);
						tmp_vt_remnFgMask.push_back(_vt_remnFgMask[j]);
					}
					else {
						tmp_vt_rect.push_back(_vt_rect[i]);
					}
				}
				else {
					tmp_vt_rect.push_back(_vt_rect[i]);
				}
			}
		}
	}
	if (_vt_rect.size() > 0) {
		_vt_remnFgMask.clear();
	}
	for (int i = 0; i < tmp_vt_remnFgMask.size(); i++) {
		_vt_remnFgMask.push_back(tmp_vt_remnFgMask[i]);
	}
	for (int i = 0; i < tmp_vt_rect.size(); i++) {
		time_t startTime = time(&now) - _stayTimeThr;
		_vt_remnFgMask.push_back(SuspFgMask{ tmp_vt_rect[i],startTime,time(&now) });
	}

	return tmp_vt_rect;
}

void Remnamts::processFgMask() {
	cv::Mat element_3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::morphologyEx(_zoneFgMask, _zoneFgMask, cv::MORPH_OPEN, element_3);   // ������

	cv::Mat element_9 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));
	cv::erode(_zoneFgMask, _zoneFgMask, element_3);                           // ��ʴ
	cv::dilate(_zoneFgMask, _zoneFgMask, element_9);                          // ����
}

void Remnamts::setMaxSize(cv::Size size) {
	//_vibe.setMaxSize(size);
	_maxSize = size;
}

void Remnamts::setMinSize(cv::Size size) {
	//_vibe.setMinSize(size);
	_minSize = size;
}

void Remnamts::setRadius(int r) {
	_vibe.setRadius(r);
}

void Remnamts::setReqMathces(int m) {
	_vibe.setReqMathces(m);
}

void Remnamts::setSubsamplingFactor(int f) {
	_vibe.setSubsamplingFactor(f);
}