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
    HWND gameHwnd; // 窗口句柄
    dmsoft* dm; // 大漠插件实例
    RECT gameWindowRect;//主窗口位置
    RECT miniMapRect; // 小地图位置（假设使用OpenCV的Point结构）
    RECT pointRect;     //指针位置
    double characterAngle; // 当前角色方向角度
    Logger logger; // 日志记录器
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





    // Function to resize the screenshot
    cv::Mat resizeScreenshot(const cv::Mat& screenshot, double scale)
    {
        cv::Mat resizedImage;
        cv::Size size = cv::Size(0, 0);
        cv::resize(screenshot, resizedImage, size, scale, scale);
        return resizedImage;
    }

    // Function to get coordinates
    cv::Point getCoordinates(const cv::Mat& currentMap, const cv::Mat& screenshot, double radarScale, bool& enableLocalPositioning, cv::Point& localPositioningCoords, int& positioningErrorCount)
    {
        cv::Mat radarImage = resizeScreenshot(screenshot, radarScale);
        cv::Point coordinates = cv::Point(0, 0);

        if (enableLocalPositioning)
        {
            cv::Rect roi(localPositioningCoords.x - 200, localPositioningCoords.y - 200, 400, 400);
            cv::Mat localPositioningArea = currentMap(roi);

            if (templateMatching(localPositioningArea, radarImage, coordinates, 0.4))
            {
                localPositioningCoords.x = localPositioningCoords.x - 200 + coordinates.x;
                localPositioningCoords.y = localPositioningCoords.y - 200 + coordinates.y;
                coordinates = localPositioningCoords;
            }
            else
            {
                positioningErrorCount++;
                if (positioningErrorCount >= 3)
                {
                    enableLocalPositioning = false;
                    coordinates.x = 0;
                    coordinates.y = 0;
                    positioningErrorCount = 0;
                }
            }
        }
        else
        {
            if (templateMatching(currentMap, radarImage, coordinates, 0.4))
            {
                localPositioningCoords.x = coordinates.x;
                localPositioningCoords.y = coordinates.y;
                enableLocalPositioning = true;
            }
            else
            {
                enableLocalPositioning = false;
            }
        }

        return coordinates;
    }

    // Function to print coordinates
    void printCoordinates(const cv::Mat& currentMap, const cv::Mat& screenshot, double radarScale, bool& enableLocalPositioning, cv::Point& localPositioningCoords, int& positioningErrorCount)
    {
        cv::Point coordinates = getCoordinates(currentMap, screenshot, radarScale, enableLocalPositioning, localPositioningCoords, positioningErrorCount);
        std::cout << "Current coordinates: " << coordinates.x << ", " << coordinates.y << std::endl;
    }

    // Function for template matching
    bool templateMatching(const cv::Mat& maxImage, const cv::Mat& minImage, cv::Point& coordinates, double similarityThreshold)
    {
        cv::Mat resultImage;
        int resultCols = maxImage.cols - minImage.cols + 1;
        int resultRows = maxImage.rows - minImage.rows + 1;
        if (resultCols < 0 || resultRows < 0)
            return false;

        resultImage.create(resultRows, resultCols, CV_32FC1);

        int matchMethod = cv::TM_CCOEFF_NORMED;
        cv::matchTemplate(maxImage, minImage, resultImage, matchMethod);
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(resultImage, &minVal, &maxVal, &minLoc, &maxLoc);

        cv::Point matchLoc = (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) ? minLoc : maxLoc;

        if (maxVal >= similarityThreshold)
        {
            coordinates.x = matchLoc.x + minImage.cols / 2;
            coordinates.y = matchLoc.y + minImage.rows / 2;
            return true;
        }
        else
        {
            coordinates.x = 0;
            coordinates.y = 0;
            return false;
        }
    }
  
    
       
};