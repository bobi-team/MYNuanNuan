#pragma once
#include <iostream>
#include <Windows.h>
#include<string.h>
#include <vector>
#include <cmath>
#include <chrono>
#include<opencv2/opencv.hpp>
#include "obj.h" 
#pragma comment(lib,"opencv_world4110d.lib")
using namespace cv;
using namespace std;
#define 大漠插件路径 "E:\\DmLib\\xd47243.dll"
#define 破解Dll路径 "E:\\DmLib\\Go.dll"
typedef void (WINAPI* GoFunProc)(DWORD g_dm);


class NGame {
public:
    HWND gameHwnd; // 窗口句柄
    dmsoft* dm; // 大漠插件实例
    RECT gameWindowRect;//主窗口位置
    RECT miniMapRect; // 小地图位置（假设使用OpenCV的Point结构）
    RECT pointRect;     //指针位置
    double characterAngle; // 当前角色方向角度
    
    mutable std::mutex angleMutex;
public:
    // 构造函数
    NGame() {}

    // 初始化方法
    bool Initialize(const string& windowName) {
        DWORD dm_hmodule = LoadDm(大漠插件路径);
        if (dm_hmodule == NULL) { return 0; }
        HMODULE go_hmodule = LoadLibraryA(破解Dll路径);
        GoFunProc GoFun = NULL;
        GoFun = (GoFunProc)GetProcAddress(go_hmodule, "Go");
        if (GoFun == NULL) { return 0; }
        GoFun(dm_hmodule);
        dm = new dmsoft();
        long nret = dm->Reg("", "");
        cout << nret << endl;
        if (nret == 1)
        {
            cout << "大漠注册成功" << endl;
        }
        else
        {
            cout << "大漠注册失败" << endl;
            // 释放资源
            delete dm;
            FreeLibrary(go_hmodule);
            FreeLibrary((HMODULE)dm_hmodule);
            return 0;
        }

        // 查找游戏窗口
        gameHwnd = (HWND)dm->FindWindowW("", windowName.c_str());
        if (gameHwnd == NULL) {
            cout << "未找到目标窗口！" << endl;
            delete dm;
            FreeLibrary(go_hmodule);
            FreeLibrary((HMODULE)dm_hmodule);
            return false;
        }


        // 获取窗口位置
        //不同游戏根据小地图位置进行偏移计算
        if (GetWindowRect(gameHwnd, &gameWindowRect)) {
            cout << "初始化坐标成功 " << endl;
            cout << "开始初始化小地图 " << endl;
            miniMapRect.left = gameWindowRect.left + 75;
            miniMapRect.top = gameWindowRect.top + 65;
            miniMapRect.right = gameWindowRect.left +290;
            miniMapRect.bottom = gameWindowRect.top + 280;
            cout << "小地图初始化成功 " << endl;
            cout << "开始初始化指针 " << endl;
            pointRect.left = gameWindowRect.left + 170;
            pointRect.top = gameWindowRect.top + 155;
            pointRect.right = gameWindowRect.left + 200;
            pointRect.bottom = gameWindowRect.top + 185;
            cout << "小地图初始化成功 " << endl;
        }
        else {
            cout << "获取游戏窗口位置失败！" << endl;
            return false;
        }

        return true;
    }

    bool CaptureGameWindow(const std::string& filename) {
        if (dm->Capture(gameWindowRect.left+10, gameWindowRect.top+10, gameWindowRect.right-10, gameWindowRect.bottom-10, filename.c_str())) {
            cout << "捕地图成功"<< endl;
            return true;
        }
        else {
            return false;
        }

    }

    bool CaptureMiniMap(const std::string& filename) {
    
        if (dm->Capture(miniMapRect.left, miniMapRect.top, miniMapRect.right, miniMapRect.bottom, filename.c_str())) {
            cout << "捕获小地图成功，保存为minimap.bmp" << endl;
            return true;
        }
        else {
            return false;
        }
       
    }
    bool CaptureMiniMapEx(const std::string& filename) {

        if (dm->Capture(miniMapRect.left+45, miniMapRect.top+35, miniMapRect.left+180, miniMapRect.top+170, filename.c_str())) {
            cout << "捕获小地图成功，保存为minimap.bmp" << endl;
            return true;
        }
        else {
            return false;
        }

    }

    bool CapturePoint(const std::string& filename) {
        if (dm->Capture(pointRect.left, pointRect.top, pointRect.right, pointRect.bottom, filename.c_str())) {
            std::cout << "捕获指针成功，保存为" << filename << std::endl;
            return true;
        }
        else {
            return false;
        }
    }

    std::vector<int> find_peaks(const cv::Mat& gradX, int minDistance = 10, double threshold = 0.5) {
        std::vector<int> peaks;
        for (int col = 1; col < gradX.cols - 1; ++col) {
            float prev = gradX.at<float>(col - 1);
            float curr = gradX.at<float>(col);
            float next = gradX.at<float>(col + 1);
            if (curr > prev && curr > next && curr > threshold) {
                peaks.push_back(col);
            }
        }
        return peaks;
    }
    void setCharacterAngle(double newAngle) {
        std::lock_guard<std::mutex> lock(angleMutex);
        characterAngle = newAngle;
    }
    double getCharacterAngle() const {
        std::lock_guard<std::mutex> lock(angleMutex);
        return characterAngle;
    }
   
    void RunLoop() {
        while(1){

            
            // 间隔0.2秒
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        
    }
    void createAndSaveMaskFromImage(const std::string& image_path, const std::string& mask_path) {
        // 读取带有透明通道的图片
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);

        // 检查图片是否成功读取
        if (image.empty()) {
            std::cerr << "Error: Unable to load image at path " << image_path << std::endl;
            return;
        }

        // 检查图片是否有透明通道
        if (image.channels() < 4) {
            std::cerr << "Error: Image does not have an alpha channel" << std::endl;
            return;
        }

        // 提取透明通道
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        cv::Mat alpha_channel = channels[3]; // Alpha通道通常是在最后一个通道

        // 创建掩码图像，透明部分填充黑色，不透明部分填充白色
        cv::Mat mask;
        cv::threshold(alpha_channel, mask, 0, 255, cv::THRESH_BINARY); // 二值化处理

        // 保存掩码图像
        if (!cv::imwrite(mask_path, mask)) {
            std::cerr << "Error: Unable to save mask image at path " << mask_path << std::endl;
            return;
        }

        std::cout << "Mask image saved successfully at " << mask_path << std::endl;
    }

    int masktest() {
        std::string image_path = "template.png"; // 替换为你的图片路径
        std::string mask_path = "mask.png";   // 替换为你要保存掩码图片的路径

        createAndSaveMaskFromImage(image_path, mask_path);

        return 0;
    }


    void extractImageUsingMask(const std::string& image_path, const std::string& mask_path, const std::string& output_path) {
        // 读取原始图像
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // 读取掩码图像
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // 检查图像和掩码是否成功读取
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // 确保掩码是单通道的
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // 根据掩码扣取图像
        cv::Mat masked_image;
        image.copyTo(masked_image, mask);

        // 保存扣取后的图像
        if (!cv::imwrite(output_path, masked_image)) {
            std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            return;
        }

        std::cout << "Extracted image saved successfully at " << output_path << std::endl;
    }
    int extracttest() {
       
        std::string image_path = "point.png";       // 替换为你的原始图片路径
        std::string mask_path = "mask.png";         // 替换为你的掩码图片路径
        std::string output_path = "tiqu.png";  // 替换为你要保存扣取后图片的路径
        CapturePoint(image_path);
        extractImageUsingMask(image_path, mask_path, output_path);

        return 0;
    }


   


    void rotateAndExtractImageUsingMask(const std::string& image_path, const std::string& mask_path, const std::string& output_dir) {
        // 读取原始图像
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // 读取掩码图像
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // 检查图像和掩码是否成功读取
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // 确保掩码是单通道的
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // 初始化变量以存储旋转的角度
        int angle = 0;

        // 循环旋转掩码图像，每10度旋转一次，总共旋转360度
        for (int rotation_count = 0; rotation_count < 36; ++rotation_count) {
            // 计算旋转矩阵
            cv::Point2f center(mask.cols / 2.0, mask.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // 执行旋转
            cv::Mat rotated_mask;
            cv::warpAffine(mask, rotated_mask, rotation_matrix, mask.size());

            // 更新旋转角度
            angle += 10;

            // 根据旋转后的掩码扣取图像
            cv::Mat masked_image;
            image.copyTo(masked_image, rotated_mask);

            // 生成输出路径
            std::string output_path = output_dir + "/rotated_masked_image_" + std::to_string(rotation_count) + ".png";

            // 保存扣取后的图像
            if (!cv::imwrite(output_path, masked_image)) {
                std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            }
            else {
                std::cout << "Extracted image " << rotation_count << " saved successfully at " << output_path << std::endl;
            }
        }

        std::cout << "All rotations and extractions completed." << std::endl;
    }

    int rotatemasktest() {
        std::string image_path = "point.png";       // 替换为你的原始图片路径
        std::string mask_path = "mask.png";
        std::string output_dir = "image";     // 替换为输出目录路径

        rotateAndExtractImageUsingMask(image_path, mask_path, output_dir);

        return 0;
    }
    void rotateImageAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& output_dir) {
        // 读取原始图像
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // 读取掩码图像
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // 检查图像和掩码是否成功读取
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // 确保掩码是单通道的
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // 初始化变量以存储旋转的角度
        int angle = 0;

        // 循环旋转原始图像，每10度旋转一次，总共旋转360度
        for (int rotation_count = 0; rotation_count < 36; ++rotation_count) {
            // 计算旋转矩阵
            cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // 执行旋转
            cv::Mat rotated_image;
            cv::warpAffine(image, rotated_image, rotation_matrix, image.size());

            // 更新旋转角度
            angle += 10;

            // 根据不旋转的掩码扣取旋转后的图像
            cv::Mat masked_image;
            rotated_image.copyTo(masked_image, mask);

            // 生成输出路径
            std::string output_path = output_dir + "/rotated_image_masked_" + std::to_string(rotation_count) + ".png";

            // 保存扣取后的图像
            if (!cv::imwrite(output_path, masked_image)) {
                std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            }
            else {
                std::cout << "Extracted image " << rotation_count << " saved successfully at " << output_path << std::endl;
            }
        }

        std::cout << "All rotations and extractions completed." << std::endl;
    }

    int rotateimagetest() {
        std::string image_path = "point.png";       // 替换为你的原始图片路径
        std::string mask_path = "mask.png";
        std::string output_dir = "image1";     // 替换为输出目录路径

        rotateImageAndExtractWithMask(image_path, mask_path, output_dir);

        return 0;
    }



    double calculateSimilarity(const cv::Mat& template_image, const cv::Mat& masked_image) {
        // 检查输入图像是否为空
        if (template_image.empty() || masked_image.empty()) {
            std::cerr << "Input images should not be empty" << std::endl;
            return -1.0;
        }

        // 检查图像尺寸是否相同
        if (template_image.size() != masked_image.size()) {
            std::cerr << "Template and masked images should have the same size" << std::endl;
            return -1.0;
        }

        // 检查图像类型是否相同
        if (template_image.type() != masked_image.type()) {
            std::cerr << "Template and masked images should have the same type" << std::endl;
            return -1.0;
        }

        // 定义结果矩阵
        cv::Mat result;
        int match_method = cv::TM_CCOEFF_NORMED; // 使用归一化相关系数

        // 进行模板匹配
        cv::matchTemplate(masked_image, template_image, result, match_method);

        // 获取最佳匹配的相似度值
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        // 返回最大相似度值
        return maxVal;
    }

    void rotateAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& template_path) {
        // 读取原始图像
        cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
        // 读取掩码图像
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);
        // 读取模板图像
        cv::Mat template_image = cv::imread(template_path, cv::IMREAD_COLOR);

        // 检查图像和掩码是否成功读取
        if (image.empty() || mask.empty() || template_image.empty()) {
            std::cerr << "Error: Unable to load image, mask or template" << std::endl;
            return;
        }

        // 确保掩码是二值的
        cv::threshold(mask, mask, 128, 255, cv::THRESH_BINARY);

        // 预处理：转换为灰度图像（如果需要）
        cv::Mat image_gray, template_gray;
        cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(template_image, template_gray, cv::COLOR_BGR2GRAY);




        // 初始化变量以存储最大相似度和对应的角度
        double max_similarity = 0.0;
        int max_similarity_angle = 0;

        // 获取开始时间
        auto start_time = std::chrono::high_resolution_clock::now();

        // 循环旋转原始图像，每10度旋转一次，总共旋转360度
        for (int angle = 0; angle < 360; angle += 10) {
            // 计算旋转矩阵
            cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // 执行旋转
            cv::Mat rotated_image;
            cv::warpAffine(image, rotated_image, rotation_matrix, image.size());

            // 应用掩码
            cv::Mat masked_image;
            rotated_image.copyTo(masked_image, mask);
            
            cv::Mat template_mask;
            template_image.copyTo(template_mask, mask);


            // 计算与模板图像的相似度
            double similarity = calculateSimilarity(template_mask, masked_image);

            // 更新最大相似度和对应的角度
            if (similarity > max_similarity) {
                max_similarity = similarity;
                max_similarity_angle = angle;
            }
        }

        // 获取结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

        // 输出最大相似度和对应的角度
        std::cout << "Maximum similarity: " << max_similarity << " at angle: " << max_similarity_angle << " degrees" << std::endl;
        // 输出耗时
        std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;
    }


    void fangiangtest() {
        std::string image_path = "point.png";      // 替换为你的原始图片路径
        std::string mask_path = "mask.png";        // 替换为你的掩码图片路径
        std::string template_path = "template.png"; // 替换为你的模板图片路径
      
        while (1) {

            CapturePoint(image_path);
            rotateAndExtractWithMask(image_path, mask_path, template_path);
        }
        

    }
    // 截取游戏窗口的函数，使用纯数字命名图片，并间隔2秒
    void CaptureGameWindowsWithInterval(int maxFrames) {
        int frameCount = 0;
        while (frameCount < maxFrames) {
            // 构建文件名
            std::string filename = std::to_string(frameCount) + ".png";

            // 调用截取游戏窗口的函数
            CaptureGameWindow(filename);

            // 输出信息，表示已经截取了一张图片
            std::cout << "Captured " << filename << std::endl;

            // 睡眠2秒
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // 增加帧计数
            ++frameCount;
        }
    }
    void findAndMarkSmallMapPosition(const std::string& bigMapPath, const std::string& smallMapPath) {
        // 加载大地图和小地图
        CaptureMiniMapEx(smallMapPath.c_str());
        cv::Mat bigMap = cv::imread(bigMapPath);
        cv::Mat smallMap = cv::imread(smallMapPath);
        if (bigMap.empty() || smallMap.empty()) {
            std::cerr << "Error: One of the images did not load properly." << std::endl;
            return;
        }

        // 转换为灰度图
        cv::Mat grayBigMap, graySmallMap;
        cv::cvtColor(bigMap, grayBigMap, cv::COLOR_BGR2GRAY);
        cv::cvtColor(smallMap, graySmallMap, cv::COLOR_BGR2GRAY);

        // 创建ORB检测器
        cv::Ptr<cv::ORB> orb = cv::ORB::create();

        // 检测关键点并计算描述符
        std::vector<cv::KeyPoint> keypointsBigMap, keypointsSmallMap;
        cv::Mat descriptorsBigMap, descriptorsSmallMap;
        orb->detectAndCompute(grayBigMap, cv::noArray(), keypointsBigMap, descriptorsBigMap);
        orb->detectAndCompute(graySmallMap, cv::noArray(), keypointsSmallMap, descriptorsSmallMap);

        // 创建匹配器
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);

        // 使用KNN匹配
        std::vector<std::vector<cv::DMatch>> knn_matches;
        matcher->knnMatch(descriptorsSmallMap, descriptorsBigMap, knn_matches, 2);

        // 比率检验
        const float ratio_threshold = 0.75f;
        std::vector<cv::DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++) {
            if (knn_matches[i].size() > 1) {
                if (knn_matches[i][0].distance < ratio_threshold * knn_matches[i][1].distance) {
                    good_matches.push_back(knn_matches[i][0]);
                }
            }
        }

        // 绘制匹配结果
        cv::Mat img_matches;
        cv::drawMatches(smallMap, keypointsSmallMap, bigMap, keypointsBigMap, good_matches, img_matches,
            cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        // 显示匹配结果
        cv::imshow("Matches", img_matches);

        // 如果有良好匹配，标记小地图在大地图上的位置
        if (!good_matches.empty()) {
            // 提取匹配点的位置
            std::vector<cv::Point2f> obj;
            std::vector<cv::Point2f> scene;
            for (size_t i = 0; i < good_matches.size(); i++) {
                obj.push_back(keypointsSmallMap[good_matches[i].queryIdx].pt);
                scene.push_back(keypointsBigMap[good_matches[i].trainIdx].pt);
            }

            // 计算Homography
            cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);

            // 从Homography计算小地图的四个角在大地图上的位置
            std::vector<cv::Point2f> obj_corners(4);
            obj_corners[0] = cv::Point2f(0, 0);
            obj_corners[1] = cv::Point2f(smallMap.cols, 0);
            obj_corners[2] = cv::Point2f(smallMap.cols, smallMap.rows);
            obj_corners[3] = cv::Point2f(0, smallMap.rows);
            std::vector<cv::Point2f> scene_corners(4);
            cv::perspectiveTransform(obj_corners, scene_corners, H);
            // 继续绘制小地图在大地图上的位置
            cv::line(bigMap, scene_corners[2] + cv::Point2f(smallMap.cols, 0), scene_corners[3] + cv::Point2f(smallMap.cols, 0), cv::Scalar(0, 255, 0), 4);
            cv::line(bigMap, scene_corners[3] + cv::Point2f(smallMap.cols, 0), scene_corners[0] + cv::Point2f(smallMap.cols, 0), cv::Scalar(0, 255, 0), 4);

            // 显示标记后的大地图
            cv::imshow("Marked Big Map", bigMap);
        }
        else {
            std::cout << "No good matches found." << std::endl;
        }

        // 等待用户按键后关闭所有窗口
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    int ditu() {
        // 调用函数，替换为实际的大地图和小地图路径
        findAndMarkSmallMapPosition("bigmap.png", "minimap.png");
        return 0;
    }
 
};