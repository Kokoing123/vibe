#pragma once
#include <opencv2/core/core.hpp>

namespace vibe_hw
{
	class VIBE
	{
	public:
		VIBE();
		VIBE(int num, int min_match, int radiu, int rand_sam, cv::Size minSize, cv::Size maxSize, bool update_neighborhood);
		void initSamples(cv::Mat& gray);                                       // ��ʼ��samples
		void reInitSamples(cv::Mat& gray);                                     // ���³�ʼ��samples
		void reInitPartSamples(cv::Mat& gray, std::vector<cv::Rect> vt);       // samples�ľֲ����³�ʼ��
		void findFgMask(cv::Mat& gray);
		cv::Mat getFGMask(bool);
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
		int radius;                     // ƥ��뾶�����ڸð뾶������Ϊ��ƥ������
		int subsamplingFactor;          // ��������ӣ�������Ϊǰ����ÿ��������1/defaultSubsamplingFactor���ʸ���������������������
		int background = 0;
		int foreground = 255;
		cv::Mat fgMask;                 // ���Ϊǰ��������
		std::vector<cv::Mat> samples;   // ͼƬ��������
		void buildNeighborArray(cv::Mat& gray);
		void processFgMask();
		void updateSamples(cv::Mat& gray); // ����samples
	};
}