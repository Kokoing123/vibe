#pragma once
#include <opencv2/core/core.hpp>
#include "utils.hpp"

namespace vibe_hw
{
	class VIBE
	{
	public:
		VIBE();
		VIBE(int num, int min_match, int radiu, int rand_sam, cv::Size minSize, cv::Size maxSize, bool update_neighborhood);
		void initSamples(cv::UMat& gray);                                        // ��ʼ��samples
		void reInitSamples(cv::UMat& gray);                                      // ���³�ʼ��samples
		void reInitPartSamples(cv::UMat& gray, std::vector<SuspFgMask> vt);        // samples�ľֲ����³�ʼ��
		void findFgMask(cv::UMat& gray);
		cv::UMat getFGMask(bool);
		unsigned long getFgNum();
		std::vector<cv::Rect> getRect();

		void setReqMathces(int);
		void setRadius(int);
		void setSubsamplingFactor(int);
		void setMinSize(cv::Size);
		void setMaxSize(cv::Size);

	private:
		bool update_neighborhood;       // �������ص�8�����Ƿ����
		int updata_index = 0;           // ���������µ�����
		cv::RNG rng;                    // �������
		cv::Size minSize;               // �ߣ���
		cv::Size maxSize;               // �ߣ���
		std::vector<cv::Rect> vt_rect;  // ���Ŀ��ľ������ꣻ���Ͻǣ�����
		unsigned long fgNum;            // ǰ�����صĸ���
		int nbSamples;                  // ÿ�����ص�������������Ĭ��20��
		int reqMatches;                 // ǰ������ƥ�����������������ֵ������Ϊ�Ǳ�������
		cv::UMat reqMatches_umat;
		int radius;                     // ƥ��뾶�����ڸð뾶������Ϊ��ƥ������
		cv::UMat radius_umat;
		int subsamplingFactor;          // ��������ӣ�������Ϊǰ����ÿ��������1/defaultSubsamplingFactor���ʸ���������������������
		int background = 0;
		int foreground = 255;
		cv::UMat fgMask;                 // ���Ϊǰ��������
		std::vector<cv::UMat> samples;   // ͼƬ��������
		void buildNeighborArray(cv::UMat& gray);
		void processFgMask();
		void updateSamples(cv::UMat& gray); // ����samples
	};
}