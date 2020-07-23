#include "vibe.hpp"
#include <iostream>
#include <opencv2/imgproc.hpp>

namespace vibe_hw {
	/*
	��ʼ��vibe����*/
	VIBE::VIBE() {
		nbSamples = 20;
		reqMatches = 2;
		radius = 20;
		subsamplingFactor = 16;
		minSize = cv::Size(50, 50);
		maxSize = cv::Size(640, 480);
		update_neighborhood = false;

		background = 0;
		foreground = 255;
		updata_index = 0;
	}

	VIBE::VIBE(int num_sam, int min_match, int radiu, int rand_sam, cv::Size minSize, cv::Size maxSize, bool update_neighborhood) :
		nbSamples(num_sam),
		reqMatches(min_match),
		radius(radiu),
		subsamplingFactor(rand_sam),
		minSize(minSize),
		maxSize(maxSize),
		update_neighborhood(update_neighborhood) {
		background = 0;
		foreground = 255;
		updata_index = 0;
	}

	/*
	��ʼ������ģ��:
		1����ʼ��ÿ�����ص�����������
		2����ʼ��ǰ�������mask
		3����ʼ��ǰ�����صļ���������
		������
		img: �����numpyͼ�����飬Ҫ��Ҷ�ͼ��*/
	void VIBE::initSamples(cv::Mat& gray) {
		buildNeighborArray(gray);
		int sz[2] = { gray.rows, gray.cols };  // {�ߣ���}
		fgMask = cv::Mat::zeros(2, sz, CV_8UC1);
	}

	void VIBE::reInitSamples(cv::Mat& gray) {
		fgMask.setTo(0);
		fgNum = 0;
		vt_rect.clear();
		samples.clear();
		buildNeighborArray(gray);
	}

	// ������
	// �ֲ�����samples�����������µĲ���Ϊvt_rect
	void VIBE::reInitPartSamples(cv::Mat& gray, std::vector<cv::Rect> vt) {
		unsigned int seed = time(NULL);
		cv::RNG rng(seed);

		cv::Mat vt_mask = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);
		for (int i = 0; i < vt.size(); i++) {
			vt_mask(vt[i]).setTo(255);
		}

		cv::Mat gray_16s_part = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		cv::Mat gray_16s = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		gray.convertTo(gray_16s, CV_16SC1);
		cv::bitwise_and(gray_16s, gray_16s, gray_16s_part, vt_mask);

		// ����
		//cv::Mat imshow_mat = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);
		//gray_16s_part.convertTo(imshow_mat, CV_8UC1);
		//cv::imshow("part", imshow_mat);
		//cv::imshow("fgMask", vt_mask);
		//cv::waitKey(1);

		cv::Mat mat_4_part = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		cv::Mat mat_8u_part = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);

		cv::Mat sample_part = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);

		for (int i = 0; i < samples.size(); i++) {
			rng.fill(mat_4_part, cv::RNG::UNIFORM, -4, 4, true);
			cv::bitwise_and(mat_4_part, mat_4_part, mat_4_part, vt_mask);

			mat_4_part = mat_4_part + gray_16s_part;
			mat_4_part.convertTo(mat_8u_part, CV_8UC1);
			mat_8u_part.convertTo(mat_4_part, CV_16SC1);

			sample_part = samples[i];
			cv::bitwise_and(sample_part, sample_part, sample_part, vt_mask);
			samples[i] = samples[i] - sample_part;
			samples[i] = samples[i] + mat_4_part;
		}
	}

	/*
	����һ��ͼ����ÿ�����ص���������(ʵ����ÿ������ֵ��һ�������һ�����򲻱�)
		����������Ҷ�ͼ��
		����ֵ��ÿ������9�������飬���浽samples��*/
	void VIBE::buildNeighborArray(cv::Mat& gray) {
		unsigned int seed = time(NULL);
		cv::RNG rng(seed);
		int sz[2] = { gray.rows, gray.cols };  // {�ߣ���}

		cv::Mat img_16s = cv::Mat::zeros(2, sz, CV_16SC1);
		gray.convertTo(img_16s, CV_16SC1);

		cv::Mat mat_4 = cv::Mat::zeros(2, sz, CV_16SC1);
		cv::Mat mat_8u = cv::Mat::zeros(2, sz, CV_8UC1);

		for (int i = 0; i < nbSamples; i++) {
			cv::Mat mat_16s = cv::Mat::zeros(2, sz, CV_16SC1);
			rng.fill(mat_4, cv::RNG::UNIFORM, -4, 4, true);
			//std::cout << "�������:" << std::endl;
			//std::cout << mat_4 << std::endl;
			mat_4 = img_16s + mat_4;
			//std::cout << "��Ӻ�:" << std::endl;
			//std::cout << mat_4 << std::endl;

			// ȷ�����ز�С��0�Ͳ�����255
			mat_4.convertTo(mat_8u, CV_8UC1);
			mat_8u.convertTo(mat_16s, CV_16SC1);
			samples.push_back(mat_16s);

			//std::cout << "�����:" << std::endl;
			//std::cout << mat_16s << std::endl;
			//std::cout << "-------------------------------" << std::endl;
		}
		/*std::cout << "����ͼƬ��:" << std::endl;
		for (int i = 0; i < samples.size(); i++) {
			std::cout << samples[i] << std::endl;
		}
		std::cout << "����ͼ������" << std::endl;*/
	}
	/*
	����������������ã�
	��һ���ҵ�ǰ��Ŀ������أ�
		���򣺱ȶԵ�ǰ����ֵ��defaultRadius�����ڣ���Ӧλ�ã��������ĸ�����С����Ϊǰ��
	�ڶ�������������������*/
	void VIBE::findFgMask(cv::Mat& gray) {
		cv::Mat img_16s = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		gray.convertTo(img_16s, CV_16SC1);                                 // ��ͼƬ��CV_8UC1ת��CV_16SC1

		cv::Mat diff = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);     // ����ͼƬ����������ľ���ֵ
		cv::Mat ones = cv::Mat::ones(gray.rows, gray.cols, CV_16SC1);
		cv::Mat ret = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		cv::Mat mask = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);;
		cv::Mat ret_tmp;

		for (int i = 0; i < samples.size(); i++) {
			//std::cout << "����ͼƬ:" << std::endl;
			//std::cout << samples[i] << std::endl;
			ret_tmp = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
			cv::absdiff(samples[i], img_16s, diff);
			//std::cout << "����ͼƬ������ͼƬ�Ĳ�:" << std::endl;
			//std::cout << diff << std::endl;
			mask = diff < radius;
			//std::cout << "����:" << std::endl;
			//std::cout << mask << std::endl;
			cv::bitwise_and(ones, ones, ret_tmp, mask);
			ret = ret + ret_tmp;
			//std::cout << "���:" << std::endl;
			//std::cout << ret << std::endl;
		}
		//std::cout << "����:" << std::endl;
		//std::cout << ret << std::endl;

		// ���С��ƥ��������ֵ����Ϊǰ��
		fgMask = ret < reqMatches;

		fgNum = countNonZero(fgMask);

		//std::cout << "ǰ������:" << std::endl;
		//std::cout << fgMask << std::endl;
		updateSamples(img_16s);
	}

	/*
	���±������ص������������Բ��Ϊ��������(ʵ�ֵ�ʱ������������һ��ʵ�ֵ�)��
	1.ÿ������������1 / defaultSubsamplingFactor���ʸ����Լ�����������
		������������ʽΪ���ѡȡ�������������е�һ��Ԫ�أ�����Ϊ��ǰ���ص�ֵ��
	2.ÿ������������1 / defaultSubsamplingFactor���ʸ����������������
		����������������ʽΪ���ѡȡһ������㣬���ڸ������������������ѡ��һ������Ϊ��ǰ����ֵ
		�����Լ���������*/
	void VIBE::updateSamples(cv::Mat& gray) {
		cv::Mat element_3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::Mat bgMask = fgMask < 100;          // ����������
		//std::cout << "��������:" << std::endl;
		//std::cout << bgMask << std::endl;
		//std::cout << fgMask << std::endl;

		if (update_neighborhood)
			cv::dilate(bgMask, bgMask, element_3);  // ��ñ���8��������
		//std::cout << "����8��������:" << std::endl;
		//std::cout << bgMask << std::endl;

		cv::Mat mat_pbt = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);  // ���ʾ���
		rng.fill(mat_pbt, cv::RNG::UNIFORM, 1, subsamplingFactor + 1, true);
		//std::cout << "����:" << std::endl;
		//std::cout << mat_pbt << std::endl;

		cv::Mat updata = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);   // ��Ҫ���µ�����
		cv::bitwise_and(mat_pbt, mat_pbt, updata, bgMask);
		//std::cout << "����8�������:" << std::endl;
		//std::cout << updata << std::endl;

		cv::Mat updata_mask = updata == 5;   // ��1 / defaultSubsamplingFactor���ʸ������غ������������
		//std::cout << "��1/n֮һ����:" << std::endl;
		//std::cout << updata_mask << std::endl;

		// ����������
		cv::Mat img_16s = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		gray.convertTo(img_16s, CV_16SC1);
		cv::Mat img_updata = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		cv::bitwise_and(img_16s, img_16s, img_updata, updata_mask);
		//std::cout << "���µ�����ֵ:" << std::endl;
		//std::cout << img_updata << std::endl;
		cv::Mat smp = cv::Mat::zeros(gray.rows, gray.cols, CV_16SC1);
		cv::bitwise_and(samples[updata_index], samples[updata_index], smp, updata_mask);
		//std::cout << "�ɵ�����ֵ:" << std::endl;
		//std::cout << smp << std::endl;
		samples[updata_index] = samples[updata_index] - smp;
		samples[updata_index] = samples[updata_index] + img_updata;
		updata_index++;
		updata_index = updata_index % nbSamples;
		//std::cout << updata_index % defaultNbSamples << std::endl;
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

	std::vector<cv::Rect> VIBE::getRect() {
		vt_rect.clear();
		processFgMask();
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(fgMask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());
		cv::Rect boundRect;
		for (int i = 0; i < contours.size(); i++) {
			boundRect = cv::boundingRect((cv::Mat)contours[i]); //����ÿ����������Ӿ���
			if ((boundRect.width <= maxSize.width) && (boundRect.width >= minSize.width) && (boundRect.height <= maxSize.height) && (boundRect.height >= minSize.height)) {
				vt_rect.push_back(cv::Rect(boundRect.x, boundRect.y, boundRect.width, boundRect.height));
			}
		}
		return vt_rect;
	}

	void VIBE::setMaxSize(cv::Size size) {
		maxSize = size;
	}

	void VIBE::setMinSize(cv::Size size) {
		minSize = size;
	}

	void VIBE::setRadius(int r) {
		radius = r;
	}

	void VIBE::setReqMathces(int m) {
		reqMatches = m;
	}

	void VIBE::setSubsamplingFactor(int f) {
		subsamplingFactor = f;
	}
}