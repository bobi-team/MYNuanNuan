#pragma once
#include <iostream>
#include <Windows.h>
#include<string.h>
#include <vector>
#include <cmath>
#include <chrono>
#include<opencv2/opencv.hpp>
#include "obj.h" 
#include"Logger.hpp"
#pragma comment(lib,"opencv_world4110d.lib")
using namespace std;
using namespace cv;
typedef void (WINAPI* GoFunProc)(DWORD g_dm);
class NGame {
public:
    string dm_path;
    string reg_path;
    HWND gameHwnd; // ���ھ��
    dmsoft* dm; // ��Į���ʵ��
    RECT gameWindowRect;//������λ��
    RECT miniMapRect; // С��ͼλ�ã�����ʹ��OpenCV��Point�ṹ��
    RECT pointRect;     //ָ��λ��
    double characterAngle; // ��ǰ��ɫ����Ƕ�
    Logger logger; // ��־��¼��
    mutable std::mutex angleMutex;
    NGame();
    bool Initialize(const string& windowName);
    bool CaptureGameWindow(const std::string& filename);
    bool CaptureMiniMap(const std::string& filename);
    bool CaptureMiniMapEx(const std::string& filename);
    bool CapturePoint(const std::string& filename);
    void setCharacterAngle(double newAngle);
    double getCharacterAngle() const;
    void createAndSaveMaskFromImage(const std::string& image_path, const std::string& mask_path);
    int masktest();
    double calculateSimilarity(const cv::Mat& template_image, const cv::Mat& masked_image);
    void rotateAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& template_path);
    void fangiangtest();
};