#pragma once
#include <opencv2/core/core.hpp>

namespace vibe
{
	class VIBE
	{
	public:
		typedef cv::Vec<uchar, 20> Vec20u;
		VIBE(int num_sam, int min_match, int radiu, int rand_sam, int fgCount, cv::Size minSize, cv::Size maxSize);
		void initBGModel(cv::Mat img);    // ��ʼ������ģ��
		void update(cv::Mat img);       // ׷���ٶȰ汾
		cv::Mat getFGMask(bool);
		int getRandom(int low, int top);  // ��[a, b]֮����������
		unsigned long getFgNum();
		std::vector<cv::Rect> getRect();

	private:
		cv::Size minSize;               // �ߣ���
		cv::Size maxSize;               // �ߣ���
		int fgCount_thr;                // ĳ��������fgCount_thr���ж�Ϊǰ��������Ϊ�Ǳ�����0��ʾ�����ж�
		std::vector<cv::Rect> vt_rect;  // ���Ŀ��ľ������ꣻ���Ͻǣ�����
		unsigned long fgNum;            // ǰ�����صĸ���
		int defaultNbSamples;           // ÿ�����ص�������������Ĭ��20��
		int defaultReqMatches;          // ǰ������ƥ�����������������ֵ������Ϊ�Ǳ�������
		int defaultRadius;              // ƥ��뾶�����ڸð뾶������Ϊ��ƥ������
		int defaultSubsamplingFactor;   // ��������ӣ�������Ϊǰ����ÿ��������1/defaultSubsamplingFactor���ʸ���������������������
		int background;
		int foreground;
		cv::Mat fgCount;                // ��¼ÿ�����ص��������Ϊǰ���Ĵ���
		cv::Mat fgMask;                 // ���Ϊǰ��������
		cv::Mat samples;                // ͼƬ��������
		int samples_size[3];
		void buildNeighborArray(cv::Mat img);
		void processFgMask();
		void updataPixel(cv::Mat& img,int i, int j);  // ��������
	};
}