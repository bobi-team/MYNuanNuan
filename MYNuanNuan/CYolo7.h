#pragma once
#include "layer.h"
#include "net.h"
#pragma comment(lib, "ncnn.lib")

#define MAX_STRIDE 32

struct YoloObject
{
	cv::Rect_<float> rect;

	int label;
	float prob;

	YoloObject()
	{
		rect = {0, 0, 0, 0};
		label = -1;
		prob = 0;
	}
};

class CYolo7
{
public:
	~CYolo7();
	CYolo7(const CYolo7&) = delete;
	CYolo7& operator=(const CYolo7&) = delete;

	static CYolo7& get_instance() {
		static CYolo7 instance;
		return instance;
	}

	void SetParam(const char* paramPath, const char* binPath, const char* szOut1, const char* szOut2, int w, int h, bool isgpu = true);
	int Detect(BYTE* pbImgData, std::vector<YoloObject>& objs);

private:
	CYolo7();

	void qsort_descent_inplace(std::vector<YoloObject>& objects, int left, int right);
	void qsort_descent_inplace(std::vector<YoloObject>& objects);
	void nms_sorted_bboxes(const std::vector<YoloObject>& faceobjects, std::vector<int>& picked, float nms_threshold, bool agnostic = false);
	void generate_proposals(const ncnn::Mat& anchors, int stride, const ncnn::Mat& in_pad, const ncnn::Mat& feat_blob, float prob_threshold, std::vector<YoloObject>& objects);
	//void draw_objects(const cv::Mat& bgr, const std::vector<YoloObject>& objects);
	
	ncnn::Net yolov7;

	std::string m_szOut1;
	std::string m_szOut2;

	cv::Mat mattmpbgra;
	cv::Mat mattmpbgr;

	int imgsize;
	int detect_yolov7(const cv::Mat& bgr, std::vector<YoloObject>& objects);
};
