#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "remnants.hpp"

// ���캯������ʼ����������
Remnamts::Remnamts(int cameraID,
					float thr,
					cv::Size minSize,
					cv::Size maxSize,
					cv::UMat& zone,
					uint64_t dataTime,
					float motionlessThr,
					unsigned long stayTimeThr,
					bool remnReInitSam,
					unsigned long remnStayTimeThr,
					unsigned int gradientThr) {
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
	_gradientThr = gradientThr;
}

// ����ͼƬ������
void Remnamts::initSamples(cv::UMat& img) {
	_vibe.initSamples(img);
	_fgMask = cv::UMat::zeros(img.rows, img.cols, CV_8UC1);
	_zoneFgMask = cv::UMat::zeros(img.rows, img.cols, CV_8UC1);
	_img_h = img.size().height;
	_img_w = img.size().width;
	_fgNum = 0;
	_vt_ret.clear();
	if(_gradientThr){
		_imgGradPre = getImgGradient(img);
	}
}

// ���¹���ͼƬͼƬ������
void Remnamts::reInitSamples(cv::UMat& img) {
	_vibe.reInitSamples(img);
	_fgMask.setTo(0);
	_zoneFgMask.setTo(0);
	_img_h = img.size().height;
	_img_w = img.size().width;
	_fgNum = 0;
	_zoneFgNum = 0;
	_vt_ret.clear();
	_vt_suspFgMask.clear();
	if (_gradientThr) {
		_imgGradPre = getImgGradient(img);
	}
}

void Remnamts::reInitPartSamples(cv::UMat& img, std::vector<cv::Rect> vt) {
	_vibe.reInitPartSamples(img, vt);
}

// �����ܳ�Ϊ�������ǰ������
void Remnamts::findFgMask(cv::UMat& img) {
	//std::cout << "findFgMask_start:" << _vt_remnFgMask.size() << std::endl;
	if (((float)_zoneFgNum / _zoneNum) >= _reInitThr) {
		reInitSamples(img);
		_vt_remnFgMask.clear();
		std::cout << "��Χ�ƹ�仯���ڵ�������������³�ʼ����---------------" << std::endl;
	}
	else if (_remnReInitSam && (_vt_ret.size() > 0)) {
		reInitSamples(img);
		//reInitPartSamples(img, _vt_rect);
		std::cout << "��⵽�����ﵼ�¾ֲ����³�ʼ����---------------" << std::endl;
	}
	else {
		_vibe.findFgMask(img);
		_fgNum = _vibe.getFgNum();
		_fgMask = _vibe.getFGMask(false);
		cv::bitwise_and(_fgMask, _zone, _zoneFgMask);
		_zoneFgNum = cv::countNonZero(_zoneFgMask);
		if (_gradientThr) {
			_imgGradCur = getImgGradient(img);
		}
	}
}

cv::UMat Remnamts::getFGMask() {
	return _zoneFgMask;
}

unsigned long Remnamts::getFgNum() {
	return _zoneFgNum;
}

// ��ȡ����������
std::vector<SuspFgMask> Remnamts::getRect() {
	_vt_ret.clear();
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

	//cv::imshow("fgMask", _fgMask);
	//cv::waitKey(1);

	for (int i = 0; i < contours.size(); i++) {
		boundRect = cv::boundingRect((cv::Mat)contours[i]);        //����ÿ����������Ӿ���
		if ((boundRect.width <= _maxSize.width) && (boundRect.width >= _minSize.width) && (boundRect.height <= _maxSize.height) && (boundRect.height >= _minSize.height)) {
			for (int j = 0; j < _vt_suspFgMask.size(); j++) {      // �����Ƿ���ԭ����������
				cv::Rect is_rect = boundRect | _vt_suspFgMask[j].rect;
				cv::Rect un_rect = boundRect & _vt_suspFgMask[j].rect;
				double IOU = un_rect.area()*1.0 / is_rect.area();
				if (IOU >= _motionlessThr) {                       // ��ԭ����������
					if (_gradientThr) {
						double gradCur = getPartImgMeanGradient_2(_imgGradCur, _fgMask, boundRect);
						tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect, _vt_suspFgMask[j].startTime, time(&now), _vt_suspFgMask[j].meanGradPre, gradCur });
						flag = 1;
						break;
					}
					else {
						tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect, _vt_suspFgMask[j].startTime, time(&now), 0, 0 });
						flag = 1;
						break;
					}
				}
			}
			if (!flag) {                                           // ����ԭ����������ʱ�����������
				if (_gradientThr) {
					double gradPre = getPartImgMeanGradient_2(_imgGradPre, _fgMask, boundRect);
					double gradCur = getPartImgMeanGradient_2(_imgGradCur, _fgMask, boundRect);
					tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect, time(&now), 0, gradPre, gradCur });
					flag = 0;
				}
				else {
					tmp_vt_suspFgMask.push_back(SuspFgMask{ boundRect, time(&now), 0, 0, 0 });
					flag = 0;
				}
			}
		}
	}

	// ����_stayTimeThr�Ĳ�������ѡ����������Ϊ�������
	_vt_suspFgMask.clear();
	for (int i = 0; i < tmp_vt_suspFgMask.size(); i++) {
		_vt_suspFgMask.push_back(tmp_vt_suspFgMask[i]);
		if ((tmp_vt_suspFgMask[i].endTime - tmp_vt_suspFgMask[i].startTime) >= _stayTimeThr) {
			if (_gradientThr) {
				if (std::abs(tmp_vt_suspFgMask[i].meanGradCur - tmp_vt_suspFgMask[i].meanGradPre) > std::abs(_gradientThr)) {
					_vt_ret.push_back(tmp_vt_suspFgMask[i]);
				}
			}
			else {
				_vt_ret.push_back(tmp_vt_suspFgMask[i]);
			}
		}
	}

	// ����ʱ��
	for (int i = 0; i < _vt_remnFgMask.size(); i++) {
		_vt_remnFgMask[i].endTime = time(&now);
	}
	// �ų�ͬһλ�ö�������������
	std::vector<SuspFgMask> tmp_vt_ret;
	std::vector<SuspFgMask> tmp_vt_remnFgMask;
	for (int i = 0; i < _vt_ret.size(); i++) {
		if (_vt_remnFgMask.size() == 0) {
			tmp_vt_ret.push_back(_vt_ret[i]);
		}
		else {
			for (int j = 0; j < _vt_remnFgMask.size(); j++) {
				cv::Rect is_rect = _vt_ret[i].rect | _vt_remnFgMask[j].rect;
				cv::Rect un_rect = _vt_ret[i].rect & _vt_remnFgMask[j].rect;
				double IOU = un_rect.area()*1.0 / is_rect.area();
				if (IOU >= _motionlessThr) {
					if ((_vt_remnFgMask[j].endTime - _vt_remnFgMask[j].startTime) < _remnStayTimeThr) {
						_vt_remnFgMask[j].endTime = time(&now);
						tmp_vt_remnFgMask.push_back(_vt_remnFgMask[j]);
					}
					else {
						tmp_vt_ret.push_back(_vt_ret[i]);
					}
				}
				else {
					tmp_vt_ret.push_back(_vt_ret[i]);
				}
			}
		}
	}

	if (_vt_ret.size() > 0) {
		_vt_remnFgMask.clear();
	}
	for (int i = 0; i < tmp_vt_remnFgMask.size(); i++) {
		_vt_remnFgMask.push_back(tmp_vt_remnFgMask[i]);
	}
	for (int i = 0; i < tmp_vt_ret.size(); i++) {
		time_t startTime = time(&now) - _stayTimeThr;
		_vt_remnFgMask.push_back(SuspFgMask{ tmp_vt_ret[i].rect,startTime,time(&now),tmp_vt_ret[i].meanGradPre, tmp_vt_ret[i].meanGradCur });
	}

	//_imgGradPre = _imgGradCur.clone();
	return tmp_vt_ret;
}

void Remnamts::processFgMask() {
	cv::Mat element_3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::morphologyEx(_zoneFgMask, _zoneFgMask, cv::MORPH_OPEN, element_3);   // ������
	cv::Mat element_9 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(9, 9));

	cv::UMat element_3_umat = cv::UMat(3, 3, CV_8UC1);
	cv::UMat element_9_umat = cv::UMat(9, 9, CV_8UC1);

	element_3.copyTo(element_3_umat);
	element_9.copyTo(element_9_umat);

	cv::erode(_zoneFgMask, _zoneFgMask, element_3_umat);          // ��ʴ
	cv::dilate(_zoneFgMask, _zoneFgMask, element_9_umat);         // ����
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